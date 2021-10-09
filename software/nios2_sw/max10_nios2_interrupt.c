
#include <stdio.h>
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include <io.h>
#include "alt_types.h"

volatile int edge_capture;
void alt_ic_isr_register();

void myISR()
{
	IOWR(HEX5_BASE, 0, 0xFF);
	IOWR(HEX4_BASE, 0, 0xF9); // I
	IOWR(HEX3_BASE, 0, 0xAB); // n
	IOWR(HEX2_BASE, 0, 0x87); // t
	IOWR(HEX1_BASE, 0, 0xFF);
	IOWR(HEX0_BASE, 0, 0xFF);
}

void resetHEX()
{
	IOWR(HEX5_BASE, 0, HEX5_RESET_VALUE);
	IOWR(HEX4_BASE, 0, HEX4_RESET_VALUE);
	IOWR(HEX3_BASE, 0, HEX3_RESET_VALUE);
	IOWR(HEX2_BASE, 0, HEX2_RESET_VALUE);
	IOWR(HEX1_BASE, 0, HEX1_RESET_VALUE);
	IOWR(HEX0_BASE, 0, HEX0_RESET_VALUE);
}

static void handle_button_interrupts(void* context)
{
	// Cast context to edge_capture's type
	// Volatile to avoid compiler optimization.
	volatile int* edge_capture_ptr = (volatile int*) context;

	// Read the edge capture register on the
	// button PIO and store value
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE);

	// Write to the edge capture register to reset it
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE, 0);

	myISR();
}

static void init_button_pio()
{
	// Recast the edge_capture pointer to match the
	// alt_irq_register() function prototype
	void* edge_capture_ptr = (void*) &edge_capture;

	// Enable all 4 button interrupts
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_BASE, 0xf);

	// Reset the edge capture register
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE, 0x0);

	// Register the ISR
	alt_ic_isr_register(
			BUTTON_IRQ_INTERRUPT_CONTROLLER_ID,
			BUTTON_IRQ,
			handle_button_interrupts,
			edge_capture_ptr, 0x0);
}

int main()
{
	int i = 0, rom_data = 0x00;
	init_button_pio();

	while (1)
	{
		if (i < 0x10)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, rom_data);
			rom_data = IORD_16DIRECT(ROM_BASE, i);
			i += 2;
			for (int a = 0; a <= 100000; a++){}
			//usleep(100000);
		} else {
			i = 0x00;
		}
		resetHEX();
	}

	return 0;
}
