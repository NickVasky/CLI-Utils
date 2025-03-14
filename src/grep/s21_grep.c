#include "s21_grep.h"

int main(int argc, char **argv) {
  programm(argc, argv);
  return 0;
}

void programm(int argc, char **argv) {
  struct grepOpts options = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL};

  if (parseOptions(argc, argv, &options)) {
    for (int i = optind; i < argc; i++) processFile(argv[i], &options);
  }

  freePatterns(&options);
  freeRegex(&options);
}

int parseOptions(int argc, char **argv, struct grepOpts *options) {
  int c = 0;
  int rValue = 1;

  struct option longOpts[] = {{"regexp", required_argument, 0, 'e'},
                              {"ignore-case", no_argument, 0, 'i'},
                              {"invert-match", no_argument, 0, 'v'},
                              {"count", no_argument, 0, 'c'},
                              {"files-with-matches", no_argument, 0, 'l'},
                              {"line-number", no_argument, 0, 'n'},
                              {"no-filename", no_argument, 0, 'h'},
                              {"no-messages", no_argument, 0, 's'},
                              {"only-matching", no_argument, 0, 'o'},
                              {"file", required_argument, 0, 'f'},
                              {0, 0, 0, 0}};

  while ((c = getopt_long(argc, argv, "e:ivclnhsof:", longOpts, NULL)) != -1) {
    if (c == 'e') {
      options->useRegexp = 1;
      addPattern(options, optarg);
    } else if (c == 'i')
      options->ignoreCase = 1;
    else if (c == 'v')
      options->invertMatch = 1;
    else if (c == 'c')
      options->countLines = 1;
    else if (c == 'l')
      options->filesWithMatches = 1;
    else if (c == 'n')
      options->printLineNumber = 1;
    else if (c == 'h')
      options->noFilename = 1;
    else if (c == 's')
      options->noMessages = 1;
    else if (c == 'o')
      options->onlyMatching = 1;
    else if (c == 'f') {
      options->filePatterns = 1;
      if (!loadPatterns(options, optarg)) rValue = 0;
    } else
      rValue = 0;
  }

  if (!options->useRegexp && !options->filePatterns && optind < argc) {
    addPattern(options, argv[optind]);
    optind++;
  };

  options->numOfFiles = argc - optind;

  if (!compileRegex(options)) rValue = 0;

  return rValue;
}

void processFile(char *filePath, struct grepOpts *options) {
  FILE *filePointer;
  char *inBuf = NULL;
  if (openFile(filePath, &filePointer)) {
    int inBufSize = getFileLength(&filePointer);
    getFileContent(&filePointer, inBufSize, &inBuf);
    iterateLines(inBuf, inBufSize, filePath, options);

    free(inBuf);
    fclose(filePointer);
  } else {
    if (!options->noMessages)
      printf("%s: %s: No such file or directory\n", PROGRAM_NAME, filePath);
  }
}

void iterateLines(char *inBuf, int inBufSize, char *filePath,
                  struct grepOpts *options) {
  int strPos = 0;
  int lineNumber = 0;
  int linesWithMatches = 0;
  int isLineFound = 0;

  do {
    char *line = NULL;
    isLineFound = getLine(inBuf, inBufSize, &line, &strPos);

    if (isLineFound) {
      lineNumber++;
      processLine(line, filePath, options, &linesWithMatches, &lineNumber);
    }

    if (line != NULL) free(line);
  } while (isLineFound);

  checkFilewiseFlags(options, filePath, linesWithMatches);
}

void checkFilewiseFlags(struct grepOpts *options, char *filePath,
                        int linesWithMatches) {
  if (options->filesWithMatches) {
    if (linesWithMatches > 0) printf("%s\n", filePath);
  } else {
    if (options->countLines) {
      if (options->numOfFiles == 1 || options->noFilename)
        printf("%d\n", linesWithMatches);
      else
        printf("%s:%d\n", filePath, linesWithMatches);
    }
  }
}

void processLine(char *line, char *filePath, struct grepOpts *options,
                 int *linesWithMatches, int *lineNumber) {
  int numOfMatches = 0;
  int lmLen = 0;
  regmatch_t *lineMatches = NULL;

  numOfMatches = applyRegex(line, options, &lineMatches, &lmLen);

  if (options->invertMatch) {
    if (numOfMatches == 0)
      numOfMatches = 1;
    else
      numOfMatches = 0;
  }

  if (!options->countLines && !options->filesWithMatches && numOfMatches > 0) {
    if (options->onlyMatching)
      printOnlyMatching(*lineNumber, options, line, &lineMatches, lmLen,
                        filePath);
    else
      printMatch(*lineNumber, options, line, filePath);
  }

  if (numOfMatches > 0) (*linesWithMatches)++;

  if (lineMatches != NULL) free(lineMatches);
}

void printMatch(int lineNumber, struct grepOpts *options, char *line,
                char *filePath) {
  printPrefixes(options, filePath, lineNumber);
  printf("%s\n", line);
}

void printOnlyMatching(int lineNumber, struct grepOpts *options, char *line,
                       regmatch_t **lineMatches, int lmLen, char *filePath) {
  for (int i = 0; i < lmLen; i++) {
    if (!options->invertMatch && (*lineMatches)[i].rm_so != -1 &&
        (*lineMatches)[i].rm_so != (*lineMatches)[i].rm_eo) {
      printPrefixes(options, filePath, lineNumber);
      printSubstring((*lineMatches)[i], line);
      printf("\n");
    }
  }
}

void printPrefixes(struct grepOpts *options, char *filePath, int lineNumber) {
  if (options->numOfFiles > 1 && options->noFilename == 0)
    printf("%s:", filePath);
  if (options->printLineNumber) printf("%d:", lineNumber);
}

void printSubstring(regmatch_t match, char *line) {
  for (int i = match.rm_so; i < match.rm_eo; i++) putchar(line[i]);
}

int getLine(char *inBuf, int inBufSize, char **line, int *strPos) {
  int lineFound = 0;
  int prevLine = *strPos;

  for (int i = *strPos; i < inBufSize; i++) {
    if (inBuf[i] == '\n') {
      *strPos = i + 1;
      break;
    } else if (i == inBufSize - 1) {
      *strPos = i + 2;
    }
  }
  if ((*strPos - prevLine) > 0) lineFound = 1;

  if (lineFound) {
    int lineLen = (*strPos - prevLine) + 1;
    *line = malloc(lineLen * sizeof(char));

    int c = 0;
    for (int i = prevLine; i < *strPos - 1; i++) (*line)[c++] = inBuf[i];

    (*line)[c] = '\0';
  }
  return lineFound;
}

int applyRegex(char *line, struct grepOpts *options, regmatch_t **lineMatches,
               int *lmLen) {
  int matchesFound = 0;
  regmatch_t match = {0};

  for (size_t i = 0; i < options->pSize; i++) {
    if (regexec(&((options->regexArray)[i]), line, 1, &match, 0) == 0) {
      matchesFound++;

      if (isMatchValid(match, lineMatches, *lmLen))
        addMatch(match, lineMatches, lmLen);
    }
  }
  if (matchesFound > 0) sortMatches(lineMatches, *lmLen);

  return matchesFound;
}

void sortMatches(regmatch_t **lineMatches, int lmLen) {
  regmatch_t temp = {0};

  for (int i = 0; i < lmLen - 1; i++) {
    for (int j = 0; j < lmLen - i - 1; j++) {
      if ((*lineMatches)[j].rm_so > (*lineMatches)[j + 1].rm_eo) {
        temp = (*lineMatches)[j];
        (*lineMatches)[j] = (*lineMatches)[j + 1];
        (*lineMatches)[j + 1] = temp;
      }
    }
  }
}

int isMatchValid(regmatch_t newMatch, regmatch_t **lineMatches, int lmLen) {
  int rValue = 1;

  if (newMatch.rm_eo == newMatch.rm_so) rValue = 0;

  if (rValue == 1) {
    for (int i = 0; i < lmLen; i++) {
      if (newMatch.rm_so >= (*lineMatches)[i].rm_so &&
          newMatch.rm_eo <= (*lineMatches)[i].rm_eo)
        rValue = 0;
    }
  }

  return rValue;
}

int addMatch(regmatch_t newMatch, regmatch_t **lineMatches, int *lmLen) {
  int rValue = 1;

  regmatch_t *temp = realloc(*lineMatches, (*lmLen + 1) * sizeof(regmatch_t));
  if (temp == NULL) rValue = 0;

  if (rValue == 1) {
    *lineMatches = temp;
    (*lineMatches)[*lmLen] = newMatch;
    (*lmLen)++;
  }

  return rValue;
}

int compileRegex(struct grepOpts *options) {
  options->regexArray = malloc((options->pSize) * sizeof(regex_t));
  int cflags = 0;
  int regRet = 0;
  char errbuf[128];
  int rValue = 1;

  if (options->ignoreCase) cflags = REG_ICASE;

  for (size_t i = 0; i < options->pSize; i++) {
    regRet =
        regcomp(&((options->regexArray)[i]), (options->patterns)[i], cflags);

    if (regRet != 0) {
      regerror(regRet, &((options->regexArray)[i]), errbuf, sizeof(errbuf));
      printf("%s: %s\n", PROGRAM_NAME, errbuf);
      break;
    }
  }
  if (regRet != 0) rValue = 0;

  return rValue;
}

void freeRegex(struct grepOpts *options) {
  for (size_t i = 0; i < options->pSize; i++) {
    regfree(&((options->regexArray)[i]));
  }
  free(options->regexArray);
}

int loadPatterns(struct grepOpts *options, char *filePath) {
  FILE *filePointer;
  char *inBuf = NULL;
  int rValue = 1;

  if (openFile(filePath, &filePointer)) {
    int inBufSize = getFileLength(&filePointer);
    getFileContent(&filePointer, inBufSize, &inBuf);

    int strPos = 0;
    int isLineFound = 0;

    do {
      char *line = NULL;
      isLineFound = getLine(inBuf, inBufSize, &line, &strPos);

      if (isLineFound) addPattern(options, line);

      if (line != NULL) free(line);
    } while (isLineFound);

    free(inBuf);
    fclose(filePointer);
  } else {
    rValue = 0;
    if (!options->noMessages)
      printf("%s: %s: No such file or directory\n", PROGRAM_NAME, filePath);
  }
  return rValue;
}

int addPattern(struct grepOpts *options, char *pattern) {
  int rValue = 1;

  char **temp =
      realloc(options->patterns, (options->pSize + 1) * sizeof(char *));
  if (temp == NULL) rValue = 0;

  if (rValue == 1) {
    options->patterns = temp;

    (options->patterns)[options->pSize] = malloc(strlen(pattern) + 1);
    if ((options->patterns)[options->pSize] == NULL) rValue = 0;
  }

  if (rValue == 1) {
    strcpy((options->patterns)[options->pSize], pattern);
    (options->pSize)++;
  }

  return rValue;
}

void freePatterns(struct grepOpts *options) {
  if (options->patterns != NULL) {
    for (size_t i = 0; i < options->pSize; i++) free((options->patterns)[i]);

    free(options->patterns);
  }
}