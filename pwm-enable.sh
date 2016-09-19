#!/bin/bash

echo out > /sys/kernel/debug/gpio_debug/gpio12/current_direction
echo out > /sys/kernel/debug/gpio_debug/gpio13/current_direction
echo out > /sys/kernel/debug/gpio_debug/gpio182/current_direction
echo out > /sys/kernel/debug/gpio_debug/gpio183/current_direction

echo mode1 > /sys/kernel/debug/gpio_debug/gpio12/current_pinmux
echo mode1 > /sys/kernel/debug/gpio_debug/gpio13/current_pinmux
echo mode1 > /sys/kernel/debug/gpio_debug/gpio182/current_pinmux
echo mode1 > /sys/kernel/debug/gpio_debug/gpio183/current_pinmux

echo 0 > /sys/class/pwm/pwmchip0/export
echo 1 > /sys/class/pwm/pwmchip0/export
echo 2 > /sys/class/pwm/pwmchip0/export
echo 3 > /sys/class/pwm/pwmchip0/export

echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable
echo 1 > /sys/class/pwm/pwmchip0/pwm1/enable
echo 1 > /sys/class/pwm/pwmchip0/pwm2/enable
echo 1 > /sys/class/pwm/pwmchip0/pwm3/enable

echo 100000 > /sys/class/pwm/pwmchip0/pwm0/period
echo 100000 > /sys/class/pwm/pwmchip0/pwm1/period
echo 100000 > /sys/class/pwm/pwmchip0/pwm2/period
echo 100000 > /sys/class/pwm/pwmchip0/pwm3/period

echo 10000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle

chown -R :i2c /sys/class/pwm/pwmchip0/pwm?
chmod -R g+w  /sys/class/pwm/pwmchip0/pwm?

