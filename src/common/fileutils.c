#include "fileutils.h"

int openFile(char* filePath, FILE** filePointer) {
  int rValue = 0;

  *filePointer = fopen(filePath, "r");
  if (NULL != *filePointer) {
    freopen(filePath, "a+", *filePointer);
    rValue = 1;
  } else
    rValue = 0;
  return rValue;
}

int getFileLength(FILE** filePointer) {
  fseek(*filePointer, 0, SEEK_END);
  int size = ftell(*filePointer);
  fseek(*filePointer, 0, SEEK_SET);
  return size;
}

void printFileContent(FILE** filePointer, int size) {
  char* strBuffer = malloc(size * sizeof(char));
  fread(strBuffer, size * sizeof(char), sizeof(char), *filePointer);
  printf("%s\n", strBuffer);
  free(strBuffer);
}

void getFileContent(FILE** filePointer, int size, char** fileContent) {
  *fileContent = malloc(size * sizeof(char));
  fread(*fileContent, size * sizeof(char), sizeof(char), *filePointer);
}