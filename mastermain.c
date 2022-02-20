/*
 * File:   lab2.c
 * Author: Diana Alvarado
 *
 * Created on 31 de enero de 2022, 02:19 PM
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//----------------------Librerias--------------------------------

#include <xc.h>
#include "LCD.h"
#include "USART.h"
#include "SPI.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pic16f887.h>

//-------------------------Constantes
#define _XTAL_FREQ 8000000

/*
 * ----------------------------- Variables ----------------------------------
 */
float V3, V2, V1, tempconvert;
char variable;
float V1con, V2con = 0;
char V1c[5];
char V2c[5];
char V3c[5];
char centena;
char decena;
char unidad;
char cont;
char ADC;
float temp;
//Definir para el usart
#define _8MHz  4
#define trans  1
#define recep  1
/*
 * -------------------------- Prototipos de funciones -----------------------
 */

void setup(void); //configuraciones
void convert(void);
void ADC_SPI(void);
void cont_SPI(void);
void temperatura(void);


/*
 * ------------------------------Interupciones----------------------------
 */


/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */
void main (void)
{
    //setup de ADC, USART y demás
    setup();
    USART_setup(_8MHz,trans,recep);
    __delay_us(50);
    ADCON0bits.GO = 1;
    
    //LCD
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1:   S2:   S3:");
    
    while(1)
    {
        ADC_SPI(); //Recibir datos del ADC
        cont_SPI();//Recibir datos del contador
        temperatura();//Recibir datos de la temperatura
        tempconvert = temp*(1.16); //Convertir el valor de temperatura
        V1con = (ADC*0.0196);// COnvertir valor del ADC
        convert(); //Convertir contador
        
        
        Lcd_Set_Cursor(2,1);                //2da fila 
        sprintf(V1c,"%.2fV", V1con);    //Se convierte a string
        cadena(" S1: \r"); //Se envía a la computadora
        cadena(V1c); //Se envía a la computadora
        Lcd_Write_String(V1c);      //los strings obtenidos
        sprintf(V1c," %.2fV ", tempconvert);
        Lcd_Write_String(V1c);//Se envia al LCD
        cadena(" S2: \r"); //Se envía a la computadora
        cadena(V1c); //Se envía a la computadora
        sprintf(V1c, " %d", centena);
        Lcd_Write_String(V1c);//Se envía a la computadora
        cadena(" S3: \r"); //Se envía a la computadora
        cadena(V1c); //Se envía a la computadora
        sprintf(V1c, "%d", decena);
        Lcd_Write_String(V1c);//Se envía a la computadora
        cadena(V1c); //Se envía a la computadora
        sprintf(V1c, "%d ", unidad);
        Lcd_Write_String(V1c);//Se envía a la computadora
        cadena(V1c); //Se envía a la computadora

    }
}

/*
 * -------------------------------- Funciones --------------------------------
 */
void setup(void)
{
    //Configuración entradas y salidas
    ANSEL = 0;
    ANSELH = 0;
        
    //Limpiar todos los puertos
    PORTA = 0; 
    PORTB = 0; 
    PORTD = 0; 
    PORTE = 0; 
    TRISB = 0;
    TRISD = 0;
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0111; // 8MHz
    OSCCONbits.SCS = 1;

    //---------SPI---------
    TRISA0 = 0;
    PORTAbits.RA0 = 1;
    TRISA1 = 0;
    PORTAbits.RA1 = 1;
    TRISA2 = 0;
    PORTAbits.RA2 = 1;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    return; 

}
void convert (){
    centena = (cont/100);
    decena = ((cont-(centena*100))/10);
    unidad = (cont-(centena*100 + decena*10));
    return;
}

void ADC_SPI(void){
    PORTAbits.RA1 = 0; 
    __delay_ms(1);
    
    spiWrite(1); 
    ADC = spiRead(); //Se recibe la señal del pic
    
    __delay_ms(1);
    PORTAbits.RA1 = 1;
    __delay_ms(250);
    
}

void cont_SPI(void){
    PORTAbits.RA2 = 0;
    __delay_ms(1);
    
    spiWrite(1);
    cont = spiRead();//Se recibe la señal del pic
    
    __delay_ms(1);
    PORTAbits.RA2 = 1;
    __delay_ms(250);
    
}

void temperatura(void){
    PORTAbits.RA0 = 0;
    __delay_ms(1);
    
    spiWrite(1);
    temp = spiRead();//Se recibe la señal del pic
    
    __delay_ms(1);
    PORTAbits.RA0 = 1;
    __delay_ms(250);
    
}