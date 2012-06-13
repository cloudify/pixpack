#ifndef _GLOBALSH_
#define _GLOBALSH_

#include "midas.h"
#include "main.h"

#ifndef frameCount
extern volatile long frameCount;
#endif

#ifndef vgaStart
extern uword vgaStart;
#endif

#ifndef tmrSync200
extern ushort tmrSync200;
#endif

#ifndef pMpInfo
extern mpInformation *pMpInfo;
#endif

#endif