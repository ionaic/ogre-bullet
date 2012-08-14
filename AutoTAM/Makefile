CC=g++
#CC=clang++
CFLAGS+=-g -v
#LDFLAGS+=-lGL -lGLEW -lglfw -lIL -lILU -lILUT
#INCLUDEFLAGS=-I/usr/include/eigen3
CFILES=autotam.cpp image.cpp
#OFILES=*.o
NAME=tam

all:
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) $(LDFLAGS) -o $(NAME) $(CFILES)
	#./tam
