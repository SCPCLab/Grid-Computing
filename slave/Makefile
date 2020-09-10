CC=sw5cc
CXX=sw5CC
LINK=swld453
AR=swar

CFLAG=-host -O2
CXXFLAG=-host -O2
SLAVECC=sw5cc
SLAVECXX=sw5CC
SLAVEFLAG=-slave
LINKFLAG=

HOME=$(shell pwd)

CXXOBJS=./wrapper.o

SLAVEOBJS=./kernel_slave.o

INCLUDE=${HOME}/../include
LIB=${HOME}/../lib
OBJ=${HOME}/../obj

DEPS:=$(CXXOBJS:.o=.d)

SLAVE=libkernel.a

OBJ: ${CXXOBJS} ${SLAVEOBJS}
	cp -f $^ ${OBJ}

${CXXOBJS}:%.o:%.cpp
	${CXX} ${CXXFLAG} -c $< -o $@ -I${INCLUDE}

${SLAVEOBJS}:%.o:%.c
	${CC} ${SLAVEFLAG} -c $< -o $@ -I${INCLUDE}

clean:
	rm -f ${CXXOBJS} ${SLAVEOBJS}

-include $(DEPS)