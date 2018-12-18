#include <stdlib.h>
#include "animations.h"
#include "colors.h"


static inline void ClearFrame(led_unit_p frame);
static inline void FillFrame(led_unit_p frame, led_unit_t val) ;
/**
 *
 */
Animation_function volatile Animation;

/**
 *
 */
static inline void ClearFrame(led_unit_p frame)
{
	for(uint32_t i = 0; i < STRIP_LEDS_NUM; i++)
	{
		SET_COLOR(&frame[i], LED_OFF);
	}
}


/**
 *
 */
static inline void FillFrame(led_unit_p frame, led_unit_t val) 
{
	for(uint32_t i = 0; i < STRIP_LEDS_NUM; i++)
	{
		SET_COLOR(&frame[i], *(uint32_t*)&val);
	}
}


/**
 *
 */
void SetAnimation(Animation_function Animation_function)
{
	Animation = Animation_function;
}


/**
 *
 */
void Animation_RunningLed(HSV_p leds_frame, HSV_p notUsed)
{
	static uint32_t frame = 0, stage = 0, color = HSV_GREEN;
	const uint32_t preset_colors[8] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE, LED_OFF};
	
	ClearFrame(leds_frame);
	
	if (stage % 2 == 0) // прямой ход
	{
		SET_COLOR(&leds_frame[frame], color);
	}
	else // обратный ход
	{
		SET_COLOR(&leds_frame[STRIP_LEDS_NUM-1-frame], color);
	}
	
	frame++;
	if (STRIP_LEDS_NUM == frame)
	{
		frame = 0;
		stage++;
		if (stage % 2 == 0)
		{
			if (stage < 13)
			{
				color = preset_colors[stage/2];
			}
			else if (stage == 14)
			{
				stage = 0;
				color = preset_colors[0];
			}
			else
			{
				color = LED_OFF;
			}
		}
	}
}


/**
 *
 */
void Animation_RunningLed2(HSV_p leds_frame, HSV_p prev_frame)
{
	static uint32_t frame = 0;

	for (uint16_t i = 0; i < STRIP_LEDS_NUM-1; i++)
	{
		leds_frame[i+1] = prev_frame[i];
	}

	if (frame % 16 == 0)
	{
		leds_frame[0] = GetRandomHsvColor();
	}
	else
	{
		leds_frame[0] = HSV_ModBrightness(prev_frame[0], -5);
	}

	frame++;
}


/**
 *
 */
void Animation_Pulse(HSV_p leds_frame, HSV_p notUsed)
{
	const uint32_t preset_colors[8] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE, LED_OFF};
	HSV_t color;
	static uint8_t frame = 0, stage = 0;
	
	SET_COLOR(&color, preset_colors[stage]);

	if (frame > 0x40)
	{
		color.V = 0x80 - frame;
	}
	else
	{
		color.V = frame;
	}

	frame++;

	if (frame == 0x80)
	{
		frame = 0;
		stage++;
		if (stage == 7)
		{
			stage = 0;
		}
	}
		
	FillFrame(leds_frame, color);
}


/**
 *
 */
void Animation_RunningLine(HSV_p leds_frame, HSV_p notUsed)
{
	static uint32_t frame = 0, stage = 0, color = HSV_GREEN;
	const uint32_t preset_colors[7] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE};

	for (uint32_t i = 0; i < frame; i++) 
	{
		SET_COLOR(&leds_frame[i], color);
	}
	
	frame++;
	if (frame > STRIP_LEDS_NUM+1)
	{
		frame = 0;
		if (color == LED_OFF) 
		{
			stage++;
			if (stage == 7) 
			{
				stage = 0;
			}
			color = preset_colors[ stage ];
		}
		else
		{
			color = LED_OFF;
		}	
	}
}


/**
 *
 */
void Animation_Train(HSV_p leds_frame, HSV_p notUsed)
{
	static led_unit_t train[5];
	static int32_t train_position = -3;
	static int8_t speed;
	
	ClearFrame(leds_frame);
	
	if (train_position <= -3) 
	{
		speed = 1;
		HSV_t color = GetRandomHsvColor();
		color.V = 0x10;
		train[0] = color;
		color.V = 0x20;
		train[1] = color;
		color.V = 0x30;
		train[2] = color;
		color.V = 0x40;
		train[3] = color;
		color.V = 0x38;
		train[4] = color;
	}
	
	for (uint8_t i = 0; i < 5; i++)
	{
		int32_t carriage_position = train_position + i*speed;

		if (carriage_position >= 0 && carriage_position < STRIP_LEDS_NUM)
		{
			leds_frame[carriage_position] = train[i];
		}
	}
	
	train_position += speed;
	
	if (train_position > STRIP_LEDS_NUM+3)
	{
		speed = -1;
	}
}


/**
 *
 */
void Animation_DoubleTrain(HSV_p leds_frame, HSV_p prev_frame)
{
	static HSV_t color1, color2;
	static uint32_t position = STRIP_LEDS_NUM,
					frame = 5;

	for (uint32_t i = 0; i < STRIP_LEDS_NUM; i++)
	{
		leds_frame[i] = HSV_ModBrightness(prev_frame[i], -1);
	}

	if (position == STRIP_LEDS_NUM)
	{
		position = 0;
		color1 = GetRandomHsvColor();
		color2 = GetRandomHsvColor();
	}

	leds_frame[position] = color1;
	leds_frame[STRIP_LEDS_NUM-1 - position] = color2;

	position++;
	frame++;
}


static inline HSV_t getNextRainbowColor(HSV_t current, const uint8_t step)
{
	current.H += step;
	if (current.H >= MAX_H_VALUE)
	{
		current.H -= MAX_H_VALUE;
	}
	return current;
}
/**
 *
 */
void Animation_Rainbow(HSV_p leds_frame, HSV_p prev_frame)
{
	const uint8_t step = MAX_H_VALUE / STRIP_LEDS_NUM;
	static uint32_t frame = 0;
	
	if (!frame)
	{
		leds_frame[0] = hsv(0, 255, 0x40);
	}
	else
	{
		leds_frame[0] = getNextRainbowColor(prev_frame[0], 1);
		
	}
			
	for(uint16_t i = 1; i < STRIP_LEDS_NUM; i++)
	{
		leds_frame[i] = getNextRainbowColor(leds_frame[i-1], step);
	}
	frame++;
}


/**
 *
 */
#define SNAKE_LENGTH		16

void Animation_Snake(HSV_p leds_frame, HSV_p notUsed)
{
	static uint32_t	frame = 0;
	static HSV_t	snake_colors[SNAKE_LENGTH];
	static uint16_t	snake_positions[SNAKE_LENGTH];
	static HSV_t	food_color;
	static uint16_t food_position;

	for (uint8_t i = SNAKE_LENGTH-1; i > 0; i--) // последние 2 должны быть чёрными, чтобы затирать хвост.
	{
		snake_positions[i] = snake_positions[i-1];
		leds_frame[snake_positions[i]] = snake_colors[i];
	}

	if (snake_positions[0] > food_position)
	{
		snake_positions[0]--;
	}
	else
	{
		snake_positions[0]++;
	}

	if (snake_positions[0] == food_position)
	{
		for (uint8_t i = SNAKE_LENGTH-3; i > 0; i--) // последние 2 должны быть чёрными, чтобы затирать хвост.
		{
			snake_colors[i] = snake_colors[i-1];
		}
		snake_colors[0] = food_color;
		food_color = GetRandomHsvColor();
		food_position = rand() % STRIP_LEDS_NUM;
	}

	leds_frame[food_position] = food_color;
	frame++;
}


/**
 *
 */
#define STARS_NUM	10

void Animation_Stars(HSV_p leds_frame, HSV_p prev_frame)
{
#define		STAR	leds_frame[stars[i]]

	static uint32_t stars[STARS_NUM];
	static uint32_t frame = 0;

	for (uint16_t i = 0; i < STRIP_LEDS_NUM; i++) {
		leds_frame[i] = HSV_ModBrightness(prev_frame[i], -1);
	}


	for (uint8_t i = 0; i < STARS_NUM; i++)
	{
		if (rand() < (RAND_MAX / 4))
		{
			STAR = HSV_ModBrightness(STAR, 3);
		}

		if (STAR.V < 2)
		{
			STAR.V = 0;
			stars[i] = rand() % STRIP_LEDS_NUM;
			STAR = GetRandomHsvColor();
		}
	}

	frame++;

#undef STAR
}


/**
 *
 */
void Animation_RoundDance(HSV_p leds_frame, HSV_p prev_frame)
{
	static uint32_t frame = 0;
	static uint16_t H = 0;

	for (uint16_t i = STRIP_LEDS_NUM-1; i > 0; i--)
	{
		leds_frame[i] = prev_frame[i-1];
	}

	if (frame % 16 == 0) // 16 separation
	{
		leds_frame[0] = hsv(H, 0xFF, MAX_V_VALUE);
	}
	else
	{
		leds_frame[0].V = 0;
	}

	H++;
	if (H == MAX_H_VALUE)
	{
		H = 0;
	}
	frame++;
}


/**
 *
 */
void Animation_MarbleTube(HSV_p leds_frame, HSV_p prev_frame)
{
	static uint32_t frame = 0;
	static uint16_t end = STRIP_LEDS_NUM; // последний стоящий СД
	uint16_t i;

	for (i = end-1; i > 0; i--)
	{
		leds_frame[i] = prev_frame[i-1];
	}

	if (frame % 16 == 0 )
	{
		leds_frame[0] = GetRandomHsvColor();
	}
	else
	{
		SET_COLOR(&leds_frame[0], LED_OFF);
	}

	leds_frame[end] = prev_frame[end];

	// wave pulse
	for (i = STRIP_LEDS_NUM-1; i > end; i--)
	{
		leds_frame[i].S = prev_frame[i-1].S; // передача пульсации
	}

	if (end < STRIP_LEDS_NUM) // сглаживание пульсации
	{
		uint16_t S = leds_frame[end].S + 0x01;
		if (S > 0xFF)
		{
			S = 0xFF;
		}
		leds_frame[end].S = S;
	}

	if (end < STRIP_LEDS_NUM	&&	(frame % 32 == 0)) // для пульсации уже стоящих, чтобы скучно не было.
	{
		leds_frame[end].S = 0x10;
	}


	if (leds_frame[end-1].V != 0) // прицепился ещё один
	{
		end--; // увеличиваем цепь стоящих

		if (end == 0) // длина цепи = вся лента
		{ // restart
			end = STRIP_LEDS_NUM-1;
			ClearFrame(leds_frame);
			frame = 0;
		}
	}

	frame++;
}
