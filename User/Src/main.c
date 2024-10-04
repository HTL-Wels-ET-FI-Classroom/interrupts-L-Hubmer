/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

int timer1 = 0;
int timer2 = 0;

volatile int TextColor = 0;

uint8_t state = 0;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();

	switch(state)
	{
	case 1:
		timer1++;
		break;
	case 2:
		timer2++;
		break;
	}
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/*GPIO Init*/
	GPIO_InitTypeDef InitVar;
	InitVar.Alternate = 0;
	InitVar.Pull = GPIO_NOPULL;
	InitVar.Speed = GPIO_SPEED_MEDIUM;

	InitVar.Pin = GPIO_PIN_0;
	InitVar.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOA, &InitVar);

	InitVar.Pin = GPIO_PIN_3;
	InitVar.Mode = GPIO_MODE_IT_RISING;
	InitVar.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &InitVar);

	InitVar.Pin = GPIO_PIN_13;
	InitVar.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOG, &InitVar);

	/*NVIC Activation*/

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright Hubmer", CENTER_MODE);

	while (1)
	{

		// display timer

		switch(TextColor)
		{
		case 0:
			LCD_SetTextColor(LCD_COLOR_WHITE);
			break;

		case 1:
			LCD_SetTextColor(LCD_COLOR_RED);
			break;

		case 2:
			LCD_SetTextColor(LCD_COLOR_GREEN);
			break;

		case 3:
			LCD_SetTextColor(LCD_COLOR_BLUE);
			break;

		default:
			TextColor = 0;
			break;
		}
		LCD_SetFont(&Font20);
		LCD_SetPrintPosition(5, 0);
		printf("   Timer: %.3f    \n", timer1/1000.0);
		printf("   Timer: %.3f", timer2/1000.0);


	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
/*
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}


 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected

static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
 *xCoord = TsState.X;
 *yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
 *xCoord = TS_Calibration_GetX(*xCoord);
 *yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}
 */

void EXTI0_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
	if(state == 1)
	{
		state = 2;
	}else
	{
		state = 1;
	}

}

void EXTI3_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
	TextColor ++;
	HAL_Delay(10);
}
