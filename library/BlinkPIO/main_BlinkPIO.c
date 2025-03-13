#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "lib/blink_pio.h"


#define CLK_PIO_PIN 2
#define CLK_FRQ_VAL	1000


int main(){   
    // Init Phase
    PIO pio = pio0;
    clk_generation_pio_init(pio, CLK_PIO_PIN, CLK_FRQ_VAL);
	
	// Infinity Loop
    while (true){
		sleep_ms(1000);
    }
}
