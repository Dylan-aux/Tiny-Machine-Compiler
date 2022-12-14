%{
#include <stdlib.h>
#include <stdio.h>
#include "scanType.h"
#include "treeUtils.h"
#include "parser.tab.h"

char* lastToken;
extern int numErrors;
extern int numWarnings;
int line = 1;


static int setValue( int linenum, int tokenClass, char *sval) 
{
	lastToken = strdup(sval);
	yylval.tokenData = new TokenData;
	yylval.tokenData->tokenClass = tokenClass;	
	{return setValue(line, RETURN, yytext);}

	yylval.tokenData->lineNum = linenum;
	yylval.tokenData->token = strdup(sval);

	if (tokenClass == 262)
	{
		int i,j, k;
		char *temp = strdup(sval);
		for (i = 0; i < strlen(temp) ; i++)
		{
			k = i;
			if(temp[i] == '\\' && temp[k++] != 'n' )
			{

				j = i;
				memmove(&temp[i], &temp[j+1], strlen(temp)-j);
			}
		}
		for (i = 0; i < strlen(temp) ; i++)
		{
			if(temp[i] == '\"' )
			{
				j = i;
				memmove(&temp[i], &temp[j+1], strlen(temp)-j);
			}
		}
	yylval.tokenData->tokenVal = temp;
	}
	if(tokenClass == NUMCONST) {

	char * temp;
	temp = strdup(sval);
	yylval.tokenData->numVal = atoi(temp);
}
	return tokenClass;
}
%}

%option noyywrap


%%

\/\/.*         		;	


static 			{return setValue(line, STATIC, yytext);
			lastToken = strdup(yytext);
			}
bool 			{return setValue(line, BOOL, yytext);
			lastToken = strdup(yytext);
			}
char		 	{return setValue(line, CHAR, yytext);
			lastToken = strdup(yytext);
			}
if 			{return setValue(line, IF, yytext);
			lastToken = strdup(yytext);
			}
int 			{return setValue(line, INT, yytext);
			lastToken = strdup(yytext);
			}
else			{return setValue(line, ELSE, yytext);
			lastToken = strdup(yytext);
			}
while		 	{return setValue(line, WHILE, yytext);
			lastToken = strdup(yytext);
			}
return 			{return setValue(line, RETURN, yytext);
			lastToken = strdup(yytext);
			}
break			{return setValue(line, BREAK, yytext); 	
			lastToken = strdup(yytext);
			}
true 	 		{return setValue(line, BOOLCONST, yytext);			
			lastToken = strdup(yytext);
			}
false			{return setValue(line, BOOLCONST, yytext);
			lastToken = strdup(yytext);
			}
for				{return setValue(line, FOR, yytext);
			lastToken = strdup(yytext);
			}
in				{return setValue(line, IN, yytext);
			lastToken = strdup(yytext);
				}


";" 			{return setValue(line,';',yytext); 
			lastToken = strdup(yytext);
			}
"," 			{return setValue(line,',',yytext);
			lastToken = strdup(yytext);
			}
":" 			{return setValue(line,':',yytext);
			lastToken = strdup(yytext);
			}
"(" 			{return setValue(line,'(',yytext);
			lastToken = strdup(yytext);
			}
")" 			{return setValue(line,')',yytext);
			lastToken = strdup(yytext);
			}
"[" 			{return setValue(line,'[',yytext);
			lastToken = strdup(yytext);
			}
"]" 			{return setValue(line,']',yytext);
			lastToken = strdup(yytext);
			}
"=" 			{return setValue(line,'=',yytext);
			lastToken = strdup(yytext);
			}
"<" 			{return setValue(line,'<',yytext);
			lastToken = strdup(yytext);
			}
">" 			{return setValue(line,'>',yytext);
			lastToken = strdup(yytext);
			}
"+" 			{return setValue(line,'+',yytext);
			lastToken = strdup(yytext);
			}
"-" 			{return setValue(line,'-',yytext);
			lastToken = strdup(yytext);
			}
"*" 			{return setValue(line,'*',yytext);
			lastToken = strdup(yytext);
			}
"/" 			{return setValue(line,'/',yytext);
			lastToken = strdup(yytext);
			}
"%" 			{return setValue(line,'%',yytext);
			lastToken = strdup(yytext);
			}
"?" 			{return setValue(line,'?',yytext);
			lastToken = strdup(yytext);
			}
"{" 			{return setValue(line,'{',yytext);
			lastToken = strdup(yytext);
			}
"}" 			{return setValue(line,'}',yytext);
			lastToken = strdup(yytext);
			}
"|"				{return setValue(line,'|',yytext);
			lastToken = strdup(yytext);
			}
"&"				{return setValue(line,'&',yytext);
			lastToken = strdup(yytext);
			}
"!"				{return setValue(line,'!',yytext);
			lastToken = strdup(yytext);
			}


"+=" 			{return setValue(line,ADDASS, yytext);
			lastToken = strdup(yytext);
			}
"-="			{return setValue(line,SUBASS, yytext);
			lastToken = strdup(yytext);
			}
"/="			{return setValue(line,DIVASS, yytext); 
			lastToken = strdup(yytext);
			}
"*="			{return setValue(line,MULASS, yytext);
			lastToken = strdup(yytext);
			}
"<="			{return setValue(line,LESSEQ, yytext);
			lastToken = strdup(yytext);
			}
">="			{return setValue(line,GRTEQ, yytext);
			lastToken = strdup(yytext);
			}
"!="			{return setValue(line,NOTEQ, yytext);
			lastToken = strdup(yytext);
			}
"=="			{return setValue(line,EQ, yytext); 
			lastToken = strdup(yytext);
			}
"--"			{return setValue(line,DEC, yytext); 
			lastToken = strdup(yytext);
			}
"++"			{return setValue(line,INC, yytext); 
			lastToken = strdup(yytext);
			}
 

 



 
[0-9]+		     		{return setValue(line,NUMCONST,yytext);
			lastToken = strdup(yytext);
			}
"\""([^\"\n]|[\[^n0]])*"\""	{return setValue(line, CHARCONST, yytext); 
			lastToken = strdup(yytext);
			}
\'[\\]?.\'			{return setValue(line, CHARCONST, yytext); 
			lastToken = strdup(yytext);
			}
[_A-Za-z][_a-zA-Z0-9]*	 	{return setValue(line, ID, yytext);
			lastToken = strdup(yytext);
			}

"\"""\\\""'"\""			{return setValue(line, CHARCONST, yytext);
			lastToken = strdup(yytext);
			}

[\n]				{line++;}
[ ]* 			;
[\t]*			;


.				{printf("ERROR(%d): Invalid or misplaced input character: \"%s\". Character Ignored.\n", line, yytext); numErrors++;
			lastToken = strdup(yytext);
				}
"\'".[.]+"\'"			{printf("WARNING(%d): character is %d characters long and not a single character %s.  The first char will be used.\n", line, strlen(yytext), yytext[0]); numWarnings++; return setValue(line, CHARCONST, yytext);
			lastToken = strdup(yytext);
				}
"\'""\'"			{printf("ERROR(%d): Empter character ''.  Characters ignored.\n"); numErrors++;
			lastToken = strdup(yytext);
				}

