//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan

#define _XTAL_FREQ 8000000 //i.e. for an 8MHz clock frequency
#include<stdio.h>
#include <xc.h>
#include <string.h>
#include "dc_motor.h"
#include "LCD.h"
#include "RFIDandIR.h"
#pragma config OSC = IRCIO, WDTEN = OFF // internal oscillator 

//-----------------------------EXTRA FUNCTION PROTOTYPES------------------------
char getCharSerial(void);
//--------------------------Global/volatile Variables---------------------------
volatile char RFID_Captured = 0;

//-------------------------------MAIN-------------------------------------------
void main(void) {

    //----------------------------------Initialisation--------------------------
    OSCCON = 0x72; //8MHz clock
    while (!OSCCONbits.IOFS); //wait until stable
    Motor_Init();
    LCD_Init();
    IR_Init();
    RFID_Interrupt_Init();

    //----------------------------------Motor Structure Setup-------------------
    struct DC_motor motorL, motorR;

    motorL.power = 0; //zero power to start 
    motorL.direction = 1; //set default motor direction 
    motorL.dutyLowByte = (unsigned char *) (&PDC0L); //store address of PWM1 duty low byte 
    motorL.dutyHighByte = (unsigned char *) (&PDC0H); //store address of PWM1 duty high byte 
    motorL.dir_pin = 0; //pin RB0/PWM0 controls direction 
    motorL.PWMperiod = 199; //Max value of PTPER/PWM FULL TIME

    motorR.power = 0; //zero power to start 
    motorR.direction = 1; //set default motor direction 
    motorR.dutyLowByte = (unsigned char *) (&PDC1L); //store address of PWM3 duty low byte 
    motorR.dutyHighByte = (unsigned char *) (&PDC1H); //store address of PWM3 duty high byte 
    motorR.dir_pin = 2; //pin RB2/PWM2 controls direction 
    motorR.PWMperiod = 199; //Max value of PTPER/PWM FULL TIME

    int pwm1 = 0,  pwm2 = 0;

    //----------------Loop to check the IR values before beginning--------------
    while (!PORTCbits.RC3) {            //Press Button when done
        IR_Read(&pwm1, &pwm2);
        IR_Disp(pwm1, pwm2);
    }
        
    //------------------Initialise Retrieving Sequence--------------------------
    CAP1BUFL = 0;
    CAP1BUFH = 0;
    CAP2BUFL = 0;
    CAP2BUFH = 0;       //Reset pwm values to 0

    //Move storage variables/arrays
    char moves [100] = {0};         //Stores the moves
    char move_times [100] = {0};    //Stores the time taken for each move 
    char array_cnt = 0;             //Used for indexing move_times[array_cnt] array starting at 0
    unsigned int time = 0;          //placeholder for recording time values
    
    //-------------------Begin Retrieving Sequence------------------------------
    IR_Read(&pwm1, &pwm2);
    
    //------------------Get close to target-------------------------------------
    while (pwm1 < 12000 & pwm2 < 12000) {
        IR_Read(&pwm1, &pwm2);
        turnRight(&motorL, &motorR);
    }
    stop(&motorL, &motorR);
    fullSpeedAhead(&motorL, &motorR);
    delay_s(5);


    //--------------Finish off with fine movement-------------------------------

    while (!RFID_Captured) {
        IR_Read(&pwm1, &pwm2);
        //-----------FOR DEBUGGING show values-------------
//        IR_Disp(pwm1, pwm2);
        //----------------------IR Locating-------------------------------------
        time = 0;
        
        if (pwm1 > 12000 & pwm2 > 12000) { //Target Straight ahead at 12000
            fullSpeedAhead(&motorL, &motorR);
            while ((pwm1 > 12000 & pwm2 > 12000) & !RFID_Captured) {    //While loop to continue movement whilst condition is still met
                IR_Read(&pwm1, &pwm2) ;  //Updates readings (THIS ALSO CONTAINS 100ms DELAY )
                time++;                  //Updates time 
            }
            strcat(moves, "b"); //Store move as backwards for reverse sequence 

        } else if (pwm1 > pwm2) {//Target to the right 
            turnRight(&motorL, &motorR);
            while ((pwm1 > pwm2)& !RFID_Captured) {
                IR_Read(&pwm1, &pwm2);
                time++;
            }
            strcat(moves, "l"); //Store move as left
          
        } else if (pwm2 > pwm1) { //Target to the left 
            turnLeft(&motorL, &motorR);
            while ((pwm2 > pwm1) & !RFID_Captured) {
                IR_Read(&pwm1, &pwm2);
                time++;
            }
            strcat(moves, "r"); //Store move as right        
        
        }  
        
        move_times[array_cnt] = time;
        stop(&motorL, &motorR);
        array_cnt++;
    }
    stop(&motorL, &motorR);

    //----------Code Retrieved Now Returning -----------------------------------
    Return_Sequence(moves, move_times, &motorL, &motorR);
    stop(&motorL, &motorR);
    //----------------MISSION COMPLETE------------------------------------------
    while (1);
}


// ---------------------ISR to collect RFID code--------------

volatile char __interrupt(high_priority) RFIDCapture(void) {
    
    char message = getCharSerial();
    if (message == 0x02) { // Only begin if STRT code is first
        //Clear display
        SendLCD(0b0000000001, 0);
        __delay_ms(2);
        SetLine(1);
        
        //Loop to print first 10 values ONLY i.e the tag number
        for (int j = 0; j < 15; j++) {
            if (j < 10) {
                message = getCharSerial();
                SendLCD(message, 1);
            } else {    //Just read don't print
                message = getCharSerial();
            }
        }
        SetLine(2);
        LCD_String("Code Retrieved!");
        RFID_Captured = 1;
    } else {
        //Not captured Properly so don't do anything
    }
    return RFID_Captured;
}

//------------------------EXTRA FUNCTION DEFINITIONS----------------------------

char getCharSerial(void) {
    while (!PIR1bits.RCIF); //wait for the data to arrive
    return RCREG; //return byte in RCREG
}



