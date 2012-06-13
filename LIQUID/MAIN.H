#ifndef _MAINH_
#define _MAINH_

typedef unsigned char ubyte;
typedef unsigned int uword;
typedef unsigned long udword;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#include "midas.h"

void InitPlayer(void);
void LoadModule(void);
void StartPlayer(void);
void ClosePlayer(void);
mpInformation *pInfoUpdate(void);
void musicFadeOut(void);

void Error(char *pcText);

#endif