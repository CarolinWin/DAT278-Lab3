/***************************************************************************//**
 * @file
 * @brief Capacitive sense driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// XXX: This was hardware\kit\common\drivers\caplesense.c from the standard library, but it was conflicting with
// my lightsensor.c so I had to modify it slightly.
// Modifications are marked with XXX.

/* EM header files */
#include "em_device.h"

/* Drivers */
#include "caplesense.h"
#include "em_emu.h"
#include "em_acmp.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_lesense.h"

//#include "caplesenseconfig.h"
#include "globals.h"
#include "common_lesense.h"
#include "slider.h"


/**************************************************************************//**
 * @brief Get the position of the slider
 * @return The position of the slider if it can be determined,
 *         -1 otherwise.
 *****************************************************************************/
static int32_t getSliderPosition(void)
{
  int      i;
  int      minPos = -1;
  uint32_t minVal = 236; /* adjust it */
  /* Values used for interpolation. There is two more which represents the edges.
   * This makes the interpolation code a bit cleaner as we do not have to make special
   * cases for handling them */
  uint32_t interpol[6]      = { 255, 255, 255, 255, 255, 255 };
  uint32_t channelPattern[] = { 0, SLIDER_PART0_CHANNEL + 1,
                                SLIDER_PART1_CHANNEL + 1,
                                SLIDER_PART2_CHANNEL + 1,
                                SLIDER_PART3_CHANNEL + 1 };

  /* The calculated slider position. */
  int position;

  /* Iterate through the 4 slider bars and calculate the current value divided by
   * the maximum value multiplied by 256.
   * Note that there is an offset of 1 between channelValues and interpol.
   * This is done to make interpolation easier.
   */
  for (i = 1; i < CAPLESENSE_NUMOF_SLIDERS + 1; i++) {
    /* interpol[i] will be in the range 0-256 depending on channelMax */
    interpol[i]  = LESENSE_CHANNEL_VALUES[channelPattern[i] - 1] << 8;
    interpol[i] /= LESENSE_CHANNEL_MAX_VALUES[channelPattern[i] - 1];
    /* Find the minimum value and position */
    if (interpol[i] < minVal) {
      minVal = interpol[i];
      minPos = i;
    }
  }
  /* Check if the slider has not been touched */
  if (minPos == -1) {
    return -1;
  }

  /* Start position. Shift by 4 to get additional resolution. */
  /* Because of the interpol trick earlier we have to substract one to offset that effect */
  position = (minPos - 1) << 4;

  /* Interpolate with pad to the left */
  position -= ((256 - interpol[minPos - 1]) << 3)
              / (256 - interpol[minPos]);

  /* Interpolate with pad to the right */
  position += ((256 - interpol[minPos + 1]) << 3)
              / (256 - interpol[minPos]);

  return position;
}



void SetupSlider() {
  // All the setup has been moved to common_lesense since the hardware is shared with the light sensor
}

float Slider() {

  // -1 if not touched, between 0 and 48 if touched.
  int32_t slider = getSliderPosition();

  if (slider < 0) return -1.0;

  return ((float) slider) / 48.0f;

}
