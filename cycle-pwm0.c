/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>

#include "mraa.h"

int
main()
{
    const char* board_name = mraa_get_platform_name();
    printf("hello mraa\n Version: %s\n Running on %s\n",
	   mraa_get_version(), board_name);

    printf("Hello, world 0\n");

   mraa_init();
    //! [Interesting]
    mraa_pwm_context pwm0, pwm1, pwm2, pwm3;
    
    //    http://qiita.com/yoneken/items/ee0735cb42239750ee8b
    // The physical pin mapping is defined at 
    // https://github.com/intel-iot-devkit/mraa/blob/master/docs/edison.md
    //
    // For example,
    // MRAA Number(20) => Physical Pin(J18-7)

    pwm0 = mraa_pwm_init(20);
        mraa_pwm_period_us(pwm0, 200);
    mraa_pwm_enable(pwm0, 1);

   
   printf("Hello, world 1\n");

   if (pwm0 == NULL) {
      printf("pwm0 cannot init\n");
        return 1;
    }
   
    int period = 256;

   printf("Hello, world 2\n");
   
   
   printf("Hello, world 3\n");
   mraa_pwm_period_us(pwm0, period);
      printf("Hello, world 4\n");

   mraa_pwm_enable(pwm0, 1);
   
    float value = 0.5f;


   
   while (1) {
        value = value + 0.01f;
        //mraa_pwm_write(pwm, value);

#if 0
       mraa_pwm_write(pwm0,(int)(value * period));
#else  
       	mraa_pwm_pulsewidth_us(pwm0, (int)(value * period));
#endif  

        usleep(50000);
        if (value >= 0.99f) {
            value = 0.0f;
        }
        float output = mraa_pwm_read(pwm0);
	printf("pwm 0 = %f %f\n", value, output);
    }
    //! [Interesting]
    return 0;
}
