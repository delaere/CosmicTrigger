########################################################################
#                                                                      
#              --- CAEN SpA - Computing Division ---                   
#                                                                      
#   CAENVMElib Software Project                                        
#                                                                      
#   Created  :  March 2004      (Rel. 1.0)                                             
#                                                                      
#   Auth: S. Coluccini                                                 
#                                                                      
########################################################################

# location of the Python header files
PYTHON_VERSION = 2.7
PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)

# location of the Boost Python include files and library
BOOST_INC = /usr/include
BOOST_LIB = /usr/lib64
PYTHION_LIB = /usr/lib/python$(PYTHON_VERSION)/config

PYLIB	=	VMEPythonModule.so

EXE	=	main

CC	=	g++

COPTS	=	-c -fPIC -g -DLINUX -Wall -std=c++17 -DBOOST_STACKTRACE_LINK -DBOOST_LOG_DYN_LINK

LOPTS	=	-Wl,-h -Wl,$(PYLIB) -shared -Wl,-Bstatic  -Wl,-Bdynamic -fPIC -g

FLAGS	=	-Wall -s -std=c++17

LFLAGS	=	-L$(BOOST_LIB) -L$(PYTHION_LIB)

DEPLIBS	=       -lCAENVME -lc -lm -lboost_stacktrace_basic -ldl -lboost_log_setup -lboost_log -lboost_thread -lpthread

DEPLIBSP=	-lboost_python27 -ldl -lpthread -lpython2.7 -lutil

LIBS	=	

INCLUDEDIR =	-I. -I$(PYTHON_INCLUDE)

OBJS	=	main.o include/Discri.o include/TDC.o include/TTCvi.o include/VmeBoard.o include/VmeController.o include/VmeUsbBridge.o include/CommonDef.o include/Scaler.o include/CaenetBridge.o include/HVmodule.o include/logger.o

LOBJS   =       include/PythonModule.o

INCLUDES =	include/CAENVMElib.h include/CAENVMEtypes.h include/CAENVMEoslib.h include/Discri.h include/TDC.h include/TTCvi.h include/VmeBoard.h include/VmeController.h include/VmeUsbBridge.h include/CommonDef.h include/Scaler.h include/CaenetBridge.h include/HVmodule.h include/logger.h

#########################################################################

all	:	$(PYLIB) $(EXE)

clean	:
		/bin/rm -f $(LOBJS) $(OBJS) $(PYLIB) $(EXE)

$(PYLIB):	$(LOBJS) $(OBJS)
		/bin/rm -f $(PYLIB)
		$(CC) $(LFLAGS) -o $(PYLIB) $(LOPTS) $(OBJS) $(LOBJS) $(DEPLIBS) $(DEPLIBSP)

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXE)
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

$(LOBJS):	$(INCLUDES) Makefile

%.o	:	%.cpp
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<
