CC=sw5cc
CXX=sw5CC
LINK=mpiCC

CFLAG=-host -O2
CXXFLAG=-host -O2
SLAVECC=sw5cc
SLAVECXX=sw5CC
SLAVEFLAG=-slave
LINKFLAG=

HOME=$(shell pwd)

HEADER=$(shell find ${HOME} -maxdepth 1 -name "*.h*")
HEADER+=$(shell find ${HOME}/src -name "*.h*")
HEADER+=$(shell find ${HOME}/slave -name "*.h*")

CXXFILE=main.cpp 

CXXOBJS=main.o

OBJ=$(shell find ${HOME}/obj -name "*.o")


INCLUDE=-I./include

EXE=test


${EXE}: ${CXXOBJS} ${OBJ}
	${LINK} ${LINKFLAG} -o ${EXE} $^ ${INCLUDE}

${CXXOBJS}:%.o:%.cpp
	${CXX} ${CXXFLAG} -c $< -o $@ ${INCLUDE}


clean:
	rm -f ${EXE} ${CXXOBJS} include/*

link:
	ln -sf ${HEADER} include/

-include $(DEPS)