
#ifndef _FILEIOH_
#define _FILEIOH_

void *pLoadFile(char *pcName, void *pBuffer, long lSize);
int hOpen(char *pcName);
void Read(int hFile, void *pBuffer, long lLen);
int Close(int hFile);

#endif