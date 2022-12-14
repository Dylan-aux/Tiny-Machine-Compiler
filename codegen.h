#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include "treeUtils.h"
#include "scanType.h"
#include "emitcode.h"

//struct to handle lists of functions present in the input program
typedef struct FuncList{
	char * ID;
	int locationInMemory;
	char * location;
	FuncList * next;
} funcList;

//struct to handle lists of variables present in the input program
typedef struct VarList{
	char * ID;
	int locationInMemory;
	char * location;
	VarList * next;
} varList;


//all generation functions needed to handle emits for each type and subtype
void codeGenGlobal(std::string, void* pointer);
void countSize(TreeNode*);
void codeGenIO(TreeNode*);
void expGen(TreeNode *);
void stmtGen(TreeNode*);
void declGen(TreeNode*);
void codeGen(TreeNode*, TreeNode*);
void setCalls(TreeNode *parseTree, char* funck, int memLoc);
