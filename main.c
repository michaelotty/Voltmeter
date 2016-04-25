#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

//#include <stdio.h>
#include <xc.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000

#define HOLD    RA4 //Hold button
#define MAX     RB2 //Max button
#define CS1     RA3 //Chip select for ADC 1
#define CS2     RB3 //Chip select for ADC 2

//unsigned int StoredVoltage1 = 0; //Stored Voltage 1
//unsigned int StoredVoltage2 = 0; //Sored Voltage 2
unsigned int StoredVoltage1;
unsigned int StoredVoltage2;
unsigned int Max = 0; 
unsigned int voltage;
int hasTimerStarted = 0;

int counter = 0; //Counter for the timer

/*********************************Timer Interrupt******************************/
//When the TMR0 register overflows (goes over 255), the interrupt is triggered
void interrupt isr()
{
    extern int counter;
    if(T0IF)
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
    Lcd_Clear();
    Lcd_Write_String("Welcome");
    __delay_ms(1000);
    Lcd_Clear();
    Lcd_Write_String("Range is");
    Lcd_Set_Cursor(2, 1); //Set cursor to second half of LCD
    Lcd_Write_String(" 0 to 5V");
    __delay_ms(1100);
}


/******Converts the data from the ADC and displays on the LCD as a float******/
void CalculateAndWrite(unsigned int rawValue) 
{
    extern unsigned int voltage;
    voltage = (rawValue * 5) / 1020; //Calculate integer part of the voltage
    unsigned int decimal = (rawValue * 5) % 1020; //Calculate decimal part
    Lcd_Write_Int(voltage); //Write integer
    Lcd_Write_Char('.'); //Write decimal point
    if (decimal > 99) //If statements to prevent LCD from displaying 
        //0.00x as 0.x
    {
        if (decimal == 1000) 
        {
            Lcd_Write_String("000");
        } 
        else 
        {
            Lcd_Write_Int(decimal);
        }
    }
    else if (decimal > 9)
    {
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    } else 
    {
        Lcd_Write_Char('0');
        Lcd_Write_Char('0');
        Lcd_Write_Int(decimal);
    }
}

/************************************Buzzer************************************/
void Buzzer(unsigned int threshold) {
    if (threshold < 2) //If readADC() falls under 500 (about 2.5V)
        //                    //the buzzer will sound
    {
                            if(!hasTimerStarted) //Checks if the timer has been started
                                //before. 
                            {
                                TMR0 = 0; //Reset timer
                                counter = 0;
                                OPTION_REGbits.T0CS = 0; //Set TMR0 to increase once
                                //every instruction
                                INTCONbits.T0IE = 1; //Enable timer overflow interrupt
                                OPTION_REGbits.INTEDG = 0; //Falling edge trigger
                                 INTCONbits. GIE = 1; //Enable global interrupt
                                hasTimerStarted = !hasTimerStarted;
                            }
        for (int i = 0; i < 20; i++) {
            PORTAbits.RA0 = 0; //Toggle RA0 on
            __delay_ms(1);
            PORTAbits.RA0 = 1; //Toggle RA0 off
            __delay_ms(1);
        }                         
           INTCONbits.T0IE = 0;              
    }
}

void elapsedTime()
{
    Lcd_Clear();
    
    Lcd_Write_Int(counter);
}
/******************************Button debounce*********************************/
void debounce() 
{
    while (HOLD) 
    {
        if(MAX)
        {
            elapsedTime();
        }     
        __delay_us(10); //Waits until user removes finger from the button
    }
    while (MAX) 
    {
        __delay_us(10);
    }
}


/******************Stores the maximum value of readADC() so far****************/
int Maximum(int Value) 
{
    extern unsigned int Max;
    if (Value > Max) //If the current value is greater than the old value
    {
        Max = Value; //Replace the old value with the new one
    }
    return Max; //Return with the value of 'Max'
}


/*******************************MAIN FUNCTION**********************************/
void main() 
{   
    extern unsigned int voltage;
    extern unsigned int Max;
    extern int counter;
    TRISA = 0b00010100; //Set up RA2 & RA4 as inputs 
    TRISB = 0b00000100; //Set up RB3 as input

    CLK = 0;
    init(); //Initialise
    while (1) 
    {
        Lcd_Clear(); 
        Lcd_Set_Cursor(1, 1); //Write to first half of LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
        
        CS1 = 0; //Turn on chip select for ADC1
        CalculateAndWrite(readADC()); //Convert value of readADC to float and display
        //on LCD
        Max = Maximum(readADC()); //hasTimerStarted if this is maximum value so far
        Buzzer(voltage);
        CS1 = 1;

        Lcd_Set_Cursor(2, 1); //Write to second half if LCD
        Lcd_Write_Char('V');
        Lcd_Write_Char(':');
        
        CS2 = 0; //Turn on Chip Select for ADC2
        CalculateAndWrite(readADC()); //Read voltage from the second ADC
        Max = Maximum(readADC());
        Buzzer(voltage);
        CS2 = 1;
        __delay_ms(50);

        


        

/*******************************Hold and Store*********************************/
        if (HOLD) //If the HOLD button is pushed
        {
            debounce(); //Wait till user stops pushing the button
            while (1) //Pause so that current value stays on the display
            {
                if (MAX) //If the MAX button is pushed afterwards,stores 
                    //the current value
                {
                    debounce();
                    CS1 = 0;
                    
                    StoredVoltage1 = readADC(); //Save the value
                    CS1 = 1;
//
                    CS2 = 0;
                    StoredVoltage2 = readADC();
                    CS2 = 1;
                    Lcd_Clear();
                    Lcd_Write_String("Saved!");
                    
                   // EEPROM_WRITE(0x05, V1); //Write to memory
                   // EEPROM_WRITE(0x06, V2);
                    __delay_ms(1000);
                }
                if (HOLD) //If the HOLD button is pushed again 
                {
                    debounce(); //Wait till user stops pushing the button
                    break; //Break out of while loop
                }
            }
        }

/********************************Max Button************************************/
        if (MAX) //If the MAX button is pushed
        {
            debounce();
            while (1) 
            {
                Lcd_Clear();
                CalculateAndWrite(Max); //Display the value of max on the LCD
                if (HOLD) //If the HOLD button is pushed 
                {
                    debounce();
                    //  unsigned char address = 0xE5;
                    Lcd_Clear();
                    Lcd_Write_String("loading");
                    __delay_ms(500);
                   // V1 = EEPROM_READ(0x05); // Read from memory
                   // V2 = EEPROM_READ(0x06);
                    Lcd_Clear();
                    Lcd_Set_Cursor(1,1);
                    CalculateAndWrite(StoredVoltage1); //Write the stored value
                    //on the LCD, 
                    Lcd_Clear();
                    Lcd_Write_Int(2);
                    Lcd_Set_Cursor(2, 1);
                    CalculateAndWrite(StoredVoltage2);
                }

                if (MAX) //If the MAX button is pushed again
                {
                    debounce();
                    break; //Break out of while loop
                }
            }
        }
//        if (MAX && HOLD)
//        {
//            debounce();
//            while(1)
//            {
//                Lcd_Clear();
//                Lcd_Write_Int(counter);
//            }
//        }
    }
}
//Counter increases every 256 uS, so every second counter increases by 3906s
//( 1/(256 x10^-6) )
//so Time(s) = counter / 3906
