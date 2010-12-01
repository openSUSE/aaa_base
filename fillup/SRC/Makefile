
################################################################################
#									       #
#									       #
# Copyrights to S.u.S.E. GmbH Fuerth (c) 1996                                  #
#									       #
# Time-stamp: <>                                                               #
# Project:                                                                     #
# Module:                                                                      #
# Filename:                                                                    #
# Authors:                                                                     #
# Description:                                                                 #
#									       #
################################################################################

VERSION = 0.1

# Set "COMPILE_OPTION" to "DEBUG", "OPTIMIZE" or "PROFILE"
# COMPILE_OPTION = DEBUG
COMPILE_OPTION = OPTIMIZE
# COMPILE_OPTION = PROFILE

# the ldb directories
PROJ      = Fillup                      # project name
ROOTDIR   = ..                          # directory where project dir is located
PROJDIR   = ..
SRC	  = ${PROJDIR}/SRC
OBJ	  = ${PROJDIR}/OBJ
BIN	  = ${PROJDIR}/BIN
ARCHIVE   = ${PROJDIR}/ARCHIVE

# Set "WARNINGS" to whatever you want
# WARNINGS = -Wall
WARNINGS = -Wunused -Wswitch -Wformat -Wreturn-type -Wimplicit -Wmissing-prototypes \
	   -Wmissing-declarations
#-Winline

DEFINES   = 

# LINUX system
OPTISPLUS = -m486
TM	  = \"LINUX\"
INC       = -I/usr/include
COMPILER  = -DGCC=1

# Set OPTIS to the following values
OPTIS = $(OPTISPLUS) -O2 -fforce-addr -finline-functions -fno-function-cse \
	-fkeep-inline-functions

ifeq ($(COMPILE_OPTION),DEBUG)
  # compile for debugging
  COMPILE  = gcc -DDEBUG -ansi -g -c ${WARNINGS} -I${SRC} -I${INC} ${COMPILER} ${SYS} ${DEFINES}
  LINK	   = gcc -g
endif

ifeq ($(COMPILE_OPTION),OPTIMIZE)
  # compile with all optimizations
  COMPILE  = gcc -ansi -c $(OPTIS) $(WARNINGS) -I${SRC} -I${INC} ${COMPILER} ${SYS} ${DEFINES}
  LINK	   = gcc -O -s
endif

ifeq ($(COMPILE_OPTION),PROFILE)
  # compile for use with "gprof"
  COMPILE  = gcc -ansi -pg -c ${WARNINGS} -I${SRC} -I${INC} ${COMPILER} ${SYS} ${DEFINES}
  LINK	   = gcc -pg
endif

#--------------------------------- targets to build -----------------------------------#

all: $(BIN)/fillup


#---------------------------------- linking files -------------------------------------#

OBJS = $(OBJ)/fillup.o $(OBJ)/file.o $(OBJ)/parse.o $(OBJ)/err.o

$(BIN)/fillup: $(OBJS)
	$(LINK) $(OBJS) -o $(BIN)/fillup 

#----------------------------------- object files -------------------------------------#

$(OBJ)/fillup.o: $(SRC)/import.h $(SRC)/export.h $(SRC)/portab.h $(SRC)/fillup.c  \
		$(SRC)/fillup.h $(SRC)/err.h $(SRC)/vars.h $(SRC)/defs.h
	$(COMPILE) -o $(OBJ)/fillup.o $(SRC)/fillup.c

$(OBJ)/file.o:  $(SRC)/import.h $(SRC)/export.h $(SRC)/portab.h $(SRC)/file.c \
		$(SRC)/file.h $(SRC)/err.h $(SRC)/vars.h $(SRC)/defs.h
	$(COMPILE) -o $(OBJ)/file.o $(SRC)/file.c

$(OBJ)/parse.o: $(SRC)/import.h $(SRC)/export.h $(SRC)/portab.h $(SRC)/parse.c \
		$(SRC)/parse.h $(SRC)/err.h $(SRC)/vars.h $(SRC)/defs.h
	$(COMPILE) -o $(OBJ)/parse.o $(SRC)/parse.c

$(OBJ)/err.o: $(SRC)/import.h $(SRC)/export.h $(SRC)/portab.h $(SRC)/err.c \
		$(SRC)/err.h $(SRC)/vars.h $(SRC)/defs.h
	$(COMPILE) -o $(OBJ)/err.o $(SRC)/err.c

#----------------------------------- any functions ------------------------------------#

install: all
	install -s -o root -m 755 $(BIN)/fillup /usr/local/bin

clean:
	rm -f $(OBJ)/*.o $(SRC)/\#* $(SRC)/*~ 

distclean: clean
	rm -f $(BIN)/*











