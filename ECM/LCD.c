//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan

#define _XTAL_FREQ 8000000 //i.e. for an 8MHz clock frequency
#include <stdio.h>
#include <xc.h>
#include <string.h>
#include "LCD.h"
#pragma config OSC = IRCIO, WDTEN = OFF     // internal oscillator 

//----------Function Definitions----------------

//function to toggle enable bit on then off
void E_TOG(void) {

    LCD_ENABLE = 1; //Enable bit 
    __delay_us(5); 
    LCD_ENABLE = 0;

}


//function to send four bits to the LCD
void LCDout(unsigned char number) {
    //set data pins using the four bits from number

    LATCbits.LATC1 = number & 0b00000001;
    LATCbits.LATC2 = (number & 0b00000010) >> 1;

    //Collect Low bits & shift for C1 & C2

    LATDbits.LATD0 = (number & 0b00000100) >> 2;
    LATDbits.LATD1 = (number & 0b00001000) >> 3;

    //Collect High bits & Shift to turn on D0 & D1 

    E_TOG();        //toggle the enable bit to send data
    __delay_us(5); 

}


//function to send data/commands over a 4bit interface
void SendLCD(unsigned char Byte, char type) {
    // set RS pin to the type: Command (0) or Data/Char (1)
    LCD_RS = type;

    // send high bits of Byte using LCDout function
    LCDout(Byte >> 4);
    __delay_us(10); 
    
    // send low bits of Byte using LCDout function
    LCDout(Byte & 0b00001111);

}


//LCD Initialisation sequence
void LCD_Init(void) {

    //Setup all pins as outputs for LCD etc. apart from RC3(button) and RC7 (RFID) 
    TRISD = 0;
    //TRISC = 0b10001000;   Check if below works before finalising 
    TRISC = 0;
    BTN_INPUT = 1;
    RFID_INPUT = 1;
    TRISAbits.TRISA6 = 0; //A6 as output

    // set initial LAT output values (they start up in a random state)
    LATC = 0;
    LATD = 0;
    LCD_ENABLE = 0;
    LCD_RS = 0;


    // Initialisation sequence code - see the data sheet
    
    __delay_ms(15);
    LCDout(0b0011);
    __delay_ms(5);  //>4.1 ms from data sheet operation time 

    LCDout(0b0011);
    __delay_us(200);

    LCDout(0b0011);
    __delay_us(50);

    //send 0b0010 using LCDout set to four bit mode
    LCDout(0b0010);
    __delay_us(50);

    // now use SendLCD to send whole bytes 
    //send function set, 
    SendLCD(0b0000101100, 0); //Function Set
    __delay_us(50); // >42 us
    //clear screen, 
    SendLCD(0b0000001000, 0); //Display off
    __delay_us(50); // >42 us

    SendLCD(0b0000000001, 0); //Display Clear 
    __delay_ms(2); // >1.64 ms

    SendLCD(0b0000000110, 0); //Set Entry mode
    __delay_us(50); // >42 us

    //display on etc to finish initialisation
    SendLCD(0b0000001111, 0); // Change to SendLCD(0b00001100,0); to turn off cursor & blinking
    __delay_us(50); // >42 us
}


//function to put cursor to start of line
void SetLine(char line) {

    if (line == 1) {
        //Send 0x80 to set line to 1 (0x00 ddram address)
        SendLCD(0x80, 0);
    } else if (line == 2) {
        //Send 0xC0 to set line to 2 (0x40 ddram address)
        SendLCD(0xC0, 0);
    }
    __delay_us(50); // 50us delay   
}


//Function to print strings
void LCD_String(char *string) {

    while (*string != 0) { //While the data pointed to isn't Null (0)

        SendLCD(*string++, 1); //++ sends out then increments
    }
}

