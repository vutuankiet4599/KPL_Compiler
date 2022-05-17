/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

FILE *inputStream;
int lineNo, colNo;
int currentChar;

/*
	Doc tung ky tu roi tra ve gia tri int cua ky tu do trong ascii.
	Vi du 'a' = 97 (De dung kiem tra trong CharCode[]).
	Vi du CharCode[91] = CHAR_LETTER.
*/
int readChar(void) {
  currentChar = getc(inputStream);
  colNo ++;
  if (currentChar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

/*
	Mo file source code kpl.
	Neu thanh cong return IO_SUCCESS, that bai IO_ERROR.
*/
int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return IO_ERROR;
  lineNo = 1;
  colNo = 0;
  readChar();
  return IO_SUCCESS;
}

/*
	Dong file input.
*/
void closeInputStream() {
  fclose(inputStream);
}

