#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

//#include <stdio.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
#define BUTTON PORTAbits.RA4

void main()
{
	TRISA = 0b00010100;
	TRISB = 0x00;

	CS = 1;
	CLK = 0;
	RW = 0;
	Lcd_Init();
	Lcd_Clear();

	unsigned int voltage = 5;
	while (1)
	{
	//	Lcd_Clear();                                     
        
		Lcd_Set_Cursor(1, 1);
		char *string = "Voltage:";
                   
		Lcd_Write_String(string);
		Lcd_Set_Cursor(2, 1);

		voltage = (readADC() * 5) / 1020;

        //Lcd_Write_Int(voltage);
                                                                                                                                                                                                                                                                                                                                                               
        int decimal;
        decimal = ((readADC() * 5) % 1020);
        //ones = (voltage / 10000);
        Lcd_Write_Int(voltage);
        Lcd_Write_Char('.');
        if (decimal < 10)
        {
            Lcd_Write_Int(0);
            Lcd_Write_Int(0);
            Lcd_Write_Int(decimal);
        }
        else if (decimal < 100)
        {
            Lcd_Write_Int(0);
            Lcd_Write_Int(decimal);
        }
        else
        {
            Lcd_Write_Int(decimal);
        }
        
        if(BUTTON)
        {
            //int run = 1;               
                while (BUTTON) 
                {
                    __delay_ms(50);
                }
                while (BUTTON == 0) 
                {
                       __delay_ms(50);
                }
                while (BUTTON) 
                {
                       __delay_ms(50);
                }
                while(BUTTON == 0)
                {
                       __delay_ms(50);
                }
                
        }
	}
}
