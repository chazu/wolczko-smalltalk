#
# $Header$
#
# $Log$
#
CFLAGS= -O

SRC=		st80.c          \
		BC.c            \
		$(BITBLT).c     \
		$(IO).c         \
		IO.c		\
		PrimTable.c     \
		OM.c            \
		Process.c       \
		ArrStorePrim.c  \
		ControlPrims.c  \
		FloatPrim.c     \
		IOPrims.c       \
		IntegerPrims.c  \
		SystemPrims.c   \
		Snapshot.c      \
		CharScanPrim.c

OBJ=		st80.o          \
		BC.o            \
		$(BITBLT).o     \
		$(IO).o         \
		IO.o		\
		PrimTable.o     \
		OM.o            \
		Process.o       \
		ArrStorePrim.o  \
		ControlPrims.o  \
		FloatPrim.o     \
		IOPrims.o       \
		IntegerPrims.o  \
		SystemPrims.o   \
		Snapshot.o      \
		CharScanPrim.o

$(ST80) : $(OBJ)
	$(CC) $(CFLAGS) -o $(ST80) \
		OM.o st80.o BC.o $(BITBLT).o $(IO).o PrimTable.o Process.o \
		ArrStorePrim.o ControlPrims.o FloatPrim.o IOPrims.o \
		IntegerPrims.o SystemPrims.o Snapshot.o IO.o \
		CharScanPrim.o $(LIBES)

ArrStorePrim.o : Prim.h pos32int.h pos32val.h

ControlPrims.o FloatPrim.o IntegerPrims.o : Prim.h

SystemPrims.o : Prim.h pos32val.h

Process.o : Prim.h BCFreq.h

IOPrims.o : Prim.h BitBlt.h pos32int.h

CharScanPrim.o : Prim.h BitBlt.h

Prim.h :  ST.h OM.h OMMacros.h Regs.h StdPtrs.h ClassPtrs.h CtxMacros.h
	touch Prim.h

IO.h : ST.h StdPtrs.h
	touch IO.h

st80.o : BCFreq.h  ST.h OM.h OMMacros.h CtxMacros.h ClassPtrs.h StdPtrs.h IO.h
	$(CC) -c $(SS) $(CFLAGS) st80.c

BC.o :  ST.h OM.h OMMacros.h Regs.h StdPtrs.h \
	ClassPtrs.h Selectors.h CtxMacros.h

GenericBitBlt.o SunBitBlt.o XBitBlt.o :  ST.h

GenericIO.o :  IO.h OM.h OMMacros.h

SunIO.o :  ST.h StdPtrs.h IO.h

IO.o : IO.h

OM.o :  ST.h OM.h OMMacros.h StdPtrs.h ClassPtrs.h CtxMacros.h Regs.h

PrimTable.o : ST.h

Snapshot.o :  ST.h OM.h OMMacros.h ClassPtrs.h StdPtrs.h

lint : $(SRC)
	lint $(CFLAGS) -bach $(SRC)

clean : 
	rm -f *.o

tar :
	tar cfv /dev/rst8 doc *.[ch] st80*sun* Makefile makest *icon \
		tests/class* tests/method* tests/FileSys.st \
		tests/images/xerox.image  tests/images/X.image \
		tests/scripts/bootx*
