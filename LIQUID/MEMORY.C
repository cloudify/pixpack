
#include <dos.h>
#include <alloc.h>

#include "main.h"
#include "memory.h"

void *pAlloc(long lSize)
{
	void *pPtr=farmalloc(lSize + 16);
	unsigned long ulAligned;
	unsigned int uiSeg;

	if(pPtr == NULL)
		Error("MEMORY.C pAlloc - farmalloc -> NULL");

#ifdef DEBUG
	//printf(" * AllocSeg(%ld) %Fp (%ldk free)\n", lSize, pPtr, farcoreleft() >> 10);
#endif

	return pPtr;
}

void Free(void *pPtr)
{
	farfree(pPtr);
}