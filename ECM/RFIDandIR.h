//Mohammed Farhadul Islam Nasir & Salman Alfarisi Nasir Khan

#ifndef RFIDANDIR_H
#define	RFIDANDIR_H

#define _XTAL_FREQ 8000000 //i.e. for an 8MHz clock frequency
#include <stdio.h>
#include <xc.h>
#include <string.h>
#include "RFIDandIR.h"
#include "LCD.h"

void IR_Init(void);
void RFID_Interrupt_Init(void);
void IR_Read(int *pwm1, int *pwm2);
void IR_Disp (int pwm1,int pwm2);



#endif	/* RFIDANDIR_H */

