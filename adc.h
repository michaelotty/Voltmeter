// ADC Functions for the MCP3001
// B.W.Metcalfe 2016

// Define the oscillator frequency, needed for delays
#define _XTAL_FREQ  4000000

// Define the pin connections
#define CS RA2
#define CLK RA3
#define DIN RA4

// Read a single (8-bit) sample from the ADC
unsigned int readADC();