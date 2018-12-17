#ifndef __WS2812B_H__
#define __WS2812B_H__

#include "main.h"
#include "color_schemes.h"


#define STRIP_LEDS_NUM				300					// сколько светодиодов ленты используем
#define LEDS_BUFFER_LENGTH			2 * STRIP_LEDS_NUM 	// размер буфера кадров

#define DMA_BUFF_LEDS				10 					// сколько целых светодиодов содержит страница буфера DMA
#define DMA_PAGE_LENGTH				DMA_BITS_PER_LED * DMA_BUFF_LEDS
#define DMA_BUFF_LENGTH				2 * DMA_PAGE_LENGTH // у нас же 2 страницы, так что умножаем на 2.


#ifdef __cplusplus
 extern "C" {
#endif

typedef uint8_t bit_unit_t, *bit_unit_p;
typedef HSV_t	 led_unit_t, *led_unit_p;

extern led_unit_t			g_leds[LEDS_BUFFER_LENGTH];
extern bit_unit_t			g_dma_double_buffer[DMA_BUFF_LENGTH];
extern bit_unit_p const 	g_dma_page_a;
extern bit_unit_p const 	g_dma_page_b;


void InitBuffers2812B(void);
void CalcNextFrame(void);
void UpdateDmaPage(bit_unit_p dma_page);


#ifdef __cplusplus
}
#endif
#endif
