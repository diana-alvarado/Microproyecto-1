/*
 * File:   slaveadc.c
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
#pragma config LVP = OFF       // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 8000000
#include "ADC.h" //Incluir librerias
#include "SPI.h"



/*
 Variables
 */
char ADC;
/*
 *Prototipos de funciones
 */
void setup(void);

/*
 * Interrupci�n
 */
void __interrupt() isr (void)
{ 
    
    if (PIR1bits.ADIF)   // Interrupcion ADC
    {  
            ADC = ADRESH; //La se�al del pot colocar en una varibale
            PIR1bits.ADIF = 0;//Limpiar bandera

    }
    if (SSPIF == 1){
        spiWrite(ADC); //Mandar la variable al master
        SSPIF = 0; //limpiar la bandera
    }   
}


/*
 Main
 */
void main(void)
{
    setup(); //Configuraciones iniciales
    ADC_setup(); //Configuraciones del ADC
    ADCON0bits.GO = 1; //Empezar a transformar
    
    while(1)
    {
        ADC_channel(); //Siempre recibir los datos
        PORTB = ADC; //Verificar si el ADC funciona en un puerto
    }
   
} 

void setup(void)
{
    //Configuraci�n entradas y salidas
    ANSEL = 0b00000001;
    ANSELH = 0;

    TRISA = 0b11111111;
    TRISB = 0;
    PORTB = 0;
    TRISAbits.TRISA5 = 1;

    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0111; // 8MHz
    OSCCONbits.SCS = 1;
        
    //configuracion de las interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    // ------- SPI------
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE = 1;
    TRISAbits.TRISA5 = 1;
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);

    return; 
}