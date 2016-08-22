#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


class Servocontroller{
public:
	Servocontroller();
	~Servocontroller();

	unsigned char channel;
	//unsigned short targets[];


	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;

	void set_target(unsigned char channel, unsigned short target);

	void set_speed(unsigned char channel, unsigned short speed);

	void set_acceleration(unsigned char channel, unsigned short acc);

	void set_pwm(unsigned char channel, unsigned short on_time, unsigned short period);

	int get_position(unsigned char channel);

	unsigned char get_moving_state(void);




private:
	int tty_fd;
};


