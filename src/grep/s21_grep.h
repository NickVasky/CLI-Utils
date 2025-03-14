#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/fileutils.h"

#define PROGRAM_NAME "s21_grep"

struct grepOpts {
  int useRegexp;
  int ignoreCase;
  int invertMatch;
  int countLines;
  int filesWithMatches;
  int printLineNumber;
  int numOfFiles;
  int noFilename;
  int noMessages;
  int onlyMatching;
  int filePatterns;
  size_t pSize;
  char **patterns;
  regex_t *regexArray;
};

void programm(int argc, char **argv);
int parseOptions(int argc, char **argv, struct grepOpts *options);
int addPattern(struct grepOpts *options, char *pattern);
void freePatterns(struct grepOpts *options);
void processFile(char *filePath, struct grepOpts *options);
void iterateLines(char *inBuf, int inBufSize, char *filePath,
                  struct grepOpts *options);
int getLine(char *inBuf, int inBufSize, char **line, int *lineSize);
int compileRegex(struct grepOpts *options);
void freeRegex(struct grepOpts *options);
void sortMatches(regmatch_t **lineMatches, int lmLen);
int isMatchValid(regmatch_t newMatch, regmatch_t **lineMatches, int lmLen);
int addMatch(regmatch_t newMatch, regmatch_t **lineMatches, int *lmLen);
int applyRegex(char *line, struct grepOpts *options, regmatch_t **lineMatches,
               int *lmLen);
void processLine(char *line, char *filePath, struct grepOpts *options,
                 int *linesWithMatches, int *lineNumber);
void checkFilewiseFlags(struct grepOpts *options, char *filePath,
                        int linesWithMatches);
void printMatch(int lineNumber, struct grepOpts *options, char *line,
                char *filePath);
void printOnlyMatching(int lineNumber, struct grepOpts *options, char *line,
                       regmatch_t **lineMatches, int lmLen, char *filePath);
void printSubstring(regmatch_t match, char *line);
void printPrefixes(struct grepOpts *options, char *filePath, int lineNumber);
int loadPatterns(struct grepOpts *options, char *filePath);