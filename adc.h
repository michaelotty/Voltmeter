// ADC Functions for the MCP3001
// B.W.Metcalfe 2016

// Define the oscillator frequency, needed for delays
#define _XTAL_FREQ  4000000

// Define the pin connections
#define CLK RA1
#define DIN0 RA2
#define DIN1 RB3
#define CS RA3

// Read a single (8-bit) sample from the ADC
unsigned int readADC(int selectedADC);
