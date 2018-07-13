/*
Text Scrolling Code

The following code is used to run Tivaware microcontroller (TM4C123GXL), Shift registers(74HC5895), Counters(CD4022BE) and Dot Matrix.

The main aim of this code is to control the LED's in the dot matrix to output desired input.
This input may include data being transferred to the shift registers with the help of counters and later being 'lached' to produce the output on the dot matrix.

Input is given in form of an array which contain the letters to be produced at the output from our board.
*/




// header files.
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"


const char letterMSE[8][1] = { //Letter MSE is being covered by the top four LED's of our circuit

{ 0x00000000} ,
{ 0x044183C0} ,
{ 0X06C24200} ,
{ 0X05420200} ,
{ 0X044183C0} ,
{ 0X04404200} ,
{ 0X04424200} ,
{ 0X044183C0} ,
};
const char letterROCKS[8][1] = { //Letter ROCKS is being covered by the bottom four LED's of our circuit
{ 0x00000000} ,
{ 0x3CF3C88C} ,
{ 0X24924912} ,
{ 0X28920A10} ,
{ 0X30920C0C} ,
{ 0X28920A10} ,
{ 0X24624912} ,
{ 0X24F3C88C} ,
};

/*
The following function 'function'reads data from the array values[j][0] & (ands) it with a mask 0x01.
This masking gives us either a high (1) or a low (0). The data is then stored in the shift register.
Working of shift register is explained in the report.
the for loop 'i' gets the data for one row.
the for loop 'j' gets the data for the whole 8 rows.
once all the data is recieved the shift registers latches the information and the circuit starts working.
*/
void function ()
{

uint32_t data; // intitializing
uint32_t i;
uint32_t j;


GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); // RESET COUNTER
GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2); //NOT RESET COUNTER (setting high always)




for(j = 0; j<8; j ++){

for(i=0; i<32; i++)
{

uint32_t mask = 0x01; // assign mask
mask = mask << i;
data = mask & ~values[j][0]; // ASSIGN ARRAY VALUES

if(data > 0)
{
GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2 ); //SHIFT REGISTER SER HIGH

}
else
{
GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0); //SHIFT REGISTER SER LOW

}// end else

GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3); // SHIFT REGISTOR CLOCK HIGH

GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00); // SHIFT REGISTOR CLOCK LOW

GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0); //SHIFT REGISTOR SER LOW
}// end for



GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); //LATCH HIGH

GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //LATCH LOW

if(j!=0)
{
GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3); //COUNTER CLOCK

GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0X00); //CLOCK LOW COUNTER



}// end if

SysCtlDelay(1000); //adding a delay (count to 1000 before going to next line)

}//end for



}// end function "function"






/* the following function 'UpdateDisplay' is the function that creates the scrolling effect in our circuit.
Basically, we call this function in our main depending on how much we want to shift the data in our dot matrix.
The working of this function is explained briefly in the lab report.
*/


void UpdateDisplay(unsigned short call)
{

uint8_t shift1 = call; //initializing
uint8_t shift2 = 32 - call;
uint8_t count;
uint8_t count1;


uint32_t frame1[8][1] = { // empty frame1 with zeroes
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
};

uint32_t frame2[8][1] = { // empty frame2 with zeroes
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
{0x00},
};


for (count1 = 0; count1 < 8; count1 ++){


for(count=0; count<8; count++)
{

frame1[count][0] = letterMSE[count][0] << shift1; // stores data from this line into frame1[count][0]
frame2[count][0] = letterMSE[count][0] >> shift2; // stores data from this line into frame2[count][0]
values[count][0] = frame1[count][0] | frame2[count][0]; // stores the or '|' of frame1[count][0] and frame2[count][0] in values[count][0]


} // end for

} //end for

}// end UpdateDisplay





/* the main function includes initializing, enabling and writing the ports A,D and F, pins 1, 2 and 3.
the while loop in our main contains the clearing of data in pins 1,2,3 of ports F, data in pin 1 of port D, data in pin 3 of port A.
it also contains 2 for loops, for loop 'i' is used to update the update display function by increments of 1, creating our desired scrolling sensation.
the for loop 'j' is used to slow down the scrolling speed of the text on the dot matrices.
*/

int main(void)
{
uint8_t i; //initializing
uint8_t j;
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enabling port F
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //enabling port D
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //enabling port A
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //setting pins 1,2 and 3 of port F to be an output
GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //setting pins 1,2 and 3 of port D to be an output
GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //setting pins 1,2 and 3 of port A to be an output

while(1) // while 1 is true keep running (infinite loop)
{


GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00); //clearing any data from pins 1,2 and 3 of port F
GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00); //clearing any data from pin 1 of port D
GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0X00); //clearing any data from pin 3 of port A



for(i=0;i<32;i++){ // sends input to the updatedisplay function changing from 1 to 32

UpdateDisplay(i); // call update display

for(j=0;j<200;j++){ // slows the time the text takes to scroll down the dot matrix

function(); // call function

} // end for



}// end for




} // end while

} // end main





