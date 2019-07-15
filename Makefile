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

EXE	=	main

CC	=	g++

COPTS	=	-fPIC -DLINUX -Wall -std=c++17
#COPTS	=	-g -fPIC -DLINUX -Wall 

FLAGS	=	-Wall -s -std=c++17
#FLAGS	=	-Wall

DEPLIBS	=       -l CAENVME -l ncurses -lc -lm

LIBS	=	

INCLUDEDIR =	-I.

OBJS	=	main.o include/Discri.o include/TDC.o include/TTCvi.o include/VmeBoard.o include/VmeController.o include/VmeUsbBridge.o include/CommonDef.o include/Scaler.o include/CaenetBridge.o include/HVmodule.o


INCLUDES =	include/CAENVMElib.h include/CAENVMEtypes.h include/CAENVMEoslib.h

#########################################################################

all	:	$(EXE)

clean	:
		/bin/rm -f $(OBJS) $(EXE)

$(EXE)	:	$(OBJS)
		/bin/rm -f $(EXE)
		$(CC) $(FLAGS) -o $(EXE) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.cpp
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<
