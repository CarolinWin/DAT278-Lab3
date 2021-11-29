/*
 * pwmled.h
 *
 *  Created on: 30 nov. 2019
 *      Author: Albin
 */

#ifndef SRC_PWMLED_H_
#define SRC_PWMLED_H_


void SetupPWM();

// Must be called very often, otherwise the illusion is broken!!
void PWMLed();

// Input: anything between 0 (off) and 1.0 (full intensity)
void PWMIntensity(float intensity);


#endif /* SRC_PWMLED_H_ */
