
#include <conio.h>
#include <math.h>
#include <stdio.h>

#define SEGMENTS 78
#define STEPS 12
#define VERTEXS (SEGMENTS * STEPS)
#define FACES (SEGMENTS * STEPS)
#define BRAD 45.0
#define SRAD 20.0
#define SLEV 5
#define SVAL 5

typedef struct {
	float x, y, z;
} sFVERTEX;

typedef struct {
	int Vex[4];
	//int VNormal[4];
	int Normal;
	sFVERTEX Center;
} sFACE;

void CalcVList(void);
void CalcCircle(int seg, sFVERTEX center, float alpha);
void CalcFList(void);
void CalcNormal(void);
void CalcVNList(void);
void Write3DO(char *name);

sFVERTEX huge VList[VERTEXS];
sFVERTEX huge VNList[VERTEXS];
sFACE huge FList[FACES];
sFVERTEX huge NList[FACES];

int main(int argc, char * argv[])
{
	int i;

	if(argc < 2)
		return;

	CalcVList();
	CalcFList();
	CalcNormal();
	CalcVNList();

	Write3DO(argv[1]);

	return 0;
}

void CalcVList(void)
{
	float alpha;
	int seg;
	sFVERTEX center;

	for(seg=0; seg < SEGMENTS; seg++) {
		alpha=((double)2.0 * M_PI * seg) / SEGMENTS;
		center.z=SLEV * sin(alpha*SVAL);
		center.x=BRAD * cos(alpha);
		center.y=BRAD * sin(alpha);
		if((seg & 31) == 0)
			printf(" - seg %d/%d           \r", seg, SEGMENTS);
		CalcCircle(seg, center, alpha);
	}
	printf(" + VList completed %d vertexs\n", VERTEXS);
}

void CalcCircle(int seg, sFVERTEX center, float alpha)
{
	int v0=seg * STEPS;
	int v;
	float beta;
	sFVERTEX Vect;

	for(v=0; v < STEPS; v++) {
		beta=((double)2 * M_PI * v) / STEPS;
		Vect.x=cos(alpha) * cos(beta) * SRAD + center.x;
		Vect.y=sin(alpha) * cos(beta) * SRAD + center.y;
		Vect.z=sin(beta) * SRAD + center.z;
		VList[v0 + v]=Vect;
	}
}

void CalcFList(void)
{
	int f, seg, step;

	for(seg=0; seg < SEGMENTS; seg++)
	{
		for(step=0; step < STEPS; step++)
		{
			f=seg * STEPS + step;
			FList[f].Vex[0]=seg * STEPS + step;
			FList[f].Vex[1]=seg * STEPS + ((step + 1) % STEPS);
			FList[f].Vex[2]=((seg + 1) % SEGMENTS) * STEPS + ((step + 1) % STEPS);
			FList[f].Vex[3]=((seg + 1) % SEGMENTS) * STEPS + step;
			FList[f].Center.x=(VList[FList[f].Vex[0]].x + VList[FList[f].Vex[1]].x + VList[FList[f].Vex[2]].x + VList[FList[f].Vex[3]].x) / 4;
			FList[f].Center.y=(VList[FList[f].Vex[0]].y + VList[FList[f].Vex[1]].y + VList[FList[f].Vex[2]].y + VList[FList[f].Vex[3]].y) / 4;
			FList[f].Center.z=(VList[FList[f].Vex[0]].z + VList[FList[f].Vex[1]].z + VList[FList[f].Vex[2]].z + VList[FList[f].Vex[3]].z) / 4;
		}
		printf(" - face %d/%d           \r", f, FACES);
	}
	printf(" + FList completed %d facexs\n", FACES);
}

int Normalize(sFVERTEX *N)
{
	double module;

	module = sqrt((N->x * N->x) + (N->y * N->y) + (N->z * N->z));
	if(module > 0)
	{
		N->x/=module;
		N->y/=module;
		N->z/=module;
		return 0;
	}
	return 1;
}

void CalcNormal(void)
{
	sFVERTEX V, W, Normal, Normal1, Normal2;
	double Module;
	int f;

for(f=0; f < FACES; f++) {

	V.x = VList[FList[f].Vex[0]].x - VList[FList[f].Vex[1]].x;
	V.y = VList[FList[f].Vex[0]].y - VList[FList[f].Vex[1]].y;
	V.z = VList[FList[f].Vex[0]].z - VList[FList[f].Vex[1]].z;

	W.x = VList[FList[f].Vex[0]].x - VList[FList[f].Vex[2]].x;
	W.y = VList[FList[f].Vex[0]].y - VList[FList[f].Vex[2]].y;
	W.z = VList[FList[f].Vex[0]].z - VList[FList[f].Vex[2]].z;

	Normal1.x = (V.y * W.z) - (V.z * W.y);
	Normal1.y = (V.z * W.x) - (V.x * W.z);
	Normal1.z = (V.x * W.y) - (V.y * W.x);

	if(Normalize(&Normal1))
		printf(" > 0 MOD1\n");

	V.x = VList[FList[f].Vex[3]].x - VList[FList[f].Vex[1]].x;
	V.y = VList[FList[f].Vex[3]].y - VList[FList[f].Vex[1]].y;
	V.z = VList[FList[f].Vex[3]].z - VList[FList[f].Vex[1]].z;

	W.x = VList[FList[f].Vex[3]].x - VList[FList[f].Vex[2]].x;
	W.y = VList[FList[f].Vex[3]].y - VList[FList[f].Vex[2]].y;
	W.z = VList[FList[f].Vex[3]].z - VList[FList[f].Vex[2]].z;

	Normal2.x = (V.y * W.z) - (V.z * W.y);
	Normal2.y = (V.z * W.x) - (V.x * W.z);
	Normal2.z = (V.x * W.y) - (V.y * W.x);

	if(Normalize(&Normal2))
		printf(" > 0 MOD2\n");

	Normal.x=-(Normal1.x + Normal2.x) / 2;
	Normal.y=-(Normal1.y + Normal2.y) / 2;
	Normal.z=-(Normal1.z + Normal2.z) / 2;

	//Normal.x=Normal1.x;
	//Normal.y=Normal1.y;
	//Normal.z=Normal1.z;

	if(Normalize(&Normal))
	{
		Normal=Normal1;
		printf(" > 0 MODM\n");
	}

	FList[f].Normal=f;
	NList[f]=Normal;

	if((f & 63) == 0)
		printf(" - normal %d/%d\r",	f, FACES);

	/*
	printf(" - normal %d/%d -> (%f, %f, %f) (%f, %f, %f) -> (%f, %f, %f)\n",
		f, FACES,
		Normal1.x, Normal1.y, Normal1.y,
		Normal2.x, Normal2.y, Normal2.y,
		Normal.x, Normal.y, Normal.y);
	*/

}
	printf(" + NList completed %d normals\n", FACES);
};

void CalcVNList(void)
{
	int v, fv, f, count;
	sFVERTEX vex;

	for(v=0; v < VERTEXS; v++)
	{
		vex.x=vex.y=vex.z=0;
		count=0;

		for(f=0; f < FACES; f++)
		{
			for(fv=0; fv < 4; fv++)
			{
				if(FList[f].Vex[fv] == v)
				{
					vex.x+=NList[FList[f].Normal].x;
					vex.y+=NList[FList[f].Normal].y;
					vex.z+=NList[FList[f].Normal].z;
					count++;
				}
			}
		}

		if(count > 0)
		{
			vex.x/=count;
			vex.y/=count;
			vex.z/=count;
		}

		if(Normalize(&vex))
			printf(" > 0MODVN\n");

		if((v & 31) == 0)
			printf(" - Vertex %d/%d      \r", v, VERTEXS);

		VNList[v]=vex;
	}
}

#define _3DOB 0x424f4433L
#define _VLST 0x54534c56L
#define _FLST 0x54534c46L
#define _VNOR 0x524f4e56L
#define _FNOR 0x524f4e46L

void Write3DO(char *name)
{
	FILE *file;
	long x, y, z, i;
	long id, tmp;

	file=fopen(name, "wb");

#define _wdw(v) { tmp=(long)(v); fwrite(&tmp, 4, 1, file); }

	// 3DOB chunck
	printf(" %ld: 3DOB\n", ftell(file));
	_wdw(_3DOB);
	_wdw(4L * (3L * VERTEXS + 2 + 9L * FACES + 2 + 3L * VERTEXS + 2 + 3L * FACES + 2));
	_wdw(VERTEXS);
	_wdw(FACES);

	// VLST chunk
	printf(" %ld: VLST\n", ftell(file));
	_wdw(_VLST);
	_wdw(4L * 3L * VERTEXS);
	for(i=0; i < VERTEXS; i++)
	{
		_wdw(VList[i].x * 65536.0);
		_wdw(VList[i].y * 65536.0);
		_wdw(VList[i].z * 65536.0);
	}

	// FLST chunk
	printf(" %ld: FLST\n", ftell(file));
	_wdw(_FLST);
	_wdw(4L * 9L * FACES);
	for(i=0; i < FACES; i++)
	{
		_wdw(4);
		_wdw(FList[i].Vex[0]);
		_wdw(FList[i].Vex[1]);
		_wdw(FList[i].Vex[2]);
		_wdw(FList[i].Vex[3]);
		_wdw(FList[i].Normal);
		_wdw(FList[i].Center.x * 65536.0);
		_wdw(FList[i].Center.y * 65536.0);
		_wdw(FList[i].Center.z * 65536.0);
	}

	// VNOR chunk
	printf(" %ld: VNOR\n", ftell(file));
	_wdw(_VNOR);
	_wdw(4L * 3L * VERTEXS);
	for(i=0; i < VERTEXS; i++)
	{
		_wdw(VNList[i].x * 65536.0);
		_wdw(VNList[i].y * 65536.0);
		_wdw(VNList[i].z * 65536.0);
	}

	// FNOR chunk
	printf(" %ld: FNOR\n", ftell(file));
	_wdw(_FNOR);
	_wdw(4L * 3L * FACES);
	for(i=0; i < FACES; i++)
	{
		_wdw(NList[i].x * 65536.0);
		_wdw(NList[i].y * 65536.0);
		_wdw(NList[i].z * 65536.0);
	}

	fclose(file);
}