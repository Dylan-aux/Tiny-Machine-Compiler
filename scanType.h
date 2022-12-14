#ifndef SCANTYPE_H
#define SCANTYPE_H

//struct to hold all data that may be necassary in a token
struct TokenData 
{
	int lineNum, tokenClass;
	
	char *token;
	char *tokenVal;

	char charConst;
	int boolVal;
	
	int numVal;

	
};
#endif	
