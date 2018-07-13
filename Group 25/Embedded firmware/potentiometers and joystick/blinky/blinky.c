/* The following code converts analog to digitial and sets a pulse width modulation for two different potentiometers. The potentiometers are controlled by a thumb joystick.
the vertical movement of the joystick controls one potentiometer and the horizontal movement controls the other. */

// header files
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"





#define PWM_FREQUENCY 55 // setting PWM frequency to 55

volatile uint8_t ui8Adjust=83; // intial position of potentiometer is 83
volatile uint8_t ui8Adjust2=83; // intial position of potentiometer is 83


/* the main function includes the core of the setting up a PWM and using it along with converting analog signals to digital signals */

int main(void) { // main starts

volatile uint32_t ui32Load; // creating global variables
volatile uint32_t ui32PWMClock; // creating global variables

volatile uint32_t ui32Load2; // creating global variables
volatile uint32_t ui32PWMClock2; // creating global variables



SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); //setting a clock


SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // setting a clock for PWM

SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // enable peripheral PWM1

SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // enable peripheral PWM0

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // enable peripheral GPIOD

SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // enable peripheral GPIOF



GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); // set PD0 as PWM pin

GPIOPinTypePWM(GPIO_PORTD_BASE,GPIO_PIN_1); // set PD1 as PWM pin



GPIOPinConfigure(GPIO_PD0_M1PWM0); // configure PD0 as MIPWM0

GPIOPinConfigure(GPIO_PD1_M0PWM7); // configure PD1 as MIPWM7


SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // enable peripheral GPIOA


GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 ); // set pin 0 of port A as GPIO UART



HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; //unlocking locked ports

HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;

HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;



GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN); // set directories



GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //the base address of the GPIO port is assigned.
// GPIO_PIN_0 is the bit-packed representation of the pin(s).
//GPIO_STRENGTH_2MA specifies the output drive strength.
//GPIO_PIN_TYPE_STD_WPU specifies the pin type.



ui32PWMClock = SysCtlClockGet() / 64; // giving our variable a clock

ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1; // setting another clock for our load variable



ui32PWMClock2 = SysCtlClockGet() / 64; // giving our variable a clock

ui32Load2 = (ui32PWMClock2 / PWM_FREQUENCY) - 1; // setting another clock for our load variable



PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); // configuring PWM

PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load); // Setting PWM period

PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);// setting a specific value of pulse width for PWM

PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true); // setting an output state for PWM as PWM0

PWMGenEnable(PWM1_BASE, PWM_GEN_0); // enable PWM to generator 0





PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN); // configuring PWM

PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, ui32Load2); // Setting PWM period

PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, ui8Adjust2 * ui32Load2 / 1000);// setting a specific value of pulse width for PWM

PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true); // setting an output state for PWM as PWM7

PWMGenEnable(PWM0_BASE, PWM_GEN_3); // enable PWM to generator 3



uint32_t ui32ADC0Value; //intialializing variable

uint32_t ui32ADC1Value2; //intialializing variable




SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // setting a clock for ADC

SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); // enable peripheral ADC0

ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // configuring ADC0, setting trigger processor

ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH2|ADC_CTL_IE|ADC_CTL_END); // the results going to channel 2 of ADC0

ADCSequenceEnable(ADC0_BASE, 3); // ADC0 enabled



SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1); // enable peripheral ADC1

ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // configuring ADC0, setting trigger processor

ADCSequenceStepConfigure(ADC1_BASE,3,0,ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END); // the results going to channel 1 of ADC1

ADCSequenceEnable(ADC1_BASE, 3); // ADC1 enabled





while (1) { // infinite loop


ADCIntClear(ADC0_BASE, 3); // clear any values in ADC0

ADCProcessorTrigger(ADC0_BASE, 3); // ADC trigger

while(!ADCIntStatus(ADC0_BASE, 3, false)) // while ADC0_BASE is not true;

{

}

ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value); // keep on getting data from ui32ADC0Value







if (ui32ADC0Value>3300) { // limit of the joystick; do something when value is greather than this value (pushing the button up)

ui8Adjust--;

if (ui8Adjust < 40) { // rotate the potentiometer until the value reaches to 40

ui8Adjust = 40;

}

PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,ui8Adjust * ui32Load / 1000); // setting Pulse with

}

if (ui32ADC0Value<2000) { // limit of the joystick; do something when value is less than this value (pushing the button down)

ui8Adjust++;

if (ui8Adjust > 130) { // rotate the value intil the value is less than 130

ui8Adjust = 130;

}

PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,ui8Adjust * ui32Load / 1000); //set pulse width

}

SysCtlDelay(1000); // set time delay


ADCIntClear(ADC1_BASE, 3); // clear any values in ADC1

ADCProcessorTrigger(ADC1_BASE, 3); // ADC trigger

while(!ADCIntStatus(ADC1_BASE, 3, false)) // while ADC0_BASE is not true;

{

}

ADCSequenceDataGet(ADC1_BASE, 3, &ui32ADC1Value2); // get the values from ui32ADC1Value2 while not true



SysCtlDelay(1000); // set time delay



if (ui32ADC1Value2>3300) { // limit of the joystick; do something when value is greather than this value (pushing the button left)

ui8Adjust2--;

if (ui8Adjust2 < 40) { // rotate the value intil the value is less than 40

ui8Adjust2 = 40;

}

PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,ui8Adjust2 * ui32Load2 / 1000); // set pulse width to PWM

}

if (ui32ADC1Value2<2000) { // limit of the joystick; do something when value is less than this value (pushing the button right)


ui8Adjust2++;

if (ui8Adjust2 > 130) { // rotate the value intil the value is less than 130

ui8Adjust2 = 130;

}

PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,ui8Adjust2 * ui32Load2 / 1000); // set pulse width as PWM


}

SysCtlDelay(1000); // add delay

}

}
