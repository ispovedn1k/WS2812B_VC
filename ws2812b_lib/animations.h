#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__

#include "ws2812b.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef void (* Animation_function)(led_unit_p, led_unit_p); // указатель на функцию анимации
	 
extern Animation_function volatile Animation;

void SetAnimation(Animation_function Animation_function);
uint8_t isAnimationComplete();

void Animation_RunningLed(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_RunningLed2(led_unit_p leds_frame, led_unit_p prev_frame);
void Animation_Pulse(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_RunningLine(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_Train(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_DoubleTrain(led_unit_p leds_frame, led_unit_p prev_frame);
void Animation_Rainbow(led_unit_p leds_frame, led_unit_p prev_frame);
void Animation_Snake(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_Stars(led_unit_p leds_frame, led_unit_p notUsed);
void Animation_RoundDance(led_unit_p leds_frame, led_unit_p prev_frame);
void Animation_MarbleTube(led_unit_p leds_frame, led_unit_p prev_frame);
void Animation_Dropdown(led_unit_p leds_frame, led_unit_p prev_frame);

void ClearFrame(led_unit_p frame, led_unit_p prev_frame);

	 
#ifdef __cplusplus
}
#endif
#endif
