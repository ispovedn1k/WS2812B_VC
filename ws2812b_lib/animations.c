#include <stdlib.h>
#include "animations.h"
#include "colors.h"


/**
 * Animations Flags
 */
static uint32_t frame_counter = 0;

#define ANIM_CPLT		0x01		// Animation cycle complete. Continue, restart it, or set new one.

#define IS_ANIM_CPLT()			(animations_flags & ANIM_CPLT)

#define SET_ANIM_CPLT()			animations_flags |= ANIM_CPLT

#define CLR_ANIM_CPLT()			animations_flags &= ~(ANIM_CPLT)


static uint8_t animations_flags = 0;


static inline void FillFrame(led_unit_p frame, uint32_t val) ;
/**
 *
 */
Animation_function volatile Animation;

/**
 *
 */
void ClearFrame(led_unit_p frame, led_unit_p prev_frame)
{
	FillFrame(frame, LED_OFF);
	frame_counter++;
	if (frame_counter > 1)
	{
		SET_ANIM_CPLT();
	}
}


/**
 *
 */
static inline void FillFrame(led_unit_p frame, uint32_t val)
{
	for(uint32_t i = 0; i < STRIP_LEDS_NUM; i++)
	{
		SET_COLOR(&frame[i], val);
	}
}


/**
 *
 */
void SetAnimation(Animation_function Animation_function)
{
	Animation = Animation_function;
	frame_counter = 0;
	CLR_ANIM_CPLT();
}


/**
 *
 */
uint8_t isAnimationComplete()
{
	return animations_flags & ANIM_CPLT;
}


/**
 *
 */
void Animation_RunningLed(HSV_p leds_frame, HSV_p notUsed)
{
	static uint32_t stage = 0, color = HSV_GREEN;
	const uint32_t preset_colors[8] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE, LED_OFF};
	
	FillFrame(leds_frame, LED_OFF); // ClearFrame

	if (stage % 2 == 0) // прямой ход
	{
		SET_COLOR(&leds_frame[frame_counter], color);
	}
	else // обратный ход
	{
		SET_COLOR(&leds_frame[STRIP_LEDS_NUM-1 - frame_counter], color);
	}
	
	frame_counter++;
	if (STRIP_LEDS_NUM == frame_counter)
	{
		frame_counter = 0; // reset
		stage++;
		if (stage % 2 == 0)
		{
			if (stage < 13)
			{
				color = preset_colors[stage/2];
			}
			else if (stage == 14)
			{
				stage = 0; // animation completed
				color = preset_colors[0];
				SET_ANIM_CPLT();
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
	for (uint16_t i = 0; i < STRIP_LEDS_NUM-1; i++)
	{
		leds_frame[i+1] = prev_frame[i];
	}

	if (frame_counter % 16 == 0)
	{
		leds_frame[0] = GetRandomHsvColor();
	}
	else
	{
		leds_frame[0] = HSV_ModBrightness(prev_frame[0], -5);
	}

	if (frame_counter > STRIP_LEDS_NUM * 4)
	{
		SET_ANIM_CPLT();
	}

	frame_counter++;
}


/**
 *
 */
void Animation_Pulse(HSV_p leds_frame, HSV_p notUsed)
{
	const uint32_t preset_colors[8] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE, LED_OFF};
	HSV_ut color;
	static uint8_t stage = 0;
	
	SET_COLOR(&color, preset_colors[stage]);

	if (frame_counter > 0x40)
	{
		color.HSV.V = 0x80 - frame_counter;
	}
	else
	{
		color.HSV.V = frame_counter;
	}

	frame_counter++;

	if (frame_counter == 0x80)
	{
		frame_counter = 0;
		stage++;
		if (stage == 7)
		{
			stage = 0; // reset
			SET_ANIM_CPLT();
		}
	}
		
	FillFrame(leds_frame, color.ui);
}


/**
 *
 */
void Animation_RunningLine(HSV_p leds_frame, HSV_p notUsed)
{
	static uint32_t stage = 0, color = HSV_GREEN;
	const uint32_t preset_colors[7] = {HSV_GREEN, HSV_RED, HSV_BLUE, HSV_YELLOW, HSV_MAGENTA, HSV_CYAN, HSV_WHITE};

	for (uint32_t i = 0; i < frame_counter; i++)
	{
		SET_COLOR(&leds_frame[i], color);
	}
	
	frame_counter++;
	if (frame_counter > STRIP_LEDS_NUM+1)
	{
		frame_counter = 0;
		if (color == LED_OFF) 
		{
			stage++;
			if (stage == 7) 
			{
				stage = 0; // reset
				SET_ANIM_CPLT();
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
	
	FillFrame(leds_frame, LED_OFF); // ClearFrame

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

	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 4)
	{
		SET_ANIM_CPLT();
	}
}


/**
 *
 */
void Animation_DoubleTrain(HSV_p leds_frame, HSV_p prev_frame)
{
	static HSV_t color1, color2;
	static uint32_t position = STRIP_LEDS_NUM;

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
	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 4)
	{
		SET_ANIM_CPLT();
	}
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
	
	if (! frame_counter)
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
	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 4)
	{
		SET_ANIM_CPLT();
	}
}


/**
 *
 */
#define SNAKE_LENGTH		16

void Animation_Snake(HSV_p leds_frame, HSV_p notUsed)
{
	// перед началом анимации желательпо вызвать ClearFrame
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
	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 8)
	{
		SET_ANIM_CPLT();
	}
}


/**
 *
 */
#define STARS_NUM	10

void Animation_Stars(HSV_p leds_frame, HSV_p prev_frame)
{
#define		STAR	leds_frame[stars[i]]

	static uint32_t stars[STARS_NUM];

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

	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 16)
	{
		SET_ANIM_CPLT();
	}
#undef STAR
}


/**
 *
 */
void Animation_RoundDance(HSV_p leds_frame, HSV_p prev_frame)
{
	static uint16_t H = 0;

	for (uint16_t i = STRIP_LEDS_NUM-1; i > 0; i--)
	{
		leds_frame[i] = prev_frame[i-1];
	}

	if (frame_counter % 16 == 0) // 16 separation
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

	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM * 4)
	{
		SET_ANIM_CPLT();
	}
}


/**
 *
 */
void Animation_MarbleTube(HSV_p leds_frame, HSV_p prev_frame)
{
	static uint16_t end = STRIP_LEDS_NUM; // последний стоящий СД
	uint16_t i;

	for (i = end-1; i > 0; i--)
	{
		leds_frame[i] = prev_frame[i-1];
	}

	if (frame_counter % 16 == 0 )
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
		leds_frame[end].S = 0xFF;

		if (frame_counter % 32 == 0) // для пульсации уже стоящих, чтобы скучно не было.
		{
			leds_frame[end].S = 0x10;
		}
	}

	if (leds_frame[end-1].V != 0) // прицепился ещё один
	{
		end--; // увеличиваем цепь стоящих

		if (end == 0) // длина цепи = вся лента
		{ // restart
			end = STRIP_LEDS_NUM-1;
			SET_ANIM_CPLT();
		}
	}

	frame_counter++;
}


void Animation_Dropdown(HSV_p leds_frame, HSV_p prev_frame)
{
	uint16_t i;

	for (i = 1; i < STRIP_LEDS_NUM; i++)
	{
		leds_frame[i-1] = prev_frame[i];
	}
	SET_COLOR(&leds_frame[i-1], LED_OFF);

	frame_counter++;

	if (frame_counter > STRIP_LEDS_NUM)  // 966 по расчётам
	{	// restart
		SET_ANIM_CPLT();
	}
}
