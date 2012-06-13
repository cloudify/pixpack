/*
 * plasma tunnel
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#ifndef _TUNNELH_
#define _TUNNELH_

typedef unsigned char ubyte;
typedef unsigned int uword;

void SetPalette(void);
void MakeLine(ubyte *pBuffer);
void DrawLine(int y);
void MakeOffsetMap(void);

void DrawScreen(int OffX, int OffY);
void Smooth(void);

#endif
