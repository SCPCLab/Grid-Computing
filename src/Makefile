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

# CXXFILE=main.cpp \
# 	./slave/wrapper.cpp \
# 	./CG_switch/CG_switch.cpp
CXXFILE=$(shell find ${HOME} -name "*.cpp")
CHOSTFILES=$(shell find ${HOME} -name "*.c")
COBJS=$(shell echo ${CHOSTFILES} | sed 's/\.c/\.o/g')
CXXOBJS=$(shell echo ${CXXFILE} | sed 's/\.cpp/\.o/g')

# CFILE=dataSet.c kernel.c \
# 	./CG_switch/athread_switch.c

# CXXOBJS=main.o \
# 	./slave/wrapper.o \
# 	./CG_switch/CG_switch.o

# COBJS=dataSet.o callback.o \
# 	./CG_switch/athread_switch.o


# SLAVEFILE=./slave/kernel_slave.c

# SLAVEOBJS=./slave/kernel_slave.o

INCLUDE=${HOME}/../include
LIB=${HOME}/../lib
OBJ=${HOME}/../obj

LIBS=-lm

DEPS:=$(CXXOBJS:.o=.d)

OBJ: ${CXXOBJS} ${COBJS} callback_slave.o
	cp -f $^ ${OBJ}

${CXXOBJS}:%.o:%.cpp
	${CXX} ${CXXFLAG} -c $< -o $@ -I${INCLUDE}

${COBJS}:%.o:%.c
	${CC} ${CFLAG} -c $< -o $@ -I${INCLUDE}

callback_slave.o:callback.c
	${CC} ${SLAVEFLAG} -c $< -o $@ ${INCLUDE}

clean:
	rm -f ${CXXOBJS} ${COBJS} callback_slave.o

-include $(DEPS)