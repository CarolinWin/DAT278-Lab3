/*
 * globals.c
 *
 *  Created on: 30 nov. 2019
 *      Author: Albin
 */

#include <em_core.h>
#include <em_cmu.h>

#include <rtcdriver.h>
#include <em_emu.h>

#include "pwmled.h"
#include "game.h"

volatile unsigned int sysTicks;

#define NULL false



// Global function.
// Call this every now and then, because some things have to
// run very often.
void IDLE() {
  static int a = 0;
  // HACK: If we take too long before flipping the LEDs, the intensity illusion
  // is broken. As long as IDLE() gets called often enough, it should be fine.
  // It's fine. Don't worry about it.
  a = (a + 1) % 10000;
  if (a == 0) PWMLed();

  // Don't miss button presses just because it took long to poll them again
  Buttons();
}


void DelayTicks(unsigned int t) {
  uint32_t target = sysTicks + t;
  while (sysTicks < target) { IDLE(); }
}

void DelayMilli(unsigned int ms) {
	// DelayTicks(ms * 10);
	RTCDRV_TimerID_t rtc_sleep_timer;
	RTCDRV_AllocateTimer(&rtc_sleep_timer);

	// Schedule an interrupt that will wake us
	RTCDRV_StartTimer(rtc_sleep_timer, rtcdrvTimerTypeOneshot, ms, NULL, NULL);

	// Deep sleep until then
	// Since other interrupts may also wake the system,
	// we must potentially go back to sleep until the time
	// has actually passed.
	// This may look like busy-waiting, but it's far better.
	bool running = true;
	while (true) {
		RTCDRV_IsRunning(rtc_sleep_timer, &running);
		if (running) {
			CORE_DECLARE_IRQ_STATE;
			CORE_ENTER_ATOMIC();
			EMU_EnterEM2(true);
			CORE_EXIT_ATOMIC();
		} else {
			break;
		}
	}

	RTCDRV_FreeTimer(rtc_sleep_timer);
}


void SysTick_Handler(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  sysTicks += 1;
  CORE_EXIT_ATOMIC();
}


void SetupGlobals() {

  sysTicks = 0;

  // Call SysTick_Handler at 0.1ms intervals
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 10000)) while (1) {};
}
