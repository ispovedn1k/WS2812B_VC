#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "ws2812b.h"
#include "colors.h"

typedef void (* Animation_function)(led_unit_p, led_unit_p); // указатель на функцию анимации
	 
extern Animation_function volatile Animation;

void SetAnimation(Animation_function Animation_function);

void Animation_RunningLed(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_Pulse(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_RunningLine(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_Train(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_Rainbow(led_unit_p leds_frame, led_unit_p prev_frame);	 
	 
#ifdef __cplusplus
}
#endif
#endif
