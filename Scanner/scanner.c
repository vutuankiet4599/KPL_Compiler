/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
	/* Khi currentChar con la khoang trang va khong phai ket thuc file thi readChar */
  	while(charCodes[currentChar] == CHAR_SPACE && currentChar != EOF){
  		readChar();
  }
}

void skipComment() {
	/*
		Khi getToken() da doc duoc '(' va '*' 
		thi kiem tra xem '*' va '*' co xuat hien cung nhau khong.
		'*" xuat hien thi count = 1, tiep do la ')' thi count = 2 va break ra khoi vong lap.
	*/
	int count = 0;
	while(currentChar != EOF && count < 2){
		if(charCodes[currentChar] == CHAR_TIMES){
			count = 1;
		}else if(charCodes[currentChar] == CHAR_RPAR){
			if(count == 1){
				count = 2;
			}else count = 0;
		}else {
			count = 0;
		}
		readChar();
	}
	
	/*
		Con neu doc het file ma chua thay "*)" thi bao loi.
	*/
	if(count != 2){
		error(ERR_ENDOFCOMMENT, lineNo, colNo);
	}
}

Token* readIdentKeyword(void) {
	/* Tao mot token rong */
	Token *tk = makeToken(TK_NONE, lineNo, colNo);
	tk->string[0] = (char) currentChar; // Phai ep kieu vi currentChar la dang int
	
	readChar();
	int count = 1;
	
	while(currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)){
		/* 
			Khi chua phai la EOF va ky tu hien tai la chu cai hoac so thi gan no vao tk->string 
			Phai kiem tra xem count no con nho hon MAX_IDENT_LEN khong.
		*/
		if(count <= MAX_IDENT_LEN){
			tk->string[count] = (char) currentChar;
		}
		count++;
		readChar();
	}
	/*
		Neu so luong ky tu trong tk->string lon hon MAX_INDENT_LEN thi bao loi
	*/
	if(count > MAX_IDENT_LEN){
		error(ERR_IDENTTOOLONG, lineNo, colNo);
	}
	
	tk->string[count] = '\0'; // Phai co ket thuc neu khong se co loi
	/* Kiem tra xem no co phai la mot keyword khong (CONST, PROGRAM, ...) */
	tk->tokenType = checkKeyword(tk->string);
	
	/* 
		Neu khong phai keywords thi tokenType la TK_NONE.
		Gan no bang TK_IDENT.
	*/
	if(tk->tokenType == TK_NONE){
		tk->tokenType = TK_IDENT;
	}
	return tk;
}

Token* readNumber(void) {
	/*
		Lam tuong tu nhu readIdentKeyWord.
		Chuyen string ve int bang ham atoi() roi gan cho value.
	*/
	
	Token *tk = makeToken(TK_NONE, lineNo, colNo);
	int count = 0;
	
	while(currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT){
		tk->string[count] = (char) currentChar;
		count++;
		readChar();
	}
	
	tk->string[count] = '\0';
	tk->value = atoi(tk->string);
	tk->tokenType = TK_NUMBER;
	return tk;
}

Token* readConstChar(void) {
	/* Tao mot token moi */
	Token *tk = makeToken(TK_CHAR, lineNo, colNo);
	
	/* Doc mot ky tu neu la EOF thi bao loi khong thi gan vao tk->string*/
	readChar();
	if(currentChar == EOF){
		error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
		tk->tokenType = TK_NONE;
		return tk;
	}
	
	tk->string[0] = currentChar;
	tk->string[1] = '\0';
	
	/* Doc mot ky tu neu la ' ' ' thi doc tiep va return con khong thi bao loi */
	readChar();
	if(currentChar == EOF){
		error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
		tk->tokenType = TK_NONE;
		return tk;
	}
	
	if(charCodes[currentChar] == CHAR_SINGLEQUOTE){
		readChar();
		return tk;
	}else{
		tk->tokenType = TK_NONE;
		error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
		return tk;
	}
}

Token* getToken(void) {
	
	Token *token;
	int ln, cn;

	if (currentChar == EOF) 
		return makeToken(TK_EOF, lineNo, colNo);

	switch (charCodes[currentChar]) {	
  		case CHAR_SPACE: /* Bo qua khoang trang */
		  	skipBlank(); 
			return getToken();
			
  		case CHAR_LETTER: /* Doc ident hoac keyword */
			return readIdentKeyword();
			
  		case CHAR_DIGIT: /* Doc so */
			return readNumber();
			
  		case CHAR_PLUS:  /* Doc dau cong */
   			token = makeToken(SB_PLUS, lineNo, colNo);
    		readChar(); 
    		return token;
    	
    	case CHAR_TIMES: /* Doc dau nhan */
    		token = makeToken(SB_TIMES, lineNo, colNo);
    		readChar();
    		return token;
    		
    	case CHAR_MINUS: /* Doc dau tru */
    		token = makeToken(SB_MINUS, lineNo, colNo);
    		readChar();
    		return token;
    	
    	case CHAR_SLASH: /* Doc dau chia */
    		token = makeToken(SB_SLASH, lineNo, colNo);
    		readChar();
    		return token;
    	
    	case CHAR_LT: /* Doc duoc '<' */
    		ln = lineNo;
    		cn = colNo;
    		readChar();
    		/* Neu tiep theo la EOF tra ve '<' */
    		if(currentChar == EOF){
    			return makeToken(SB_LT, ln, cn);
			}
			/* Neu tiep theo la '=' thi doc them mot ky tu va tra ve '<=' khong thi '<' */
			if(charCodes[currentChar] == CHAR_EQ){
				readChar();
				return makeToken(SB_LE, ln, cn);
			}else{
				return makeToken(SB_LT, ln, cn); // Khong doc ky tu nua vi da doc ky tu khong phai '='
			}
		
		case CHAR_GT: /* Doc duoc '>' */
			ln = lineNo;
    		cn = colNo;
    		readChar();
    		/* Neu tiep theo la EOF tra ve '>' */
    		if(currentChar == EOF){
    			return makeToken(SB_GT, ln, cn);
			}
			/* Neu tiep theo la '=' thi doc them mot ky tu va tra ve '>=' khong thi '>' */
			if(charCodes[currentChar] == CHAR_EQ){
				readChar();
				return makeToken(SB_GE, ln, cn);
			}else{
				return makeToken(SB_GT, ln, cn); // Khong doc ky tu nua vi da doc ky tu khong phai '='
			}
			
    	case CHAR_EQ: /* Doc dau '=' */
    		token = makeToken(SB_EQ, lineNo, colNo);
    		readChar();
    		return token;
    	
    	case CHAR_EXCLAIMATION: /* Doc duoc dau '!' */
    		ln = lineNo;
    		cn = colNo;
    		readChar();
    		/* Tra lai '!=' neu doc duoc dau '=' khong thi bao loi */
    		if(currentChar != EOF && charCodes[currentChar] == CHAR_EQ){
    			readChar();
    			return makeToken(SB_NEQ, ln, cn);
			}else{
				error(ERR_INVALIDSYMBOL, ln, cn);
				return makeToken(TK_NONE, ln, cn);
			}
		
		case CHAR_COMMA: /* Doc duoc ',' */
			token = makeToken(SB_COMMA, lineNo, colNo);
			readChar();
			return token;
		
		case CHAR_PERIOD: /* Doc duoc dau '.' */
			ln = lineNo;
    		cn = colNo;
			readChar();
			if(currentChar != EOF && charCodes[currentChar] == CHAR_RPAR){
				readChar();
				return makeToken(SB_RSEL, ln, cn);
			}else{
				return makeToken(SB_PERIOD, ln, cn);
			}
		
		case CHAR_SEMICOLON: /* Doc duoc dau ';' */
			token = makeToken(SB_SEMICOLON, lineNo, colNo);
			readChar();
			return token;
		
		case CHAR_COLON: /* Doc duoc dau ':' */
			ln = lineNo;
    		cn = colNo;
			readChar();
			/* Neu ky tu tiep theo la '=' tra ve assign khong thi tra ve ':' */
			if(currentChar != EOF && charCodes[currentChar] == CHAR_EQ){
				readChar();
				return makeToken(SB_ASSIGN, ln, cn);
			}else{
				return makeToken(SB_COLON, ln, cn);
			}
		
		case CHAR_SINGLEQUOTE: /* Doc duoc dau ' ' ' thi doc constant char*/
			return readConstChar();
			
    	case CHAR_LPAR: /* Doc thay '(' */
    		ln = lineNo;
    		cn = colNo;
    		readChar();
    		
    		/* Khong co gi o dang sau tra lai SB_LPAR */
    		if(currentChar == EOF){
    			return makeToken(SB_LPAR, ln, cn);
			}
			
			// (. va .) danh dau chi muc cua mang			
			/* 
				Dang sau la '.' tra lai SB_LSEL, la '*' thi skipComment().
				Neu khong phai tra ve SB_LPAR.
			*/
			if(charCodes[currentChar] == CHAR_PERIOD){
				readChar();
				return makeToken(SB_LSEL, ln, cn);
			}else if(charCodes[currentChar] == CHAR_TIMES){
				readChar();
				skipComment();
				return getToken();
			}else{
				return makeToken(SB_LPAR, ln, cn);
			}
		
		case CHAR_RPAR: /* Doc duoc dau ')' */
			token = makeToken(SB_RPAR, lineNo, colNo);
			readChar();
			return token;

  		default:
    		token = makeToken(TK_NONE, lineNo, colNo);
    		error(ERR_INVALIDSYMBOL, lineNo, colNo);
    		readChar(); 
    		return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}
