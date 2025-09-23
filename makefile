# compiler
COMP = g++
# compilation flags
FLAGS = -g -I include/ -L lib/ -lraylib -lgdi32 -lwinmm 

# \/ target   \/ prerequsites
game.exe: main.o
	${COMP} $^ -o game.exe ${FLAGS}
#          /\ automatically adds all the prereqs listed above
main.o: main.cpp
	${COMP} -c main.cpp -o main.o ${FLAGS}
