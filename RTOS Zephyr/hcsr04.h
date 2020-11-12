#include <kernel.h>


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

