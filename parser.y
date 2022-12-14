%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"scanType.h"
#include"treeUtils.h"
#include "ourgetopt.h"
#include "semantic.h"
#include "symbolTable.h"
#include "yyerror.h"
#include "codegen.h"
#include "emitcode.h"

extern int printTyping;
extern int numWarnings;
extern int numErrors;
extern int yylex();
extern FILE *yyin;
void yyerror(const char *msg)
{
	printf("ERROR(PARSER): %s\n", msg);
}

extern OpKind setOperator(TokenData*);
extern TreeNode * newDeclarationNode(DeclKind, TokenData *);
extern TreeNode * newStatementNode(StmtKind, TokenData *);
extern TreeNode * newExpressionNode(ExpKind, TokenData *);
extern  ExpType setNodeType (TokenData*);

extern void printTree(TreeNode *);
TreeNode *addSibling(TreeNode *t, TreeNode *s)
{
	t->sibling = s;
}

void setType(TreeNode *t, ExpType type, bool isStatic)
{

}

TreeNode *syntaxTree;


int nodeCount = 0;

bool fullPrint = false;


//union used as a specific piece can never be both a token and a node
// and no piece can be anything other than a token or a node to a token
%}

%union {
	TokenData *tokenData;
	TreeNode * tree;
}



%type <tree> program 
%type <tree> declarationList declaration 
%type <tree> varDeclaration funDeclaration
%type <tree> varDeclList 
%type <tree> scopedVarDeclaration 
%type <tree> varDeclInitialize
%type <tree> varDeclId simpleExpression

%type <tree> params statement
%type <tree> paramList
%type <tree> paramTypeList
%type <tree> paramIdList
%type <tree> paramId
%type <tree> expressionStmt compoundStmt returnStmt breakStmt
%type <tree> expression
%type <tree> localDeclarations statementList

%type <tree> matched unmatched partialStmt


%type <tree> mutable 
%type <tree> andExpression
%type <tree> unaryRelExpression
%type <tree> relExpression
%type <tree> sumExpression
%type <tree> mulExpression
%type <tree> unaryExpression
%type <tree> factor
%type <tree> immutable 
%type <tree> call constant
%type <tree> args
%type <tree> argList


%type <tokenData> unaryop relop mulop sumop
%type <tokenData> typeSpecifier


%token <tokenData> STATIC BOOL CHAR IF INT ELSE WHILE RETURN BREAK BOOLCONST FOR IN  
%token <tokenData> ';' ',' ':' '(' ')' '[' ']' '=' '<' '>' '+' '-' '*' '/' '%' '?' '{' '}' '|' '&' '!'
%token <tokenData> ADDASS SUBASS DIVASS MULASS LESSEQ GRTEQ NOTEQ EQ DEC INC
%token <tokenData> NUMCONST CHARCONST ID 
%token <tokenData> ERROR


%define parse.error verbose
%%

program 		: declarationList	{ $$ = $1;
						syntaxTree = $$;
								
	}		;

declarationList 	: declarationList declaration 	{TreeNode *t = $1;
							 if (t != NULL)
							 {
								while(t->sibling != NULL)
								{
									t = t->sibling;
								}	
								t->sibling = $2;
								$$ = $1;
							} else {
							$$ = $2;}}
			| declaration			{$$ = $1;}			
			;

declaration		: varDeclaration	{
						if (fullPrint) printf("declaration <- varDeclaration\n");	
						$$ = $1; 
						
						}
			| funDeclaration	{
						if (fullPrint)printf("declaration <- funDeclaration\n");
						$$ = $1;}
			| error			{$$ = NULL;}
			;				






varDeclaration 		: typeSpecifier varDeclList';' 		{
								if(fullPrint)  printf("varDeclList <- typeSpecifier varDeclList\n");
								{TreeNode *t = $2;
								t-> expType = setNodeType($1);
								while(t-> sibling != NULL)
								{	
								t-> expType = setNodeType($1);
								
							 	t = t-> sibling;
								t-> expType = setNodeType($1);
									 
										
								}
								$$ = $2;
								}
								}
			| error varDeclList ';'			{ $$ = NULL;}
			| typeSpecifier error ';'		{ $$ = NULL; yyerrok;}
			;

scopedVarDeclaration	: STATIC typeSpecifier varDeclList ';'	{ if(fullPrint) printf("scopedVarDeclaration <- Static Typespecdiferi vardelclist\n");
								TreeNode *t = $3;
								t -> expType = setNodeType($2);
								while(t -> sibling != NULL)
								{
									t -> expType = setNodeType($2);
									t -> isStatic = 1;
									t = t->sibling;
									t -> expType = setNodeType($2);
								}
								 $$ = $3;
								}
			| typeSpecifier varDeclList ';'		{ if(fullPrint) printf("scopedVarDeclaration <- typescpecifier vardelclist\n");
								TreeNode *t = $2;
								t -> expType = setNodeType($1);
								while(t -> sibling != NULL)
								{
									t -> expType = setNodeType($1);
									t -> isStatic = 0;
									t = t->sibling;
									t -> expType = setNodeType($1);
								}
								 $$ = $2;
								}	

			| error varDeclList ';'			{ $$ = NULL; yyerrok;}				
									
									
			;

varDeclList		: varDeclList ',' varDeclInitialize	{
								if(fullPrint) printf("varDeclList <- varDeclList varDelInitialize\n");
								TreeNode *t = $1;
								if(t !=NULL)
								{
									while(t->sibling !=NULL)
									{
										t = t->sibling;
									}
								 	t->sibling = $3;
									$$ = $1;
								} else { $$ = $3;}

								}

			| varDeclInitialize		{$$ = $1;
								}	

			| varDeclList ',' error			{$$ = NULL;}
			| error					{$$ = NULL;}
			;

varDeclInitialize	: varDeclId				{
								if(fullPrint) printf("varDeclInitialize <- varDeclId @ line: %d\n", $1 -> linenum);
									$$ = $1;
								}

			| varDeclId ':' simpleExpression	{
									$$ = newExpressionNode(InitK, $2);
								
								$1 -> child[0] = $3;
								$$ = $1;
							nodeCount++;
								if(fullPrint) printf("varDeclInitialize <- varDecI : simpleExpression @ line: %d\n", $1 -> linenum);
								}

			| error ':' simpleExpression		{$$ = NULL; yyerrok;}
			| varDeclId ':' error			{$$ =NULL;}	
			;

varDeclId		: ID			{
						if(fullPrint) printf("varDeclID <- ID\n");
						$$ = newDeclarationNode(VarK, $1);
						 $$ -> ID = strdup($1 -> token);
						$$ -> size = 1;
							nodeCount++;
						
	}

			| ID '['NUMCONST']'	{	
						if(fullPrint) printf(" varDeclID <- ID [NUMCONST]\n");
						$$ = newDeclarationNode(VarK, $1);
						 $$ -> ID = strdup($1 -> token);
						 $$ -> isArray = 1;
	
						char *temp = strdup($3 -> token);
						$$ -> size = atoi(temp) +1;


						nodeCount++;
						}
			| ID '[' error		{$$ = NULL;}
			| error ']'		{$$ = NULL; yyerrok;}							
			;


typeSpecifier		: INT			{
						if (fullPrint) printf("typeSpecifier <- INT\n");
						$$ = $1;}
			| BOOL			{
						if (fullPrint) printf("typeSpecifier <- BOOL\n");
						$$ = $1;}
			| CHAR			{
						if (fullPrint) printf("typeSpecifier <- CHAR\n");		
						$$ = $1;}
			;





funDeclaration		: typeSpecifier ID '('params')' statement	{

									if(fullPrint) printf("funDeclaration <- typeSpecifier ID (params) statement\n");

										$$ = newDeclarationNode(FuncK, $2);
									$$ -> ID = strdup($2 ->token);
									$$ -> expType = setNodeType($1);
									$$ -> child[0] = $4;
									$$ -> child[1] = $6;
							nodeCount++;}

			| ID '('params')' statement			{
									if(fullPrint) printf("funDeclaration <- ID (params) statement\n");	
									$$ = newDeclarationNode(FuncK, $1);
									$$ -> expType = Void;
									$$ -> child[0] = $3;
									$$ -> child[1] = $5;
									$$ -> ID = strdup($1->token);
							nodeCount++;
									}
			| typeSpecifier error				{$$ = NULL;}
			| typeSpecifier ID '(' error			{$$ = NULL;}
			| typeSpecifier ID '(' params ')' error 	{$$ = NULL;}
			| ID '(' error					{$$ = NULL;}
			| ID '(' params ')' error			{$$ = NULL;}
			;

params			: paramList			{
							if(fullPrint) printf("params <- paramList\n");
								$$ = $1; }
			|%empty				{
							if(fullPrint) printf("params <- empty\n");
							$$ = NULL;}
			;

paramList		: paramList ';' paramTypeList	{
							if(fullPrint) printf("paramList <- paramList ; paramTypeList\n");
							TreeNode *t = $1;
							if (t != NULL)
							{
								while(t->sibling != NULL)
								{
									t = t->sibling;
								}
								t-> sibling = $3;
								$$ = $1;
							} else {
								$$ = $3;
							}
							}	

			| paramTypeList			{
							if(fullPrint) printf("paramList <- paramTypeList\n");
							$$ = $1;}
			
			| paramList ';' error		{$$ = NULL;}
			| error				{$$ = NULL;}
			;

paramTypeList 		: typeSpecifier paramIdList	{if(fullPrint) printf("paramTypeList <- typeSpecifier paramIdList\n");
								{TreeNode *t = $2;
								t-> expType = setNodeType($1);
								while(t-> sibling != NULL)
								{	
								t-> expType = setNodeType($1);
							 	t = t-> sibling;
								t-> expType = setNodeType($1);
									 
										
								}
								$$ = $2;
								}
							$$ = $2;}

			| typeSpecifier error		{$$ = NULL;}
			;

paramIdList		: paramIdList ',' paramId	{
							if(fullPrint) printf("paramIdList <- paramIdList , paraId \n");
							

								TreeNode *t = $1;
								if(t !=NULL)
								{
									while(t->sibling !=NULL)
									{
										t = t->sibling;
									}
								 	t->sibling = $3;
									$$ = $1;
							} else {
							 $$ = $1;}
				}	

			| paramId			{
							if(fullPrint) printf("paraIdList <- paramId\n");
							$$ = $1;}
			| paramIdList ',' error		{$$ = NULL;}
			| error				{$$ = NULL;}
			;

paramId			: ID							{
										if(fullPrint) printf("paramId <- ID\n");
										$$ = newDeclarationNode(ParamK, $1);
										 $$ -> ID = strdup($1 -> token);
										$$ -> size = 1;
	
										
							nodeCount++;}

			| ID '['']'						{$$ = newDeclarationNode(ParamK, $1);
										if(fullPrint) printf("paramId <- ID[]\n");
										$$ -> ID = strdup($1 -> token);
										$$ -> isArray = 1;
										char * temp;
										$$ -> size = 1;




										nodeCount++; }

			| error ']'						{yyerrok; $$ = NULL;}
			;







statement		: matched 						{$$ = $1;}
			| unmatched						{$$ = $1;}								;

partialStmt		: expressionStmt					{$$ = $1;
										if(fullPrint) printf("partialStmt <- expressionStmt\n");}
			| compoundStmt						{$$ = $1;
										if(fullPrint) printf("partialStmt <- compoundStmt\n");}
			| returnStmt						{$$ = $1;
										if(fullPrint) printf("partialStmt <- returnStmt\n");}
			| breakStmt						{$$ = $1;
										if(fullPrint) printf("partialStmt <- breakStmt\n");}
			;

expressionStmt		: expression ';'					{$$ = $1; yyerrok;
										if(fullPrint) printf("expressionStmt <- expression ;\n");}
			| ';'							{$$ = NULL; yyerrok;
										if(fullPrint) printf("expressionStmt <- ; \n");}
			| error ';'						{$$ = NULL; yyerrok;}
			
			;

compoundStmt		: '{' localDeclarations statementList '}'		{	if(fullPrint) printf("compoundStmt <- {localDeclaration statmentLis} \n");
										$$ = newStatementNode(CompoundK, $1);
										$$ -> child[0] = $2;
										$$ -> child[1] = $3;
								
										}
			| '{' error statementList '}'				{ $$ = NULL; yyerrok;}
			| '{' localDeclarations error '}'			{ $$ = NULL; yyerrok;}
			;
localDeclarations	: localDeclarations scopedVarDeclaration		{
										if(fullPrint) printf("localDelcarations <- localDeclarations scopedvarDeclaration\n");
										TreeNode *t = $1;
										 if(t != NULL)
										 { 
										 	while(t-> sibling != NULL)
											{
												t = t-> sibling;
											}
											t->sibling = $2;
											$$ = $1;
										} else {
										$$ = $2;}
										}
								
			| %empty			{
							if(fullPrint) printf("localDeclarations <- empty\n");
							$$ = NULL;}
			;

statementList		: statementList statement				{ TreeNode * t = $1;
										 if(fullPrint) printf("statementList <- statementList statement\n");
										 
										 if(t!=NULL)
										 {
										 while(t-> sibling != NULL)
										 {
											t = t-> sibling;
										 }
										 t-> sibling = $2;
										 $$ = $1;}
										 else {
										 $$ = $2;
										 }
							nodeCount++;
	}
			| %empty					{
									if(fullPrint) printf("statementList <- \%empty\n");	
										$$ = NULL;}
			;
	
matched			:  partialStmt						{
										if(fullPrint) printf("matched <- partialStmt\n");
										$$ = $1;}
			| WHILE '('simpleExpression')' matched		    {if(fullPrint) printf("iterationStmt <- while '('simpleExpression')' statement\n");
										$$ = newStatementNode(WhileK, $1);
									     $$ -> child[0] = $3;
									     $$ -> child[1] = $5;
							nodeCount++;
										}
			| FOR '('ID IN ID')' matched			     {if(fullPrint) printf("iterationStmt <- for (ID in ID) statement\n");
												$$ = newStatementNode(IterationK, $1);
												TreeNode *s = newDeclarationNode(VarK, $3);
												s -> ID = strdup($3 -> token);
								    				$$ -> child[0] = s;
												TreeNode * t =  newExpressionNode(IdK, $1);
												t -> ID = strdup($5 -> token);
								    				$$ -> child[1] = t;
								    				$$ -> child[2] = $7;
							nodeCount++;
												}
			| IF'('simpleExpression')' matched ELSE matched	{if(fullPrint) printf("matched <- else unmatched\n");
										$$ = newStatementNode(IfK, $1);
										$$ -> child[0] = $3;
										$$ -> child[1] = $5;
										$$ -> child[2] = $7;
										nodeCount++;
										}
			| IF error						{$$ = NULL;}
			| IF error ELSE matched					{$$ = NULL; yyerrok;}
			| IF error ')' matched ELSE matched			{$$ = NULL; yyerrok;}
			| WHILE error ')' matched				{$$ = NULL; yyerrok;}
			| WHILE error						{$$ = NULL;}
			| FOR ')' matched					{$$ = NULL; yyerrok;}
			| FOR error						{$$ = NULL;}
			


			;

unmatched		: IF '('simpleExpression')' unmatched
			  					{
										if(fullPrint) printf("unmatched <- IF (simpleExpression) THEN matched\n");
										$$ = newStatementNode(IfK, $1);
										 $$ -> child[0] = $3;
										 $$ -> child[1] = $5;
							nodeCount++;}

			| IF '('simpleExpression')' matched 			{if(fullPrint) printf(" matched <- IF (simpleExpression) matched  \n");
										$$ = newStatementNode(IfK, $1);
										$$ -> child[0] = $3;
										$$ -> child[1] = $5;
										nodeCount++;
										}

			| IF'('simpleExpression')' matched ELSE unmatched	{if(fullPrint) printf("matched <- else unmatched\n");
										$$ = newStatementNode(IfK, $1);
										$$ -> child[0] = $3;
										$$ -> child[1] = $5;
										$$ -> child[2] = $7;
										nodeCount++;
										}
			| WHILE '('simpleExpression')' unmatched	    {if(fullPrint) printf("iterationStmt <- while '('simpleExpression')' statement\n");
										$$ = newStatementNode(WhileK, $1);
									     $$ -> child[0] = $3;
									     $$ -> child[1] = $5;
									    
							nodeCount++;
									  
										}
			| FOR '('ID IN ID')' unmatched			     {if(fullPrint) printf("iterationStmt <- for (ID in ID) statement\n");
												$$ = newStatementNode(IterationK, $1);
												TreeNode *s = newDeclarationNode(VarK, $3);
												s -> ID = strdup($3 -> token);
								    				$$ -> child[0] = s;
												TreeNode * t =  newExpressionNode(IdK, $1);
												t -> ID = strdup($5 -> token);
								    				$$ -> child[1] = t;
								    				$$ -> child[2] = $7;
							nodeCount++;
												}
			
			| IF error ')' statement				{$$ = NULL; yyerrok;}
			| IF error ELSE unmatched				{$$ = NULL; yyerrok;}
			| IF error ')' matched ELSE unmatched			{$$ = NULL; yyerrok;}
			| WHILE error ')' unmatched				{$$ = NULL; yyerrok;}
			| FOR error ')' unmatched				{$$ = NULL; yyerrok;}
			;





returnStmt		: RETURN ';'			{if(fullPrint) printf("returnStmt <- RETURN ; \n");
							$$ = newStatementNode(ReturnK, $1);
							nodeCount++;
							}
			| RETURN expression ';'		{	if(fullPrint) printf("returnStmt <- RETURN expression ; \n");
							$$ = newStatementNode(ReturnK, $1);
							 $$ -> child[0] = $2;
							nodeCount++; yyerrok;
						}
			;

breakStmt		: BREAK	';'			{	if(fullPrint) printf("breakStmt <- BREAK \n");
							$$ = newStatementNode(BreakK, $1);
							nodeCount++;
						}
			;






expression		: mutable '=' expression	{	if(fullPrint) printf("expression <- mutable = expression\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> child[1] = $3;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
						}
			| mutable ADDASS expression	{if(fullPrint) printf("expression <- mutable ADDASS expression\n");
								$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> child[1] = $3;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
							}
			| mutable SUBASS expression	{	if(fullPrint) printf("expression <- mutable SUBASS expression\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> child[1] = $3;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
						}
			| mutable MULASS expression	{	if(fullPrint) printf("expression <- mutable MULASS expression\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> child[1] = $3;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
						}
			| mutable DIVASS expression	{if(fullPrint) printf("expression <- mutable DIVASS expression\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> child[1] = $3;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
							}	
			| mutable INC			{if(fullPrint) printf("expression <- mutable INC\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
							}

			| mutable DEC			{if(fullPrint) printf("expression <- mutable DEC\n");
							$$ = newExpressionNode(AssignK, $2);
							 $$ -> child[0] = $1;
							 $$ -> attr.op = setOperator($2);
							nodeCount++;
							}
			| simpleExpression		{if(fullPrint) printf("expression <- simpleExpression\n");
							$$ = $1;
							}
			| error '=' error 		{$$ = NULL;}
			| error ADDASS error 		{$$ = NULL;}
			| error SUBASS error 		{$$ = NULL;}
			| error MULASS error 		{$$ = NULL;}
			| error DIVASS error 		{$$ = NULL;}
			| error INC			{$$ = NULL; yyerrok;}
			| error DEC			{$$ = NULL; yyerrok;}
			;

simpleExpression	: simpleExpression '|' andExpression 	{if(fullPrint) printf("simpleExpression <- simpleExpression OR andExpression\n");
								$$ = newExpressionNode(OpK, $2);
								 $$ -> child[0] = $1;
								 $$ -> child[1] = $3;
								 $$ -> attr.op = setOperator($2);
							nodeCount++;
								}
			| andExpression				{if(fullPrint) printf("simpleExpression <- andExpression\n");
								$$ = $1;
								}
			| simpleExpression '|' error 		{$$= NULL;}
			;

andExpression 		: andExpression '&' unaryRelExpression 	{if(fullPrint) printf("andExpression <- andExpression AND unaryRelExpression\n");
								$$ = newExpressionNode(OpK, $2);
								 $$ -> child[0] = $1;	
								 $$ -> child[1] = $3;
								 $$ -> attr.op = setOperator($2);
							nodeCount++;
								}
			| unaryRelExpression			{if(fullPrint) printf("andExpression <- unaryRelExpression\n");
								$$ = $1;
								}
			| andExpression '&' error		{$$ = NULL;}

			;

unaryRelExpression	: '!' unaryRelExpression	{if(fullPrint) printf("unaryRelExpression <- NOT unaryRelExpression\n");
							$$ = newExpressionNode(OpK, $1);
							 $$ -> child[0] = $2;
							 $$ -> attr.op = setOperator($1);
							nodeCount++;
							}
			| relExpression			{if(fullPrint) printf("unaryRelExpression <- reExpression\n");
							$$ = $1;
							}

			| '!' error			{$$ = NULL;}

			;

relExpression 		: sumExpression relop sumExpression	{if(fullPrint) printf("relExpression <- sumExpression relop sumExpression\n");
								$$ = newExpressionNode(OpK, $2);
								 $$ -> child[0] = $1;
								 $$ -> child[1] = $3;
								 $$ -> attr.op = setOperator($2);
							nodeCount++;
								}
			| sumExpression				{if(fullPrint) printf("relExpression <- sumExpression\n");
								$$ = $1;
								}

			| sumExpression relop error		{$$ = NULL;}
			;

relop			: LESSEQ			{$$ = $1;
							if(fullPrint) printf("relop <- LESSEQ\n");}
			| '<'				{$$ = $1;
							if(fullPrint) printf("relop <- <\n");}
			| '>'				{$$ = $1;
							if(fullPrint) printf("relop <- >\n");}
			| GRTEQ				{$$ = $1;
							if(fullPrint) printf("relop <- GRTEQ\n");}
			| EQ				{$$ = $1;
							if(fullPrint) printf("relop <- EQ\n");}
			| NOTEQ				{$$ = $1;
							if(fullPrint) printf("relop <- NOTEQ\n");}
			;

sumExpression 		: sumExpression sumop mulExpression	{if(fullPrint) printf("sumExpression <- sumExpression sumop mulExpression\n");
								$$ = newExpressionNode(OpK, $2);
								 $$ -> child[0] = $1;
								 $$ -> child[1] = $3;
							nodeCount++;
								 $$ -> attr.op = setOperator($2);
								}
			| mulExpression				{if(fullPrint) printf("sumExpression <- mulExpression\n");
								$$ = $1;
								}
			| sumExpression sumop error		{$$ = NULL; yyerrok;}
			;
	
sumop			: '+'				{$$ = $1;
							if(fullPrint) printf("sumop <- +\n");}
			| '-'				{$$ = $1;
							if(fullPrint) printf("sumop <- -\n");}
			;

mulExpression		: mulExpression mulop unaryExpression	{if(fullPrint) printf("mulExpression <- mulExpression mulop unaryExpression\n");
								$$ = newExpressionNode(OpK, $2);
								 $$ -> child[0] = $1;
								 $$ -> child[1] = $3;
								 $$ -> attr.op = setOperator($2);
							nodeCount++;
								}
			| unaryExpression			{$$ = $1;
								if(fullPrint) printf("mulExpression <- unaryExpression\n");}
			| mulExpression mulop error		{$$ = NULL;}
			;

mulop			: '*'				{$$ = $1;
							if(fullPrint) printf("mulop <- *\n");}
			| '/'				{$$ = $1;
							if(fullPrint) printf("mulop <- /\n");}
			| '%'				{$$ = $1;
							if(fullPrint) printf("mulop <- %\n");}
			;

unaryExpression		: unaryop unaryExpression	{if(fullPrint) printf("unaryExpression <- unaryop unaryExpression \n");
							$$ = newExpressionNode(OpK, $1);
							 $$ -> attr.op = setOperator($1);
							 $$ -> child[0] = $2;
							nodeCount++;
							}
			| factor			{$$ = $1;
							if(fullPrint) printf("unaryExpression <- factor\n");}
			| unaryop error			{$$ = NULL;}

			;
		
unaryop			: '-' 				{$$ = $1;
							if(fullPrint) printf("unaryop <- -\n");}
			| '*'				{$$ = $1;
							if(fullPrint) printf("unaryop <- *\n");}
			| '?'				{$$ = $1;
							if(fullPrint) printf("unaryop <- ?\n");}
			;

factor			: immutable			{$$ = $1;
							if(fullPrint) printf("factor <- immutable\n");}
			| mutable			{$$ = $1;
							if(fullPrint) printf("factor <- mutable\n");}
			;

mutable			: ID				{if(fullPrint) printf("mutable <- ID\n");
							$$ = newExpressionNode(IdK, $1);
							$$ -> ID = strdup($1->token);
							nodeCount++;
							}

			| ID '['expression']'	{if(fullPrint) printf("mutable <- mutable [expression]\n");
							$$ = newExpressionNode(IdK, $2);
							$$ -> attr.op = setOperator($2);
							
							$$ -> ID = strdup($1 -> token);
							$$ -> child[0] = $3;
							nodeCount++;
							$$ ->isArray = 1;
							 //$$ = $1;
							}
			;

immutable		: '('expression')'		{$$ = $2; yyerrok;
							if(fullPrint) printf("immutable <- (expression)\n");}

			| call				{$$ = $1;
							if(fullPrint) printf("immutable <- call\n");}

			| constant			{$$ = $1;
							if(fullPrint) printf("immutable <- constant\n");}

			| '(' error 			{$$ = NULL;}
			| error ')'			{$$ = NULL; yyerrok;}
			;

call			: ID '('args')'			{if(fullPrint) printf("call <- ID (args)\n");
							$$ = newExpressionNode(CallK, $1);
							 $$ -> ID = strdup($1 -> token);
							 $$ -> child[0] = $3; 
							nodeCount++;
							}
			| error '('			{$$ = NULL;}
			;



args			: argList 			{$$ = $1;
							if(fullPrint) printf("args <- argList\n");}

			| %empty			{$$ = NULL;
							if(fullPrint) printf("args <- \%empty\n");}
			;

argList			: argList ',' expression	{
							if(fullPrint) printf("argList <- argList , expression\n"); 
							$$ = newExpressionNode(AssignK, $2);
							 TreeNode *t = $1;
							 while (t-> sibling != NULL)
							 {
							 	t = t->sibling;
							 }
							 t-> sibling = $3;
							 $$ = $1;
							nodeCount++; yyerrok;
							}

			| expression			{$$ = $1;
							if(fullPrint) printf("argList <- expression\n");}		
			| argList ',' error		{$$ = NULL;}
			;				

constant		: NUMCONST  			{if(fullPrint) printf("constant <- NUMCONST\n");
							$$ = newExpressionNode(ConstantK, $1);
							 $$ -> value = $1 -> numVal;
						 $$ -> ID = strdup($1 -> token);
							if(fullPrint) printf("numVal = %d\n\n\n", $$ -> value);
							 $$ -> expType = Int;							
							nodeCount++;
							}
	
			| CHARCONST			{if(fullPrint) printf("constant <- CHARCONST\n");
							$$ = newExpressionNode(CharconstK, $1);
							$$ -> attr.cvalue = $1->token;
							$$ -> ID = strdup($1 -> token);	
							nodeCount++;
							$$ -> size = strlen(strdup($1->token)) +1;

							 $$ -> expType = Char;							
							}

			
					 							
							

			| BOOLCONST			{if(fullPrint) printf("constant <- BOOLCONST\n");
							$$ = newExpressionNode(ConstantK, $1);
							char * temp;
						 $$ -> ID = strdup($1 -> token);
							$$ -> isBool = 1;
							temp = strdup($1 -> token);
							if(strcmp(temp, "true"))
							{
								$$ -> attr.boolvalue = 0;
							}
							else {
					 		$$ -> attr.boolvalue = 1;
							}

							nodeCount++;
							}
			;






































%%
int main(int argc, char *argv[])
{
	yydebug = 0;	
	int c;
	extern char *optarg;
	extern int optind;
	int file = 1;
	int dflag, pflag, errflag, mflag;

	char *ofile;
	dflag = pflag = errflag = 0;
	ofile = NULL;

	initErrorProcessing();

	extern FILE * code;

	char* temp; 


	temp = strdup(argv[1]);
	int strlength = strlen(temp);
	temp[strlength-3] = '.';
	temp[strlength-2] = 't';
	temp[strlength-1] = 'm';
	const char * output;
	output = strdup(temp);

	code = fopen(output, "w");




	yyin = fopen(argv[1], "r");
	yyparse();
	callSemAnalysis(syntaxTree);
	TreeNode * inputTree;
	inputTree = generateIOTree();	
	codeGen(syntaxTree, inputTree);	



	while(1) {
		while((c = ourGetopt(argc, argv, (char *)"PpdfM:")) != EOF)
	{		switch (c) {
			case 'd':
				dflag = 1;
				yydebug = 1;
				break;	
			case 'p':
				pflag = 1;
				break;
			case 'P':
				pflag = 1;
				break;
			case 'M':
				pflag = 1;
				break;
			case 'f':
				fullPrint = true;
				break;
			case 'h':
				printf("Usage: c- [options] [sourceFile]\n-d turn on Bison debugging\n-h this usage message\n-p printf abstract syntax tree + types\n-s turn on symbol table debugging\n");
				break;
		}
	file++;
	}	
	
	if(errflag) {
			exit(2);
		    }

	
	if( optind < argc)
	{
		optind++;
	}
	else {
	break;
	}
}

	if( argc > 1)
	{
		if(yyin = fopen ( argv[file], "r"))
		{
			yyparse();
		
			if(numErrors == 0)
				callSemAnalysis(syntaxTree);
	TreeNode * inputTree;
	inputTree = generateIOTree();	
	
			if(pflag) {
				printTyping = 1;
				printTree(syntaxTree);
	codeGen(syntaxTree, inputTree);	
			}

		}
		else if (argc == file)
		{
			yyparse();
		
			if(numErrors == 0)
				callSemAnalysis(syntaxTree);
			if(pflag) {
				printTyping = 1;
				printTree(syntaxTree);

				}
		}
		else
		{
			printf("EROORREOROEOROE\n");
			exit(1);
		}
	}
	else
	{
		yyparse();
			if(pflag) {
				printTyping = 1;
				printTree(syntaxTree);
				}
	}


printf("Number of warnings: %d\nNumber of errors: %d\n", numWarnings, numErrors);



return 0;
}


