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
	printf("%02X [ R %02X ]\n",i2c_addr,buf[0]);

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
	Si5351A_clock_pll(&si,Clock0,PLLA);
	Si5351A_clock_msynth(&si,Clock0,FractionalMode);
	Si5351A_set_pll(&si,0,24,0,1048575);
	Si5351A_set_msynth(&si,0,90,0,1048575);
	Si5351A_msynth_div(&si,0,RxDiv4);
	Si5351A_clock_enable(&si,Clock0,true);
	Si5351A_clock_enable_pin(&si,Clock0,false);

	close(i2c_fd);
}

// End pi_gen.c
