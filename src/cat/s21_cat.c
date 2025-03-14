#include "s21_cat.h"

int main(int argc, char **argv) {
  programm(argc, argv);
  return 0;
}

void programm(int argc, char **argv) {
  struct flags catFlags = {0, 0, 0, 0, 0, 0};
  int lineNum = 1;
  unsigned char ch = '\n';
  unsigned char prevCh = '\0';
  int blankCounter = 1;

  if (parseOptions(argc, argv, &catFlags)) {
    for (int i = optind; i < argc; i++)
      processFile(argv[i], &catFlags, &lineNum, &ch, &prevCh, &blankCounter);
  }
}

int parseOptions(int argc, char **argv, struct flags *catFlags) {
  int c = 0;
  int rValue = 1;

  struct option longOptions[] = {{"number-nonblank", no_argument, 0, 'b'},
                                 {"number", no_argument, 0, 'n'},
                                 {"squeeze-blank", no_argument, 0, 's'},
                                 {"show-ends", no_argument, 0, 'E'},
                                 {"show-all", no_argument, 0, 'A'},
                                 {"show-tabs", no_argument, 0, 'T'},
                                 {"show-nonprinting", no_argument, 0, 'v'},
                                 {0, 0, 0, 0}};

  while ((c = getopt_long(argc, argv, "AbeEnstTuv", longOptions, NULL)) != -1) {
    if (c == 'A')
      catFlags->showNonPrinting = catFlags->showEnds = catFlags->showTabs = 1;
    else if (c == 'b')
      catFlags->numberNonblank = 1;
    else if (c == 'e')
      catFlags->showNonPrinting = catFlags->showEnds = 1;
    else if (c == 'E')
      catFlags->showEnds = 1;
    else if (c == 'n')
      catFlags->numberAll = 1;
    else if (c == 's')
      catFlags->squeezeBlank = 1;
    else if (c == 't')
      catFlags->showNonPrinting = catFlags->showTabs = 1;
    else if (c == 'T')
      catFlags->showTabs = 1;
    else if (c == 'u') {
    } else if (c == 'v')
      catFlags->showNonPrinting = 1;
    else
      rValue = 0;
  }

  if (catFlags->numberNonblank) catFlags->numberAll = 0;

  return rValue;
}

void processFile(char *filePath, struct flags *catFlags, int *lineNum,
                 unsigned char *ch, unsigned char *prevCh, int *blankCounter) {
  FILE *filePointer;
  char *inBuf = NULL;
  if (openFile(filePath, &filePointer)) {
    int inBufSize = getFileLength(&filePointer);
    getFileContent(&filePointer, inBufSize, &inBuf);
    cat(inBuf, inBufSize, catFlags, lineNum, ch, prevCh, blankCounter);

    free(inBuf);
    fclose(filePointer);
  } else
    printf("%s: %s: No such file\n", PROGRAM_NAME, filePath);
}

void cat(char *inBuf, int inBufSize, struct flags *catFlags, int *lineNum,
         unsigned char *ch, unsigned char *prevCh, int *blankCounter) {
  for (int i = 0; i < inBufSize; i++) {
    *prevCh = *ch;
    *ch = inBuf[i];

    if (*ch != '\n') {
      if (*prevCh == '\n') *blankCounter = 0;
    } else
      (*blankCounter)++;

    if (catFlags->squeezeBlank && *blankCounter > 2) continue;

    if (*prevCh == '\n') {
      if (catFlags->numberAll) printf("%6d\t", (*lineNum)++);
      if (catFlags->numberNonblank && *ch != '\n')
        printf("%6d\t", (*lineNum)++);
    }

    if (!(*ch == '\t' || *ch == '\n')) {
      if (catFlags->showNonPrinting) {
        char mSeq[4 + 1] = {'\0'};
        charToNonPrinting(*ch, mSeq);
        printf("%s", mSeq);
      } else
        printf("%c", *ch);
    } else if (*ch == '\t') {
      if (catFlags->showTabs)
        printf("^%c", *ch + 64);
      else
        printf("%c", *ch);
    } else if (*ch == '\n') {
      if (catFlags->showEnds)
        printf("$\n");
      else
        printf("\n");
    } else
      printf("%c", *ch);
  }
}

void charToNonPrinting(unsigned char ch, char *mSeq) {
  if (ch >= 32) {
    if (ch < 127)
      *mSeq++ = ch;
    else if (ch == 127) {
      *mSeq++ = '^';
      *mSeq++ = '?';
    } else {
      *mSeq++ = 'M';
      *mSeq++ = '-';
      if (ch >= 128 + 32) {
        if (ch < 128 + 127)
          *mSeq++ = ch - 128;
        else {
          *mSeq++ = '^';
          *mSeq++ = '?';
        }
      } else {
        *mSeq++ = '^';
        *mSeq++ = ch - 128 + 64;
      }
    }
  } else {
    *mSeq++ = '^';
    *mSeq++ = ch + 64;
  }
}