PARSECDIR=/scratch/c25/parsec/build/install
PARSEC_PKGCONFIG=${PARSECDIR}/lib64/pkgconfig

export PKG_CONFIG_PATH+="${PARSEC_PKGCONFIG}"
l = $(shell export  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PARSECDIR}/lib64)
CC = gcc
#CC = cc
MPICC=mpicc

INCLUDES += ${PARSECDIR}/include
LIBS += ${PARSECDIR}/lib64
LDFLAGS += -Wl,-rpath,

PTGPP = ${PARSECDIR}/bin/parsec-ptgpp

TESTS = Ex04_ChainData

all:${TESTS}
JDFS += Ex04_ChainData.jdf
GENERATED_SRC = $(subst .jdf,.c,${JDFS})
GENERATED_HDR = $(subst .jdf,.h,${JDFS})
GENERATED_OBJ = $(subst .jdf,.o,${JDFS})

Ex04_ChainData.c Ex04_ChainData.h: Ex04_ChainData.jdf
	${PTGPP} -E -i $< -o `basename $@ .c` ${PTGPPFLAGS}

%.o: %.c
	${MPICC} -o $@ -c $< ${CFLAGS} -L${LIBS} -I${INCLUDES} ${LDFLAGS} -lparsec

Ex04_ChainData: Ex04_ChainData.o
	${MPICC} -o $@ $^ -L${LIBS} -I${INCLUDES} ${LDFLAGS} -lparsec


clean:
	rm -f *.o ${TESTS} ${PTESTS} ${GENERATED_OBJ} ${GENERATED_HDR} ${GENERATED_SRC}
