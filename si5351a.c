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

#define Offset(member) (uint16_t)(((uint8_t*)&(((Si5351A*)0)->member)) - ((uint8_t*)0))

static const struct s_reg {
	uint8_t		reg;
	uint16_t	offset;
} regs[] = {
	{ 0,	Offset(r0) },	
	{ 1,	Offset(r1) },	
	{ 2,	Offset(r2) },	
	{ 3,	Offset(r3) },	
	{ 9,	Offset(r9) },	
	{ 15,	Offset(r15) },	
	{ 16,	Offset(r16) },	
	{ 17,	Offset(r17) },	
	{ 18,	Offset(r18) },	
	{ 24,	Offset(r24) },	
	{ 42,	Offset(r42) },	
	{ 43,	Offset(r43) },	
	{ 44,	Offset(r44) },	
	{ 45,	Offset(r45) },	
	{ 46,	Offset(r46) },	
	{ 47,	Offset(r47) },	
	{ 48,	Offset(r48) },	
	{ 49,	Offset(r49) },	
	{ 50,	Offset(r50) },	
	{ 51,	Offset(r51) },	
	{ 52,	Offset(r52) },	
	{ 53,	Offset(r53) },	
	{ 54,	Offset(r54) },	
	{ 55,	Offset(r55) },	
	{ 56,	Offset(r56) },	
	{ 57,	Offset(r57) },	
	{ 58,	Offset(r58) },	
	{ 59,	Offset(r59) },	
	{ 60,	Offset(r60) },	
	{ 61,	Offset(r61) },	
	{ 62,	Offset(r62) },	
	{ 63,	Offset(r63) },	
	{ 64,	Offset(r64) },	
	{ 65,	Offset(r65) },	
	{ 149,	Offset(r149) },	
	{ 150,	Offset(r150) },	
	{ 151,	Offset(r151) },	
	{ 152,	Offset(r152) },	
	{ 153,	Offset(r153) },	
	{ 154,	Offset(r154) },	
	{ 155,	Offset(r155) },	
	{ 156,	Offset(r156) },	
	{ 157,	Offset(r157) },	
	{ 158,	Offset(r158) },	
	{ 159,	Offset(r159) },	
	{ 160,	Offset(r160) },	
	{ 161,	Offset(r161) },	
	{ 165,	Offset(r165) },	
	{ 166,	Offset(r166) },	
	{ 167,	Offset(r167) },	
	{ 177,	Offset(r177) },	
	{ 183,	Offset(r183) },	
	{ 255, 0 }
};

static int
readbuf(Si5351A *si,uint8_t reg,uint8_t *buf,uint8_t buflen) {
	int n;

	if ( (n = si->i2c_write(si->i2c_addr,&reg,1)) != buflen )
		return -1;
	return si->i2c_read(si->i2c_addr,buf,buflen);
}

static int
writebuf(Si5351A *si,uint8_t reg,uint8_t *buf,uint8_t buflen) {
	int n;

	if ( (n = si->i2c_write(si->i2c_addr,&reg,1)) != buflen )
		return -1;
	return si->i2c_write(si->i2c_addr,buf,buflen);
}

static int
write1(Si5351A *si,uint8_t reg,void *dat) {
	return writebuf(si,reg,(uint8_t*)&dat,1);
}

static int
read1(Si5351A *si,uint8_t reg,void *dat) {
	return readbuf(si,reg,(uint8_t*)dat,1);
}

void
Si5351A_init(Si5351A *si,uint8_t i2c_addr,i2c_readcb_t readcb,i2c_writecb_t writecb,void *arg) {

	memset(si,0,sizeof *si);
	si->i2c_read = readcb;
	si->i2c_write = writecb;
	si->arg = arg;
}

bool
Si5351A_is_busy(Si5351A *si) {

	read1(si,0,&si->r0);
	return !!si->r0.sys_init;
}

static uint8_t *
register_addr(Si5351A *si,uint8_t reg) {

	for ( unsigned x=0; regs[x].reg != 255; ++x )
		if ( regs[x].reg == reg )
			return ((uint8_t *)si) + regs[x].offset;

	return 0;	// Not found
}

static void
read_all(Si5351A *si) {

	for ( unsigned x=0; regs[x].reg != 255; ++x )
		read1(si,regs[x].reg,(uint8_t*)si + regs[x].offset);
}

void
Si5351A_device_reset(Si5351A *si) {

	while ( Si5351A_is_busy(si) )
		;
	read_all(si);	
	si->r177.plla_rst = 1;
	si->r177.pllb_rst = 1;
	write1(si,177,&si->r177);
	do	{
		read1(si,177,&si->r177);
	} while ( si->r177.plla_rst || si->r177.pllb_rst );
}

// End si5351a.c
