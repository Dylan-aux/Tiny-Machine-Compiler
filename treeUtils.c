#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "scanType.h"
#include "treeUtils.h"
#include "parser.tab.h" //generated when lex and yacc are run


int printTyping = 1;

/* Prototypes for each of the different types of TreeNodes
 * that will be generated to create the parse tree
 */

//3 Types of nodes
/* TreeNode * newDeclarationNode(DeclKind, TokenData*);
 TreeNode * newStatmentNode(StmtKind, TokenData*);
 TreeNode * newExpressionNode(ExpKind, TokenData*);
*/
		
//bool printAll = true;
bool printAll = false;
//if printAll is set true it will print the nodes at each critical point to track the progress of each function



//creates a new declaration node
TreeNode * newDeclarationNode(DeclKind kind, TokenData * token) 
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	for (i = 0; i<MAXCHILDREN; i++)
	{
		t->child[i] = NULL;
	}
	t->sibling = NULL;
	t->nodeKind = DeclK;
	t->kind.decl = kind;
	t->linenum = token -> lineNum;
	t-> isBool = -1;
	t-> expType = Void;
	return t;
}

//creates a new statement node
TreeNode * newStatementNode(StmtKind kind, TokenData * token) 
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	for ( i = 0; i < MAXCHILDREN; i++)
	{
		t ->child[i] = NULL;
	}
	t->sibling = NULL;
	t->nodeKind = StmtK;
	t->kind.stmt = kind;
	t->linenum = token -> lineNum;
	t -> isBool = -1; 

	return t;
}

//creates a new expression node
TreeNode * newExpressionNode(ExpKind kind, TokenData * token) 
{
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	for (i = 0; i<MAXCHILDREN; i++)
	{
		t->child[i] = NULL;
	}
	t->sibling = NULL;
	t->nodeKind = ExpK;
	t->kind.exp = kind;
	t->linenum = token -> lineNum;
	t-> expType = Und;
	t -> isBool = -1;

	return t;
}


//sets the enumerated node type
ExpType setNodeType (TokenData *tokenNode)
{
	switch (tokenNode -> tokenClass)
		{
		case INT:
		case Int:
			return Int;
			break;
		case BOOL:
		case Bool:
			return Bool;
			break;
			case CHAR:
	case Char:
			return Char;
			break;
		default :
			printf("--------------set type default\n");
			return Void;
			break;
		}
}


//sets the enumerated operator 
OpKind setOperator(TokenData * token)
{
	switch(token ->tokenClass)
	{
		case ADDASS:
			return addassOp;
		case SUBASS:
			return subassOp;
		case DIVASS:
			return divassOp;
		case MULASS:
			return mulassOp;
		case LESSEQ:
			return lesseqOp;
		case GRTEQ:
			return greateqOp;
		case NOTEQ:
			return noteqOp;
		case EQ:
			return eeqOp;
		case DEC:
			return decOp;
		case INC:
			return incOp;
		case '&':
			return andOp;
		case '|':
			return orOp;
		case '!':
			return notOp;
		case '=':
			return eqOp;
		case '+':
			return plusOp;
		case '-':
			return minusOp;
		case '*':
			return mulOp;
		case '/':
			return divOp;
		case '>':
			return greatOp;
		case '<':
			return lessOp;
		case '%':
			return percOp;
		case '?':
			return queOp;
		case '[':
			return leftbrackOp;
		case ']':
			return rightbrackOp;
		
	}
}


//prints declaration nodes
void printDeclarationNode(TreeNode *treeNode)
{
	ExpType expressionType;
	const char * type;
	
	//initial switch converts the enumerated expType to a char* that can be printed
	switch (treeNode -> expType)
	{
		case Void:
			type = "type void";
			break;
		case Int:
			type = "type int";
			break;
		case Bool:
			type = "type bool";
			break;
		case Char:
		case CHAR:	
			type = "type char";
			break;
		case Und:
			type = "undefined type";
			break;
		default:
			type = "no valid expType";
			break;	
	}

	//selects a specific print statement/s based on the type and data present in each of the declaration nodes as well as their line numbers
	expressionType = treeNode -> expType;
	switch(treeNode -> kind.decl)
	{
		case VarK:	
				if(printAll) printf("vark case\n");
			if(treeNode ->isArray)
			{ 
				if(type == "type void")
				{
					printf("Var %s is array undefined type [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID,  treeNode -> location, treeNode -> size, treeNode -> offset,  treeNode ->linenum);
				} else
				{
					printf("Var %s is array of %s [mem: %s size: %d loc: %d]  [line: %d]\n", treeNode -> ID, type, treeNode -> location,  treeNode -> size, treeNode -> offset,treeNode ->linenum);
				}
			
				
			} else {
				if(type == "type void")
				{
					printf("Var %s is undefined type [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID, treeNode -> location,  treeNode -> size, treeNode -> offset,treeNode ->linenum);
				} else
				{
					printf("Var %s: %s [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID, type, treeNode -> location, treeNode -> size, treeNode -> offset,treeNode ->linenum);
				}
			}
			break;
		case FuncK:
			if(printAll) printf("funck case\n");
			printf("Func %s: returns %s [line: %d]\n", treeNode -> ID, type, treeNode -> linenum);
			break;
		case ParamK:
			if(printAll) printf("paramk case\n");
			if(treeNode ->isArray)
			{ printf("Param %s is array of %s [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID, type,  treeNode -> location, treeNode -> size, treeNode -> offset,  treeNode ->linenum);
			} else {
			printf("Param %s: %s [mem: %s size: %d loc: %d] [line: %d]\n", treeNode->ID, type, treeNode -> location, treeNode -> size, treeNode -> offset,  treeNode ->linenum );
			}
			break;
		default:
			if(printAll) printf("defaultdeclk case\n");
			printf("Unknown declKind\n");
			break;
	}
}

//print function for statement nodes
void printStatementNode(TreeNode *treeNode)
{
	
	switch (treeNode -> kind.stmt)
	{
		case ElsifK:
				if(printAll) printf("elseifk case\n");
			printf("Elsif [line: %d]\n", treeNode -> linenum );
			break;
		case IfK:
				if(printAll) printf("ifk case\n");
			printf("If [line: %d]\n", treeNode -> linenum );
			break;
		case WhileK:
				if(printAll) printf("whilek case\n");
			printf("While [line: %d]\n", treeNode -> linenum );
			break;
		case CompoundK:
				if(printAll) printf("compiundk case\n");
			printf("Compound [line: %d]\n", treeNode -> linenum );
			break;
		case IterationK:
				if(printAll) printf("rangek case\n");
			if(treeNode -> child[0] -> nodeKind != DeclK)
			{	
				printf("While [line: %d]\n", treeNode -> linenum );
			} else 
			{
				printf("For [line: %d]\n", treeNode -> linenum);
			}	


			break;
		case ReturnK:	
				if(printAll) printf("returnk case\n");
			printf("Return [line: %d]\n", treeNode -> linenum );
			break;
		case BreakK:
				if(printAll) printf("break case\n");
			printf("Break [line: %d]\n", treeNode -> linenum );
			break;
		default:
				if(printAll) printf("default statement case\n");
			printf("Unknown Stmt\n");
			break;
	}
}


//prints expression nodes
void printExpressionNode(TreeNode * treeNode)
{
	ExpType expressionType;
	const char * type;
	
	const char * mem;

	char* printKind;
	//switch statement to change the enumerated operator to a char* that can be printed
	switch(treeNode -> attr.op)
	{
		case leftbrackOp:
			printKind = strdup("[");
			break;
		case rightbrackOp:
			printKind = strdup("]");
			break;		
		case eeqOp:
			printKind = strdup("==");
			break;
		case andOp:
			printKind = strdup("&");
			break;
		case orOp:
			printKind = strdup("|");
			break;
		case notOp:
			printKind = strdup("!");
			break;
		case eqOp:
			printKind = strdup("=");
			break;
		case noteqOp:
			printKind = strdup("!=");
			break;
		case plusOp:
			printKind = strdup("+");
			break;
		case minusOp:
			printKind = strdup("chsign");
			break;
		case mulOp:
			printKind = strdup("*");
			break;
		case divOp:
			printKind = strdup("/");
			break;
		case incOp:
			printKind = strdup("++");
			break;
		case decOp:
			printKind = strdup("--");
			break;
		case addassOp:
			printKind = strdup("+=");
			break;
		case subassOp:
			printKind = strdup("-=");
			break;
		case mulassOp:
			printKind = strdup("*=");
			break;
		case divassOp:
			printKind = strdup("/=");
			break;
		case lesseqOp:
			printKind = strdup("<=");
			break;
		case lessOp:
			printKind = strdup("<");
			break;
		case greateqOp:
			printKind = strdup(">=");
			break;
		case greatOp:
			printKind = strdup(">");
			break;
		case percOp:
			printKind = strdup("%");
			break;
		case queOp:
			printKind = strdup("?");
			break;
		default:
			printKind = strdup("WHAT THE FUCK");
			break;
	}
	switch(treeNode -> kind.exp)
	{
		case OpK:
				if(printAll) printf("opk case\n");
			printf("Op: %s [line: %d]\n", printKind, treeNode ->linenum);
			break;
		case ConstantK:
	switch (treeNode -> expType)
	{
		case Void:
			type = "type void";
			break;
		case Int:
			type = "type int";
			break;
		case Bool:
			type = "type bool";
			break;
		case Char:
		case CHAR:	
			type = "type char";
			break;
		case Und:
			type = "undefined type";
			break;
		default:
			type = "no valid expType";
			break;	
	}
			if(treeNode ->isBool > 0)
			{
				if(treeNode -> attr.boolvalue == 1 )
				{
					printf("Const: false: %s [line: %d]\n", type,treeNode -> linenum);
				}
				if(treeNode -> attr.boolvalue != 1)
				{
					printf("Const: true: %s [line: %d]\n",type,  treeNode -> linenum);
				}
			} else {

			if(printAll) printf("constant case\n");
			if(treeNode -> isArray == 1)
			{
				printf("Const: %d: %s [line: %d]\n", treeNode -> value, type, treeNode -> linenum);
			} else{
			printf("Const: %d: %s [line: %d]\n", treeNode -> value, type, treeNode -> linenum);
			}
			}
			break;

		case CharconstK:
			if(printAll) printf("Charconst case\n");
			
			if(treeNode -> attr.cvalue[1] == '\\' && treeNode -> attr.cvalue[2] == 'n')
			{printf("Const: \'\n\' [line: %d]\n", treeNode -> linenum);
			}
			else if(treeNode -> attr.cvalue[1] == '\\' && treeNode -> attr.cvalue[2] == '0')
			{printf("Const: \' \' [line: %d]\n", treeNode -> linenum);
			} else if (treeNode -> attr.cvalue[1] == '\\')
			{printf("Const: \'%c\' [line: %d]\n", treeNode-> attr.cvalue[2], treeNode -> linenum);
			} else {	
			printf("Const: %s [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID, treeNode -> location, treeNode -> size, treeNode -> offset,treeNode -> linenum);
			}
			break;
		case IdK:
				if(printAll) printf("idk case\n");
				printf("Id: %s [mem: %s size: %d loc: %d] [line: %d]\n", treeNode -> ID, treeNode -> location, treeNode -> size, treeNode -> offset,treeNode -> linenum);
			break;
		case AssignK:
	switch (treeNode  -> expType)
	{
		case Void:
			type = "type void";
			break;
		case Int:
			type = "type int";
			break;
		case Bool:
			type = "type bool";
			break;
		case Char:
		case CHAR:	
			type = "type char";
			break;
		case Und:
			type = "undefined type";
			break;
		default:
			type = "invalid expType";
			break;	
	}
			if(printAll) printf("assignk case\n");
			printf("Assign: %s : %s [line: %d]\n", printKind, type,  treeNode -> linenum);
			break;
		case CallK:
				if(printAll) printf("callk case\n");
			printf("Call: %s [line: %d]\n", treeNode -> ID, treeNode -> linenum);
			break;
			
		default:
				if(printAll) printf("defauly expression case\n");
			printf("Unknown exp\n");
			break;
	
	}
}


static int indentno = 0;

#define INDENT indentno+=1;
#define UNINDENT indentno-=1;


static void printSpaces(void)
{
	int i;
	for (i=0; i<indentno; i++)
		printf(".   ", i);
}


void printTree(TreeNode * tree)
{
	int i , siblingCount = 1;;
	while (tree!=NULL)
	{
		switch(tree -> nodeKind)
		{
			case DeclK:
				if(printAll) printf("decl case\n");
				printDeclarationNode(tree);
				break;
			case StmtK:
				if(printAll) printf("stmtk case\n");
				printStatementNode(tree);
				break;
			case ExpK:
				if(printAll) printf("expk case\n");
				printExpressionNode(tree);
				break;
			default:
				if(printAll) printf("default case\n");
				printf("UNK NODEKIND\n");
				return;
		}		
			
		for(i = 0; i<MAXCHILDREN; i++)
		{
			if (tree -> child[i] != NULL)
			{
	INDENT;
		printSpaces();
				printf("Child: %d ", i);
				printTree(tree->child[i]);
				
			}
		}
		//printf("traversing sibling\n");

		tree = tree->sibling;
		if(tree != NULL){	
		printSpaces();
		printf("Sibling: %d ", siblingCount);
		siblingCount++;
		}
	}

				UNINDENT;
}


/*//Types of declarations
static TreeNode * variableDeclaration(void);
static TreeNode * functionDeclaration(void);
static TreeNode * parameterDeclaration(void);

//Types of Statements
static TreeNode * compountStatment(void);
static TreeNode * nullStatement(void);
static TreeNode * elsifStatment(void);
static TreeNode * ifStatement(void);
static TreeNode * whileStatment(void);
static TreeNode * loopStatment(void);
static TreeNode * loopForeverStatement(void);
static TreeNode * compoundStatement(void);
static TreeNode * rangeStatment(void);
static TreeNode * returnStatment(void);
static TreeNode * breakStatement(void);

//Types of Expressions
static TreeNode * operationExpression(void);
static TreeNode * constantexpression(void);
static TreeNode * idExpression(void);
static TreeNode * assignmentExpression(void);
static TreeNode * initExpression(void);
static TreeNode * callExpression(void);
*/

