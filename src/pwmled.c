#include <bsp.h>

#include "globals.h"

/*
 *
 * Pulse Width Modulation lets us light an LED with
 * any intensity we want. Call PWMLed often to keep up the illusion!
 *
 */
const uint32_t PWMPeriod = 30;
volatile bool PWMOnFirst = false;
volatile uint32_t PWMLongTicks = 0;
volatile uint32_t PWMShortTicks = 0;

void PWMLed() {

  // Try to compensate for stuff executed outside this function.
  // Whatever time it took to call this function again,
  // we still know when the next LED blink should happen.
  static uint32_t end_of_last_period = 0;


  uint32_t switch_time = end_of_last_period + PWMLongTicks;
  uint32_t end_time = switch_time + PWMShortTicks;
  end_of_last_period = end_time;

  // Flash one period of the PWM signal
  while (sysTicks < (switch_time)) {}
  if (PWMShortTicks) PWMOnFirst ? BSP_LedClear(0) : BSP_LedSet(0);
  while (sysTicks < (end_time)) {}
  PWMOnFirst ? BSP_LedSet(0) : BSP_LedClear(0);

}


void PWMIntensity(float intensity) {
  if (intensity < 0) intensity = 0.0;
  if (intensity > 1) intensity = 1.0;


  // The longest half of the period should be done first,
  // so that we have the best chance of switching not being delayed.
  PWMOnFirst = intensity > 0.5f;

  if (PWMOnFirst) {
    PWMLongTicks = (int) (PWMPeriod * intensity);
  } else {
    PWMLongTicks = (int) (PWMPeriod * (1.0f - intensity));
  }

  PWMShortTicks = PWMPeriod - PWMLongTicks;
}

void SetupPWM() {
  PWMIntensity(0.3f);
}
