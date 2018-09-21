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
#include "rational.h"

#define SI5351_PLL_VCO_MIN              600000000
#define SI5351_PLL_VCO_MAX              900000000
#define SI5351_PLL_A_MIN                15
#define SI5351_PLL_A_MAX                90
#define SI5351_PLL_B_MAX                (SI5351_PLL_C_MAX-1)
#define SI5351_PLL_C_MAX                1048575

#define Offset(member) (uint16_t)(((uint8_t*)&(((Si5351A*)0)->member)) - ((uint8_t*)0))

typedef union {
	uint32_t	bits_7_0 : 8;
	uint32_t	bits_15_8 : 8;
	uint32_t	bits_19_16 : 4;
	uint32_t	unused : 12;
} u_value;

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
	{ 26,	Offset(pll[0].r26) },
	{ 27,	Offset(pll[0].r27) },
	{ 28,	Offset(pll[0].r28) },
	{ 29,	Offset(pll[0].r29) },
	{ 30,	Offset(pll[0].r30) },
	{ 31,	Offset(pll[0].r31) },
	{ 32,	Offset(pll[0].r32) },
	{ 33,	Offset(pll[0].r33) },
	{ 34,	Offset(pll[1].r26) },
	{ 35,	Offset(pll[1].r27) },
	{ 36,	Offset(pll[1].r28) },
	{ 37,	Offset(pll[1].r29) },
	{ 38,	Offset(pll[1].r30) },
	{ 39,	Offset(pll[1].r31) },
	{ 40,	Offset(pll[1].r32) },
	{ 41,	Offset(pll[1].r33) },
	{ 42,	Offset(m[0].r42) },	
	{ 43,	Offset(m[0].r43) },	
	{ 44,	Offset(m[0].r44) },	
	{ 45,	Offset(m[0].r45) },	
	{ 46,	Offset(m[0].r46) },	
	{ 47,	Offset(m[0].r47) },	
	{ 48,	Offset(m[0].r48) },	
	{ 49,	Offset(m[0].r49) },	
	{ 50,	Offset(m[1].r42) },	
	{ 51,	Offset(m[1].r43) },	
	{ 52,	Offset(m[1].r44) },	
	{ 53,	Offset(m[1].r45) },	
	{ 54,	Offset(m[1].r46) },	
	{ 55,	Offset(m[1].r47) },	
	{ 56,	Offset(m[1].r48) },	
	{ 57,	Offset(m[1].r49) },	
	{ 58,	Offset(m[2].r42) },	
	{ 59,	Offset(m[2].r43) },	
	{ 60,	Offset(m[2].r44) },	
	{ 61,	Offset(m[2].r45) },	
	{ 62,	Offset(m[2].r46) },	
	{ 63,	Offset(m[2].r47) },	
	{ 64,	Offset(m[2].r48) },	
	{ 65,	Offset(m[2].r49) },	
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
	uint8_t iobuf[1+buflen];

	iobuf[0] = reg;
	memcpy(iobuf+1,buf,buflen);
	return si->i2c_write(si->i2c_addr,iobuf,1+buflen);
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
Si5351A_init(Si5351A *si,uint8_t i2c_addr,i2c_readcb_t readcb,i2c_writecb_t writecb,void *arg,XtalCap cap) {

	memset(si,0,sizeof *si);
	si->i2c_addr = i2c_addr;
	si->i2c_read = readcb;
	si->i2c_write = writecb;
	si->arg = arg;
	Si5351A_device_reset(si,cap);
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

bool
Si5351A_set_msynth(Si5351A *si,short msynthx,uint32_t A,uint32_t B,uint32_t C) {
	u_value *pA = (u_value*)&A, *pB = (u_value*)&B, *pC = (u_value*)&C;
	struct s_msynth_params *mp;
	
	if ( msynthx < 0 || msynthx > 2 )
		return false;	// Unsupported msynth

	mp = &si->m[msynthx];

	mp->r46.msx_p1_7_0 = pA->bits_7_0;	// Integer
	mp->r45.msx_p1_15_8 = pA->bits_15_8;
	mp->r44.msx_p1_17_16 = pA->bits_19_16;	// Cheat (drops upper bits)

	mp->r49.msx_p2_7_0 = pB->bits_7_0;
	mp->r48.msx_p2_15_8 = pB->bits_15_8;
	mp->r47.msx_p2_19_16 = pB->bits_19_16;

	mp->r43.msx_p3_7_0 = pC->bits_7_0;
	mp->r42.msx_p3_15_8 = pC->bits_15_8;
	mp->r47.msx_p3_19_16 = pC->bits_19_16;

	return writebuf(si,42+msynthx*8,(void*)&mp->r42,8) == 8;
}

bool
Si5351A_msynth_div(Si5351A *si,short msynth,RxDiv div) {
	struct s_msynth_params *mp;
	int regoff;
	unsigned udiv = (unsigned)div;
	
	if ( msynth < 0 || msynth > 2 )
		return false;	// Unsupported msynth

	mp = &si->m[msynth];
	regoff = msynth * 8;

	mp->r44.rx_div = udiv;
	write1(si,regoff+44,&mp->r44);
	return true;
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
Si5351A_xtal_cap(Si5351A *si,XtalCap cap) {

	si->r183.xtal_cl = (uint8_t)cap;
}

void
Si5351A_pll_reset(Si5351A *si,PLL pll) {

	switch ( pll ) {
	case PLLA:
		si->r177.plla_rst = 1;
		break;
	case PLLB:
		si->r177.pllb_rst = 1;
		break;
	}
}

bool
Si5351A_pll_is_reset(Si5351A *si,PLL pll) {

	read1(si,177,&si->r177);

	if ( pll == PLLA )
		return !si->r177.plla_rst;
	else	return !si->r177.pllb_rst;
}

void
Si5351A_device_reset(Si5351A *si,XtalCap cap) {

#ifndef TEST
	while ( Si5351A_is_busy(si) )
		;
	read_all(si);	
#endif
	si->r177.plla_rst = 1;
	si->r177.pllb_rst = 1;
#ifndef TEST
	while ( !Si5351A_pll_is_reset(si,PLLA) );
	while ( !Si5351A_pll_is_reset(si,PLLB) );
#endif
	write1(si,177,&si->r177);

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
	
	Si5351A_xtal_cap(si,cap);

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

bool
Si5351A_set_pll(Si5351A *si,short pllx,uint32_t A,uint32_t B,uint32_t C) {
	struct s_pll *pll = &si->pll[pllx];
	u_value *pA = (u_value*)&A, *pB = (u_value*)&B, *pC = (u_value*)&C;

	if ( pllx < 0 || pllx > 1 )
		return false;

	pll->r30.msnx_p1_7_0 = pA->bits_7_0;	// Integer
	pll->r29.msnx_p1_15_8 = pA->bits_15_8;
	pll->r28.msnx_p1_17_16 = pA->bits_19_16; // Cheat (drops high order bits)

	pll->r33.msnx_p2_7_0 = pB->bits_7_0;	// Numerator
	pll->r32.msnx_p2_15_8 = pB->bits_15_8;
	pll->r31.msnx_p2_19_16 = pB->bits_19_16;

	pll->r27.msnx_p3_7_0 = pC->bits_7_0;	// Denominator
	pll->r26.msnx_p3_15_8 = pC->bits_15_8;
	pll->r31.msnx_p3_19_16 = pC->bits_19_16;

	return writebuf(si,26+pllx*8,(void*)&pll->r26,8) == 8;
}

// End si5351a.c
