# CC  = sw5cc
# CPP = swg++453
# HOST_FLAGS  = -host
# SLAVE_FLAGS = -slave
# ROOT = /home/export/online1/systest/swrh/guhf/Openfoam/source/OpenFOAM_SW/OpenFOAM-3.0.0
# INCLUDE=-fPIC -Dlinux64 -DWM_ARCH_OPTION=64 -DWM_DP -DWM_LABEL_SIZE=64 -Wall -Wextra -Wno-old-style-cast -Wnon-virtual-dtor -Wno-unused-parameter -O3 -DNoRepository -ftemplate-depth-100 -DSW_SLAVE -I/usr/sw-mpp/mpi2/include -I/usr/sw-mpp/swcc/swgcc453-binary/cross-tools/include -I/home/export/online1/systest/swrh/include

# ATHREAD_INCLUDE = -I/usr/sw-mpp/mpi2/include -I/usr/sw-mpp/swcc/swgcc453-binary/cross-tools/include

defult: CG_switch.o athread_switch.o
	swar cr $(LIB_ROOT)/libOSspecific.a $^
#cp ./*.h ../../lnInclude/

CG_switch.o:CG_switch.cpp
	${CPP} -o $@ -c $(INCLUDE) $<

athread_switch.o:athread_switch.c
	${CC} $(HOST_FLAGS) -c -o $@ $^

clean:
	rm *.o
	swar d $(LIB_ROOT)/libOSspecific.a CG_switch.o athread_switch.o

