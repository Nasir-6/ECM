//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan

#ifndef LCD_H
#define	LCD_H

#define _XTAL_FREQ 8000000 //i.e. for an 8MHz clock frequency
#include <stdio.h>
#include <xc.h>
#include <string.h>
#pragma config OSC = IRCIO, WDTEN = OFF     // internal oscillator 


// The #define allows you to make it easier to read
#define LCD_ENABLE LATCbits.LATC0           //LCD Enable Pin 
#define LCD_RS LATAbits.LATA6               //LCD Register Select Pin (1:Data 0:Command)

//ADDED THIS IN CHECK IF IT STILL WORKS
#define RFID_INPUT TRISCbits.TRISC7         //RFID signal pin
#define BTN_INPUT TRISCbits.TRISC3          //Button input pin

//-----------------------------------FUNCTION PROTOTYPES-------------------------
void E_TOG(void);
void LCDout(unsigned char number);
void SendLCD(unsigned char Byte, char type);
void LCD_Init(void);
void SetLine(char line);
void LCD_String(char *string);

#endif	/* LCD_H */

