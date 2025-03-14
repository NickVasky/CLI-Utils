CC=gcc
CFLAGS= -c -Wall -Werror -Wextra -std=c11

all: s21_cat clean_o_files

debug: CFLAGS += -g
debug: all

s21_cat: s21_cat.o fileutils.o
	$(CC) s21_cat.o fileutils.o -o ../cat/s21_cat

s21_cat.o: ../cat/s21_cat.c ../cat/s21_cat.h ../common/fileutils.h
	$(CC) $(CFLAGS) ../cat/s21_cat.c

fileutils.o: ../common/fileutils.c ../common/fileutils.h
	$(CC) $(CFLAGS) ../common/fileutils.c

clean: clean_o_files
	rm -rf ../cat/s21_cat

clean_o_files:
	rm -rf ../cat/*.o

clang_apply:
	clang-format -i ../cat/*.c ../cat/*.h ../common/*.c ../common/*.h

clang_check:
	clang-format -n ../cat/*.c ../cat/*.h ../common/*.c ../common/*.h

rebuild: clean all