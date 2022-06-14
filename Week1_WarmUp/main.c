#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 1000

typedef struct line
{
    int line;
    struct line* next;
}line;

typedef struct list
{
    char str[MAX_LEN];
    int f;
    line *l_line;
    struct list* next;
}list;

typedef struct stopw
{
    char str[MAX_LEN];
    struct stopw* next;
}stopw;

stopw *liststopw;
list *l;
int nline = 0;
int count = 0;

void insertLine(line **l, int data){
    line *pNew = (line*)malloc(sizeof(line));
    pNew->line = data;
    pNew->next = *l;
    *l = pNew;    
}

void insert(list **data, char str[MAX_LEN]){
    list *pNew = (list*)malloc(sizeof(list));
    strcpy(pNew->str, str);
    pNew->l_line = NULL;
    pNew->f = 1;
    insertLine(&(pNew->l_line), nline);
    pNew->next = (*data);
    (*data) = pNew;
}

void insertstw(stopw **data, char str[MAX_LEN]){
    stopw *pNew = (stopw*)malloc(sizeof(stopw));
    strcpy(pNew->str , str);        
    pNew->next = (*data);
    (*data) = pNew;
}

list *isExist(list *li, char str[MAX_LEN]){
    list *p = li;
    while(NULL != p){
        if(strcmp(str, p->str) == 0){
            return p;
        }else{
        	p = p->next;
		}
    }
    return NULL;
}

int isExistLine(line *l, int n_line){
    line *p = l;
    while(NULL != p){
        if(p->line == n_line){
            return 1;
        }else{
        	p = p->next;
		}    
    }
    return 0;
}

int isExistStop(char str[MAX_LEN]){
    stopw *p = liststopw;
    while(NULL != p){
        if(!strcmp(p->str,str)){
            return 1;
        }else{
        	p = p->next;
		}
    }
    return 0;
}

char *toLower(char *str, size_t len)
{
    char *str_l = calloc(len+1, sizeof(char));

    for (size_t i = 0; i < len; ++i) {
        str_l[i] = tolower((unsigned char)str[i]);
    }
    return str_l;
}

void readName(){
    FILE *fptr = fopen("vanban.txt", "r");
    FILE *fptr1 = fopen("vanban.txt", "r");
    FILE *fptr2 = fopen("vanban.txt", "r");
    if(fptr == NULL){
        printf("Khong co file");
        return;
    }

    char str[MAX_LEN], c1, c2, c3;
    char a[2], b[2], c[2];
    a[1] = '\0';
    b[1] = '\0';
    c[1] = '\0';
	int i;
	for(i = 0; !feof(fptr), i < 3; i++){
		c1 = fgetc(fptr);
		a[0] = c1;
	}
	
	for(i = 0; !feof(fptr1), i < 2; i++){
		c2 = fgetc(fptr1);
		b[0] = c2;
	}
	
	for(i = 0; !feof(fptr2), i < 1; i++){
		c3 = fgetc(fptr2);
		c[0] = c3;
	}
	
	while(!feof(fptr) && !feof(fptr2) && !feof(fptr1)){
		if(c3!='.' && c2 ==' ' && isupper(c1)){
			strcpy(str, "");
			if(isalpha(c1) || c1=='_'){
				strcat(str, a);
			}		
			while(isalpha(c1) || c1=='_'){
				c1 = fgetc(fptr);
				a[0] = c1;
				c2 = fgetc(fptr1);
				b[0] = c2;
				c3 = fgetc(fptr2);
				c[0] = c3;
				if(isalpha(c1) || c1=='_'){
					strcat(str, a);
				}
			}
			strcpy(str, toLower(str, MAX_LEN));
			insertstw(&liststopw, str);
		}
		c1 = fgetc(fptr);
		a[0] = c1;
		c2 = fgetc(fptr1);
		b[0] = c2;
		c3 = fgetc(fptr2);
		c[0] = c3;
	}
    
    fclose(fptr);
    fclose(fptr1);
    fclose(fptr2);
}

void readFilestw(){
    FILE *fptr = fopen("stopw.txt", "r");
    if(fptr == NULL){
        printf("Khong co file stopw");
        return;
    }

    char str[MAX_LEN];
    while (!feof(fptr))
    {
        fscanf(fptr, "%s", str);
        strcpy(str, toLower(str, MAX_LEN));
        insertstw(&liststopw, str);
    }
    fclose(fptr);
    
}

void readFile(){
	FILE *fptr = fopen("vanban.txt", "r");
	if(fptr == NULL){
		printf("Khong co file\n");
		return;
	}
	
	char *str[MAX_LEN], str1[MAX_LEN], str2[MAX_LEN], lastchar, *line;
	int i, nf;
	
	while(fgets(str1, MAX_LEN-1, fptr) != NULL){		
		nf = 0;
		nline++;
		lastchar = ' ';
		strcpy(str2, str1);
		line = str2;
		
		for(i = 0; line[i] != '\0' && i < MAX_LEN-1; i++){
			if(isspace(line[i]) || ispunct(line[i]) || isdigit(line[i])){
				lastchar = line[i];
				line[i] = '\0';
			}else{
				if(isspace(lastchar)){
					str[nf] = line+i;
					nf++;
				}
				lastchar = line[i];
			}
		}
		
		for(i = 0; i < nf; i++){
			if(isExistStop(str[i])){
				continue;
			}else{
				str[i] = toLower(str[i], MAX_LEN);
				list *p = isExist(l, str[i]);
				if(NULL != p)
				{
					p->f++;
					if(!isExistLine(p->l_line, nline)){
						insertLine(&(p->l_line), nline);
					}
				}else{
					insert(&l, str[i]);
					count++;
				}
			}
		}
	}
	
	fclose(fptr);
}

list* swap(list *x, list *y){
	list *tmp = y->next;
	y->next = x;
	x->next = tmp;
	return y;
}

int sort(list **head, int count){
	list **h;
	int i, j, swapped;
	for(i = 0; i <= count; i++){
		h = head;
		swapped = 0;
		
		for(j = 0; j < count-  i - 1; j++){
			list *p1 = *h;
			list *p2 = p1->next;
			
			if(strcmp(p1->str, p2->str) > 0){
				*h = swap(p1, p2);
				swapped = 1;
			}
			h = &(*h)->next;
		}
		
		if(swapped == 0){
			break;
		}
	}
}

int main(){
	readFilestw();
    readName();
    readFile();
    sort(&l, count);
    while(l != NULL){
    	printf("%s %d || ", l->str, l->f);
    	while(l->l_line != NULL){
    		printf("%d ", l->l_line->line);
    		l->l_line = l->l_line->next;
		}
		printf("\n");
    	l = l->next;
	}
    return 1;
}
