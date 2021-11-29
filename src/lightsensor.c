#include <stdint.h>
#include <stdbool.h>


#include "common_lesense.h"



void SetupLightSensor() {

  // I think it goes something like this:
  // The light sensor is an analog signal on pin PC6
  // That pin is connected to the Analog Comparator module (ACMP)
  // ACMP compares the analog measurement to some reference voltage
  // The LESENSE module can detect when ACMP's comparison result changes.

  // Setup of all this moved to the common_lesense module since it's shared with other sensors.
}


bool LightSensor() {
  return LESENSE_CHANNEL_VALUES[LIGHTSENSE_CHANNEL];
}
