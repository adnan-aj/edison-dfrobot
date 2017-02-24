/*
 * Adnan's Intel Edison 4 wheel drive xbox controlled rover
 *
 * adnan@singnet.com.sg
 *
 * uses pwm0 - pwm3 for motor speeds
 * uses gpio128, gpio129, gpio48, gpio49 for motor directions
 *
 * Nice joystick-to-differential drive reference:
 * http://www.phidgets.com/docs/Mobile_Robot_(MURVV)#Calculate_Wheel_Speeds
 *
 * uses evtest.c for debugging info
 *
 * gcc -o xbox-4wd evtest.c xbox-4wd.c
 *
 */


#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>

extern char *events[];
extern char **names[];
extern char *absval[];

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

int open_xbox()
{
	int fd, i;
	char devname[64];
	char name[256];
	
	for (i = 0; i < 64; i++) {
		sprintf(devname, "/dev/input/event%d", i);
		printf("Testing %s...  ", devname);
		if ((fd = open(devname, O_RDONLY)) >= 0) {
			strcpy(name, "Unknown");
			ioctl(fd, EVIOCGNAME(sizeof(name)), name);
			printf("found: %s\n", name);
			if (strcasestr(name, "xbox")) {
				printf("Using this one!\n");
				return fd;
			}
			close(fd);
		}
	}
	return 0;
}

int fd_pwm[4];
int fd_dir[4];
int gpio_dir[] = {128, 129, 48, 49};
int motor_open()
{
    int i;
    char devname[128];
#if 0
    for (i = 0; i < 4; i++) {
	sprintf(devname, "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", i);
	printf("Opening %s...\n", devname);
	if ((fd_pwm[i] = open(devname, O_WRONLY)) < 0) {
	    printf("failed\n");
	}
	else {
	    printf("successful\n");
	}
	
	sprintf(devname, "/sys/class/gpio/gpio%d/value", gpio_dir[i]);
	printf("Opening %s...", devname);
	if ((fd_dir[i] = open(devname, O_WRONLY)) < 0) {
	    printf("failed\n");
	}
	else {
	    printf("successful\n");
	}
    }
#endif
    int fd; /* File descriptor for the port */
    
    
    fd = open("/dev/ttyMFD1", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
	/*
	 * Could not open the port.
	 */
	
	perror("open_port: Unable to open /dev/ttyf1 - ");
    }
    else
	fcntl(fd, F_SETFL, 0);
    
    return (fd);
}

int motor_speed(int motor, int speed)
{
	int dir = 0;
	char bintext[] = "01";
	char textval[16];

	if ((motor < 0) || (motor > 3))
		return 1;
	if (speed < 0) dir = 1;
	write(fd_dir[motor], &bintext[dir], 1);	
	speed = abs(speed);
	if (speed > 100000) speed = 100000;
	snprintf(textval, 16,"%d", speed);
	write(fd_pwm[motor], textval, strlen(textval));
	return 0;
}

void motor_close()
{
	int i;
	char pwmzero[] = "0";
	char bintext[] = "01";
	for (i = 0; i < 4; i++) {
		write(fd_pwm[i], pwmzero, strlen(pwmzero));
		write(fd_dir[i], &bintext[0], 1);	
		close(fd_pwm[i]);
		close(fd_dir[i]);
	}
}	
		
void print_event(char *typename, struct input_event ev)
{
	printf("%sEvent: time %ld.%06ld, type %d (%s), code %d (%s), value %d\n",
		typename, ev.time.tv_sec, ev.time.tv_usec, ev.type,
		events[ev.type] ? events[ev.type] : "?",
		ev.code,
		names[ev.type] ? (names[ev.type][ev.code] ? names[ev.type][ev.code] : "?") : "?",
		ev.value);
}

int joystickX = 0;
int joystickY = 0;
int motorA_setting = 0;
int motorB_setting = 0;


static volatile int interrupted = 0;

void int_handler(int dummy) {
    interrupted = 1;
}

int main (int argc, char **argv)
{
    int fd, rd, i, j, k, mfd;
    struct input_event ev[64];
    int version;
    unsigned short id[4];
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
    char name[256] = "Unknown";
    int abs[5];
    int btnselect = 0, btnstart = 0, btnmode = 0, xboxtext = 0;
    int btnA = 0, btnB = 0;
    char textval[16];
    bool motor_update = false, run = true;;
    FILE *mfp;
    char outbuf[80];
    
    
    if ((fd = open_xbox()) == 0) {
	printf("Xbox driver not found fd = %d\n", fd);
	return 1;
    }
#if 1    
    if ((mfd = motor_open()) < 0) {
	printf("Hercules motor serial port cannot open fd = %d\n", mfd);
	return 1;
    }
#else
    if ((mfp = fopen("/dev/ttyMFD1", "rw")) == NULL) {
	printf("Hercules motor serial port cannot open fd = %d\n", mfd);
	return 1;
    }
#endif	
    
    
    if (ioctl(fd, EVIOCGVERSION, &version)) {
	perror("evtest: can't get version");
	return 1;
    }
    
    printf("Input driver version is %d.%d.%d\n",
	   version >> 16, (version >> 8) & 0xff, version & 0xff);
    
    ioctl(fd, EVIOCGID, id);
    printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
	   id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);
    
    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("Input device name: \"%s\"\n", name);
    signal(SIGINT, int_handler);
    printf("Testing ... (interrupt to exit)\n");
    
    while (run) {
	rd = read(fd, ev, sizeof(struct input_event) * 64);
	
	if (rd < (int) sizeof(struct input_event)) {
	    printf("yyy\n");
	    perror("\nevtest: error reading");
	    return 1;
	}
	
	for (i = 0; i < rd / sizeof(struct input_event); i++) {
	    
	    if (ev[i].type == EV_SYN) {
		if (xboxtext)
		    print_event("Sync", ev[i]);
		
	    } else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].		code == MSC_SCAN)) {
		if (xboxtext)
		    print_event("MSC_", ev[i]);
		
	    } else {
		if (xboxtext)
		    print_event("Else", ev[i]);
		
		if (ev[i].type == EV_KEY && ev[i].code == BTN_SELECT) {
		    btnselect = ev[i].value;
		    printf("Interesting %d!!!\n", btnselect);
		    if (btnselect == 0)
			xboxtext = 1 - xboxtext;
		}
		if (ev[i].type == EV_KEY && ev[i].code == BTN_START) {
		    btnstart = ev[i].value;
		    printf("Interesting %d!!!\n", btnstart);
		}

		if (ev[i].type == EV_ABS && ev[i].code == ABS_RX) {
		    //motorB_setting = -ev[i].value w/ 64;
		    joystickX = -ev[i].value;
		    motor_update = true;
		}

		if (ev[i].type == EV_ABS && ev[i].code == ABS_Y) {
		    //motorA_setting = -ev[i].value / 64;
		    joystickY = -ev[i].value;
		    motor_update = true;
		}
		
		
		if ((ev[i].type == EV_KEY && ev[i].code == BTN_MODE) || interrupted) {
		    btnmode = ev[i].value;
		    printf("Interesting %d!!!\n", btnmode);
		    if (btnmode && btnselect) {
			printf("Ending program\n");
			joystickX = 0;
			joystickY = 0;
			motor_update = true;
			run = 0;
		    }
		}
		
		if (motor_update) {
		    diff_steering(joystickX, joystickY);
		    sprintf(outbuf, "mot %d %d\n", motorA_setting, motorB_setting);
		    printf("%s", outbuf);
		    write(mfd, outbuf, strlen(outbuf));
		    motor_update = false;
		}
		
	    }	
	}
    } //endwhile(run)
    motor_close();
}

int diff_steering(int joyX, int joyY)
{
    // Differential Steering Joystick Algorithm
    // ========================================
    //   by Calvin Hass
    //   http://www.impulseadventure.com/elec/
    //
    // Converts a single dual-axis joystick into a differential
    // drive motor control, with support for both drive, turn
    // and pivot operations.
    //
    
    // INPUTS
    int     nJoyX = joyX / 256;              // Joystick X input                     (-128..+127)
    int     nJoyY = joyY / 256;              // Joystick Y input                     (-128..+127)

    // OUTPUTS
    int     nMotMixL;           // Motor (left)  mixed output           (-128..+127)
    int     nMotMixR;           // Motor (right) mixed output           (-128..+127)

    // CONFIG
    // - fPivYLimt  : The threshold at which the pivot action starts
    //                This threshold is measured in units on the Y-axis
    //                away from the X-axis (Y=0). A greater value will assign
    //                more of the joystick's range to pivot actions.
    //                Allowable range: (0..+127)
    float fPivYLimit = 32.0;
    
    // TEMP VARIABLES
    float   nMotPremixL;    // Motor (left)  premixed output        (-128..+127)
    float   nMotPremixR;    // Motor (right) premixed output        (-128..+127)
    int     nPivSpeed;      // Pivot Speed                          (-128..+127)
    float   fPivScale;      // Balance scale b/w drive and pivot    (   0..1   )
    
    
    // Calculate Drive Turn output due to Joystick X input
    if (nJoyY >= 0) {
	// Forward
	nMotPremixL = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
	nMotPremixR = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
    } else {
	// Reverse
	nMotPremixL = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
	nMotPremixR = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
    }
    
    // Scale Drive output due to Joystick Y input (throttle)
    nMotPremixL = nMotPremixL * nJoyY/128.0;
    nMotPremixR = nMotPremixR * nJoyY/128.0;
    
    // Now calculate pivot amount
    // - Strength of pivot (nPivSpeed) based on Joystick X input
    // - Blending of pivot vs drive (fPivScale) based on Joystick Y input
    nPivSpeed = nJoyX;
    fPivScale = (abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - abs(nJoyY)/fPivYLimit);
    
    // Calculate final mix of Drive and Pivot
    nMotMixL = (1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed);
    nMotMixR = (1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed);
    
    // Convert to Motor PWM range
    // ...
    motorB_setting = nMotMixL;
    motorA_setting = nMotMixR;
}