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

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "si5351a.h"

static const char *i2cbus = "/dev/i2c-1";
static int i2c_fd = -1;

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
	printf("%02X [ ",i2c_addr);
	for ( uint8_t x=0; x<bytes; ++x )
		printf(" %02X",buf[x]);
	printf(" ] (%d)\n",(int)bytes);

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
	if ( bytes ==1 ) 
		printf("%02X [ W r%d ]\n",i2c_addr,buf[0]);
	else	{
		printf("%02X [ W r%d :",i2c_addr,buf[0]);
		for ( uint8_t x=1; x<bytes; ++x )
			printf(" %02X",buf[x]);
		printf(" ] (%d)\n",(int)bytes);
	}
	assert(rc == 1);
	return rc;
}

static void
dump_regs(uint8_t reg,uint8_t n) {
	uint8_t sbuf[16];

	sbuf[0] = reg;
	writecb(0x60,sbuf,1);
	readcb(0x60,sbuf,n);
}

void
dump_all() {

	puts("Register Dump:");
	dump_regs(0,16);
	dump_regs(16,3);
	dump_regs(24,1);
	dump_regs(26,8);
	dump_regs(34,8);
	dump_regs(42,8);
	dump_regs(50,8);
	dump_regs(149,13);
	dump_regs(162,3);
	dump_regs(165,3);
	dump_regs(177,1);
	dump_regs(183,1);
}

int
main(int argc,char **argv) {
	Si5351A si;

	i2c_fd = open(i2cbus,O_RDWR);
	if ( i2c_fd < 0 ) {
		fprintf(stderr,"%s: opening %s\n",strerror(errno),i2cbus);
		exit(1);
	}

	Si5351A_init(&si,0x60,readcb,writecb,&si,Cap6pF);
	
	puts("Initialized, enabling\n");

	Si5351A_clock_power(&si,Clock0,true);
	Si5351A_clock_insrc(&si,Clock0,MSynth_Source);
	Si5351A_clock_msynth(&si,Clock0,FractionalMode);
	Si5351A_set_msynth(&si,0,36,0,1048575);
	Si5351A_set_pll(&si,0,28,838860,1048575);
	Si5351A_msynth_div(&si,0,RxDiv1);
	Si5351A_clock_pll(&si,Clock0,PLLA);
	Si5351A_pll_reset(&si,PLLA);
	Si5351A_clock_enable(&si,Clock0,true);
	Si5351A_clock_enable_pin(&si,Clock0,false);

	usleep(10);
	dump_all();

#if 0
	time_t t0, t1;
	bool a=false, b=false, f;

	t0 = time(0);
	while ( (t1 = time(0)) - t0 < 10 ) {
		if ( (f=Si5351A_is_lol(&si,PLLA)) != a ) {
			a = f;
			printf("LOL PLLA: %d\n",a);
		}
		if ( (f=Si5351A_is_lol(&si,PLLB)) != a ) {
			b = f;
			printf("LOL PLLB: %d\n",b);
		}
		usleep(1000);
	}
#endif
	close(i2c_fd);
}

// End pi_gen.c
