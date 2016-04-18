#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000

void main()
{
    TRISA = 0xFF;
    TRISB = 0x00;    
    
    RW = 0;    
    Lcd_Init();
    Lcd_Clear();
    
    unsigned int voltage = readADC();
    Lcd_Set_Cursor(1,1);
    
    char *string = "Voltage: 4.111";
    Lcd_Write_String(string);
}