//////////////////////////////////////////////////////////////////////
// si5351a.h -- Library for the Si5351A (only) Chip
// Date: Fri Sep 14 21:35:50 2018   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (i2c_writecb_t)(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes);
typedef int (i2c_readcb_t)(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes);

struct s_Si5351A {			// Si5351A Register Definitions
	struct s_r0 {			// Device status
		uint8_t	revid : 2;	// R: Device revision ID
		uint8_t	not_used : 5;	// Not Si5351A
		uint8_t	sys_init : 1;	// R: System init status (1=Initializing)
	}	r0;
	struct s_r1 {			// Interrupt Status Sticky
		uint8_t	reserved : 4;	// X: Leave as default
		uint8_t	unused : 1;	// X: Si5351C only
		uint8_t	lol_a_stky : 1;	// RW: PLLA Loss of Lock Status Sticky bit (PLLA interrupt = 1)
		uint8_t	lol_b_stky : 1;	// RW: PLLB Loss of Lock Status Sticky bit (PLLB interrupt = 1)
		uint8_t	sys_init_stky:1;// RW: A SYS_INIT interrupt has occurred
	}	r1;
	struct s_r2 {			// Interrupt status mask
		uint8_t	reserved : 4;	// X: Leave as default
		uint8_t	unused : 1;	// X: Si5351C only
		uint8_t	lol_a_mask : 1;	// RW: PLLA Loss of Lock Status mask (masked = 1)
		uint8_t	lol_b_mask : 1;	// RW: PLLB Loss of Lock Status mask (masked = 1)
		uint8_t	sys_init_mask:1;// RW: SYS_INIT interrupt mask (masked = 1)
	}	r2;
	struct s_r3 {			// Output enable control
		uint8_t	clk0_oeb : 1;	// RW: 1=Disable 
		uint8_t	clk1_oeb : 1;	// RW: 1=Disable 
		uint8_t	clk2_oeb : 1;	// RW: 1=Disable 
		uint8_t	unused : 5;	// Not for Si5351A
	}	r3;
	struct s_r9 {			// OEB Pin Enable Control
		uint8_t	oeb_clk0 : 1;	// RW: 1=OEB pin does not control enable of output
		uint8_t	oeb_clk1 : 1;	// RW: 1=OEB pin does not control enable of output
		uint8_t	oeb_clk2 : 1;	// RW: 1=OEB pin does not control enable of output
		uint8_t	unused : 5;	// Not for Si5351A
	}	r9;
	struct s_r15 {			// PLL Input Source
		uint8_t reserved : 2;	// X: Leave as default
		uint8_t	plla_src : 1;	// RW: PLLA, 0=XTLAL (1 for Si5351C only)
		uint8_t	pllb_src : 1;	// RW: PLLB, 0=XTLAL (1 for Si5351C only)
		uint8_t	unused : 4;	// X: Leave as default
	}	r15;
	struct s_r16 {			// Clk0 control
		uint8_t clk0_idrv : 2;	// RW: 00=2, 01=4, 10=6, 11=8 mA
		uint8_t	clk0_src : 2;	// RW: 00=XTAL, 11=MultiSynth 0
		uint8_t	clk0_inv : 1;	// RW: 1=Invert
		uint8_t	ms0_src : 1;	// RW: 0=PLLA, 1=PLLB
		uint8_t	ms0_int : 1;	// RW: 0=Fractional mode, 1=Integer mode
		uint8_t	clk0_pdn : 1;	// RW: 0=Powered up, 1=CLK0 driver powered down
	}	r16;
	struct s_r16 r17;		// Clk1 control
	struct s_r16 r18;		// Clk2 control
	struct s_r24 {			// Clik3-0 Disable state
		uint8_t clk0_dis_state:2; // RW: 00=Low, 01=High, 10=HiZ, 11=Never disabled
		uint8_t clk1_dis_state:2; // RW: 00=Low, 01=High, 10=HiZ, 11=Never disabled
		uint8_t clk2_dis_state:2; // RW: 00=Low, 01=High, 10=HiZ, 11=Never disabled
		uint8_t	unused : 2;
	}	r24;

	// MultiSynth 0:
	struct s_r42 {			// MultiSynth0 Parameters
		uint8_t	msx_p3_15_8;	// part of denominator
	}	r42;
	struct s_r43 {			// MultiSynth0 Parameters
		uint8_t	msx_p3_7_0;	// part of denominator
	}	r43;
	struct s_r44 {			// MultiSynth0 Parameters
		uint8_t	msx_p1_17_16:2;	// integer part of divider
		uint8_t	reserved : 2;
		uint8_t	r0_div : 2;	// 000=1,001=2,010=4,011=8,100=16,101=32,110=64,111=128
 		uint8_t	unused : 1;	
	}	r44;
	struct s_r45 {			// MultiSynth0 Parameters
		uint8_t	msx_p1_15_8:8;	// integer part of divider
	}	r45;
	struct s_r46 {			// MultiSynth0 Parameters
		uint8_t	msx_p1_7_0;	// integer part of divider
	}	r46;
	struct s_r47 {			// MultiSynth0 Parameters
		uint8_t	msx_p2_19_16:4;	// Part of numerator
		uint8_t	msx_p3_19_16:4;	// Part of denominator
	}	r47;
	struct s_r48 {			// MultiSynth0 Parameters
		uint8_t	msx_p2_15_8;	// Part of fractional numerator
	}	r48;
	struct s_r49 {			// MultiSynth0 Parameters
		uint8_t	msx_p2_7_0;	// Part of fractional numerator
	}	r49;

	// MultiSynth 1:
	struct s_r42 r50;
	struct s_r43 r51;
	struct s_r44 r52;
	struct s_r45 r53;
	struct s_r46 r54;
	struct s_r47 r55;
	struct s_r48 r56;
	struct s_r49 r57;

	// MultiSynth 2:
	struct s_r42 r58;
	struct s_r43 r59;
	struct s_r44 r60;
	struct s_r45 r61;
	struct s_r46 r62;
	struct s_r47 r63;
	struct s_r48 r64;
	struct s_r49 r65;

	struct s_r149 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p2_14_8:7;	// PLL A Spread Spectrum Down P2
		uint8_t	reserved : 1;
	}	r149;
	struct s_r150 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p2_7_0;	// PLL A Spread Spectrum Down P2
		uint8_t	reserved : 1;
	}	r150;
	struct s_r151 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p3_14_8:7; // PLL A Spread Spectrum Down P3
		uint8_t	ssc_mode : 1;	// 1=Center, 0=Down spread
	}	r151;
	struct s_r152 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p3_7_0;	// PLL A Spread Spectrum Down P3
	}	r152;
	struct s_r153 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p1_7_0;	// PLL A Spread Spectrum Down P1
	}	r153;
	struct s_r154 {			// Spread Spectrum Parameters
		uint8_t	ssdn_p1_11_8:4;	// PLL A Spread Spectrum Down P1
		uint8_t	ssudp_11_8 : 4;	// PLL A Spread Spectrum Up/Down
	}	r154;
	struct s_r155 {			// Spread Spectrum Parameters
		uint8_t	ssudp_7_0;	// PLL A Spread Spectrum Up/Down
	}	r155;
	struct s_r156 {			// Spread Spectrum Parameters
		uint8_t	ssup_p2_14_8:7;	// PLL A Spread Up Spectrum P2
		uint8_t	reserved : 1;
	}	r156;
	struct s_r157 {			// Spread Spectrum Parameters
		uint8_t	ssup_p2_7_0;	// PLL A Spread Up Spectrum P2
	}	r157;
	struct s_r158 {			// Spread Spectrum Parameters
		uint8_t	ssup_p3_14_8:7;	// PLL A Spread Up Spectrum P3
		uint8_t	reserved : 1;
	}	r158;
	struct s_r159 {			// Spread Spectrum Parameters
		uint8_t	ssup_p3_7_0;	// PLL A Spread Up Spectrum P3
	}	r159;
	struct s_r160 {			// Spread Spectrum Parameters
		uint8_t	ssup_p1_7_0;	// PLL A Spread Up Spectrum P1
	}	r160;
	struct s_r161 {			// Spread Spectrum Parameters
		uint8_t	ssup_p1_11_8:4;	// PLL A Spread Up Spectrum P1
		uint8_t	ss_nclk : 4;	// Must be zero
	}	r161;

	struct s_r165 {			// Clk0 Initial Phase Offset
		uint8_t	clkx_phoff : 6;	// RW: Time delay of Tvco/4
		uint8_t	reserved : 1;
	}	r165;
	struct s_r165 r166;		// Clk1 Initial Phase Offset
	struct s_r165 r167;		// Clk2 Initial Phase Offset

	struct s_r177 {			// PLL Reset
		uint8_t	reserved : 5;
		uint8_t	plla_rst : 1;	// RW: Writing 1 resets
		uint8_t	unused : 1;
		uint8_t	pllb_rst : 1;	// RW: Writing 1 resets
	}	r177;

	struct s_r183 {			// Crystal Internal Load Capacitance
		uint8_t	reserved : 6;
		uint8_t	xtal_cl : 2;	// RW: 01=6, 10=8, 11=10 pF (10pF is default)
	}	r183;

	uint8_t		i2c_addr;
	i2c_writecb_t	*i2c_write;
	i2c_readcb_t	*i2c_read;
	void		*arg;
};

typedef struct s_Si5351A Si5351A;

void Si5351A_init(Si5351A *si,uint8_t i2c_addr,i2c_readcb_t readcb,i2c_writecb_t writecb,void *arg);
void Si5351A_device_reset(Si5351A *si);

#ifdef __cplusplus
}
#endif

// End si5351a.h
