#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS 500



static const struct gpio_dt_spec leds[] = {GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios), 
                                           GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios), 
                                           GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios), 
                                           GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios)};

static const struct gpio_dt_spec buttons[] = {GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
                                              GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios),
                                              GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios),
                                              GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios)};


int64_t previous, current;
uint8_t pat_cnt, time_cnt = 0;

void main(void) {

    int ret;
    bool pattern[64];
    uint32_t timer[128];

    for(int i = 0; i < 4; i++) {
        if (!device_is_ready(leds[i].port) || !device_is_ready(buttons[i].port)) {
            printk("Port is not ready \n");
		    return;
	    }
        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
	    if (ret < 0) {
            printk("Failed to configure LED %i \n", i);
            return;
        }
        ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
        if (ret < 0) {
            printk("Failed to configure button %i \n", i);
            return;
        }
    }
    
    while(1) {

        if (gpio_pin_get_dt(&buttons[0]) == 1) {
            
            if (pat_cnt > 0) {
                current = k_uptime_delta(&previous);
                timer[time_cnt++] = current;
            }
            else {
                printk("Reading pattern... \n");
            }
            previous = k_uptime_get();
            while (gpio_pin_get_dt(&buttons[0]) == 1) {
                gpio_pin_set_dt(&leds[0], 1);
            }
            current = k_uptime_delta(&previous);
            pattern[pat_cnt++] = 0;
            timer[time_cnt++] = current;
            previous = k_uptime_get();
        }
        gpio_pin_set_dt(&leds[0], 0);


        if (gpio_pin_get_dt(&buttons[1]) == 1) {

            if (pat_cnt > 0) {
                current = k_uptime_delta(&previous);
                timer[time_cnt++] = current;
            }
            else{
                printk("Reading pattern... \n");
            }
            previous = k_uptime_get();
            while (gpio_pin_get_dt(&buttons[1]) == 1) {
                gpio_pin_set_dt(&leds[1], 1);
            }
            current = k_uptime_delta(&previous);
            pattern[pat_cnt++] = 1;
            timer[time_cnt++] = current;
            previous = k_uptime_get();
        }
        gpio_pin_set_dt(&leds[1], 0);


        if (gpio_pin_get_dt(&buttons[2]) == 1) {

            printk("Displaying pattern... \n");
            int j = 0;
            for (int i = 0; i < pat_cnt; i++) {

                gpio_pin_set_dt(&leds[pattern[i]], 1);
                k_msleep(timer[j++]);
                gpio_pin_set_dt(&leds[pattern[i]], 0);
                k_msleep(timer[j++]);
            }
        }


        if (gpio_pin_get_dt(&buttons[3]) == 1) {
            printk("Pattern reset \n");
            while (gpio_pin_get_dt(&buttons[3]) == 1) {}
            time_cnt = 0;
            pat_cnt = 0;
        }

        k_msleep(1);
        
    }
}   