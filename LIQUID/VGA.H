
#ifndef _VGAH_
#define _VGAH_

#include "main.h"

void vgaInit(void);
void vgaSet200c(void);
void vgaSet3(void);
void vgaDupeScan(uword val);

void TimerSync(void);
void frameWait(int count);
void preVR(void);
void frameWaitOne(void);

void FadePalFromBlack(char *pDst, char *pSrc, int Pos);
void RemapPal(char *pPalette, int Index, int Count);
void MakeBlackToWhitePal(char *pPalette, int pos);
void FadePalFromWhite(char *pDst, char *pSrc, int Pos);

#endif
