/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
//#include <board.h>
#include <version.h>
#include <device.h>
#include <sensor.h>
#include <gpio.h>
#include <stdio.h>
#include <pwm.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <pinmux.h>
#include "../boards/x86/galileo/board.h"
#include "../boards/x86/galileo/pinmux_galileo.h"
#include "../drivers/gpio/gpio_dw_registers.h"
#include "../drivers/gpio/gpio_dw.h"
#include <shell/shell.h>
#include <inttypes.h>
#include <stdlib.h>
#define EDGE_FALLING    (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW)
#define EDGE_RISING	(GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH)
#define PULL_UP 	0	/* change this to enable pull-up/pull-down */
#define SLEEP_TIME	1000	/* Sleep time */
struct device *dev_cmd,*dev_cmd_1;
struct k_sem my_sem, my_sem_1;
struct k_sem c_sem;
struct k_sem d_sem;
int user_count;
char selected_sensor;
u32_t time1,time2;
K_SEM_DEFINE(my_sem, 1, 1);
K_SEM_DEFINE(my_sem_1, 0, 1);
K_SEM_DEFINE(c_sem, 0, 1); // high prioity thread's semaphore is initially not available
K_SEM_DEFINE(d_sem, 1, 1); // low prioity thread's semaphore is initially available
struct device  *pwm0;
struct device *gpiob_1;
struct measurements{ 
        u32_t measured_distance;
        u32_t measured_timestamp;
        };
struct measurements value[256];
int distance_count = 0;
int count = 0;
static struct gpio_callback gpio_cb;


static int cmd_select(const struct shell *shell, size_t argc, char **argv){
    printk("the number of arguments passed is %zd\n",argc);
    printk("the value of argument is %s\n",argv[1]);
    if (atoi(argv[1])==0){
        printk("No sensor is enabled \n");
    }
    else if (atoi(argv[1])==1){
        printk("Sensor 1 selected \n");
        dev_cmd = device_get_binding("HCSRO4_1");
    }
    else if (atoi(argv[1])==2){
        printk("Sensor 2 selected \n");
        dev_cmd = device_get_binding("HCSRO4_2");
    }
    else{
        printk("invalid sensor\n");
    }
   
    

    if (dev_cmd == NULL) {
		printk("Could not get sensor device\n");
		return 0;
    }
 
    printk("dev %p name %s\n", dev_cmd, dev_cmd->config->name);

    sensor_sample_fetch(dev_cmd);
    sensor_channel_get(dev_cmd, SENSOR_CHAN_DISTANCE, &val);
    return 0;
}

static int cmd_start(const struct shell *shell, size_t argc, char **argv){
    if((atoi(argv[1]))>256){
        printk("The number of measurement should be less than 256\n"); 
        
    }
    else{
        user_count=atoi(argv[1]);
        
        struct sensor_value val;
        for (int i=0;i<atoi(argv[1]);i++){ 
            printk("while loop entered\n");
            sensor_sample_fetch(dev_cmd);  
            value[i].measured_distance=sensor_channel_get(dev_cmd, SENSOR_CHAN_DISTANCE, &val);
            value[i].measured_timestamp=SYS_CLOCK_HW_CYCLES_TO_NS(k_cycle_get_32());
            sensor_sample_fetch(dev_cmd);
            k_sleep(500); 
       
        } 
    printk("Distance measurement over\n");
    }

    return 0;


}

static int cmd_dump(const struct shell *shell, size_t argc, char **argv){
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    for (int i=0;i<user_count;i++){
        printk("Distance %d: %"PRIu32" centimeters, Timestamp: %"PRIu32"\n",i+1, value[i].measured_distance,value[i].measured_timestamp);
    }
    return 0;
}



SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo,
        SHELL_CMD_ARG(Select_HCSR,NULL,"select.",cmd_select,2,0),
        SHELL_CMD_ARG(Start_HCSR,NULL,"start.",cmd_start,2,0),
        SHELL_CMD_ARG(Dump_HCSR,NULL,"dump.",cmd_dump,1,0),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);


SHELL_CMD_REGISTER(demo, &sub_demo, "Demo commands", NULL); //parent shell command demo



static struct device *pinmux;  

void main(void)
{
	
        printk("passing over to shell\n");

}
