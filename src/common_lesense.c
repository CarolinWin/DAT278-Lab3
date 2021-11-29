
#ifndef SRC_COMMON_LESENSE_C_
#define SRC_COMMON_LESENSE_C_

#include <em_acmp.h>
#include <em_gpio.h>
#include <em_lesense.h>
#include <em_prs.h>

#include "globals.h"
#include "common_lesense.h"
#include "lesense_conf.h"

#define CAPLESENSE_NUMOF_SLIDERS       4                          /**< Number of sliders */

volatile uint32_t LESENSE_CHANNEL_VALUES[LESENSE_CHANNELS] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

volatile uint32_t LESENSE_CHANNEL_MAX_VALUES[LESENSE_CHANNELS] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

static const bool channelsInUse[LESENSE_CHANNELS] = LESENSE_CH_IN_USE;


static void setupGPIO(void)
{
  /* Configure the drive strength of the ports for the light sensor. */
  GPIO_DriveModeSet(LIGHTSENSE_EXCITE_PORT, gpioDriveModeStandard);
  GPIO_DriveModeSet(LIGHTSENSE_SENSOR_PORT, gpioDriveModeStandard);

  /* Initialize the 2 pins of the light sensor setup. */
  GPIO_PinModeSet(LIGHTSENSE_EXCITE_PORT, LIGHTSENSE_EXCITE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(LIGHTSENSE_SENSOR_PORT, LIGHTSENSE_SENSOR_PIN, gpioModeDisabled, 0);

  /* Configure the drive strength of the ports for the touch slider. */
  GPIO_DriveModeSet(CAPLESENSE_SLIDER_PORT0, gpioDriveModeStandard);

  /* Initialize the 4 GPIO pins of the touch slider for using them as LESENSE
   * scan channels for capacitive sensing. */
  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER0_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER1_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER2_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER3_PIN, gpioModeDisabled, 0);
}

static void setupACMP0() {
  // Light sensor is connected to ACMP0
  static const ACMP_Init_TypeDef initACMP =
  {
    .fullBias = false,                 /* fullBias */
    .halfBias = true,
    .biasProg =  0x0,
    .interruptOnFallingEdge =  false,  /* interrupt on rising edge */
    .interruptOnRisingEdge =  false,   /* interrupt on falling edge */
    .warmTime = acmpWarmTime512,       /* 512 cycle warmup to be safe */
    .hysteresisLevel = acmpHysteresisLevel5,
    .inactiveValue = false,            /* inactive value */
    .lowPowerReferenceEnabled = false, /* low power reference */
    .vddLevel = 0x00,                  /* VDD level */
    .enable = false                    /* Don't request enabling. */
  };


  ACMP_Init(ACMP0, &initACMP);

  // Channel 0 is connected to the light sensor
  ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel7);

}

static void setupACMP1() {
  // The touch-sensitive slider is connected to ACMP1
  static const ACMP_CapsenseInit_TypeDef initACMP =
  {
    .fullBias                 = false,
    .halfBias                 = false,
    .biasProg                 =                  0x7,
    .warmTime                 = acmpWarmTime512,
    .hysteresisLevel          = acmpHysteresisLevel7,
    .resistor                 = acmpResistor0,
    .lowPowerReferenceEnabled = false,
    .vddLevel                 =                 0x3D,
    .enable                   = false
  };

  /* Configure ACMP locations, ACMP output to pin disabled. */
  ACMP_GPIOSetup(ACMP1, 0, false, false);

  /* Initialize ACMPs in capacitive sense mode. */
  ACMP_CapsenseInit(ACMP1, &initACMP);

}

static void setupPRS(void)
{
  PRS->ROUTE = 0x01U;
  PRS_SourceAsyncSignalSet(0U,
                           PRS_CH_CTRL_SOURCESEL_LESENSEL,
                           PRS_CH_CTRL_SIGSEL_LESENSESCANRES6);
}

static void calibrateCapSense() {
  /* Assuming that the pads are not touched at first, we can use the result as
   * the threshold value to calibrate the capacitive sensing in LESENSE. */

  uint16_t capsenseCalibrateVals[4];

  /* Read out steady state values from LESENSE for calibration. */
  for (int i = 0U, j = 0U; j < LESENSE_CHANNELS; j++) {
    if (channelsInUse[j]) {
      if (j == SLIDER_PART0_CHANNEL ||
          j == SLIDER_PART1_CHANNEL ||
          j == SLIDER_PART2_CHANNEL ||
          j == SLIDER_PART3_CHANNEL) {
        if (i < CAPLESENSE_NUMOF_SLIDERS) {
          capsenseCalibrateVals[i] = LESENSE_CHANNEL_VALUES[j]
                                     - CAPLESENSE_SENSITIVITY_OFFS;
        }
        i++;
      }
    }
  }

  /* Set calibration values. */
  LESENSE_ChannelThresSet(SLIDER_PART0_CHANNEL, CAPLESENSE_ACMP_VDD_SCALE, capsenseCalibrateVals[0]);
  LESENSE_ChannelThresSet(SLIDER_PART1_CHANNEL, CAPLESENSE_ACMP_VDD_SCALE, capsenseCalibrateVals[1]);
  LESENSE_ChannelThresSet(SLIDER_PART2_CHANNEL, CAPLESENSE_ACMP_VDD_SCALE, capsenseCalibrateVals[2]);
  LESENSE_ChannelThresSet(SLIDER_PART3_CHANNEL, CAPLESENSE_ACMP_VDD_SCALE, capsenseCalibrateVals[3]);

}

static void calibrateLCSense() {
  LESENSE_ChannelThresSet(LCSENSE_CHANNEL, 0, LESENSE_CHANNEL_VALUES[LCSENSE_CHANNEL]);
}

static void setupLESense() {
  /* LESENSE channel configuration constant table. */
   static const LESENSE_ChAll_TypeDef initChs = LESENSE_COMBINED_SCAN_CONF;
   /* LESENSE alternate excitation channel configuration constant table. */
   static const LESENSE_ConfAltEx_TypeDef initAltEx = LESENSE_LIGHTSENSE_ALTEX_CONF;
   /* LESENSE central configuration constant table. */
   static const LESENSE_Init_TypeDef initLESENSE =
   {
     .coreCtrl =
     {
       .scanStart = lesenseScanStartPeriodic,
       .prsSel = lesensePRSCh0,
       .scanConfSel = lesenseScanConfDirMap,
       .invACMP0 = false,
       .invACMP1 = false,
       .dualSample = false,
       .storeScanRes = false,
       .bufOverWr = true,
       .bufTrigLevel = lesenseBufTrigHalf,
       .wakeupOnDMA = lesenseDMAWakeUpDisable,
       .biasMode = lesenseBiasModeDutyCycle,
       .debugRun = false
     },

     .timeCtrl =
     {
       .startDelay = 0U
     },

     .perCtrl =
     {
       .dacCh0Data = lesenseDACIfData,
       .dacCh0ConvMode = lesenseDACConvModeDisable,
       .dacCh0OutMode = lesenseDACOutModeDisable,
       .dacCh1Data = lesenseDACIfData,
       .dacCh1ConvMode = lesenseDACConvModeDisable,
       .dacCh1OutMode = lesenseDACOutModeDisable,
       .dacPresc = 0U,
       .dacRef = lesenseDACRefBandGap,
       .acmp0Mode = lesenseACMPModeMuxThres,
       .acmp1Mode = lesenseACMPModeMuxThres,
       .warmupMode = lesenseWarmupModeNormal
     },

     .decCtrl =
     {
       .decInput = lesenseDecInputSensorSt,
       .initState = 0U,
       .chkState = false,
       .intMap = true,
       .hystPRS0 = false,
       .hystPRS1 = false,
       .hystPRS2 = false,
       .hystIRQ = false,
       .prsCount = true,
       .prsChSel0 = lesensePRSCh0,
       .prsChSel1 = lesensePRSCh1,
       .prsChSel2 = lesensePRSCh2,
       .prsChSel3 = lesensePRSCh3
     }
   };

   LESENSE_Reset();

   /* Initialize LESENSE interface with RESET. */
   LESENSE_Init(&initLESENSE, true);

   /* Clear result buffer. */
   LESENSE_ResultBufferClear();

   /* Configure scan channels. */
   LESENSE_ChannelAllConfig(&initChs);

   /* Configure alternate excitation channels. */
   LESENSE_AltExConfig(&initAltEx);

   /* Set scan frequency (in Hz). */
   (void)LESENSE_ScanFreqSet(0U, 10U);

   /* Set clock divisor for LF clock. */
   LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);
   LESENSE_ClkDivSet(lesenseClkHF, lesenseClkDiv_1);

   /* Start scanning LESENSE channels. */
   LESENSE_ScanStart();

   ReadCommonLESense();

   calibrateLCSense();
   calibrateCapSense();
}


void SetupCommonLESense() {
  setupGPIO();
  setupACMP0();
  setupACMP1();
  setupPRS();
  setupLESense();

  LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);

}

void ReadCommonLESense() {

  // Wait for there to be some sensor data
  while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL)) IDLE();

  // Check all the LESense channels we care about
  for (int currentChannel = 0; currentChannel < LESENSE_CHANNELS; currentChannel++) {
    /* If this channel is not in use, skip to the next one */
    if (!channelsInUse[currentChannel]) {
      continue;
    }

    /* Read out value from LESENSE buffer */
    uint32_t count = LESENSE_ScanResultDataGet();

    /* Store value in channelValues */
    LESENSE_CHANNEL_VALUES[currentChannel] = count;

    if (count > LESENSE_CHANNEL_MAX_VALUES[currentChannel]) {
      LESENSE_CHANNEL_MAX_VALUES[currentChannel] = count;
    }
  }
}

#endif /* SRC_COMMON_LESENSE_C_ */
