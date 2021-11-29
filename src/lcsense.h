

#ifndef SRC_LCSENSE_H_
#define SRC_LCSENSE_H_

#include <stdbool.h>

void SetupLCSense();

// Returns TRUE if there's something detected by the LC-SENSE coil (metal or magnetic)
bool LCSense();

#endif /* SRC_LCSENSE_H_ */
