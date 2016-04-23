
#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

//#include <stdio.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000
#define HOLD_BUTTON RA4
#define MAX_BUTTON RB2

/**************Initialise the LCD and write the welcome message****************/
void init()
{
    Lcd_Init();
    Lcd_Clear();
    Lcd_Write_String("Welcome! The range is 0 to 5V");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String(" The range is 0 to 5V");
}

/******Converts the data from the ADC and displays on the LCD as a float******/
void WriteVoltage(unsigned int vTemp)
{
    unsigned int voltage = (vTemp * 5) / 1020; //Calculate integer part of the voltage
    unsigned int decimal = (vTemp * 5) % 1020; //Calculate decimal part
    
    Lcd_Write_Int(voltage); //Write integer
    Lcd_Write_Char('.'); //Write decimal point
    
    if (decimal < 10/*or maybe 100*/) //Add 0 in front of any int smaller than 
        //100 as if an int has any 0s in front of it, they will be ignored.
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

/******************************Button debounce*********************************/
void Debounce()
{
    while (HOLD_BUTTON) 
    {
        __delay_us(10); //Waits until user removes finger from the button
    }
}

/**************************Scrolls the welcome text****************************/
void ScrollText() 
{
    int i = 0;
    while (i < 30) //30 is the number of characters in the welcome message
    {
        __delay_ms(250);
        Lcd_Shift_Left(); //Shift the characters on the LCD one space to the left
        i++;
    }
}

/******************Stores the maximum value of readADC() so far****************/
int Maximum(int value)
{
    int max = 0;
    if (value > max) //If the current value is greater than the old value
    {
        max = value; //Replace the old value with the new one
    }
    return max; //Return with the value of 'Max'
}

/*******************************MAIN FUNCTION**********************************/
void main() {
    TRISA = 0b00010100; //Set up RA2 & RA4 as inputs 
    TRISB = 0b00001100; //Set up RB3 and RB7 as inputs

    CS = 1; 
    CLK = 0;

    init(); //Initialise
    ScrollText(); //Scroll the text
    

    while (1) 
    {
       // int sample = 0;
        Lcd_Clear(); //PERHAPS TURN OFF THE POWER?
        Lcd_Set_Cursor(1, 1); //Write to first half of LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
        WriteVoltage(readADC(0)); //Convert value of readADC to float and display
        //on LCD
        int max = Maximum(readADC(0)); //Check if this is maximum value so far
        

        Lcd_Set_Cursor(2, 1); //Write to second half if LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
        WriteVoltage(readADC(1)); //Read voltage from the second ADC
        int max = Maximum(readADC(1));
        __delay_ms(50);

        
        
/********************************Hold Button***********************************/        
        if (HOLD_BUTTON) //If the HOLD button is pushed
        {
            Debounce(); //Wait till user stops pushing the button
            while (1) //Pause so that current value stays on the display
            {
//                if (MAX_BUTTON) //If the MAX button is pushed, it stores the current
//                    //value
//                {
//                    Debounce();
//                    int sample;
//                    sample = readADC(0); //Save the value to 'sample'
//                    sample = readADC(1);         
//                    
//                }
                if (HOLD_BUTTON) //If the HOLD button is pushed again 
                {
                    Debounce(); //Wait till user stops pushing the button
                    break; //Break out of while loop
                }
            }
        }
        
/********************************Max Button************************************/
        if (MAX_BUTTON) //If the MAX button is pushed
        {
            Debounce();
            while (1) 
            {
                WriteVoltage(max); //Display the value of max on the LCD
//                if (HOLD_BUTTON) //If the HOLD button is pushed 
//                {
//                    Debounce();
//                    int sample;
//                    WriteVoltage(sample); //Write the stored value on the LCD, 
//                    //if there is no stored value, sample will be 0
//                  //  unsigned char address = 0xE5;
//                  //  sample = eeprom_read (address);    // Reading the value from address 0xE5
//                }

                if (MAX_BUTTON) //If the MAX button is pushed again
                {
                    Debounce();
                    break; //Break out of while loop
                }
            }
        }
/***********************************Buzzer*************************************/
//        while (readADC(0) < 500) //If readADC() falls under 500 (about 2.5V), the
//            //buzzer will sound
//        {
//            PORTAbits.RA0 = 1; //Toggle RA0 on
//            PORTAbits.RA0 = 0; //Toggle RA0 off
//        }
    }
}

