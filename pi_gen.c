///////////////////////////////////////////////////////////////////////
// pi_gen.c -- Pi Test Bed for Si5351a.c
// Date: Sun Sep 16 17:03:00 2018   (C) ve3wwg@gmail.com
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "si5351a.h"

static const char *i2cbus = "/dev/i2c-1";
static int i2c_fd = -1;
static bool debugf = false;

static void
usage(const char *cmd) {
	const char *cp = strrchr(cmd,'/');

	if ( cp )
		cmd = cp + 1;
	printf("Usage: %s [-optons]\n"
		"where:\n"
		"\t-x :\tClock 0..3 (default 0)\n"
		"\t-p :\tPLLA = 0, PLLB = 1 (default PLLA)\n"
		"\t-a :\tInteger value a for PLL,\n"
		"\t-b :\tnumerator b for PLL,\n"
		"\t-c :\tdenominator c for PLL\n"
		"\t-A :\tInteger value A for MultiSynth,\n"
		"\t-B :\tnumerator B for MultiSynth,\n"
		"\t-C :\tdenominator value C for MultiSynth\n"
		"\t-r :\tDivider R 1/2/4/.../128 (default 1)\n"
		"\t-i\tInteger division (default fractional)\n"
		"\t-I\tInvert output\n"
		"\t-X\tOutput source is XTAL\n"
		"\t-d\tDebug output\n"
		"\t-h\tThis help.\n",cmd);
}

static int
readcb(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes) {
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msg;
	int rc;
	
	msg.addr = i2c_addr;
	msg.flags = I2C_M_RD;
	msg.buf = (char*)buf;
	msg.len = bytes;

	msgset.msgs = &msg;
	msgset.nmsgs = 1;

	rc = ioctl(i2c_fd,I2C_RDWR,&msgset);
	if ( rc < 0 )
		fprintf(stderr,"%s: reading i2c 0x%02X for %u bytes\n",
			strerror(errno),
			(unsigned)i2c_addr,
			(unsigned)bytes);
	assert(rc == 1);
	if ( debugf ) {
		printf("%02X [ ",i2c_addr);
		for ( uint8_t x=0; x<bytes; ++x )
			printf(" %02X",buf[x]);
		printf(" ] (%d)\n",(int)bytes);
	}

	return rc;
}

static int
writecb(uint8_t i2c_addr,uint8_t *buf,uint8_t bytes) {
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msg;
	int rc;
	
	msg.addr = i2c_addr;
	msg.flags = 0;
	msg.buf = (char*)buf;
	msg.len = bytes;

	msgset.msgs = &msg;
	msgset.nmsgs = 1;

	rc = ioctl(i2c_fd,I2C_RDWR,&msgset);
	if ( rc < 0 )
		fprintf(stderr,"%s: writing i2c 0x%02X for %u bytes\n",
			strerror(errno),
			(unsigned)i2c_addr,
			(unsigned)bytes);
	if ( debugf ) {
		if ( bytes ==1 ) 
			printf("%02X [ W r%d ]\n",i2c_addr,buf[0]);
		else	{
			printf("%02X [ W r%d :",i2c_addr,buf[0]);
			for ( uint8_t x=1; x<bytes; ++x )
				printf(" %02X",buf[x]);
			printf(" ] (%d)\n",(int)bytes);
		}
	}
	assert(rc == 1);
	return rc;
}

int
main(int argc,char **argv) {
	static const char cmdopts[] = ":ha:b:c:A:B:C:r:x:XiIp:d";
	static const struct {
		unsigned	v;
		RxDiv		d;
	} rxdivs[] = {
		{ 1, RxDiv1 },
		{ 2, RxDiv2 },
		{ 4, RxDiv4 },
		{ 8, RxDiv8 },
		{ 16, RxDiv16 },
		{ 32, RxDiv32 },
		{ 64, RxDiv64 },
		{ 128, RxDiv128 }
	};
	Si5351A si;
	unsigned a = 28u, b = 0u, c = 1048575u, A = 36, B = 0, C = 1048575u, d = 1, rxdiv = 1;
	int optch, clockx = 0, pllx = 0;

	i2c_fd = open(i2cbus,O_RDWR);
	if ( i2c_fd < 0 ) {
		fprintf(stderr,"%s: opening %s\n",strerror(errno),i2cbus);
		exit(1);
	}

	Si5351A_init(&si,0x60,readcb,writecb,&si,Cap6pF);
	
	Si5351A_clock_power(&si,clockx,true);
	Si5351A_clock_source(&si,clockx,MSynth_Source);
	Si5351A_clock_msynth(&si,clockx,FractionalMode);
	Si5351A_clock_pll(&si,clockx,pllx);

	while ( (optch = getopt(argc,argv,cmdopts)) != -1 ) {
		switch ( optch ) {
		case 'a':
			a = strtoul(optarg,0,10);
			Si5351A_set_pll(&si,clockx,a,b,c);
			break;
		case 'b':
			b = strtoul(optarg,0,10);
			Si5351A_set_pll(&si,clockx,a,b,c);
			break;
		case 'c':
			c = strtoul(optarg,0,10);
			Si5351A_set_pll(&si,clockx,a,b,c);
			break;
		case 'A':
			A = strtoul(optarg,0,10);
			Si5351A_set_msynth(&si,0,A,B,C);
			break;
		case 'B':
			B = strtoul(optarg,0,10);
			Si5351A_set_msynth(&si,clockx,A,B,C);
			break;
		case 'C':
			C = strtoul(optarg,0,10);
			Si5351A_set_msynth(&si,clockx,A,B,C);
			break;
		case 'x':
			clockx = strtol(optarg,0,10);
			break;
		case 'p':
			pllx = strtol(optarg,0,10);
			Si5351A_clock_pll(&si,clockx,pllx);
			break;
		case 'i':
			Si5351A_clock_msynth(&si,clockx,IntegerMode);
			break;
		case 'I':
			Si5351A_clock_polarity(&si,clockx,true);
			break;
		case 'r':
			d = strtoul(optarg,0,10);
			rxdiv = ~0;
			for ( short x=0; x<8; ++x ) {
				if ( d == rxdivs[x].v ) {
					rxdiv = rxdivs[x].d;
					break;
				}
			}
			if ( rxdiv == ~0u ) {
				fprintf(stderr,"Invalid -r\n");
				exit(1);
			}
			Si5351A_msynth_div(&si,0,rxdiv);
			break;
		case 'X':
			Si5351A_clock_source(&si,clockx,XTAL_Source);
			break;
		case 'd':
			debugf = true;
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		default:
			fprintf(stderr,"Unknown option -%c\n",optch);
			exit(1);
		}
	}

	Si5351A_pll_reset(&si,pllx);
	Si5351A_clock_enable(&si,clockx,true);
	Si5351A_clock_enable_pin(&si,clockx,false);

	close(i2c_fd);
}

// End pi_gen.c
