
#include <dos.h>
#include <fcntl.h>
#include <io.h>

#include "main.h"
#include "fileio.h"
#include "memory.h"

#define _hOpenFile(pcName) _open(pcName, O_RDWR)
#define _CloseFile(hFile) _close(hFile)

void *pLoadFile(char *pcName, void *pBuffer, long lSize)
{
	int hFile;
	long lFileSize;

	hFile=_hOpenFile(pcName);
	if(hFile < 0)
		Error("FILEIO.C pLoadFile() - Open error");

	if(_read(hFile, pBuffer, (uword)lSize) == -1)
		Error("FILEIO.C pLoadFile - read error");
	_CloseFile(hFile);
	return pBuffer;
}

int hOpen(char *pcName)
{
	int hFile;
	hFile=_hOpenFile(pcName);
	if(hFile < 0)
		Error("FILEIO.C pOpen() - Open error");
	return hFile;
}

void Read(int hFile, void *pBuffer, long lLen)
{
	if(_read(hFile, pBuffer, (uword)lLen) == -1)
		Error("FILEIO.C pLoadFile - read error");
}

int Close(int hFile)
{
	return _CloseFile(hFile);
}