#include "rgb.h"


inline RGB_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (RGB_t) { b, r, g, 255 };
}


RGB_t GetRandomRgbaColor(void) 
{
	return (RGB_t) { 128, 255, 30, 255 }; // @todo: сделать рандом
}


/**
 * @brief: Переводит цветность без учёта канала яркости
 */
bit_unit_p RGB2PWM(RGB_p pRGBA, bit_unit_p pPWM_buff)
{
	uint32_t color = *(uint32_t *)pRGBA;
	for (uint8_t i = 0; i < DMA_BITS_PER_LED; i++)
	{
		*pPWM_buff = (color & (uint32_t)(1 << ((DMA_BITS_PER_LED-1)-i)) ) ? T1H : T0H;
		pPWM_buff++;
	}
	return pPWM_buff;
}


/**
 * Переводит цвет и яркость светодиода в биты, которые будем выдавать на ленту.
 * @todo: сделать учёт альфа-канала.
 */
bit_unit_p RGBA2PWM(RGBA_p pRGBA, bit_unit_p pPWM_buff)
{
	uint32_t alpha = *(uint32_t *)pRGBA >> 24;
	RGBA_t c;
	c.B = (alpha * pRGBA->B) >> 8;
	c.R = (alpha * pRGBA->R) >> 8;
	c.G = (alpha * pRGBA->G) >> 8;
	return RGB2PWM(&c, pPWM_buff);
}
