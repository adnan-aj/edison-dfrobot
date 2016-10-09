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
	
	return 0;
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

int main (int argc, char **argv)
{
	int fd, rd, i, j, k;
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	char name[256] = "Unknown";
	int abs[5];
	int btnselect = 0, btnstart = 0, btnmode = 0, run = 1, xboxtext = 0;
	int btnA = 0, btnB = 0;
	char textval[16];
	FILE *fp_pwm0;
	int fd_pwm0;

	if ((fd = open_xbox()) == 0) {
		printf("Xbox driver not found fd = %d\n", fd);
		return 1;
	}
	
	motor_open();
#if 0	
//	if ((fp_pwm0 = fopen("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", "w")) == NULL)
	if ((fd_pwm0 = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY)) < 0)
	{
		printf("Can't open pwm0\n");
	}
	else
		printf("pwm0 open successfully\n");
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

#if 0
	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	printf("Supported events:\n");

	for (i = 0; i < EV_MAX; i++)
		if (test_bit(i, bit[0])) {
			printf("  Event type %d (%s)\n", i, events[i] ? events[i] : "?");
			if (!i) continue;
			ioctl(fd, EVIOCGBIT(i, KEY_MAX), bit[i]);
			for (j = 0; j < KEY_MAX; j++) 
				if (test_bit(j, bit[i])) {
					printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
					if (i == EV_ABS) {
						ioctl(fd, EVIOCGABS(j), abs);
						for (k = 0; k < 5; k++)
							if ((k < 3) || abs[k])
								printf("      %s %6d\n", absval[k], abs[k]);
					}
				}
		}
#endif

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
				if (ev[i].type == EV_KEY && ev[i].code == BTN_MODE) {
					btnmode = ev[i].value;
					printf("Interesting %d!!!\n", btnmode);
					if (btnmode && btnselect) {
						printf("Ending program\n");
						run = 0;
					}
				}

				if (ev[i].type == EV_ABS && ev[i].code == ABS_Y) {
					motor_speed(0, ev[i].value * 2);
				}
				
				if (ev[i].type == EV_ABS && ev[i].code == ABS_RY) {
					motor_speed(1, ev[i].value * 2);
				}
				
			}	
		}
	} //endwhile(run)
		motor_close();
}
