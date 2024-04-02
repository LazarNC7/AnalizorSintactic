#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

char *array[] = {"ID"
		 // keywords
		 ,"TYPE_CHAR", "TYPE_DOUBLE", "ELSE", "IF", "TYPE_INT",
		 "RETURN", "STRUCT", "VOID", "WHILE",
		 // constants
		 "INT", "DOUBLE", "CHAR", "STRING"
		 // delimiters
		 ,"COMMA","END", "SEMICOLON", "LPAR", "RPAR",
		 "LBRACKET", "RBRACKET", "LACC", "RACC"
		 // operators
		 ,"ASSIGN","EQUAL", "ADD", "SUB", "MUL", "DIV", "DOT", "AND",
		 "OR", "NOT", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ"};

Token *tokens;	// single linked list of tokens
Token *lastTk;		// the last token in list

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code){
  Token *tk=safeAlloc(sizeof(Token));
  tk->code=code;
  tk->line=line;
  tk->next=NULL;
  if(lastTk){
    lastTk->next=tk;
  }else{
    tokens=tk;
  }
  lastTk=tk;
  return tk;
}

char *extract(const char *begin,const char *end){
  size_t size = end-begin;
  char *buffer = (char*)malloc((size+1)*sizeof(char));
  if(!buffer){
    printf("nu se poate aloca sirul\n");
    exit(-1);
  }
  for(int i=0; i<size; i++)
    {
      buffer[i]=begin[i];
    }
  buffer[size]='\0';
  return buffer;
}

Token *tokenize(const char *pch){
  const char *start;
  Token *tk;
  for(;;){
    switch(*pch){
    case ' ':case '\t':pch++;break;
    case '\r':		// handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
      if(pch[1]=='\n')pch++;
      // fallthrough to \n
    case '\n':
      line++;
      pch++;
      break;
    case '\0':addTk(END);return tokens;
    case ',':addTk(COMMA);pch++;break;
    case '.':addTk(DOT);pch++;break;
    case '*':addTk(MUL);pch++;break;
    case '-':addTk(DIV);pch++;break;
    case '+':addTk(ADD);pch++;break;
    case ';':addTk(SEMICOLON);pch++;break;
    case '(':addTk(LPAR);pch++;break;
    case ')':addTk(RPAR);pch++;break;
    case '[':addTk(LBRACKET);pch++;break;
    case ']':addTk(RBRACKET);pch++;break;
    case '{':addTk(LACC);pch++;break;
    case '}':addTk(RACC);pch++;break;
    case '/':
      if(pch[1]=='/'){
	while(*pch!='\n' && *pch!='\r' && *pch!='\0')pch++;
      }else{
	addTk(DIV);
	pch++;
      }
      break;
    case '!':
      if(pch[1]=='='){
	addTk(NOTEQ);
	pch+=2;
      }else{
	addTk(NOT);
	pch++;
      }
      break;
    case '&':
      if(pch[1]=='&'){
	addTk(AND);
	pch+=2;
      }
      else err("invalid char: %c (%d)",*pch,*pch);
      break;
    case '|':
      if(pch[1]=='|'){
	addTk(OR);
	pch+=2;
      }
      else err("invalid char: %c (%d)",*pch,*pch);
      break;
    case '<':
      if(pch[1]=='='){
	addTk(LESSEQ);
	pch+=2;
      }else{
	addTk(LESS);
	pch++;
      }
      break;
    case '>':
      if(pch[1]=='='){
	addTk(GREATEREQ);
	pch+=2;
      }else{
	addTk(GREATER);
	pch++;
      }
      break;
    case '=':
      if(pch[1]=='='){
	addTk(EQUAL);
	pch+=2;
      }else{
	addTk(ASSIGN);
	pch++;
      }
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':{
      char *number=(char*)safeAlloc(15);//salvam numarul intr-un string
      int i=0;
      while(isdigit(*pch) || *pch=='.' || *pch=='e' || *pch=='E' || *pch=='-'){ //poate fi int sau double
	number[i]=*pch;
	i++;
	pch++;
      }
      number[i]='\0';
      if(strchr(number, '.')||strchr(number, 'e')||strchr(number, 'E')) //daca e double
	{
	  double number_d = atof(number);
	  tk=addTk(DOUBLE);
	  tk->d=number_d;
	}
      else{ //daca e int
	int nr = atoi(number);
	tk=addTk(INT);
	tk->i=nr;
      }
			    
      break;
    }
			
    case '\'':
      if(*(pch+2)=='\''){ //daca e char
	tk=addTk(CHAR);
	tk->c=*(pch+1);
	pch+=2;
      }
      pch++;
      break;
			
    case '\"':{
      char *string=(char*)malloc(900*sizeof(char));
      pch++;
      int i;
      for(i=0; *pch != '\"'; i++)
	{
	  string[i]=*pch; pch++;}
      string[i]='\0';
      tk=addTk(STRING);
      tk->text=string;
      pch++;
      break;
    }
			
    default:
      if(isalpha(*pch)||*pch=='_'){ //daca e identificator
	for(start=pch++;isalnum(*pch)||*pch=='_';pch++){}
	char *text=extract(start,pch);
	if(strcmp(text,"char")==0)addTk(TYPE_CHAR);
	else if(strcmp(text,"double")==0)addTk(TYPE_DOUBLE);
	else if(strcmp(text,"else")==0)addTk(ELSE);
	else if(strcmp(text,"if")==0)addTk(IF);
	else if(strcmp(text,"int")==0)addTk(TYPE_INT);
	else if(strcmp(text,"return")==0)addTk(RETURN);
	else if(strcmp(text,"struct")==0)addTk(STRUCT);
	else if(strcmp(text,"void")==0)addTk(VOID);
	else if(strcmp(text,"while")==0)addTk(WHILE);
	else{
	  tk=addTk(ID);
	  tk->text=text;
	}
      }
      else err("invalid char: %c (%d) at line %d",*pch,*pch, line);
    }
  }
}

void showTokens(const Token *tokens){
  for(const Token *tk=tokens;tk;tk=tk->next){
    printf("%d %s",tk->line, array[tk->code]);
    if(tk->code == ID)
      printf(":%s", tk->text);
    else if(tk->code==INT)
      printf(":%d", tk->i);
    else if(tk->code==DOUBLE)
      printf(":%f", tk->d);
    else if(tk->code==CHAR)
      printf(":%c", tk->c);
    else if(tk->code==STRING)
      printf(":%s", tk->text);
    printf("\n");
  }
}
