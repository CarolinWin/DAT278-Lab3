#ifndef SRC_SLIDER_H_
#define SRC_SLIDER_H_

#include <inttypes.h>

void SetupSlider();

// Returns a value between 0.0 and 1.0 if the slider is being touched.
// Returns a negative value if there is no touching going on.
float Slider();

#endif /* SRC_SLIDER_H_ */
