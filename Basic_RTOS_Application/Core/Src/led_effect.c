/*
 * led_effect.c
 *
 *      Author: vinodhini
 */
#include "main.h"

void stop_led_effect()
{
	for(int i=0; i<4; i++)
		xTimerStop(handle_led_timer[i],portMAX_DELAY);

}


void blink_led_in_pattern(int n)
{
	stop_led_effect();
	xTimerStart(handle_led_timer[n-1],portMAX_DELAY);

}

void turnoff_all_leds(void){
	HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_Y_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LED_B_Pin, GPIO_PIN_RESET);
}

void turnon_all_leds(void){
	HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED_Y_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, LED_B_Pin, GPIO_PIN_SET);
}

void turnon_odd_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED_Y_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, LED_B_Pin, GPIO_PIN_RESET);
}

void turnon_even_leds(void)
{
	HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_Y_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LED_B_Pin, GPIO_PIN_SET);
}

void turnon_one_led(GPIO_TypeDef* gpio_port,uint16_t gpio_pin)
{
	turnoff_all_leds();
	HAL_GPIO_WritePin(gpio_port, gpio_pin, GPIO_PIN_SET);

}
void blink_led_in_pattern1(void)
{
	static int flag = 0;
	flag ^= 1;
	flag ? turnon_all_leds() : turnoff_all_leds;
}

void blink_led_in_pattern2(void)
{
	static int flag = 0;
	flag ^= 1;
	flag ? turnon_odd_leds() : turnon_even_leds;
}
void blink_led_in_pattern3(void)
{
	static int i =0;
	if(i==4){
		i=0;
	}
	GPIO_TypeDef* gpio_port[4]={GPIOA,GPIOA,GPIOA,GPIOB};
	uint16_t gpio_pin[4]={LED_G_Pin,LED_Y_Pin,LED_R_Pin,LED_B_Pin};
	turnon_one_led(gpio_port[i],gpio_pin[i++]);
}

void blink_led_in_pattern4(void)
{
	static int i =3;
	if(i==-1){
		i=3;
	}
	GPIO_TypeDef* gpio_port[4]={GPIOA,GPIOA,GPIOA,GPIOB};
	uint16_t gpio_pin[4]={LED_G_Pin,LED_Y_Pin,LED_R_Pin,LED_B_Pin};
	turnon_one_led(gpio_port[i],gpio_pin[i--]);
}
