/*
 * dma.h
 *
 *  Created on: Aug 22, 2019
 *      Author: albin
 */

#ifndef MAIN_DMA_H_
#define MAIN_DMA_H_

#include <stddef.h>

// Called once on startup to start the DMA controller and set up basic
// configuration.
void SetupDMA();

// Copy data from one place in memory to another, in the background.
// Returns the DMA channel used for the transfer.
uint32_t MemoryCopyDMA(void* dest, void* src, size_t bytes);

// Pass in the channel number returned from MemoryCopyDMA()
// Puts the core in sleep mode until the DMA transfer has completed
// Only call this if you've actually started a DMA memory transfer using MemoryCopyDMA,
// as it will otherwise cause the system to wait FOREVER.
void WaitForDMA(uint32_t channel);

#endif /* MAIN_DMA_H_ */
