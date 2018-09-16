//////////////////////////////////////////////////////////////////////
// testbed.c -- Test Bed for Si5351a.c
// Date: Sun Sep 16 17:03:00 2018   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "si5351a.h"

static int
readcb(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes) {
	return bytes;
}

static int
writecb(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes) {
	return bytes;
}

int
main(int argc,char **argv) {
	Si5351A si;
	uint32_t xtal_freq = 25000000;
	uint32_t freq = 20000000;

	Si5351A_init(&si,0x60,readcb,writecb,&si,Cap6pF);
	
	freq = Si5351A_set_frequency(&si,freq,xtal_freq,0);
	printf("freq = %lu\n",(unsigned long)freq);
}

// End testbed.c
