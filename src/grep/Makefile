CC=gcc
CFLAGS= -c -Wall -Werror -Wextra -std=c11

all: s21_grep clean_o_files

debug: CFLAGS += -g
debug: all

s21_grep: s21_grep.o fileutils.o
	$(CC) s21_grep.o fileutils.o -o ../grep/s21_grep

s21_grep.o: ../grep/s21_grep.c ../grep/s21_grep.h ../common/fileutils.h
	$(CC) $(CFLAGS) ../grep/s21_grep.c

fileutils.o: ../common/fileutils.c ../common/fileutils.h
	$(CC) $(CFLAGS) ../common/fileutils.c

clean: clean_o_files
	rm -rf ../grep/s21_grep

clean_o_files:
	rm -rf ../grep/*.o

clang_apply:
	clang-format -i ../grep/*.c ../grep/*.h ../common/*.c ../common/*.h

clang_check:
	clang-format -n ../grep/*.c ../grep/*.h ../common/*.c ../common/*.h

rebuild: clean all