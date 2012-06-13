
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "midas.h"

#include "main.h"
#include "vga.h"

#include "plasma.h"
#include "title.h"
#include "slime.h"

extern ushort tmrSync200;

mpInformation *pMpInfo;
mpModule *pModMusic = NULL;

void main(void)
{
	TimerSync();

	InitPlayer();

	vgaInit();
	vgaSet3();

	printf(" - load MODULE...\n");
	LoadModule();

	printf(" - start PLAYER...\n");
	StartPlayer();

	vgaSet200c();
	tmrSyncScr(tmrSync200, preVR, NULL, NULL);

	DoPlasma();
	DoTitle();
	DoSlime();

	getch();

	musicFadeOut();
	tmrStopScrSync();
	ClosePlayer();
	vgaSet3();
}

void Error(char *pcText)
{
	ClosePlayer();
	vgaSet3();
	printf(" LiQUiD - Pix / NeMeSiS Production / Italy\n"
		   " Abnormal program termination:\n"
		   " > %s \n", pcText);
	exit(1);
}

void InitPlayer(void)
{
	midasSetDefaults();
	midasConfig();
	midasInit();
}

void LoadModule(void)
{
	pModMusic = midasLoadModule("intro.mod", &mpMOD, NULL);
}

void StartPlayer(void)
{
	midasPlayModule(pModMusic, 0);
}

void ClosePlayer(void)
{
	tmrStopScrSync();
	midasStopModule(pModMusic);
	midasFreeModule(pModMusic);
	pModMusic = NULL;
	midasClose();
}

mpInformation *pInfoUpdate(void)
{
	midasMP->GetInformation(&pMpInfo);
	return pMpInfo;
}

void musicFadeOut(void)
{
	unsigned char vol;

	midasSD->GetMasterVolume(&vol);
	while((--vol) > 0)
	{
		frameWaitOne();
		midasSD->SetMasterVolume(vol);
	}
}

