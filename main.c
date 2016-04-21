#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
#define BUTTON RA4

void WriteVoltage(unsigned int vTemp) {
    unsigned int voltage = 5;
    voltage = (vTemp * 5) / 1020;
    unsigned int decimal = (vTemp * 5) % 1020;
    Lcd_Write_Int(voltage);
    Lcd_Write_Char('.');
    if (decimal < 10) {
        Lcd_Write_Char('0');
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    } else if (decimal < 100) {
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    } else {
        Lcd_Write_Int(decimal);
    }
}

void debounce() {
    while (BUTTON) {
        __delay_us(10);
    }
}

void ScrollText() {
    short int i = 0;
    while (i < 30) {
        __delay_ms(250);
        Lcd_Shift_Left();
        i++;
    }
}

void init() {
    Lcd_Init();
    Lcd_Clear();

    Lcd_Write_String("Welcome! The range is 0 to 5V");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String(" The range is 0 to 5V");
}

void main() {
    TRISA = 0b00010100;
    TRISB = 0b00001100;



    CS = 1;
    CLK = 0;

    init();
    ScrollText();

    while (1) {
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_Char('V');
        Lcd_Write_Char('1');
        Lcd_Write_Char(':');

        WriteVoltage(readADC(0));

        __delay_ms(20);
        if (BUTTON) {
            debounce();
            while (1) {
                if (BUTTON) {
                    debounce();
                    break;
                }
            }
        }

    }
}
