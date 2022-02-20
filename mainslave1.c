/*
 * File:   mainslave1.c
 * Author: motad
 *
 *
 */

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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pic16f887.h>
#include "SPI.h"

//-------------------------Constantes
#define _XTAL_FREQ 8000000
#define _t0_valor 250
/*
 * ----------------------------- Variables ----------------------------------
 */
unsigned char b1; //bandera botón incrementar
unsigned char b2; //bandera boton decrementar
unsigned char bandera; //bandera para la interrupcion del timer0
uint8_t cont; //valor del puerto b



/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup(void); //configuraciones

/*
 * ------------------------------Interupciones----------------------------
 */
void __interrupt()isr(void)
{
    if(RBIF == 1){//cambio de estado
        if (PORTBbits.RB0 == 0){ //botón incrementar
            b1++; //banderas para que no se incremente siempre, solo con un botonazo
        }
        else if(PORTBbits.RB1 == 0){ //botón decrementar
            b2++;
        }
        RBIF = 0;
    }  
    if (SSPIF == 1){
        spiWrite(cont); //Pasa el dato al maestro
        SSPIF = 0; //limpia la bandera
    }
        
    
    
}

/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */
void main (void)
{

    setup(); //configuraciones iniciales
    while(1)
    {
        if(b1 == 1){ //Si la bandera esyta encendida incrementar el contador y mostrarlo en D
            cont++;
            b1 = 0;
            PORTD = cont;
        }
        else if (b2 == 1){//Si la bandera esyta encendida decrementar el contador y mostrarlo en D
            cont--;
            b2 = 0;
            PORTD = cont;
        }
        
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
    TRISAbits.TRISA5 = 1;

    TRISB = 0b0011; //Puerto B0 y 1  como entradas
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b0011; 
    
    //Limpiar todos los puertos
    PORTA = 0; 
    PORTB = 0; 
    PORTC = 0; 
    PORTD = 0; 
    PORTE = 0; 
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0111; // 8MHz
    OSCCONbits.SCS = 1;
    
    //Config. interrupcion RB
    IOCBbits.IOCB0 = 1; // pines en lo que habra interrupcion por cambio 
    IOCBbits.IOCB1 = 1; //de estado
    IOCBbits.IOCB2 = 1;
    RBIF = 0; //BANDERA 
    
    INTCONbits.GIE = 1; //habilitar interrupciones
    INTCONbits.RBIE = 1;    //activar interrupciones por cambio de estado
    INTCONbits.RBIF = 0;    //bajar la bandera
    

    INTCONbits.PEIE = 1;
   
    
    // ------- SPI------
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE = 1;
    TRISAbits.TRISA5 = 1;
    spiInit(SPI_SLAVE_SS_EN,SPI_DATA_SAMPLE_MIDDLE,SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    return; 

}

