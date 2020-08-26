/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_HDC1008_HDC1008_H_
#define ZEPHYR_DRIVERS_SENSOR_HDC1008_HDC1008_H_

#include <kernel.h>

#define HDC1008_I2C_ADDRESS	0x40

#define HDC1008_REG_TEMP	0x0
#define HDC1008_REG_HUMIDITY	0x1
#define HDC1008_REG_MANUFID	0xFE
#define HDC1008_REG_DEVICEID	0xFF

#define HDC1008_MANUFID		0x5449
#define HDC1008_DEVICEID	0x1000

struct hcsr04_data {
	struct device *gpio;
	//struct gpio_callback gpio_cb;
	u16_t t_sample;
	u16_t rh_sample;
	int trigger_galileo;
	int echo_galileo;
	int trigger_zephyr;
	int echo_zephyr;
	u32_t buffer;
	
	struct k_sem data_sem;
};

#endif
