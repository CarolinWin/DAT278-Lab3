/*
 * globals.h
 *
 *  Created on: 30 nov. 2019
 *      Author: Albin
 */

#ifndef SRC_GLOBALS_H_
#define SRC_GLOBALS_H_

void SetupGlobals();

// Must be called pretty often.
// Only time-sensitive stuff is run in it.
void IDLE();

void DelayTicks(unsigned int ticks);
void DelayMilli(unsigned int ms);

// An incrementing counter of 0.1ms ticks.
extern volatile unsigned int sysTicks;

#endif /* SRC_GLOBALS_H_ */
