#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "servocontroller.h"

Servocontroller::Servocontroller(){
	channel = 0;
	//targets[] = {6000, 8000};


	printf("Please start with %s /dev/ttyS1 (for example)\n","/dev/ttyUSB0");
	memset(&stdio,0,sizeof(stdio));
	stdio.c_iflag=0;
	stdio.c_oflag=0;
	stdio.c_cflag=0;
	stdio.c_lflag=0;
	stdio.c_cc[VMIN]=1;
	stdio.c_cc[VTIME]=0;

	tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
																					tio.c_lflag=0;								        tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	tty_fd=open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
	cfsetospeed(&tio,B115200);            // 115200 baud
	cfsetispeed(&tio,B115200);            // 115200 baud

	tcsetattr(tty_fd,TCSANOW,&tio);

}

Servocontroller::~Servocontroller(){
	close(tty_fd);
}




void Servocontroller::set_target(unsigned char channel, unsigned short target) {
	unsigned char command[]  = {0xAA, 0xC, 0x04, channel, target & 0x7F, target >> 7 & 0x7F};
	if(write(tty_fd, command, sizeof(command)) < sizeof(command))
	{
		printf("error writing\n");
	}
}

void Servocontroller::set_speed(unsigned char channel, unsigned short speed) {
    unsigned char command[] = {0xAA, 0xC, 0x07, channel,
			       speed & 0x7F, speed >> 7 & 0x7F };
    if(write(tty_fd,command, sizeof(command)) < sizeof(command))
    {
	printf("error writing\n");
    }
}

void Servocontroller::set_acceleration(unsigned char channel, unsigned short acc) {
    unsigned char command[] = {0xAA, 0xC, 0x09, channel,
			       acc & 0x7F, acc >> 7 & 0x7F };
    if(write(tty_fd,command, sizeof(command)) < sizeof(command))
    {
	printf("error writing\n");
    }
}
	// untested
void Servocontroller::set_pwm(unsigned char channel, unsigned short on_time, unsigned short period) {
	unsigned char command[] = {0xAA, 0xC, 0x0A, channel,
				   on_time & 0x7F, on_time >> 7 & 0x7F,
				   period & 0x7F, period >> 7 & 0x7F };
	if(write(tty_fd,command, sizeof(command)) < sizeof(command))
	{
	    printf("error writing\n");
	}
}

int Servocontroller::get_position(unsigned char channel) {
    unsigned char command[] = {0xAA, 0xC, 0x10, channel};
    unsigned char response[2];
    if(write(tty_fd,command, sizeof(command)) < sizeof(command))
    {
	printf("error writing\n");
	return -1;
    }
	    // todo: sig_rec->wait_for_singnal()
    sleep(1);
	    if(read(tty_fd,response, sizeof(response)) != 2)
    {
	printf("error reading\n");
	return -1;
    }
	    return response[0] + 256*response[1];
}

unsigned char Servocontroller::get_moving_state(void) {
    unsigned char command[] = {0xAA, 0xc, 0x13};
    unsigned char response;
    if(write(tty_fd,command, sizeof(command)) < sizeof(command))
    {
	printf("error writing\n");
	return -1;
    }
	    // todo: sig_rec->wait_for_singnal()
    sleep(1);
	    if(read(tty_fd,&response, 1) != 1)
    {
	printf("error reading\n");
	return -1;
    }
    return response;
}


