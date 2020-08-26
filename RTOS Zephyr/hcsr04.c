/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
int flag=0;
int status=0;
struct device *selected_dev;
u32_t time_1,time_2,temp_buffer;
struct hcsr04_data *drv_data_1,*drv_data_2;
#include "hcsr04.h"
//K_SEM_DEFINE(my_sem_2, 1, 1);
//K_SEM_DEFINE(my_sem_3, 0, 1);
//K_SEM_DEFINE(my_sem_4, 0, 1);
#define EDGE_FALLING    (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW)
#define EDGE_RISING	(GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH)
#define PULL_UP 	0	/* change this to enable pull-up/pull-down */
#define SLEEP_TIME	1000	/* Sleep time */
#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL
int count_1=0;
static struct gpio_callback gpio_cb;
LOG_MODULE_REGISTER(HCSR04);

/*static int hdc1008_init(struct device *dev){
struct hcsr04_data *drv_data = dev->driver_data;
}
static struct hcsr04_data hcsr04_data;*/

int trigger_pin_1,echo_pin_1,trigger_pin_2,echo_pin_2;
static struct device *pinmux;
struct device *gpiob_2;
int ret_status;

void echo_1 (struct device *gpiob, struct gpio_callback *cb,
		    u32_t pins)
{
    struct hcsr04_data *drv_data_3=selected_dev->driver_data;
    if(count_1%2==0){
        if(k_sem_take(&(drv_data_3->my_sem_2), K_FOREVER) != 0) {     /*Waiting forever without timeout to take the semaphore*/
            printk("Input data not available to be sent!\n");
	 }
         time_1=SYS_CLOCK_HW_CYCLES_TO_NS(k_cycle_get_32());
         
         ret_status=gpio_pin_configure(drv_data_3->gpio, drv_data_3->zephyr_echo,
			   PULL_UP| GPIO_DIR_IN | GPIO_INT | EDGE_FALLING);

	 gpio_init_callback(&(drv_data_3->gpio_cb), echo_1, BIT(drv_data_3->zephyr_echo));

	 ret_status=gpio_add_callback(drv_data_3->gpio, &(drv_data_3->gpio_cb));

	 if(ret_status<0){
		     printk("error adding callback\n");
         }
	
	ret_status=gpio_pin_enable_callback(drv_data_3->gpio, drv_data_3->zephyr_echo);

	if(ret_status<0){
            printk("error enabling callback\n");
        }

        k_sem_give(&(drv_data_3->my_sem_3));
        printk("echo rising\n");
        printk("time1 is %"PRIu32" centimeters\n",time_1);
        count_1++;
      //  k_sem_give(&(drv_data_3->my_sem_3));
    }

    else{
         if(k_sem_take(&(drv_data_3->my_sem_3), K_FOREVER) != 0) {     /*Waiting forever without timeout to take the semaphore*/
	          printk("Input data not available to be sent!\n");
	      }
         time_2=SYS_CLOCK_HW_CYCLES_TO_NS(k_cycle_get_32());
         ret_status=gpio_pin_configure(drv_data_3->gpio, drv_data_3->zephyr_echo,
			   PULL_UP| GPIO_DIR_IN | GPIO_INT | EDGE_RISING);

	 gpio_init_callback(&(drv_data_3->gpio_cb), echo_1, BIT(drv_data_3->zephyr_echo));

	 ret_status=gpio_add_callback(drv_data_3->gpio, &(drv_data_3->gpio_cb));

	 if(ret_status<0){
		printk("error adding callback\n");
         }
	
	 ret_status=gpio_pin_enable_callback(drv_data_3->gpio, drv_data_3->zephyr_echo);

	 if(ret_status<0){
		printk("error enabling callback\n");
         }
  
         printk("echo falling\n");
         printk("time2 is %"PRIu32" centimeters\n",time_2);
         drv_data_3->buffer=(time_2-time_1)/58000;
         drv_data_3->status=0; //reset status
         printk("the distance of object from the sensor is %"PRIu32" centimeters\n",drv_data_3->buffer);
         
         count_1++;

         k_sem_give(&(drv_data_3->my_sem_2));
         
         k_sem_give(&(drv_data_3->my_sem_4));
    }

    printk("returning from trigger thread\n");
    return ;
             
}

static int hcsr04_sample_fetch(struct device *dev, enum sensor_channel chan)

{

printk("Distance measurement begins\n");
drv_data_1->buffer=0;
drv_data_2->buffer=0;
selected_dev=dev;
if(strcmp(dev->config->name,"HCSRO4_1") == 0){

        
        
/*----------------configuring gpio 6 for IO3----------*/ 
        ret_status=gpio_pin_configure(drv_data_1->gpio, drv_data_1->zephyr_echo, //https://docs.zephyrproject.org/apidoc/latest/group__gpio__interface_ga132f154ab93f0dba11649db0ab80dd5c.html
			   PULL_UP| GPIO_DIR_IN | GPIO_INT | EDGE_RISING);
	
	gpio_init_callback(&(drv_data_1->gpio_cb), echo_1, BIT(drv_data_1->zephyr_echo));

	ret_status=gpio_add_callback(drv_data_1->gpio, &(drv_data_1->gpio_cb));
	if(ret_status<0)
		printk("error adding callback\n");
	
	ret_status=gpio_pin_enable_callback(drv_data_1->gpio, drv_data_1->zephyr_echo);
	if(ret_status<0)
		printk("error enabling callback\n");

/*-------------------enabling the trigger -----------*/
        if(k_sem_take(&(drv_data_1->my_sem_2), K_FOREVER) != 0) {     /*Waiting forever without timeout to take the semaphore*/
		printk("Input data not available to be sent!\n");
	    }
            gpio_pin_write(drv_data_1->gpio, drv_data_1->zephyr_trigger, 1);
            k_sleep(0.01);
            drv_data_1->status=1;  //status is busy
            printk("trigger generated\n");

            gpio_pin_write(drv_data_1->gpio, drv_data_1->zephyr_trigger, 0);
            k_sem_give(&(drv_data_1->my_sem_2));
}
else if(strcmp(dev->config->name,"HCSRO4_2") == 0){

        ret_status=gpio_pin_configure(drv_data_2->gpio, drv_data_2->zephyr_echo,
			   PULL_UP| GPIO_DIR_IN | GPIO_INT | EDGE_RISING);

/*----------------configuring gpio 6 for IO3----------*/ 
	
	gpio_init_callback(&(drv_data_2->gpio_cb), echo_1, BIT(drv_data_2->zephyr_echo));

	ret_status=gpio_add_callback(drv_data_2->gpio, &(drv_data_2->gpio_cb));
	if(ret_status<0)
		printk("error adding callback\n");
	
	ret_status=gpio_pin_enable_callback(drv_data_2->gpio, drv_data_2->zephyr_echo);
	if(ret_status<0)
		printk("error enabling callback\n");

/*-------------------enabling the trigger -----------*/
        if(k_sem_take(&(drv_data_2->my_sem_2), K_FOREVER) != 0) {     /*Waiting forever without timeout to take the semaphore*/
		printk("Input data not available to be sent!\n");
	    }
            gpio_pin_write(drv_data_2->gpio, drv_data_2->zephyr_trigger, 1);
            k_sleep(0.01);
            drv_data_2->status=1;  //status is busy
            printk("trigger generated\n");

            gpio_pin_write(drv_data_2->gpio, drv_data_2->zephyr_trigger, 0);
            k_sem_give(&(drv_data_2->my_sem_2));
} 

return 0;

}

static int hcsr04_channel_get(struct device *dev,enum sensor_channel chan,struct sensor_value *val)
{
struct hcsr04_data *drv_data_4=dev->driver_data;
    if (drv_data_4->buffer !=0){
        temp_buffer= drv_data_4->buffer;
        drv_data_4->buffer=0;
        return temp_buffer;
        
    }
    else{
        
        if(drv_data_4->status==1){//busy
            printk("Status is busy\n");
            if(k_sem_take(&(drv_data_4->my_sem_4), K_MSEC(drv_data_4->timeout)) != 0) {     /*Waiting for a timeout of 20 ms to take the semaphore*/
	          printk("Input data not available to be sent!\n");
                  return -1;
	      }
            temp_buffer= drv_data_4->buffer; //K_MSEC(drv_data_4->timeout)
            printk("the value in temp_buffer is %"PRIu32" centimeters\n",drv_data_4->buffer);
            drv_data_4->buffer=0;
            return temp_buffer; 
        }
        else if (drv_data_4->buffer==0){ //idle
             printk("everything is 0\n");
             hcsr04_sample_fetch(dev,SENSOR_CHAN_DISTANCE);
             if(k_sem_take(&(drv_data_4->my_sem_4), K_MSEC(drv_data_4->timeout)) != 0) {     /*Waiting for a timeout of 20 ms to take the semaphore*/
	          printk("Input data not available to be sent!\n"); 
                  return -1;
	      }
              temp_buffer= drv_data_4->buffer;
              drv_data_4->buffer=0;
              return temp_buffer;


        }
    }
    return 0;

}

static int hcsr04_init(struct device *dev)
{
    pinmux=device_get_binding(CONFIG_PINMUX_NAME);
    struct galileo_data *dev_a = pinmux->driver_data;
   // gpiob_2=dev_a->gpio_dw;
   // if (!gpiob_2) {
	//	printk("error for gpio_dw\n");
	//	return 0;
    //}
    if(strcmp(dev->config->name,"HCSRO4_1") == 0){
    printk("init for sensor 1 called\n");
    drv_data_1 = dev->driver_data;
    printk("the name of sensor is %s\n",dev->config->name);
    drv_data_1->trigger=CONFIG_TRIGGER_PIN_NUMBER_1;
    drv_data_1->gpio=dev_a->gpio_dw;
    if (!drv_data_1->gpio) {
        printk("error for gpio_dw for sensor1\n");
        return 0;
    }
    drv_data_1->echo=CONFIG_ECHO_PIN_NUMBER_1;
    drv_data_1->zephyr_trigger=CONFIG_ZEPHYR_TRIGGER_PIN_1;
    drv_data_1->zephyr_echo=CONFIG_ZEPYR_ECHO_PIN_1;
    drv_data_1->timeout=CONFIG_TIMEOUT_1;
    k_sem_init(&(drv_data_1->my_sem_2), 1, 1);
    k_sem_init(&(drv_data_1->my_sem_3), 0, 1);
    k_sem_init(&(drv_data_1->my_sem_4), 0, 1);
    printk("The value of trigger pin for sensor 1 is %d\n",drv_data_1->trigger);
    printk("The value of echo pin for sensor 1 is %d\n",drv_data_1->echo);
    printk("The value of zephyr trigger pin for sensor 1 is %d\n",drv_data_1->zephyr_trigger);
    printk("The value of zephyr echo pin for sensor 1 is %d\n",drv_data_1->zephyr_echo);
   

    ret_status=pinmux_pin_set(pinmux,drv_data_1->trigger,PINMUX_FUNC_A); 	//IO1 for trigger for sensor1
    if(ret_status<0)
	printk("error setting pin for IO1\n");
    ret_status=pinmux_pin_set(pinmux,drv_data_1->echo,PINMUX_FUNC_B); 	//IO3 for echo i.e. handling interrupt for sensor 1
    if(ret_status<0)
	printk("error setting pin for IO3\n");

    
}
else if (strcmp(dev->config->name,"HCSRO4_2") == 0){
      //  printk("initialization for sensor1 over\n");
printk("init for sensor 2 called\n");
    drv_data_2 = dev->driver_data;
    printk("the name of sensor is %s\n",dev->config->name);
    drv_data_2->trigger=CONFIG_TRIGGER_PIN_NUMBER_2;
    drv_data_2->gpio=dev_a->gpio_dw;
    if (!drv_data_2->gpio) {
        printk("error for gpio_dw for sensor2\n");
        return 0;
    }
    drv_data_2->echo=CONFIG_ECHO_PIN_NUMBER_2;
    drv_data_2->zephyr_trigger=CONFIG_ZEPHYR_TRIGGER_PIN_2;
    drv_data_2->zephyr_echo=CONFIG_ZEPYR_ECHO_PIN_2;
    drv_data_2->timeout=CONFIG_TIMEOUT_2;
    k_sem_init(&(drv_data_2->my_sem_2), 1, 1);
    k_sem_init(&(drv_data_2->my_sem_3), 0, 1);
    k_sem_init(&(drv_data_2->my_sem_4), 0, 1);
   printk("The value of trigger pin is %d\n",drv_data_2->trigger);
    printk("The value of echo pin is %d\n",drv_data_2->echo);
printk("The value of zephyr trigger pin for sensor 2 is %d\n",drv_data_2->zephyr_trigger);
    printk("The value of zephyr echo pin for sensor 2 is %d\n",drv_data_2->zephyr_echo);

    ret_status=pinmux_pin_set(pinmux,drv_data_2->trigger,PINMUX_FUNC_A); 	//IO0 for trigger for sensor2
    if(ret_status<0)
	printk("error setting pin for IO0\n");
    ret_status=pinmux_pin_set(pinmux,drv_data_2->echo,PINMUX_FUNC_B); 	//IO2 for echo i.e. handling interrupt for sensor 2
    if(ret_status<0)
	printk("error setting pin for IO2\n");
}
    return 0;
}



static const struct sensor_driver_api hcsr04_driver_api = {
	.sample_fetch = hcsr04_sample_fetch,
	.channel_get = hcsr04_channel_get,
};

static struct hcsr04_data hcsr04_data_1;
static struct hcsr04_data hcsr04_data_2;


//sensor1

DEVICE_AND_API_INIT(hcsr04_sen1,"HCSRO4_1",hcsr04_init, &hcsr04_data_1,  
		    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &hcsr04_driver_api);
//sensor2
//DEVICE_AND_API_INIT(hcsr04_sen1,"HCSRO4_1",hcsr04_init, &hcsr04_data,  
//		    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
//		    &hcsr04_driver_api);
//sensor2
DEVICE_AND_API_INIT(hcsr04_sen2,"HCSRO4_2",hcsr04_init, &hcsr04_data_2,
		    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &hcsr04_driver_api);
//gpio_dw
//Sensor channel
//pinmux
//cmake
//why idle during get
//4 sem why?
//why not polling and why did we use interrupt in this case