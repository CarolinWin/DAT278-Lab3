
#include "temperature.h"
#include <em_adc.h>
#include <em_cmu.h>

#include "globals.h"

float convertToCelsius(int32_t adcSample)
{
  float temp;
  /* Factory calibration temperature from device information page. */
  float cal_temp_0 = (float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK)
                             >> _DEVINFO_CAL_TEMP_SHIFT);

  float cal_value_0 = (float)((DEVINFO->ADC0CAL2
                               & _DEVINFO_ADC0CAL2_TEMP1V25_MASK)
                              >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);

  /* Temperature gradient (from datasheet) */
  float t_grad = -6.27;

  temp = (cal_temp_0 - ((cal_value_0 - adcSample)  / t_grad));

  return temp;
}


void SetupTemperature() {
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);


  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;


  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(400000, 0);
  ADC_Init(ADC0, &init);

  /* Set input to temperature sensor. Reference must be 1.25V */
  sInit.reference = adcRef1V25;
  sInit.input = adcSingleInpTemp;

  // Perform single samples, whenever we command
  ADC_InitSingle(ADC0, &sInit);

}


float Temperature() {
  // Sample the analog channel once
  ADC_Start(ADC0, adcStartSingle);

  // Wait for ADC sampling to complete
  while (!(ADC0->STATUS & ADC_STATUS_SINGLEDV)) { IDLE(); }   // Don't mess up my lights

  // Convert to a useful scale
  uint32_t temp = ADC_DataSingleGet(ADC0);
  return convertToCelsius(temp);
}
