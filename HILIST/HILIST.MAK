PROJ	=HILIST
DEBUG	=0
CC	=qcl
CFLAGS_G	= /AC /W3 /Ze 
CFLAGS_D	= /Zi /Gi$(PROJ).mdt /Od 
CFLAGS_R	= /O /Ol /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
LFLAGS_G	= /CP:0xffff /SE:0x80 /ST:0x800 
LFLAGS_D	= /CO /M /INCR 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=
OBJS_EXT = 	draw.obj 
LIBS_EXT = 	

all:	$(PROJ).exe

hilist.obj:	hilist.c

$(PROJ).exe:	hilist.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
hilist.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	link $(LFLAGS) @$(PROJ).crf

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

