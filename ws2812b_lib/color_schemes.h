#ifndef __COLOR_SCHEMES_H__
#define __COLOR_SCHEMES_H__

#include <inttypes.h>

#define DMA_BITS_PER_LED			24
#define SET_COLOR(PTR_LED, COLOR) {*((uint32_t *)(PTR_LED)) = COLOR;}


#ifdef __cplusplus
 extern "C" {
#endif

typedef struct __RGBA {
	uint8_t		B;
	uint8_t		R;
	uint8_t		G;
	uint8_t		alpha;
} RGBA_t, RGB_t, *RGBA_p, *RGB_p;


RGB_t rgb(uint8_t r, uint8_t g, uint8_t b);
RGB_t GetRandomRgbaColor(void);


#define H_SECTOR_SIZE	256						// удобно использовать значения кратные 2^n, дабы упростить деление.
#define MAX_H_VALUE		6 * H_SECTOR_SIZE		// с этого значения начинается обнуление.

typedef struct __HSV {
	uint16_t	H;					// Hue: 		0 to MAX_H_VALUE
	uint8_t		S;					// Saturation:	0 to 255
	uint8_t		V;					// Value: 		0 to 255 Brightness
} HSV_t, *HSV_p;

HSV_t hsv(uint8_t h, uint8_t s, uint8_t v);
HSV_t GetRandomHsvColor(void);

RGB_t ConvertHsvToRgb(HSV_p hsv);			// переводит цвет формата hsv в rgb структуру, aplha всегда макс.
	 
#ifdef __cplusplus
}
#endif
#endif
