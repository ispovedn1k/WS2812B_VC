#include "ws2812b.h"
#include "animations.h"


#define WS2812B_FREQ			800000L		// 800 kHz
#define MAXRESET_HOLD_100FPS	1			// 1  страница
#define MAXRESET_HOLD_60FPS		4			// 4  страницы
#define MAXRESET_HOLD_30FPS		27			// 27 страниц

// флаги
#define NEXT_FRAME_READY		0x80	// следующий кадр расчитан
#define FRAME_END				0x40	// достигнут конец кадра
#define FRAME_TR_COMPLETE		0x20	// передача кадра завершена
#define RESET_SETTED			0x10	// на линии установлен сигнал сброса

uint8_t ws2812b_flags;

#define IsFrameReady()			(ws2812b_flags & NEXT_FRAME_READY)
#define IsFrameEnd()			(ws2812b_flags & FRAME_END)
#define IsFrameTrComplete()		(ws2812b_flags & FRAME_TR_COMPLETE)
#define IsResetSetted()			(ws2812b_flags & RESET_SETTED)

#define SetFrameReady()			ws2812b_flags |= NEXT_FRAME_READY
#define SetFrameEnd()			ws2812b_flags |= FRAME_END
#define SetFrameTrComplete()	ws2812b_flags |= FRAME_TR_COMPLETE
#define SetResetSetted()		ws2812b_flags |= RESET_SETTED

#define ClearFrameReady()		ws2812b_flags &= ~(NEXT_FRAME_READY)
#define ClearFrameEnd()			ws2812b_flags &= ~(FRAME_END)
#define ClearFrameTrComplete()	ws2812b_flags &= ~(FRAME_TR_COMPLETE)
#define ClearResetSetted()		ws2812b_flags &= ~(RESET_SETTED)


// private declarations
void SetDmaPageValue(bit_unit_p dma, bit_unit_t val);	// Заполняет всю страницу буфера DMA указанным значением.
bit_unit_p SetDmaSingleLedReset(bit_unit_p pPWM_buff);
bit_unit_p RGB2PWM(RGB_p pRGBA, bit_unit_p pPWM_buff);
bit_unit_p RGBA2PWM(RGBA_p pRGBA, bit_unit_p pPWM_buff); // переводит состояние светодиода в задержки таймера
bit_unit_p HSV2PWM(HSV_p hsv, bit_unit_p pPWM_buff);


// GLOBAL VARIABLES
led_unit_t			g_leds[LEDS_BUFFER_LENGTH];
bit_unit_t			g_dma_double_buffer[DMA_BUFF_LENGTH];


// LOCAL
led_unit_p const leds_frame_a = g_leds; // указатель на начало 1-го кадра буфера светодиодов
led_unit_p const leds_frame_b = &(g_leds[STRIP_LEDS_NUM]); // указатель на начало 2-го кадра буфера светодиодов
led_unit_p volatile p_dma_frame; // указатель на текущий кадр
led_unit_p volatile p_clc_frame; // указатель на предыдущий кадр

bit_unit_p const g_dma_page_a = g_dma_double_buffer; // указатель на 1-ю страницу DMA-буффера
bit_unit_p const g_dma_page_b = &(g_dma_double_buffer[DMA_PAGE_LENGTH]); // указатель на 2-ю страницу DMA-буффера



void TranslateFrameToDmaPage(bit_unit_p dma_page, led_unit_t leds_frame[], uint32_t start_led);
void SwapFrames(void);
/**
 *
 */
void InitBuffers2812B()
{
	SetAnimation(Animation_Stars);
	p_dma_frame = leds_frame_b;
	p_clc_frame = leds_frame_a;
	SetFrameEnd();
	
	for(uint16_t i = 0; i < DMA_BUFF_LENGTH; i++)
	{
		g_dma_double_buffer[i] = 0;
	}
}


/**
 *
 */
void CalcNextFrame(void)
{
	if (IsFrameReady()) // если следующий кадр уже расчитан, не делаем ничего.
	{
		return;
	}
	// кадр не расчитан, расчитываем
	Animation(p_clc_frame, p_dma_frame);
	// взводим флаг готовности
	SetFrameReady();
}


/**
 * @brief Устанавливает 24 бита фрагмента страницы DMA (1 LED) в 0 - сигнал сброса.
 * 	Для успешного сброса надо как минимум 2 LED сигнала сброса!!
 * @param pPWM_buff: Собственно, откуда начинать установку.
 * @retval указатель буфера, где закончили выполнение.
 */
bit_unit_p SetDmaSingleLedReset(bit_unit_p pPWM_buff)
{
	for (uint8_t i = 0; i < DMA_BITS_PER_LED; i++)
	{
		*pPWM_buff = 0;
		pPWM_buff++;
	}
	return pPWM_buff;
}


/**
 * @brief Заполняет всю страницу буфера DMA указанным значением.
 * @param dma: Начало страницы буфера.
 * @param val: Значение, которым будем заполнять.
 * @retval None.
 */
void SetDmaPageValue(bit_unit_p dma, bit_unit_t val)
{	// заполняем всю страницу буфера значением
	for (uint16_t i = 0; i < DMA_PAGE_LENGTH; i++)
	{
		dma[i] = val;
	}
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


bit_unit_p HSV2PWM(HSV_p hsv, bit_unit_p pPWM_buff)
{
	RGB_t rgb = ConvertHsvToRgb(hsv);
	return RGB2PWM(&rgb, pPWM_buff);
}


/**
 * @brief Заполняет страницу буфера dma данными из массива кадра.
 * 		Если кадр закончится раньше страницы, недостающие данные заполнит сигналом сброса.
 * @param dma_page: Страница памяти dma, которую будем сейчас заполнять.
 * @param frame_leds[]: Массив кадра, откуда берём данные.
 * @parma start_led: Позиция в кадре, откуда берём данные для заполнения страницы.
 * @retval None.
 */
void TranslateFrameToDmaPage(bit_unit_p dma_page, led_unit_t leds_frame[], uint32_t start_led)
{
	uint32_t frame_position = start_led;
	// передача цветов.
	for (uint32_t dma_pos = 0; dma_pos < DMA_BUFF_LEDS; dma_pos++)
	{ // перебираем значения светодиодов
		if (STRIP_LEDS_NUM > frame_position)
		{
			dma_page = HSV2PWM(&leds_frame[frame_position], dma_page);
		}
		else // если вдруг случилось так, что кадр кончился, то остаток добиваем сбросом.
		{
			dma_page = SetDmaSingleLedReset(dma_page);
		}
		frame_position++; //переключаемся на следующий элемент.
	}
}


/**
* @brief Переключает активный кадр на следующий.
 */
void SwapFrames(void)
{
	if (p_dma_frame == leds_frame_a)
	{
		p_dma_frame = leds_frame_b;
		p_clc_frame = leds_frame_a;
	}
	else
	{
		p_dma_frame = leds_frame_a;
		p_clc_frame = leds_frame_b;
	}
}


/**
 * @brief Готовит следующую страницу DMA
 */
void UpdateDmaPage(bit_unit_p dma_page)
{
	static uint32_t frame_position = 0;
	static uint32_t resets_num = 1; // 1, чтобы до тех пор, пока 1й кадр при запуске МК не расчитается, линия была в сбросе.
	if (IsFrameEnd())
	{
		SetDmaPageValue(dma_page, T_RESET);
		resets_num--;
		if (resets_num == 0)
		{
			SetFrameTrComplete();
			if ( IsFrameReady() )
			{
				SwapFrames();
				frame_position = 0;
				ClearFrameEnd();
				ClearFrameReady();
			}
			else // следующий кадр ещё не готов. Нужно продлить его ожидание.
			{
				resets_num = 1;
				// @todo: можно добавить сюда сигнал о том, что следующий кадр слишком долго подготавливается.
				// позволит выбрать более подходящий fps для эффекта.
			}
		}
	}
	else // кадр ещё не полностью выгружен
	{
		TranslateFrameToDmaPage(dma_page, p_dma_frame, frame_position);
		frame_position += DMA_BUFF_LEDS;
		if (frame_position >= STRIP_LEDS_NUM)
		{
			SetFrameEnd();
			resets_num = MAXRESET_HOLD_30FPS;
		}
	}
}

