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
	{ 42,	Offset(m0.r42) },	
	{ 43,	Offset(m0.r43) },	
	{ 44,	Offset(m0.r44) },	
	{ 45,	Offset(m0.r45) },	
	{ 46,	Offset(m0.r46) },	
	{ 47,	Offset(m0.r47) },	
	{ 48,	Offset(m0.r48) },	
	{ 49,	Offset(m0.r49) },	
	{ 50,	Offset(m1.r42) },	
	{ 51,	Offset(m1.r43) },	
	{ 52,	Offset(m1.r44) },	
	{ 53,	Offset(m1.r45) },	
	{ 54,	Offset(m1.r46) },	
	{ 55,	Offset(m1.r47) },	
	{ 56,	Offset(m1.r48) },	
	{ 57,	Offset(m1.r49) },	
	{ 58,	Offset(m2.r42) },	
	{ 59,	Offset(m2.r43) },	
	{ 60,	Offset(m2.r44) },	
	{ 61,	Offset(m2.r45) },	
	{ 62,	Offset(m2.r46) },	
	{ 63,	Offset(m2.r47) },	
	{ 64,	Offset(m2.r48) },	
	{ 65,	Offset(m2.r49) },	
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

#if 0
static uint8_t *
register_addr(Si5351A *si,uint8_t reg) {

	for ( unsigned x=0; regs[x].reg != 255; ++x )
		if ( regs[x].reg == reg )
			return ((uint8_t *)si) + regs[x].offset;

	return 0;	// Not found
}
#endif

static void
read_all(Si5351A *si) {

	for ( unsigned x=0; regs[x].reg != 255; ++x )
		read1(si,regs[x].reg,(uint8_t*)si + regs[x].offset);
}

void
Si5351A_clock_enable(Si5351A *si,Clock clock,bool on) {

	switch ( clock ) {
	case Clock0:
		si->r3.clk0_oeb = on;		
		break;
	case Clock1:
		si->r3.clk1_oeb = on;
		break;
	case Clock2:
		si->r3.clk2_oeb = on;
		break;
	}
	write1(si,3,&si->r3);
}

void
Si5351A_clock_enable_pin(Si5351A *si,Clock clock,bool enable) {

	switch ( clock ) {
	case Clock0:
		si->r9.oeb_clk0 = enable;		
		break;
	case Clock1:
		si->r9.oeb_clk1 = enable;
		break;
	case Clock2:
		si->r9.oeb_clk2 = enable;
		break;
	}
	write1(si,3,&si->r9);
}

void
Si5351A_clock_power(Si5351A *si,Clock clock,bool on) {

	switch ( clock ) {
	case Clock0:
		si->r16.clkx_pdn = !on;
		write1(si,16,&si->r16);
		break;
	case Clock1:
		si->r17.clkx_pdn = !on;
		write1(si,17,&si->r17);
		break;
	case Clock2:
		si->r18.clkx_pdn = !on;
		write1(si,18,&si->r18);
		break;
	}
}

void
Si5351A_clock_msynth(Si5351A *si,Clock clock,MultiSynthMode mode) {
	bool mint = mode == IntegerMode;

	switch ( clock ) {
	case Clock0:
		si->r16.msx_int = mint;
		write1(si,16,&si->r16);
		break;
	case Clock1:
		si->r17.msx_int = mint;
		write1(si,17,&si->r17);
		break;
	case Clock2:
		si->r18.msx_int = mint;
		write1(si,18,&si->r18);
		break;
	}
}

void
Si5351A_clock_polarity(Si5351A *si,Clock clock,bool invert) {

	switch ( clock ) {
	case Clock0:
		si->r16.msx_inv = invert;
		write1(si,16,&si->r16);
		break;
	case Clock1:
		si->r17.msx_inv = invert;
		write1(si,17,&si->r17);
		break;
	case Clock2:
		si->r18.msx_inv = invert;
		write1(si,18,&si->r18);
		break;
	}
}

void
Si5351A_clock_insrc(Si5351A *si,Clock clock,ClockSource src) {
	unsigned s = (unsigned)src;

	switch ( clock ) {
	case Clock0:
		si->r16.clkx_src = s;
		write1(si,16,&si->r16);
		break;
	case Clock1:
		si->r17.clkx_src = s;
		write1(si,17,&si->r17);
		break;
	case Clock2:
		si->r18.clkx_src = s;
		write1(si,18,&si->r18);
		break;
	}
}

void
Si5351A_clock_drive(Si5351A *si,Clock clock,ClockDrive drv) {
	unsigned d = (unsigned)drv;

	switch ( clock ) {
	case Clock0:
		si->r16.clkx_idrv = d;
		write1(si,16,&si->r16);
		break;
	case Clock1:
		si->r17.clkx_idrv = d;
		write1(si,17,&si->r17);
		break;
	case Clock2:
		si->r18.clkx_idrv = d;
		write1(si,18,&si->r18);
		break;
	}
}

void
Si5351A_clock_disable_state(Si5351A *si,Clock clock,DisState state) {
	unsigned s = (unsigned)state;

	switch ( clock ) {
	case Clock0:
		si->r24.clk0_dis_state = s;
		break;
	case Clock1:
		si->r24.clk1_dis_state = s;
		break;
	case Clock2:
		si->r24.clk2_dis_state = s;
		break;
	}
	write1(si,24,&si->r24);
}

void
Si5351A_msynth_param(Si5351A *si,MultiSynth msynth,MSynthParam param,uint32_t value) {
	union u_value {
		uint32_t	bits_7_0 : 8;
		uint32_t	bits_15_8 : 8;
		uint32_t	bits_19_16 : 4;
		uint32_t	unused : 12;
	} *pvalue = (union u_value *)&value;
	struct s_msynth_params *mp;
	int regoff;
	
	switch ( msynth ) {
	case MSynth0:
		mp = &si->m0;
		regoff = 0;
		break;
	case MSynth1:
		mp = &si->m1;
		regoff = 8;
		break;
	case MSynth2:
		mp = &si->m2;
		regoff = 16;
		break;
	}

	switch ( param ) {
	case MSynthP1:
		mp->r44.msx_p1_17_16 = pvalue->bits_19_16;
		mp->r45.msx_p1_15_8 = pvalue->bits_15_8;
		mp->r46.msx_p1_7_0 = pvalue->bits_7_0;
		write1(si,regoff+44,&mp->r44);
		write1(si,regoff+45,&mp->r45);
		write1(si,regoff+46,&mp->r46);
		break;
	case MSynthP2:
		mp->r47.msx_p2_19_16 = pvalue->bits_19_16;
		mp->r48.msx_p2_15_8 = pvalue->bits_15_8;
		mp->r49.msx_p2_7_0 = pvalue->bits_7_0;
		write1(si,regoff+47,&mp->r47);
		write1(si,regoff+48,&mp->r48);
		write1(si,regoff+49,&mp->r49);
		break;
	case MSynthP3:
		mp->r42.msx_p3_15_8 = pvalue->bits_15_8;
		mp->r43.msx_p3_7_0 = pvalue->bits_7_0;
		mp->r47.msx_p3_19_16 = pvalue->bits_19_16;
		write1(si,regoff+42,&mp->r42);
		write1(si,regoff+43,&mp->r43);
		write1(si,regoff+47,&mp->r47);
		break;
	}
}

void
Si5351A_msynth_div(Si5351A *si,MultiSynth msynth,RxDiv div) {
	struct s_msynth_params *mp;
	int regoff;
	unsigned udiv = (unsigned)div;
	
	switch ( msynth ) {
	case MSynth0:
		mp = &si->m0;
		regoff = 0;
		break;
	case MSynth1:
		mp = &si->m1;
		regoff = 8;
		break;
	case MSynth2:
		mp = &si->m2;
		regoff = 16;
		break;
	}

	mp->r44.rx_div = udiv;
	write1(si,regoff+44,&mp->r44);
}

void
Si5351A_clock_intmask(Si5351A *si,PLL pll,bool mask) {

	switch ( pll ) {
	case PLLA:
		si->r2.lol_a_mask = mask;
		break;
	case PLLB:
		si->r2.lol_b_mask = mask;
		break;
	}
	write1(si,2,&si->r2);
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

	Si5351A_clock_enable_pin(si,Clock0,false);
	Si5351A_clock_enable_pin(si,Clock1,false);
	Si5351A_clock_enable_pin(si,Clock2,false);
	Si5351A_clock_enable(si,Clock0,false);
	Si5351A_clock_enable(si,Clock1,false);
	Si5351A_clock_enable(si,Clock2,false);
	Si5351A_clock_intmask(si,PLLA,true);
	Si5351A_clock_intmask(si,PLLB,true);
	Si5351A_clock_power(si,Clock0,false);
	Si5351A_clock_power(si,Clock1,false);
	Si5351A_clock_power(si,Clock2,false);
	
	// Only choice for Si5351A:
	si->r15.pllb_src = 0;
	si->r15.plla_src = 0;

	Si5351A_clock_msynth(si,Clock0,FractionalMode);
	Si5351A_clock_msynth(si,Clock1,FractionalMode);
	Si5351A_clock_msynth(si,Clock2,FractionalMode);
	Si5351A_clock_polarity(si,Clock0,false);
	Si5351A_clock_polarity(si,Clock1,false);
	Si5351A_clock_polarity(si,Clock2,false);
	Si5351A_clock_insrc(si,Clock0,MSynth_Source);
	Si5351A_clock_insrc(si,Clock1,MSynth_Source);
	Si5351A_clock_insrc(si,Clock2,MSynth_Source);
	Si5351A_clock_drive(si,Clock0,Drive6mA);
	Si5351A_clock_drive(si,Clock1,Drive6mA);
	Si5351A_clock_drive(si,Clock2,Drive6mA);
	Si5351A_clock_disable_state(si,Clock0,DisHiZ);
	Si5351A_clock_disable_state(si,Clock1,DisHiZ);
	Si5351A_clock_disable_state(si,Clock2,DisHiZ);

}

// End si5351a.c
