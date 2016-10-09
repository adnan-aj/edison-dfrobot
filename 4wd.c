#include <stdio.h>
#include <string.h>

#define GPIO_PATH "/sys/class/gpio/gpio"
#define PWM_PATH  "/sys/class/pwm/pwmchip0/pwm"

FILE *gpio48p, *gpio49p, *gpio128, *gpio129p;
FILE *pwm0p, *pwm1p, *pwm2p, *pwm3pl;
char fpname[sizeof(GPIO_PATH) + 16];
char textval[16];


int main(int argc, char *argv[])
{
    printf("Hello, argc = %d, argv[1] = $%s\n", argc, argv[1]);
    
    sprintf(fpname, "%s%d/value", GPIO_PATH, 48);
    gpio48p = fopen(fpname, "r");
    fgets(textval, sizeof(textval), gpio48p);
    printf("gpio%d at %s = %s\n", 48, fpname, textval);
    return 0;
}
