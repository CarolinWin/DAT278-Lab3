#include <stdint.h>
#include <stdbool.h>
#include <em_core.h>
#include <em_device.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_emu.h>
#include <em_msc.h>
#include <em_acmp.h>
#include <bsp.h>
#include <segmentlcd.h>
#include <rtcdriver.h>
#include <bsp_trace.h>
#include <string.h>
#include <stdio.h>

#include "globals.h"
#include "common_lesense.h"
#include "slider.h"
#include "lightsensor.h"
#include "temperature.h"
#include "pwmled.h"
#include "lcsense.h"
#include "game.h"



static void setupCMU(void)
{
  SystemCoreClockUpdate();

  // Gotta go fast
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_1);

  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_ACMP0, true);
  CMU_ClockEnable(cmuClock_ACMP1, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockEnable(cmuClock_LESENSE, true);
  CMU_ClockEnable(cmuClock_DAC0, true);

  CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);

  SystemCoreClockUpdate();

}




void die(const char* message) {
  SegmentLCD_Write(message);
  // Halt and catch fire
  while (1) {};
}

int main(void)
{


  // Chip errata
  CHIP_Init();


  // Code correlation for the profiler
  BSP_TraceProfilerSetup();

  BSP_LedsInit();
  BSP_LedClear(0);

  setupCMU();
  SetupGlobals();
  SetupCommonLESense();
  SetupLightSensor();
  SetupTemperature();
  SetupPWM();
  SetupSlider();
  SetupLCSense();
  SetupGame();



  // Caches add latency, we can't have that. Turn it off!
  MSC_EnableCache(false);


  SegmentLCD_Init(false);

  int iteration=0;
  uint32_t last_temp_measurement = 0;
  while (true) {

    IDLE();

    ReadCommonLESense();

    // Light-sensitive lizard
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, !LightSensor());

    if (LCSense()) {
      BSP_LedSet(1);
    } else {
      BSP_LedClear(1);
    }

    Game();

    // Wrap-around protection!
    if (sysTicks < last_temp_measurement) last_temp_measurement = 0;

    // Don't update the temperature display too often, it gets hard to read
    if ((sysTicks - last_temp_measurement) > 5000) {
      float temp = Temperature();
      SegmentLCD_Number((int) (temp * 100.0));
      SegmentLCD_Symbol(LCD_SYMBOL_DP10, 1);   // Decimal point
      SegmentLCD_Symbol(LCD_SYMBOL_DEGC, 1);  // Celcius
      last_temp_measurement = sysTicks;
    }

    IDLE();

    if (Slider() >= 0) {
      PWMIntensity(Slider());
      //SegmentLCD_Number((int) (Slider() * 100));
    }



    IDLE();

    iteration++;
  }

  DelayMilli(5000);

  // Execution complete. Go to sleep.
  EMU_EnterEM4();
}

