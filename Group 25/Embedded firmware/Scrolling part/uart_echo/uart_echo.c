#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"

void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo character
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED
		SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
	}
}




int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_UART0); //enable the UART interrupt
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

	UARTStdioConfig(0, 115200, SysCtlClockGet());




	uint32_t ui32ADC0Value[4];
	uint32_t ui32TempAvg;
	uint32_t ui32TempValueC;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 3);


/*	volatile uint32_t starter;
	for ( starter=0x0; starter<=0x10000; starter++)
		{
			UARTStdioConfig(0,115200,SysCtlClockGet());
			UARTprintf( "\n%u", starter);

	}

*/
	while (1) //let interrupt handler do the UART echo function
	{
		ADCIntClear(ADC0_BASE, 3);
		ADCProcessorTrigger(ADC0_BASE, 3);
		while(!ADCIntStatus(ADC0_BASE, 3, false))
				{
				}
		ADCSequenceDataGet(ADC0_BASE, 3, &ui32TempAvg);
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;

		UARTprintf( " temp:\n%u", ui32TempValueC);
		// if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));

	}
}
