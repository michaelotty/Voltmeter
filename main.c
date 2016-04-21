#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

//#include <stdio.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
#define BUTTON RB0

void WriteVoltage(unsigned int vTemp)
{
    unsigned int voltage = 5;
    voltage = (vTemp * 5) / 1020;
    unsigned int decimal = (vTemp * 5) % 1020;
    Lcd_Write_Int(voltage);
    Lcd_Write_Char('.');
    if (decimal < 10)
    {
        Lcd_Write_Char('0');
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    }
    else if (decimal < 100)
    {
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    }
    else
    {
       Lcd_Write_Int(decimal);
    }
}

//void interrupt isr()
//{
//    INTCONbits.INTF = 0;
//    __delay_ms(200);
//    int run = 1;
//    while(run)
//    {
//       run = !BUTTON;
//    }
//}

void main()
{
	TRISA = 0b00000100;
	TRISB = 0b00000001;
    


	CS = 1;
	CLK = 0;
	RW = 0;
	Lcd_Init();
	Lcd_Clear();
    
   Lcd_Write_String("Welcome! The range is 0V to 5V");
   
   short int i =0;
   while(i < 30)
   {
       __delay_ms(250);
       Lcd_Shift_Left();
       i++;
   }
	while (1)
	{                
        //__delay_ms(1000);
        Lcd_Clear();
		Lcd_Set_Cursor(1,1);
        Lcd_Write_Char('V');
        Lcd_Write_Char('1');
        Lcd_Write_Char(':');

        WriteVoltage(readADC());
        __delay_ms(20);
        if(BUTTON == 1)
        {
            while(1)
            {
                __delay_ms(100);
                if(BUTTON == 1)
                {
                    __delay_ms(100);
                    break;
                }
            }
        }
        
	}
}
