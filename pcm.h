#ifndef PCM_H
#define PCM_H

#include "tm_stm32f4_fatfs.h"
#include "attributes.h"

//#define PCM_ENABLED

#ifdef PCM_ENABLED

extern FATFS FatFs;
extern FIL fil;

bool PcmPlay();
bool PcmOpenFile(char *filename,TM_FATFS_Partition apartition);
void PcmSendBuffer();

#endif

#endif
