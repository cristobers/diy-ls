# thank you to:
# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# and https://www.gnu.org/software/make/manual/make.pdf

.RECIPEPREFIX := $(.RECIPEPREFIX)  

CC = gcc
CFLAGS = -Wall -Wpedantic -Werror -o
NAME = list
FILE = file-reader.c

list: $(FILE)
    $(CC) $(CFLAGS) $(NAME) $(FILE) 

.PHONY: debug
debug: $(FILE)
    # forces a recompile even if it is "up to date"
    $(CC) -g $(CFLAGS) $(NAME) $(FILE) 