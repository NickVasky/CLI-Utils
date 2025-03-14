#ifndef FILE_UTILS
#define FILE_UTILS

#include <stdio.h>
#include <stdlib.h>

int openFile(char* filePath, FILE** filePointer);
int getFileLength(FILE** filePointer);
void printFileContent(FILE** filePointer, int size);
void getFileContent(FILE** filePointer, int size, char** fileContent);

#endif