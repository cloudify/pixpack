#define	MAXPOINT	1000

void	CopyAndClear	( void );
void	MakeBuffer		( void );
void	RotateList		( int angle );
void	RotateAndMakeBuffer		( int Angle );
void	UpdateList		( void );
void	Remap			( void );
void	MakePalette		( int r, int g, int b );

typedef unsigned char	uchar;
typedef unsigned int	uint;

typedef struct
{
	long	x, y;
} sPoint;
