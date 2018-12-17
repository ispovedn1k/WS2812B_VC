#include <stdlib.h>
#include "color_schemes.h"


inline RGB_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (RGB_t) { b, r, g, 255 };
}


RGB_t GetRandomRgbaColor(void) 
{
	uint32_t r = rand();
	return (RGB_t) { r & 0x000000FF, (r & 0x0000FF00) >> 8, (r & 0x00FF0000) >> 16, 255 };
}


inline HSV_t hsv(uint16_t h, uint8_t s, uint8_t v)
{
	return (HSV_t) { h, s, v };
	// HSV_t t = {h, s, v};
	// return t; // correct order
}


inline HSV_t GetRandomHsvColor(void) 
{
	uint32_t r = rand();
	return (HSV_t) { 6*(r & 0x000000FF), 255, MAX_V_VALUE }; // @todo: сделать рандомизацию
}


/**
 *
 */
HSV_t HSV_ModBrightness(HSV_t led, int8_t mod)
{
	int16_t v = led.V;
	v += mod;
	if (v < 0) {
		return hsv(led.H, led.S, 0);
	}
	else if (v > MAX_V_VALUE){
		return hsv(led.H, led.S, 0);
	}
	else {
		return hsv(led.H, led.S, v);
	}
}


RGB_t ConvertHsvToRgb(HSV_p hsv)
{
	if (hsv->S == 0) { // насыщенность цвета никакая, можно ничего не считать.
		return rgb(hsv->V, hsv->V, hsv->V);
	}

	if (hsv->H >= MAX_H_VALUE) {
		hsv->H = 0; // коррекция для неверного диапазона (откуда бы ему взяться?)
	}

	uint8_t Hi = hsv->H / H_SECTOR_SIZE; // @todo: оптимизировать?
	uint8_t Vmin = (255 - hsv->S) * hsv->V / 256;
	uint8_t a = (hsv->V - Vmin) * (hsv->H % H_SECTOR_SIZE) / H_SECTOR_SIZE; // @todo: оптимизировать?
	uint8_t Vinc = Vmin + a;
	uint8_t Vdec = hsv->V - a;

	switch (Hi)
	{
	case 0:
		return rgb(hsv->V, Vinc, Vmin);
	case 1:
		return rgb(Vdec, hsv->V, Vmin);
	case 2:
		return rgb(Vmin, hsv->V, Vinc);
	case 3:
		return rgb(Vmin, Vdec, hsv->V);
	case 4:
		return rgb(Vinc, Vmin, hsv->V);
	case 5:
	default:
		return rgb(hsv->V, Vmin, Vdec);
	}
}
