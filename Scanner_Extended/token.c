/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "token.h"

/*
	Dinh nghia 20 keywords.
*/
struct {
  char string[MAX_IDENT_LEN + 1];
  TokenType tokenType;
} keywords[KEYWORDS_COUNT] = {
  {"program", KW_PROGRAM},
  {"const", KW_CONST},
  {"type", KW_TYPE},
  {"var", KW_VAR},
  {"integer", KW_INTEGER},
  {"char", KW_CHAR},
  {"array", KW_ARRAY},
  {"of", KW_OF},
  {"function", KW_FUNCTION},
  {"procedure", KW_PROCEDURE},
  {"begin", KW_BEGIN},
  {"end", KW_END},
  {"call", KW_CALL},
  {"if", KW_IF},
  {"then", KW_THEN},
  {"else", KW_ELSE},
  {"while", KW_WHILE},
  {"do", KW_DO},
  {"for", KW_FOR},
  {"to", KW_TO},
  {"return", KW_RETURN},
  {"switch", KW_SWITCH}
};

/*
	Kiem tra xem keywords[i] co trung voi string hay khong.
	Neu ma bang return 1 con khong bang return 0;
*/
int keywordEq(char *kw, char *string) {
  while ((*kw != '\0') && (*string != '\0')) {
    if (*kw != *string) break;
    kw ++; string ++;
  }
  return ((*kw == '\0') && (*string == '\0'));
}

/*
	Kiem tra keyword do la gi va tra lai keywords[i].string .
	Con neu khong co thi tra lai TK_NONE.
*/
TokenType checkKeyword(char *string) {
  int i;
  for (i = 0; i < KEYWORDS_COUNT; i++)
    if (keywordEq(keywords[i].string, string)) 
      return keywords[i].tokenType;
  return TK_NONE;
}

/*
	Tao mot token moi.
*/
Token* makeToken(TokenType tokenType, int lineNo, int colNo) {
  Token *token = (Token*)malloc(sizeof(Token));
  token->tokenType = tokenType;
  token->lineNo = lineNo;
  token->colNo = colNo;
  return token;
}

