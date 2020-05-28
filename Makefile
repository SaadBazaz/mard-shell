# GBS Shell Makefile

CC = g++
CFLAGS  = -pthread -Wall -g

objects = gbsh ./bin/system ./bin/kill ./bin/Delay ./bin/Input
all: $(objects)

$(objects): %: %.cpp
	$(CC) $(CFLAGS) -o $@ $<
