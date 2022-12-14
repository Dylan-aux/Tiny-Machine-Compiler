#include "symbolTable.h"
#include "treeUtils.h"

//prototypes for all functions defined in semantic.cpp
TreeNode * generateIOTree();
void checkLeaves_AndOr(TreeNode *);
void setOffset(TreeNode * treenode);
char* getID(treeNode *);

char* convExpType(ExpType);
char* findType(TreeNode *);
char* findOp(TreeNode *);
ExpType checkType(TreeNode *);
char* setMem();
void callSemAnalysis(TreeNode *);
void declAnalyze(TreeNode *);
void expAnalyze(TreeNode *);
void stmtAnalyze(TreeNode *);
void semAnalysis(TreeNode *);
