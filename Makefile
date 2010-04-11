CC = gcc
LD = $(CC)
CFLAGS = -I$(FPM_PATH) -I$(OOMATHS_PATH) -DOOMATHS_STANDALONE=1 -g -ffast-math
LDFLAGS = -lpng -lz

EXECUTABLE = planettool


ifneq ($(debug),yes)
	CFLAGS += -O3 -ftree-vectorize -funroll-loops
endif

ifndef srcdir
	srcdir = .
endif

FPM_PATH = $(srcdir)/FloatPixMap/
OOMATHS_PATH = $(srcdir)/../../src/Core/


vpath %.h $(FPM_PATH):$(OOMATHS_PATH)
vpath %.c $(FPM_PATH)
vpath %.m $(OOMATHS_PATH)

.SUFFIXES: .m


CORE_OBJECTS = main.o SphericalPixelSource.o ReadLatLong.o ReadCube.o LatLongGridGenerator.o RenderToLatLong.o RenderToCube.o MatrixTransformer.o
FPM_OBJECTS = FloatPixMap.o FPMGamma.o FPMImageOperations.o FPMPNG.o FPMQuantize.o FPMRaw.o
OOMATHS_OBJECTS = OOFastArithmetic.o OOMatrix.o OOQuaternion.o OOVector.o

OBJECTS = $(CORE_OBJECTS) $(FPM_OBJECTS) $(OOMATHS_OBJECTS)


planettool: $(OBJECTS)
	$(LD) -o $(EXECUTABLE) $(LDFLAGS) $(OBJECTS)


# Rule to compile Objective-C maths files as C.
.m.o:
	$(CC) -c -x c $(CFLAGS) -o $@ $<


# Core dependencies.
SphericalPixelSource.h: FloatPixMap.h
LatLongGridGenerator.h ReadLatLong.h ReadCube.h MatrixTransformer.h RenderToLatLong.h RenderToCube.h: SphericalPixelSource.h

main.o: FPMPNG.h LatLongGridGenerator.h ReadLatLong.h MatrixTransformer.h RenderToLatLong.h RenderToCube.h planettool-version.h

SphericalPixelSource.o: SphericalPixelSource.h
ReadLatLong.o: ReadLatLong.h FPMImageOperations.h
ReadCube.o: ReadCube.h FPMImageOperations.h
RenderToLatLong.o: RenderToLatLong.h FPMImageOperations.h
LatLongGridGenerator.o: LatLongGridGenerator.h
RenderToCube.o: RenderToCube.h FPMImageOperations.h
MatrixTransformer.o: MatrixTransformer.h


# FloatPixMap dependencies.
FloatPixMap.h FPMVector.h: FPMBasics.h
FPMPNG.h FPMGamma.h FPMImageOperations.h FPMQuantize.h FPMRaw.h : FloatPixMap.h
FPMPNG.h : FPMGamma.h FPMQuantize.h

FloatPixMap.o: FloatPixMap.h
FPMGamma.o: FPMGamma.h FPMImageOperations.h
FPMImageOperations.o: FPMImageOperations.h FPMVector.h
FPMPNG.o: FPMPNG.h
FPMQuantize.o: FPMQuantize.h FPMImageOperations.h
FPMRaw.o: FPMRaw.h FPMImageOperations.h



.PHONY: clean
clean:
	-rm -f *.o $(EXECUTABLE)