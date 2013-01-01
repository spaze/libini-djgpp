# change 'rm' to 'del' if you have no Fileutils

CC = gcc
AR = ar
RM = rm

all: libini.a

ini.o: ini.c
	$(CC) -Wall -I. -c ini.c -o ini.o

libini.a: ini.o
	$(AR) rcs libini.a ini.o
	$(RM) ini.o
