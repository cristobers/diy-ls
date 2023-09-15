# thank you to https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -Wall -Wpedantic -Werror -o
NAME = list
FILE = file-reader.c

file-reader: file-reader.c
	$(CC) $(CFLAGS) $(NAME) $(FILE) 
