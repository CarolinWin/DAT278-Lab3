/*
 * dma.c
 *
 *  Created on: Aug 22, 2019
 *      Author: albin
 */

#include <em_core.h>
#include <em_dma.h>
#include <em_cmu.h>
#include <em_emu.h>
#include <dmactrl.h>
#include <string.h>
#include "dma.h"

// The global macro DMA_CHAN_COUNT tells us the
// maximum number of DMA channels supported by the hardware DMA controller
// We can use these all at the same time, if we need to.
volatile bool DMA_DONE[DMA_CHAN_COUNT];

// These are the structures used to configure each hardware DMA channel
DMA_CB_TypeDef dmacb[DMA_CHAN_COUNT];
DMA_CfgChannel_TypeDef chnlCfg[DMA_CHAN_COUNT];
DMA_CfgDescr_TypeDef   descrCfg[DMA_CHAN_COUNT];

// Chunk sizes for memory-to-memory transfers
#define ARB_SIZE        dmaArbitrate128
#define DATA_SIZE       dmaDataSize4



// When a DMA transfer completes, an interrupt is raised and
// the DMA controller driver will call this function to let us know.
static void dmaTransferCallback(unsigned int channel, bool primary, void* user) {

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  // Set the global flag for this channel, indicating that the transfer is complete.
  DMA_DONE[channel] = true;

  CORE_EXIT_ATOMIC();

}

// Find the first unused DMA channel
static uint32_t dmaSelectChannel() {
  for (uint32_t c=0; c<DMA_CHAN_COUNT; c++) {
    if (DMA_DONE[c]) return c;
  }

  // All the channels are in use. That's not good.
  // Safest bet is to just wait for a transfer to finish and use that.
  WaitForDMA(0);
  return 0;
}


uint32_t MemoryCopyDMA(void* dest, void* src, size_t bytes) {



  uint32_t channel = dmaSelectChannel();
  uint32_t channel_mask = (1 << channel);

  // Setup our callback function, will be invoked when a DMA transfer finishes
  dmacb[channel].cbFunc  = dmaTransferCallback;
  dmacb[channel].userPtr = NULL;

  // Configure DMA channel used
  chnlCfg[channel].highPri   = false;
  chnlCfg[channel].enableInt = true;   // Cause an interrupt when the transfer completes
  chnlCfg[channel].select    = 0;      // Transfer is triggered by software
  chnlCfg[channel].cb        = &dmacb[channel];
  DMA_CfgChannel(channel, &chnlCfg[channel]);

  // Configure the primary descriptor of that channel for our transfer
  // The Auto mode we are using only needs a primary descriptor.
  descrCfg[channel].dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg[channel].srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg[channel].size    = DATA_SIZE;
  descrCfg[channel].arbRate = ARB_SIZE;
  descrCfg[channel].hprot   = 0;
  DMA_CfgDescr(channel, true, &descrCfg[channel]);

  DMA_DONE[channel] = false;

  DMA_ActivateAuto(
       channel,      // Channel
       true,        // Primary descriptor for the channel
       dest,        // Destination buffer
       src,         // Source buffer
       (bytes / 4) - 1);   // Number of ADDITIONAL transfers to perform, after the first one

  // Transfer request from software.
  // We'll get a dmaTransferCallback in a bit.
  DMA->CHSWREQ |= channel_mask;

  return channel;

}

void WaitForDMA(uint32_t channel) {
  while(true) {

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    if ( !DMA_DONE[channel] ) {
      // Low-power mode, will wake up from interrupts
      EMU_EnterEM1();
    }
    CORE_EXIT_ATOMIC();

    if ( DMA_DONE[channel] ) break;
  }

}


void SetupDMA() {

  DMA_Reset();

  memset(&dmaControlBlock, 0x00, sizeof(DMA_DESCRIPTOR_TypeDef) * 16 * 2);

  // Enable CMU clock
  CMU_ClockEnable(cmuClock_DMA, true);


  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;


  // Configure general DMA issues
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;

  DMA_Init(&dmaInit);

  for (size_t i=0; i<DMA_CHAN_COUNT; i++) DMA_DONE[i] = true;
}

