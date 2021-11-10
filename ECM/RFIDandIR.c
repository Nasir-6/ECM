//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan

#define _XTAL_FREQ 8000000 //i.e. for an 8MHz clock frequency
#include <stdio.h>
#include <xc.h>
//String included for sprintf 
#include <string.h>
#include "RFIDandIR.h"
//LCD included for IR_disp function and also Initialising the RFID & Button PIN 
#include "LCD.h"

//----------Function Definitions----------------


//Initialise IR Pins/module
void IR_Init(void) {
    
    //Setup IR pins RA2(pwm1) & RA3(pwm2) as digital inputs 
    LATA = 0;
    TRISA = 0b00001100; 
    ANSEL0 = 0;
    ANSEL1 = 0;

    //Enable TMR5 with 1;8 prescaler since 1:2 is too small
    T5CON = 0b00011001; 
    
    TMR5L = 0;
    TMR5H = 0;
    PR5L = 0b11111111;          
    PR5H = 0b11111111;
    
    //Enable timer, Set IR pins to change on fall to rise of PWM wave 
    CAP1CON = 0b01000110; 
    CAP2CON = 0b01000110; 
}

//Initialise RFID interrupts
void RFID_Interrupt_Init(void) {
    
    SPBRG = 205; //set baud rate to 9600 for RFID    207 is unstable 205 is stable
    SPBRGH = 0;
    BAUDCONbits.BRG16 = 1; //set baud rate scaling to 16 bit mode
    TXSTAbits.BRGH = 1; //high baud rate select bit
    RCSTAbits.CREN = 1; //continuous receive mode
    TXSTAbits.SYNC = 0; //Asynchronous 
    RCSTAbits.SPEN = 1; //enable serial port, other settings default

    PIE1bits.RCIE = 1; //Enable interrupts 

    INTCONbits.GIEH = 1; //Enable global interrupts
    INTCONbits.PEIE = 1; //Enable all Peripheral interrupts
}


void IR_Read(int *pwm1, int *pwm2) {

    *pwm1 = (CAP1BUFL | (int) CAP1BUFH << 8); //Right side sensor 
    *pwm2 = (CAP2BUFL | (int) CAP2BUFH << 8); //Left side sensor
     __delay_ms(50);    
     __delay_ms(50);    //Delay to allow wave to be received & also time moves
   
}

//Displays IR readings to allow user to check if everything is working 
void IR_Disp (int pwm1, int pwm2){
        
        char PWM[17];
        
        //Clear display
        SendLCD(0b0000000001, 0);
        __delay_ms(2);
        
        SetLine(1);        
        sprintf(PWM, "PWM1: %d", pwm1);
        LCD_String(PWM);
        
        SetLine(2);
        sprintf(PWM, "PWM2: %d", pwm2);
        LCD_String(PWM);

      
}


