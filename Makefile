# To be able to play with the Dummy:
#
# 1) Copy one of the objects:
#
# AIDummy.o.Linux64 (Linux   64 bits)
# AIDummy.o.Linux32 (Linux   32 bits)
# AIDummy.o.Win32   (Windows 32 bits)
# AIDummy.o.Win64   (Windows 64 bits)
# AIDummy.o.MacOS   (Mac)
#
# to AIDummy.o
#
# That is, if you use Linux64: cp AIDummy.o.Linux64 AIDummy.o
#
# 2) Copy one of the objects:
#
# Board.o.Linux64 (Linux   64 bits)
# Board.o.Linux32 (Linux   32 bits)
# Board.o.Win32   (Windows 32 bits)
# Board.o.Win64   (Windows 64 bits)
# Board.o.MacOS   (Mac)
#
# to Board.o
#
# # That is, if you use Linux64: cp Board.o.Linux64 Board.o
#
DUMMY_OBJ = AIDummy.o
BOARD_OBJ = Board.o

# Add here any extra .o player files you want to link to the executable
EXTRA_OBJS =

# Config
OPTIMIZE = 2 # Optimization level (0 to 3)
DEBUG    = 0 # Compile for debugging (0 or 1)
PROFILE  = 0 # Compile for profile (0 or 1)
32BITS   = 0 # Produce 32 bits objects on 64 bits systems (0 or 1)


# Do not edit past this line
############################

# The following two lines will detect all your players (files matching "AI*.cc")

PLAYERS_SRC = $(wildcard AI*.cc) OK_BOOMER.cc
PLAYERS_OBJ = $(patsubst %.cc, %.o, $(PLAYERS_SRC)) $(EXTRA_OBJS) $(DUMMY_OBJ)

# Flags

ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif
ifeq ($(strip $(DEBUG)),1)
	DEBUGFLAGS=-DDEBUG -g -rdynamic
endif
ifeq ($(strip $(32BITS)),1)
	ARCHFLAGS=-m32 -L/usr/lib32
endif

CXXFLAGS = -std=c++11 -Wall -Wno-unused-variable $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE)) -fPIC

LDFLAGS  = -std=c++11 -lm $(ARCHFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIMIZE))

# Rules

all: Game 

clean:
	rm -rf Game SecGame *.o *.exe Makefile.deps

# Order of objects is important here to deactivate standard sleep function.

Game: Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o Game.o Main.o $(PLAYERS_OBJ) $(BOARD_OBJ) Utils.o 
	$(CXX) $^ -o $@ $(LDFLAGS)

SecGame: Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o Utils.o 
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

%.exe: %.o Structs.o Settings.o State.o Info.o Random.o Board.o Action.o Player.o Registry.o SecGame.o SecMain.o Utils.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lrt

Makefile.deps: *.cc
	$(CXX) $(CXXFLAGS) -MM *.cc > Makefile.deps

include Makefile.deps
