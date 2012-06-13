
#ifndef _MEMORYH_
#define _MEMORYH_

#include "main.h"

#define _pAlignPara(ptr) ((void*)(((unsigned long)ptr & 0xffff0000L) + (((unsigned long)ptr & 0xfff0L) << 12)+ (((unsigned long)ptr & 0xfL) ? 0x10000L : 0)))

void *pAlloc(long lSize);
void Free(void *pPtr);

#endif