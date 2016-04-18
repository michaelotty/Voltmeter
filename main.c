#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
char* floatToString()
{
    char *str = newchar[10];
    float floatingVoltage 
}

void main()
{
    TRISA = 0b00000100;
    TRISB = 0x00;    
    
    CS = 1;
    CLK = 0;
    RW = 0;    
    Lcd_Init();
    Lcd_Clear();
    
    unsigned int voltage = 5;
    float floatingVoltage = 0;
    while(1)
    {
        Lcd_Clear();
        
        Lcd_Set_Cursor(1,1);
        char *string = "Voltage:";
        
        Lcd_Write_String(string);
        Lcd_Set_Cursor(2,1);
        
        floatingVoltage = readADC() * 4.5;
        char *str = newchar[10];
        sprintf(str, "%.4g", floatingVoltage);
        Lcd_Write_String(floatingVoltage);
        __delay_ms(200);
    }
}