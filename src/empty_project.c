/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2012 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 4. The source and compiled code may only be used on Energy Micro "EFM32"
 *    microcontrollers and "EFR4" radios.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_timer.h"

#define LED_Tx gpioPortA,9

//timer ISR
void TIMER0_IRQHandler(void)
{
  TIMER_IntClear(TIMER0,TIMER_IF_OF);
  GPIO_PinOutToggle(LED_Tx);
}

//timer function, lets the user decide how often led should blink
void Timer(uint32_t frequency)
{
  //enable clocks needed for timer
  CMU_ClockEnable(cmuClock_HFPER,true);
  CMU_ClockEnable(cmuClock_GPIO,true);
  CMU_ClockEnable(cmuClock_TIMER0 ,true);

  //set pin 9 as output pin, initially low
  GPIO_PinModeSet(LED_Tx,gpioModePushPull,0);

  //HFPER = HF/32 = 14MHz/32 = 437500 MHz
  CMU_ClockDivSet(cmuClock_HFPER,cmuClkDiv_32);

  //initialize timer structure
  TIMER_Init_TypeDef initTimer = TIMER_INIT_DEFAULT;
  initTimer.enable = false;
  initTimer.prescale = timerPrescale1024;
  TIMER_Init(TIMER0,&initTimer);

  //allow user to decide the frequency
  uint32_t timer0frequency = CMU_ClockFreqGet(cmuClock_TIMER0 )/1024;
  if(timer0frequency>0xFFFF)
  {
    timer0frequency=0xFFFF;
  }

  uint32_t timerTop = timer0frequency/frequency - 1;
  TIMER_TopSet(TIMER0,timerTop);

  //enable interrupts
  TIMER_IntEnable(TIMER0,TIMER_IF_OF);
  NVIC_EnableIRQ(TIMER0_IRQn);

  //start the timer
  TIMER_Enable(TIMER0,true);

}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  Timer(1);

  /* Infinite loop */
  while (1) {
  }
}
