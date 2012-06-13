/*
 * image warp
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <conio.h>

typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;

uint	far *PlaneData[4];

double	cang=400.0, crad=1.5;

int		main	( int argc, char *argv[] )
{
	FILE	*stream;
	uchar	far *Video=(uchar far*)0xA0000000L;
	uint	x, y, pos, page, plane;

	if( argc == 3 )
	{
		sscanf( argv[1], "%lf", &cang );
		sscanf( argv[2], "%lf", &crad );
	}

	/*
	PlaneData[0]=malloc(32000);
	PlaneData[1]=malloc(32000);
	PlaneData[2]=malloc(32000);
	PlaneData[3]=malloc(32000);

	if( (PlaneData[0] == NULL) ||
		(PlaneData[1] == NULL) ||
		(PlaneData[2] == NULL) ||
		(PlaneData[3] == NULL) )
		return 1;
	*/

	stream=fopen("warp.dat", "wb");

	for( plane=0; plane<4; plane++ )
	{
		double	rad, ang;
		int		nx, ny;
		uint	noff;

		pos=0;
		for( y=0; y < 200; y++ )
		{
			if( (y&15)==0 )
				printf(" p(%d):%d%%...	   \r", plane, y/2 );
			for( x=plane; x < 320; x+=4 )
			{
				double	ox=(double)x-160;
				double	oy=(double)y-100;

				rad=crad + sqrt( ox*ox + oy*oy );

				if((ox != 0) || (oy != 0))
					ang=atan2( oy, ox ) + rad/cang;

				nx=(uint)( (double)160.0 + (double)rad*cos( ang ) );
				ny=(uint)( (double)100.0 + (double)rad*sin( ang ) );

				if( (nx<0) ||
					(nx>319) ||
					(ny<0) ||
					(ny>199) )
				{
					noff=64000;
				}
				else
					noff=(uint)nx + (uint)320*ny;
				fwrite( &noff, 2, 1, stream );
				//pos++;
			}
		}
	}

	fclose( stream );

	return 0;
}
