#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000

void main()
{
	TRISA = 0b00000100;
	TRISB = 0x00;

	CS = 1;
	CLK = 0;
	RW = 0;
	Lcd_Init();
	Lcd_Clear();

	unsigned int Voltage = 5;
	while (1)
	{
		Lcd_Clear();

		Lcd_Set_Cursor(1, 1);
		char *string = "Voltage:";

		Lcd_Write_String(string);
		Lcd_Set_Cursor(2, 1);

		Voltage = (readADC() * 4.5) / 1020;
		Lcd_Write_Int(Voltage / 10000); //not sure what maximum number of chars is
		/*Do we need to maybe set the cursor?*/ Lcd_Set_Cursor(2, 2);
		Lcd_Write_Char(".");
		Lcd_Set_Cursor(2, 3);
		Lcd_Write_Int(Voltage % 10000); 
		__delay_ms(200);
	}
}
