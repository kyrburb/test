/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MassStorage.c
 * Purpose: USB Device Mass Storage Example
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_fs.h"
#include "rl_usb.h"

#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "Board_Buttons.h"
#include "Board_LED.h"

#include "stm32f4xx_hal.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

extern uint32_t os_time;

uint32_t HAL_GetTick(void) {
  return os_time;
}

/* System Clock Configuration */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

int main (void) {
  uint8_t but;
  uint8_t buf[2];

  HAL_Init();
  SystemClock_Config();

  LED_Initialize    ();
  Buttons_Initialize();
  GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, "    USB Device      ");
  GLCD_DrawString         (0, 1*24, "  HID / MSC Class   ");
  GLCD_DrawString         (0, 2*24, "Mass Storage Example");
  GLCD_DrawString         (0, 4*24, "USBFS: Memory Disk  ");
  GLCD_DrawString         (0, 5*24, "USBHS: SD Card Disk ");
  GLCD_DrawString         (0, 8*24, "  Keil Tools by ARM ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");

  finit              ("M0:");           /* Initialize SD Card 0               */
  fmount             ("M0:");           /* Mount SD Card 0                    */

  USBD_Initialize    (0);               /* USB Device 0 Initialization        */
  USBD_Connect       (0);               /* USB Device 0 Connect               */

  USBD_Initialize    (1);               /* USB Device 1 Initialization        */
  USBD_Connect       (1);               /* USB Device 1 Connect               */

  while (1) {
    but = (uint8_t)(Buttons_GetState ());
    if ((but ^ buf[0]) & 3) {
      buf[0] = but & 3;
      USBD_HID_GetReportTrigger(0, 0, &buf[0], 1);
    }
    if (((but >> 2) ^ buf[1]) & 3) {
      buf[1] = (but >> 2) & 3;
      USBD_HID_GetReportTrigger(1, 0, &buf[1], 1);
    }
    osDelay(100);
  }
}
