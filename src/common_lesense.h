#ifndef SRC_COMMON_LESENSE_H_
#define SRC_COMMON_LESENSE_H_

#include <inttypes.h>

#define CAPLESENSE_CHANNEL_INT        (LESENSE_IF_CH8 | LESENSE_IF_CH9 | LESENSE_IF_CH10 | LESENSE_IF_CH11)
#define LESENSE_CHANNELS        16  /**< Number of channels for the Low Energy Sensor Interface. */

#define LIGHTSENSE_EXCITE_PORT   gpioPortD
#define LIGHTSENSE_EXCITE_PIN    6U
#define LIGHTSENSE_SENSOR_PORT   gpioPortC
#define LIGHTSENSE_SENSOR_PIN    6U

#define LIGHTSENSE_CHANNEL 6
#define LCSENSE_CHANNEL 7
#define SLIDER_PART0_CHANNEL    8   /**< Touch slider channel Part 0 */
#define SLIDER_PART1_CHANNEL    9   /**< Touch slider channel Part 1 */
#define SLIDER_PART2_CHANNEL    10  /**< Touch slider channel Part 2 */
#define SLIDER_PART3_CHANNEL    11  /**< Touch slider channel Part 3 */

#define CAPLESENSE_SENSITIVITY_OFFS    1U
#define CAPLESENSE_NUMOF_SLIDERS       4                          /**< Number of sliders */
#define CAPLESENSE_ACMP_VDD_SCALE      0x37     /**< Upper voltage threshold */

#define CAPLESENSE_SLIDER_PORT0        gpioPortC                  /**< Slider Port. GPIO Port C */
#define CAPLESENSE_SLIDER0_PORT        CAPLESENSE_SLIDER_PORT0      /**< Slider 0 Port. GPIO Port C */
#define CAPLESENSE_SLIDER0_PIN         8UL                        /**< Slider 0 Pin 8 */
#define CAPLESENSE_SLIDER1_PORT        CAPLESENSE_SLIDER_PORT0      /**< Slider 1 Port. GPIO Port C */
#define CAPLESENSE_SLIDER1_PIN         9UL                        /**< Slider 1 Pin 9 */
#define CAPLESENSE_SLIDER2_PORT        CAPLESENSE_SLIDER_PORT0      /**< Slider 2 Port. GPIO Port C */
#define CAPLESENSE_SLIDER2_PIN         10UL                       /**< Slider 2 Pin 10 */
#define CAPLESENSE_SLIDER3_PORT        CAPLESENSE_SLIDER_PORT0      /**< Slider 3 Port. GPIO Port C */
#define CAPLESENSE_SLIDER3_PIN         11UL                       /**< Slider 3 Pin 11 */

#define LCSENSE_SENSOR_PORT  gpioPortC
#define LCSENSE_SENSOR_PIN   7U


// Updated with values received from sensors
extern volatile uint32_t LESENSE_CHANNEL_VALUES[LESENSE_CHANNELS];
extern volatile uint32_t LESENSE_CHANNEL_MAX_VALUES[LESENSE_CHANNELS];

// The LESense module is shared between multiple of our sensors, so we have to
// configure it centrally.
// Light sensor is connected to ACM module 0 channel 6
// Capacitive sensor (the slider) also uses LESense
void SetupCommonLESense();

// Call periodically to get sensor readings
void ReadCommonLESense();

#endif /* SRC_COMMON_LESENSE_H_ */
