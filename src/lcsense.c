#include <em_dac.h>

#include "lcsense.h"
#include "common_lesense.h"

void writeDataDAC(DAC_TypeDef *dac, unsigned int value, unsigned int ch)
{
  /* Write data output value to the correct register. */
  if (!ch) {
    /* Write data to DAC ch 0 */
    dac->CH0DATA = value;
  } else {
    /* Write data to DAC ch 1 */
    dac->CH1DATA = value;
  }
}

void setupDAC(void)
{
  /* Configuration structure for the DAC */
  DAC_Init_TypeDef dacInit = DAC_INIT_DEFAULT;
  /* Change the reference for Vdd */
  dacInit.reference = dacRefVDD;
  /* Initialize DAC */
  DAC_Init(DAC0, &dacInit);

  /* Set data for DAC channel 1 */
  writeDataDAC(DAC0, 800, 1);
}


void SetupLCSense() {
  setupDAC();
}

// Returns TRUE if there's something detected by the LC-SENSE coil (metal or magnetic)
bool LCSense() {
  return LESENSE_CHANNEL_VALUES[LCSENSE_CHANNEL];
}
