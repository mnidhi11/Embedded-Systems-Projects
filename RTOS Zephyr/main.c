/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hcsr04.h"
#include <zephyr.h>
#include <version.h>
#include <device.h>
#include <i2c.h>
#include <gpio.h>
#include <stdio.h>
#include <pwm.h>
#include <kernel.h>
#include <sensor.h>
#include <pinmux.h>
#include "../boards/x86/galileo/board.h"
#include "../boards/x86/galileo/pinmux_galileo.h"
#include "../drivers/gpio/gpio_dw_registers.h"
#include "../drivers/gpio/gpio_dw.h"
#include <inttypes.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <misc/__assert.h>
#include <logging/log.h>
static int ret;
#define EDGE_FALLING    (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW)
#define EDGE_RISING	(GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH)
#define PULL_UP 	0	/* change this to enable pull-up/pull-down */
#define SLEEP_TIME	1000	/* Sleep time */
#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL
int count_1=0;
//static struct gpio_callback gpio_cb;
LOG_MODULE_REGISTER(HCSR04);

static struct device *pinmux;



static int hcsr04_sample_fetch(struct device *dev, enum sensor_channel chan)
{
	printk("Sample fetch called");
	return 0;
}



static int hcsr04_channel_get(struct device *dev,enum sensor_channel chan,struct sensor_value *val)
{
	printk("channel get called");
	return 0;
}

static int hcsr04_init(struct device *dev)
{
	
	printk("Init called");
	pinmux=device_get_binding(CONFIG_PINMUX_NAME);
	struct hcsr04_data *drv_data = dev->driver_data;

	struct galileo_data *dev_1 = pinmux->driver_data;

	if (strcmp(dev->config->name,"HCSR04_1")==0)
	{

	printk("HCSR04_1 initialized");
	drv_data->trigger_galileo = CONFIG_Trigger_g_1;
	drv_data->echo_galileo= CONFIG_Echo_g_1;
	drv_data->trigger_zephyr=CONFIG_Trigger_z_1;
	drv_data->echo_zephyr = CONFIG_Echo_z_1;
	drv_data->gpio=dev_1->gpio_dw;
    if (!drv_data->gpio) {
        printk("error for gpio_dw for sensor1\n");
        return 0;
    }

	ret=pinmux_pin_set(pinmux,drv_data->trigger_galileo,PINMUX_FUNC_A);
	if(ret<0)
	printk("Error for IO1\n");
    ret=pinmux_pin_set(pinmux,drv_data->echo_galileo,PINMUX_FUNC_B); 	//IO3 for echo i.e. handling interrupt for sensor 1
    if(ret<0)
	printk("Error for IO3\n");

	}


	else if (strcmp(dev->config->name,"HCSR04_2")==0)
	{
	printk("HCSR04_2 initialized");
	drv_data->trigger_galileo = CONFIG_Trigger_g_2;
	drv_data->echo_galileo= CONFIG_Echo_g_2;
	drv_data->trigger_zephyr=CONFIG_Trigger_z_2;
	drv_data->echo_zephyr = CONFIG_Echo_z_2;
	drv_data->gpio=dev_1->gpio_dw;
    if (!drv_data->gpio) {
        printk("error for gpio_dw for sensor2\n");
        return 0;
    }
	ret=pinmux_pin_set(pinmux,drv_data->trigger_galileo,PINMUX_FUNC_A);
	if(ret<0)
	printk("Error for IO0\n");
    ret=pinmux_pin_set(pinmux,drv_data->echo_galileo,PINMUX_FUNC_B); 	//IO3 for echo i.e. handling interrupt for sensor 1
    if(ret<0)
	printk("Error for IO2\n");

	}


}



static const struct sensor_driver_api hcsr04_driver_api = {
	.sample_fetch = hcsr04_sample_fetch,
	.channel_get = hcsr04_channel_get,
};


static struct hcsr04_data hcsr04_data1;

DEVICE_AND_API_INIT(hcsr04_sen1, "HCSR04_1",hcsr04_init, &hcsr04_data1,
		    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &hcsr04_driver_api);


/*DEVICE_AND_API_INIT(hcsr04_sen2 "HCSR04_2",hcsr04_init, &hcsr004_data,
		    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &hcsr04_driver_api);*/
