#ifndef _TREEUTILS_H_
#define _TREEUTILS_H_
#include<stdio.h>
#include<stdlib.h>
#include"scanType.h"





typedef enum {DeclK, StmtK, ExpK} NodeKind;
typedef enum {VarK, FuncK, ParamK} DeclKind;
typedef enum {NullK, ElsifK, IfK, WhileK,   CompoundK, IterationK, ReturnK, BreakK} StmtKind;
typedef enum {OpK, ConstantK, CharconstK, IdK, AssignK, InitK, CallK, BoolconstK} ExpKind;
typedef enum {Void, Int, Bool, Char, Und} ExpType; 
typedef enum {eeqOp, andOp, orOp, notOp, eqOp, noteqOp, plusOp, minusOp, mulOp, divOp,incOp,decOp,addassOp,subassOp,mulassOp,divassOp, lesseqOp, lessOp, greateqOp, greatOp, percOp, leftbrackOp, queOp, rightbrackOp } OpKind;
typedef enum {Input, Inputb, Inputc, Output, Outputb, Outputc, iDummy, bDummy, cDummy, Outnl, NOP} IOType;



#define MAXCHILDREN 3

typedef struct treeNode{
	int linenum;
	
	struct treeNode *sibling;
	struct treeNode *child[MAXCHILDREN];
	
	NodeKind nodeKind;

	union 	{
		DeclKind decl;
		StmtKind stmt;
		ExpKind  exp;
		} kind;
	
	union {
		OpKind op;		//Operator
		char charConst;
		char *cvalue;		//For Charconst
		bool boolvalue = NULL;
		} attr;	
	int value;	
	IOType ioType = NOP;

		char * ID = NULL;		//For ID
	int isBool = 0;	
	ExpType expType;

	bool isStatic;
	int ioTree = 0;
	int isArray = 0;
	int size = 1;
	char *location;
	int offset;
	int locationInMemory;
	
	




} TreeNode;


TreeNode * newDeclarationNode(DeclKind, TokenData *);
TreeNode * newStatementNode(StmtKind, TokenData*);
TreeNode * newExpressionNode(ExpKind, TokenData*);

ExpType setNodeType (TokenData*);
OpKind setOperator(TokenData*);
#endif
