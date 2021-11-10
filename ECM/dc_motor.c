//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan
 
#include <xc.h>
#include "dc_motor.h"
#include <string.h>

void Motor_Init(void) {

    //Set all pins to outp ut
    TRISB = 0;
    LATB = 0;

    // code to set up the PWM module
    PTCON0 = 0b00000000; // free running mode, 1:1 prescaler for max res beyond 100 lvls
    PTCON1 = 0b10000000; // enable PWM timer

    PWMCON0 = 0b01101111; // PWM0/1 & PMW3 pins enabled, all independent mode
    PWMCON1 = 0; // special features, all 0 (default)

    //199 Gives the a vlue which is large enough to split into 100 lvls
    PTPERL = 199; // Stores 8 LSBs discards MSBs
    PTPERH = 199 >> 8; // shifts to store 4 MSBs    This gives 0 anyways 

    PDC0L = 0;
    PDC0H = 0;
    PDC1L = 0;
    PDC1H = 0; //Set all PWM ON Time to 0 to ensure starting at 0 

}


// function to set PWM output from the values in the motor structure

void setMotorPWM(struct DC_motor *m) {
    int PWMduty; //tmp variable to store PWM duty cycle

    PWMduty = ((m->power)*(m->PWMperiod)) / 100; //calculate duty cycle (value between 0 and PWMperiod)

    if (m->direction) //if forward direction
    {
        LATB = LATB | (1 << (m->dir_pin)); //set dir_pin high in LATB
        PWMduty = (m->PWMperiod) - PWMduty; //need to invert duty cycle as direction is high (100% power is a duty cycle of 0)
    } else //if reverse direction
    {
        LATB = LATB & (~(1 << (m->dir_pin))); //set dir_pin low in LATB
    }

    //write duty cycle value to appropriate registers
    *(m->dutyLowByte) = PWMduty << 2;
    *(m->dutyHighByte) = PWMduty >> 6;

}


//------------DUAL MOTOR CONTROL ------------------------------

//function to stop the robot (both motors) gradually 

void stop(struct DC_motor *mL, struct DC_motor *mR) {

    for (mR->power; (mR->power) > 0; mR->power--) { //decrease until 0

       if (mR->power < mL->power) {
            mL->power = mR->power; //Match right to left motor only once it has come come down to its level
        }
        setMotorPWM(mL);
        setMotorPWM(mR); //Set both to the speed 
        __delay_ms(1); //delay of 1 ms (100 ms from 0 to 100 full power)
    }
}


//--------------------------------CALIBRATE THESE-------------------------------

//--------------------------Functions for Left/Right turns----------------------
/* 
 To slow down or speed up turning, adjust the power limit. 
 Don't forget to adjust both at the same time.
 */

void turnLeft(struct DC_motor *mL, struct DC_motor *mR) {
    mL->direction = 0; //Sets direction so right goes fwd and left back resulting in left turn
    mR->direction = 1;

    for (mR->power; (mR->power) <= 40; mR->power++) {       //Adjust the limit here
        mL->power = mR->power;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(1); 
    }


}

//function to make the robot turn right 

void turnRight(struct DC_motor *mL, struct DC_motor *mR) {
    mL->direction = 1; //Sets direction so right goes back and left fwd resulting in right turn
    mR->direction = 0;

    for (mR->power; (mR->power) <= 40; mR->power++) {   //Adjust Limit here 
        mL->power = mR->power;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(1); 
    }

}

//--------------Functions to go straight ahead/backwards------------------------
/* 
 The left motor is capped at 95 to calibrate and give a straight path. 
 Don't forget to adjust both when doing so. 
 */

void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR) {
    mL->direction = 1; // Both go Forward 
    mR->direction = 1;

    for (mR->power; (mR->power) <= 100; mR->power++) { //increase motor power until 100

        if (mR->power <= 95) {                          // left capped at 95 for calibrating wheels  
            mL->power = mR->power;
        }
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(1); 
    }
}

void Backwards(struct DC_motor *mL, struct DC_motor *mR) {
    mL->direction = 0; // Both go backwards 
    mR->direction = 0;

    for (mR->power; (mR->power) <= 100; mR->power++) { //increase motor power until 100

        if (mR->power <= 95) {                          // left capped at 95 for calibrating wheels 
            mL->power = mR->power;
        }
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_ms(1); //delay of 1 ms (100 ms from 0 to 100 full power)
    }
}

void Return_Sequence(char moves [], char move_times [], struct DC_motor *mL, struct DC_motor *mR) {
    char execute;
    for (int i = strlen(moves); i >= 0; i--) { //Execute stored moves in reverse
        execute = moves[i];

        if (execute == 'b'){
            Backwards(mL, mR);
        } else if (execute == 'l') {
            turnLeft(mL, mR);
        } else if (execute == 'r') {
            turnRight(mL, mR);
        }

        for (int j = move_times [i]; j >= 0; j--) { //loop to delay move for the recorded time 
            __delay_ms(50);  //100ms delay can be found in IR_Read Function
            __delay_ms(50);
        }
        stop(mL, mR);
    }
    Backwards(mL, mR);      //This is the reverse of the closing in 
    delay_s(5);
    stop(mL, mR);
}


//gives delays in seconds

void delay_s(char seconds) {
    for (int i = 0; i < (seconds * 1000); i++) {
        __delay_ms(1);
    }
}