ifndef CMDIR
export CMDIR=${PWD}/CMlib
endif

ifndef MFDIR
export MFDIR=${PWD}/MFlib
endif


all: CM MF model

clean:
	make -C ${CMDIR}/src clean
	make -C ${MFDIR}/src clean
	make -C WBMsed/src clean

CM:
	make -C ${CMDIR} all

MF:
	make -C ${MFDIR} all

model: CM MF
	make -C WBMsed all

