#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/fileutils.h"

#define PROGRAM_NAME "s21_cat"

struct flags {
  int numberNonblank;
  int showEnds;
  int numberAll;
  int squeezeBlank;
  int showTabs;
  int showNonPrinting;
};

void programm(int argc, char **argv);
int parseOptions(int argc, char **argv, struct flags *catFlags);
void processFile(char *filePath, struct flags *catFlags, int *lineNum,
                 unsigned char *ch, unsigned char *prevCh, int *blankCounter);
void cat(char *inBuf, int inBufSize, struct flags *catFlags, int *lineNum,
         unsigned char *ch, unsigned char *prevCh, int *blankCounter);
void charToNonPrinting(unsigned char ch, char *mSeq);