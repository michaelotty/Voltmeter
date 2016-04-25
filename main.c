#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000

#define HOLD_BUTTON     RB2 //Hold button
#define MAX_BUTTON      RA4 //Max button
#define CS1             RA3 //Chip select for ADC 1
#define CS2             RB3 //Chip select for ADC 2

unsigned int StoredVoltage1;
unsigned int StoredVoltage2;
unsigned int Max = 0;
unsigned int integer;
int hasTimerStarted = 0; //Checks if the timer has started
int counter = 0; //Counter for the timer

/*********************************Timer Interrupt******************************/
//When the TMR0 register overflows (goes over 255), the interrupt is triggered

void interrupt isr() 
{
    extern int counter;
    if (T0IF) 
    {
        counter++; //Add 1 to counter which can be used to calculate number of 
        //seconds that have passed
        T0IF = 0;
    }
}

/**************Initialise the LCD and write the welcome message****************/
void init() 
{
    Lcd_Init();
    Lcd_Clear(); //Clear the LCD
    Lcd_Write_String("Welcome");
    __delay_ms(1000);
    Lcd_Clear();
    Lcd_Write_String("Range is");
    Lcd_Set_Cursor(2, 1); //Set cursor to second half of LCD
    Lcd_Write_String(" 0 to 5V");
    __delay_ms(1200);
}

/******Converts the data from the ADC and displays on the LCD as a float******/
void CalculateAndWrite(unsigned int rawValue) 
{
    extern unsigned int integer;
    integer = (rawValue * 5) / 1020; //Calculate integer part of the voltage
    unsigned int decimal = (rawValue * 5) % 1020; //Calculate decimal part
    Lcd_Write_Int(integer); //Write integer
    Lcd_Write_Char('.'); //Write decimal point
    if (decimal > 99) //If statements to prevent LCD from displaying 
        //0.00x as 0.x
    {
        if (decimal == 1000) {
            Lcd_Write_String("000");
        }
        else {
            Lcd_Write_Int(decimal);
        }
    } else if (decimal > 9) {
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    } else {
        Lcd_Write_Char('0');
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    }
}

/******************Stores the maximum value of readADC() so far****************/
int Maximum(int currentValue) 
{
    extern unsigned int Max;
    if (currentValue > Max) //If the current value is greater than the old value
    {
        Max= currentValue; //Replace the old value with the new one
    }
    return Max; //Return with the value of 'Max'
}

/************************************Buzzer************************************/
void Buzzer(unsigned int threshold) 
{
    if (threshold < 2) //If the voltage falls below 2.000, the buzzer will sound
        //and the timer will start
    {
        if (!hasTimerStarted) //Checks if the timer has been started
            //before. 
        {
            TMR0 = 0; //Reset timer
            counter = 0; //Reset counter
            OPTION_REGbits.T0CS = 0; //Set TMR0 to increase once
            //every instruction
            INTCONbits.T0IE = 1; //Enable timer overflow interrupt
            OPTION_REGbits.INTEDG = 0; //Falling edge trigger (i.e 255 --> 0)
            INTCONbits. GIE = 1; //Enable global interrupt
            hasTimerStarted = !hasTimerStarted; //Timer will not reset again
        }
        for (int i = 0; i < 20; i++) //Sound the buzzer
        {
            PORTAbits.RA0 = 0; //Toggle RA0 on
            __delay_ms(1);
            PORTAbits.RA0 = 1; //Toggle RA0 off
            __delay_ms(1);
        }
        INTCONbits.T0IE = 0; //Disable the timer interrupt
    }
}

/**************************Display the elapsed count***************************/
void elapsedTime() 
{
    Lcd_Clear();
    Lcd_Write_String("Count:");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Int(counter); //Writte the count to the LCD
}
//Counter increases every 256 uS, so every second counter increases by 3906s
//( 1/(256 x10^-6) )
//so Time(s) = counter / 3906

/******************************Button debounce*********************************/
void Debounce() 
{
    while (HOLD_BUTTON) 
    {
        if (MAX_BUTTON) //If the user holds down the MAX button while holding 
            //down the HOLD button, the elapsed count will be displayed
        {
            elapsedTime(); 
        }
        __delay_us(10); //Waits until user removes finger from the button
    }
    while (MAX_BUTTON) 
    {
        __delay_us(10);
    }
}

/*******************************MAIN FUNCTION**********************************/
void main() 
{
    extern unsigned int integer;
    extern unsigned int Max;
    extern int counter;
    
    TRISA = 0b00010100; //Set up RA2 & RA4 as inputs 
    TRISB = 0b00000100; //Set up RB3 as input

    CLK = 0;
    init(); //Initialise
    
/*********************************Main Loop************************************/    
    while (1) 
    {
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1); //Write to first half of LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');

        CS1 = 0; //Turn on chip select for ADC1
        CalculateAndWrite(readADC()); //Convert value of readADC to float and display
        //on LCD
        Max = Maximum(readADC()); //check if this is maximum value so far
        Buzzer(integer); //Check if voltage is less than 2 
        CS1 = 1;

        Lcd_Set_Cursor(2, 1); //Write to second half if LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');

        CS2 = 0; //Turn on Chip Select for ADC2
        CalculateAndWrite(readADC()); //Read integer from the second ADC
        Max = Maximum(readADC());
        Buzzer(integer);
        CS2 = 1;
        __delay_ms(50);

/*******************************Hold and Store*********************************/
        if (HOLD_BUTTON) //If the HOLD button is pushed
        {
            Debounce(); //Wait till user stops pushing the button
            while (1) //Pause so that current value stays on the display
            {
                if (MAX_BUTTON) //If the MAX button is pushed afterwards,stores 
                    //the current value
                {
                    Debounce();
                    CS1 = 0;
                    StoredVoltage1 = readADC(); //Save the value
                    CS1 = 1;
                    
                    CS2 = 0;
                    StoredVoltage2 = readADC(); //Save value from 2nd ADC
                    CS2 = 1;
                    Lcd_Clear();
                    Lcd_Write_String("Saved!");
                    __delay_ms(1000);
                }
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
            while (1) {
                Lcd_Clear();
                CalculateAndWrite(Max); //Display the value of max on the LCD
                if (HOLD_BUTTON) //If the HOLD button is pushed 
                {
                    Debounce();
                    Lcd_Clear();
                    Lcd_Write_String("loading");
                    __delay_ms(500);
                    Lcd_Clear();
                    CalculateAndWrite(StoredVoltage1); //Write the stored value
                    //on the LCD, 
                    Lcd_Set_Cursor(2, 1);
                    CalculateAndWrite(StoredVoltage2);
                    __delay_ms(1000);
                }

                if (MAX_BUTTON) //If the MAX button is pushed again
                {
                    Debounce();
                    break; //Break out of while loop
                }
            }
        }
    }
}

