#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

//#include <stdio.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
#define BUTTON RA4

void WriteVoltage(unsigned int vTemp)
{
    unsigned int voltage;
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

void debounce()
{
    while(BUTTON)
            {
                __delay_us(10);
            }          
}

void init()
{
    Lcd_Init();
	Lcd_Clear();
    
   Lcd_Write_String("Welcome! The range is 0 to 5V");
   Lcd_Set_Cursor(2,1);
   Lcd_Write_String(" The range is 0 to 5V");
}

void ScrollText()
{
     short int i =0;
     while(i < 30)
   {
       __delay_ms(250);
       Lcd_Shift_Left();
       i++;
   }
}



void main()
{
	TRISA = 0b00010100;
	TRISB = 0b00001100;
    


	CS = 1;
	CLK = 0;

   init();
   ScrollText();
  
	while (1)
	{      
        //__delay_ms(1000);
#define DIN RA2
        Lcd_Clear();
		Lcd_Set_Cursor(1,1);
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
       // DIN = PORTAbits.RA2;
        WriteVoltage(readADC());
#undef DIN
        Lcd_Set_Cursor(2,1);
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
        //DIN = PORTBbits.RB3;
#define DIN RB3
        WriteVoltage(readADC());
#undef DIN
        __delay_ms(50);
        if(BUTTON)
        {
            debounce();
            while(1)
            {
                if(BUTTON)
                {
                    debounce();
                    break;
                }
            }
        }
        
	}
}
