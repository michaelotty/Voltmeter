#pragma config  FOSC = XT
#pragma config  WDTE = OFF
#pragma config  PWRTE = ON
#pragma config  CP = OFF

#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "lcd.h"

#define _XTAL_FREQ  4000000

void main()
{
    unsigned int voltage = readADC();
    
}