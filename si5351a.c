//////////////////////////////////////////////////////////////////////
// si5351a.c -- Library for the Si5351A Chip
// Date: Fri Sep 14 21:35:21 2018   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include <memory.h>

#include "si5351a.h"

void
Si5351A_init(Si5351A *si,uint8_t i2c_addr,i2c_readcb_t readcb,i2c_writecb_t writecb,void *arg) {

	memset(si,0,sizeof *si);
	si->i2c_read = readcb;
	si->i2c_write = writecb;
	si->arg = arg;
}

// End si5351a.c
