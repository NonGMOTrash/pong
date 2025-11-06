# compiler
COMP = g++
# compilation flags
FLAGS = -std=c++11 -g -I include/ -L lib/ -lraylib -lgdi32 -lwinmm

all: a.exe

# \/ target   \/ prerequsites
a.exe: main.cpp game.cpp
	${COMP} $^ -o a.exe ${FLAGS}
#IMPORTANT /\ automatically adds all the prereqs listed above

main.o: main.cpp
	${COMP} -c main.cpp -o main.o ${FLAGS} # -c means compile just this file, do not link
game.o: game.cpp
	${COMP} -c game.cpp -o game.o ${FLAGS}
