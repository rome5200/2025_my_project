#include "xparameters.h"
#include "xgpio.h"
#include <stdio.h>
#include "sleep.h"

int main(void)
{
	XGpio switches, leds;
	int switches_check;

	xil_printf("-- LED and Switches Control Program\r\n");

	/* Switches Initialization */
	XGpio_Initialize(&switches, XPAR_SWITCHES_DEVICE_ID);
	XGpio_SetDataDirection(&switches, 1, 0xffffffff);

	/* LEDs Initialization */
	XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x00000000);

	while (1)
	{
		switches_check = XGpio_DiscreteRead(&switches, 1); // read the value of switches
		XGpio_DiscreteWrite(&leds, 1, switches_check); // write the value of switches
		xil_printf("Switch on: %x\r\n", switches_check); // display the result
		sleep(2); // 2sec delay to reflect change
	}
}
