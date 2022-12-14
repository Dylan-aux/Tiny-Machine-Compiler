#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "treeUtils.h" 
#include "symbolTable.h"
#include "semantic.h"
#include "codegen.h"

//offsets needed to track the location in the tree
static int foffset = 0;
int prevfoffset;
int goffset = 0;

//generate a new symbol table for the tree
SymbolTable * st = new SymbolTable(); 
int numErrors = 0;
int numWarnings = 0;
int loopFlag = 0;

//used for debugging.  if set to true will print the current node at all critical points during run
bool print = false;


//calls semAnalysis then checks to ensure that there is a main function in the program if not it throws a linker error
void callSemAnalysis(TreeNode * parseTree)
{
	semAnalysis(parseTree);
	
	if (st -> lookupGlobal("main") == NULL && st -> lookup("main") == NULL)
	{		
		printf("ERROR(LINKER): Procedure main is not declared.\n");
		printf("ERROR(LINKER): Procedure main is not declared.\n");
		numErrors++;
	}
}

	//Offsets are changed when a node changes the scope of the program

	//any errors detected by this check will prevent the compiler from attempting to emit machine code once the analysis is complete

	//need a loop to first check the type of node then check the children for typing and scoping stuffs
	

	//Call symbol table insert for
	//	-functions
	//	-params
	//	-variables
	//
	//Call symbol table enter before children at
	//	-functions
	//	-compounds
	//	-loop with ID
	//
	//Call symbolTable lookup for
	//	-ID's
	//	-calls
	//	-as needed
	//
	//Call symbolTable leave after children/moving sibling
	//	-function
	//	-compound
	//	-loopwith ID
	//

//main function used to analyze each node.  Starts with the current node moving down the tree then to any sibling nodes present until there are no longer
//!Null nodes present in the tree to check
void semAnalysis(TreeNode * parseTree)
{
	while (parseTree != NULL)
	
	{
		switch(parseTree -> nodeKind)
		{
			case DeclK:
					if(print){printNode(parseTree);}	
					declAnalyze(parseTree);
					break;
			case StmtK:
					if(print){printNode(parseTree);}
					stmtAnalyze(parseTree);
					break;
			case ExpK:	
					if(print){printNode(parseTree);}
					expAnalyze(parseTree);
					break;
			default:
					printf("invalid or missing NodeKind\n\n\n");
					break;
		}	
	if(print){printNode(parseTree);}
	
	parseTree = parseTree -> sibling;

	if(print){printNode(parseTree);}
	}


}



//checks the validity of values given in the declK
void declAnalyze(TreeNode * parseTree)
{
	//switch case based on the decl subtype kind.decl
	switch(parseTree -> kind.decl)
	{
		case(VarK):
				//has if statements to handle each type of variable, ie. static, global, local, array and prepares the offset,
				//if any of the values are returned incorrect, error, or NULL then it will print there there is already a present definitiong
				//and will increment the number of errors listed
				if(print){printNode(parseTree);}
				if(st -> lookup(parseTree ->ID) == NULL)
				{
					if(parseTree ->isArray)
					{
						parseTree ->location = setMem();
						if (parseTree -> isStatic)
							parseTree -> location = strdup("Global");
	
						if(!parseTree -> isStatic)
						{
							parseTree -> offset = foffset ;
							foffset = foffset - parseTree -> size ;
						} else
						{
							parseTree -> offset = prevfoffset;
							prevfoffset = prevfoffset - parseTree -> size ;
						}
					} else 
					{
						if(!parseTree -> isStatic)
						{
							parseTree -> offset = foffset ;
							foffset = foffset - parseTree -> size ;
						} else
						{
							parseTree -> offset = prevfoffset;
							prevfoffset = prevfoffset - parseTree -> size;
						}
						parseTree -> location = setMem();
						if (parseTree -> isStatic)
							parseTree -> location = strdup("Global");
					}
		
					st -> insert( parseTree -> ID, parseTree);
				
				} else
				{
					printf("ERROR(%d): Symbol '%s' is already defined at line %d\n", parseTree -> linenum, parseTree -> ID, parseTree -> linenum);
					numErrors++;
				}		
				break;
		case(FuncK):
				//funcK case ensures that each function id is in the symbol table and that each of the child nodes for the funck 
				//node is checked through the semAnalysis function as well
				if(print){printNode(parseTree);}
				if(st -> insert( parseTree -> ID, parseTree))
				{
					st -> enter(parseTree -> ID);
					goffset = 2;
					prevfoffset = foffset;
					foffset = -2;
					

					countSize(parseTree);
					semAnalysis(parseTree -> child[0]);
					semAnalysis(parseTree -> child[1]);
					st -> leave();
					goffset = 0;
					foffset = prevfoffset;
				}
				break;
		case(ParamK):
				//ParanKchecks if each param is in the symbol table , if not then it  checks if it 
				//is an array then inserts it into the symboltable
				if(print){printNode(parseTree);}
				if(st -> lookup(parseTree -> ID) == NULL)
				{
					if(parseTree -> isArray)
					{
						parseTree -> offset = foffset -1;
						foffset = foffset - parseTree -> size ;
					} else 
					{
						parseTree -> offset = foffset;
						foffset--;
					}
					parseTree -> location = strdup("Param");
					st -> insert( parseTree -> ID, parseTree);
				}

				break;
		default:
			printf("In declAnalyze couldn't find declKind****************\n");
				break;
	}


}

//uses the symbol table to analyze statements using the subtype kind.stmt
void stmtAnalyze(TreeNode * parseTree)
{
	switch(parseTree -> kind.stmt)
	{
		case(NullK):
				break;
		case(ElsifK):
				if(print){printNode(parseTree);}
				break;
		case(IfK):
				//ifK checks that the children of the if statement are not nulll and then analyzes the children first
				//if the child is currently in the table an auxillary pointer is made to find the values present in the table
				if(print){printNode(parseTree);}
				ExpType temp;
				semAnalysis(parseTree -> child[0]);
				semAnalysis(parseTree -> child[1]);
				if ( parseTree -> child[2] != NULL)
				{
					semAnalysis(parseTree -> child[2]);
				}
				void* tempVoidPtr;
				if(parseTree -> child[0] -> ID != NULL)
				{
					tempVoidPtr = st -> lookup(parseTree -> child[0] -> ID);
				} else
				{
					tempVoidPtr = st -> lookup(getID(parseTree -> child[0]));
				}	
				if(print){printNode(parseTree);}
				break;
		case(WhileK):
				if(print){printNode(parseTree);}
				semAnalysis(parseTree -> child[0]);
				st -> enter("WhileK");
				loopFlag++ ;
				foffset = -2;	
				void* tempWhileKVoidPtr;
				tempWhileKVoidPtr = st -> lookup(getID(parseTree -> child[0]));
				TreeNode * whileKTemp;
				whileKTemp = static_cast<TreeNode *>(tempWhileKVoidPtr);
				//copy the node information of the child node from the symbol table and cast it onto another node pointer

				if(whileKTemp -> expType != Bool)
				{
					printf("ERROR(%d): Expecting Boolean test in while statment but got type %s.\n", parseTree -> linenum, findType(whileKTemp));
					numErrors++;
				}
				semAnalysis(parseTree -> child[1]);
				if(parseTree -> child[2]) 
				{
				semAnalysis(parseTree -> child[2]);
				}
				loopFlag--;
				st -> leave();
				if(print){printNode(parseTree);}
				break;
		case(IterationK):
				if(print){printNode(parseTree);}
				st -> enter("LoopK");
				loopFlag++;	
				foffset = -2;
				semAnalysis(parseTree -> child[0]);	
				semAnalysis(parseTree -> child[1]);
				semAnalysis(parseTree -> child[2]);
				//Test using the apply to all function to see if all of them are used in the scope
				loopFlag--;
				st -> leave();
				break;
		case(CompoundK):
				if(print){printNode(parseTree);}	
				st -> enter("Compound");
				semAnalysis(parseTree -> child[0]);
				semAnalysis(parseTree -> child[1]);
				st -> leave();				
				break;
		case(ReturnK):
				if(print){printNode(parseTree);}
				if(parseTree -> child[0] != NULL)
				{
					semAnalysis(parseTree -> child[0]);
				}
				break;
		case(BreakK):
				if(!loopFlag)
				{
					printf("ERROR(%d): Cannot have a break statement outside of loop.\n", parseTree -> linenum);
					numErrors++;
				}		
				if(print){printNode(parseTree);}
				break;
		default:
				printf("In stmtAnalyze, could not find the StmtKind************\n");
	}

}
	
void expAnalyze(TreeNode * parseTree)
{
	switch(parseTree -> kind.exp)
	{
		case(OpK):
				//OpK makes sure that if both children exist that they are processed into the symbol table 
				//then casts the info from their ids onto new temp nodes 
				//for each of the possible operators the program checks their children then adds that info to new temp nodes of the
				//operator is binary
				//checks to ensure that unary operators only have one arguement
				if(print){printNode(parseTree);}
				semAnalysis(parseTree -> child[0]);
				if(parseTree -> child[1] != NULL)
				{
				
					semAnalysis(parseTree -> child[1]);
				}
				if(parseTree -> attr.op == plusOp)
				{
					void* temp2 = NULL;
					void* temp1 = NULL;
					TreeNode * node1;
					TreeNode * node2;
					//lookup the id of each child node using the symbol table and copy that information into temporary void pointers that are cast to 
					//treeNode pointers
					if(parseTree -> child[0] != NULL)
					{
						
						temp1 = st->lookup(getID(parseTree -> child[0]));
						node1 = static_cast<TreeNode *>(temp1);
					}
					if(parseTree -> child[1] != NULL)
					{	
						temp2 = st->lookup(getID(parseTree -> child[1]));
						node2 = static_cast<TreeNode *>(temp2);
					}
					if(node1 != NULL)
					{
						//ensures that arrays are not being added together
						if(node1 -> isArray == true)
						{
							printf("ERROR(%d): The operation '+' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						} else {
						if(node2 != NULL && node2 -> isArray == true)
						{
							printf("ERROR(%d): The operation '+' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						}
						}
					}
				}

				if(parseTree -> attr.op == minusOp)
				{
					

						void* temp2 = NULL;
						void* temp1 = NULL;
						TreeNode * node1;
						TreeNode * node2;
					if(parseTree -> child[0] != NULL)
					{
						temp1 = st->lookup(getID(parseTree -> child[0]));
						node1 = static_cast<TreeNode *>(temp1);
					}
					if(parseTree -> child[1] != NULL)
					{	
						temp2 = st->lookup(getID(parseTree -> child[1]));
						node2 = static_cast<TreeNode *>(temp2);
					}
					if(node1 != NULL)
					{

						if(node1 -> isArray == true)
						{
							printf("ERROR(%d): The operation '-' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						} else {
						if(node2 != NULL && node2 -> isArray == true)
						{
							printf("ERROR(%d): The operation '-' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						}
						}
					}
				}
				if(parseTree -> attr.op == andOp)
				{
					

						void* temp2 = NULL;
						void* temp1 = NULL;
						TreeNode * node1;
						TreeNode * node2;
					if(parseTree -> child[0] != NULL)
					{
						temp1 = st->lookup(getID(parseTree -> child[0]));
						node1 = static_cast<TreeNode *>(temp1);
					}
					if(parseTree -> child[1] != NULL)
					{	
						temp2 = st->lookup(getID(parseTree -> child[1]));
						node2 = static_cast<TreeNode *>(temp2);
					}
					if(node1 != NULL)
					{

						if(node1 -> isArray == true)
						{
							printf("ERROR(%d): The operation '&' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						} else {
						if(node2 != NULL && node2 -> isArray == true)
						{
							printf("ERROR(%d): The operation '&' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						}
						}
					}
				}



				//this if statement addressses leftbrackop using the same methodolgy as before excepty that it checks to make sure that
				//it only is attempting to index datatypes that can be made into arrays
				if(parseTree -> attr.op == leftbrackOp)
				{
						void* temp2 = NULL;
						void* temp1 = NULL;
						TreeNode * node1;
						TreeNode * node2;
					if(parseTree -> child[0] != NULL)
					{
						temp1 = st->lookup(getID(parseTree -> child[0]));
						node1 = static_cast<TreeNode *>(temp1);
					}
					if(parseTree -> child[1] != NULL)
					{	
						temp2 = st->lookup(getID(parseTree -> child[1]));
						node2 = static_cast<TreeNode *>(temp2);
					}
					
					if(node1 == NULL || node1 -> isArray == false)
					{
						if(node1 != NULL) {	
						printf("ERROR(%d): Cannot index nonarray '%s'.\n", parseTree -> linenum, node1 -> ID);/						}
						}
						else

						{
						printf("ERROR(%d): Cannot index nonarray '%s'.\n", parseTree -> linenum, parseTree ->child[0] -> ID);
						}
						numErrors++;
					}
					if(node2 == NULL || node2 -> isArray == true)
						if(node2 != NULL) {

	
						printf("ERROR(%d): Cannot index nonarray '%s'.\n", parseTree -> linenum, node2 -> ID);
						}
						else
						{
						printf("ERROR(%d): Cannot index nonarray '%s'.\n", parseTree -> linenum, parseTree ->child[0] -> ID);
						}
						numErrors++;
					}

				if(parseTree -> attr.op == notOp)
				{	

						void* temp2 = NULL;
						void* temp1 = NULL;
						TreeNode * node1;
						TreeNode * node2;
					if(parseTree -> child[0] != NULL)
					{
						temp1 = st->lookup(getID(parseTree -> child[0]));
						node1 = static_cast<TreeNode *>(temp1);
					}
					if(parseTree -> child[1] != NULL)
					{	
						temp2 = st->lookup(getID(parseTree -> child[1]));
						node2 = static_cast<TreeNode *>(temp2);
					}

					
					if(temp2 == NULL)
					{
						if(node1 != NULL && node1 -> isArray == true)
						{
							printf("ERROR(%d): The operation '!' does not work with arrays.\n", parseTree -> linenum);
							numErrors++;
						}


					}

				}
				if(parseTree -> attr.op == queOp)
				{
					void* temp1;
					temp1 = st->lookup(getID(parseTree -> child[0]));
					TreeNode * node1;
					node1 = static_cast<TreeNode *>(temp1);
			
					if(node1 != NULL &&checkType(node1) != Int)
					{
						printf("ERROR(%d): Unary '?' requres an operand of type int but was given type %s.\n", parseTree -> linenum, findType(node1));
						numErrors++;
					}
					if(node1 != NULL && node1 -> isArray == true)
					{
						printf("ERROR(%d): The operation '?' does not work with arrays.\n", parseTree -> linenum);
						numErrors++;
					}
				} 

			if(print){printNode(parseTree);}


/************************************************************************************************************************/
//Testing array indexes for single dimensional arrays				
				
				if(parseTree -> attr.op == leftbrackOp)
				{
					
					if(parseTree -> child[0] -> nodeKind == ExpK && parseTree -> child[0] -> kind.exp == IdK)
					{
						if(parseTree -> child[1] -> nodeKind == ExpK && parseTree -> child[1] -> kind.exp == IdK) 
						{
							if(parseTree -> child[1] -> ID != NULL)
							{
										
								if(st -> lookup(parseTree -> child[1] -> ID) != NULL)	

								{
									void* temp_arrayIndexCheck = st -> lookup(parseTree -> child[1] ->ID);
									TreeNode* node_arrayIndexCheck;
									node_arrayIndexCheck = static_cast<TreeNode *>(temp_arrayIndexCheck);
									if(node_arrayIndexCheck -> expType != Int)
									{	
										printf("ERROR(%d): Array '%s' should be indexed by type int but got type %s.\n", parseTree -> linenum, parseTree -> child[0] -> ID, findType(node_arrayIndexCheck));
										numErrors++;
									}
								}
							}
						}
						if(parseTree -> child[1] ->nodeKind == ExpK && parseTree -> child[1] -> kind.exp == CharconstK)
						{
							printf("ERROR(%d): Array '%s' should be indexed by type int but got type char.\n", parseTree -> linenum, parseTree -> child[0] -> ID);
							numErrors++;
						}
					}
				}	


/**************************************************************************************************************************/
//Testing for the AND and OR operators arguements


				if(parseTree -> attr.op == andOp || parseTree -> attr.op == orOp)
				{
					checkLeaves_AndOr(parseTree);	
				}
				break;
		case(ConstantK):
				if(print){printNode(parseTree);}
				parseTree -> location = strdup("Global");
							parseTree -> offset = foffset ;
							foffset = foffset -1 ;
			
				st -> insert(parseTree -> ID, parseTree);
				break;
		case(IdK):
				if(print){printNode(parseTree);}
				if(st -> lookup( parseTree -> ID) != NULL)
				{
/*********************************************************************/
//Check to see if a fucntion is trying to be used as  a variable
				void* temp;
				temp = st -> lookup(parseTree -> ID);
				TreeNode* node;
				node  = static_cast<TreeNode *>(temp);
				
				if(node -> nodeKind == 0)
				{
					if(node -> kind.decl == 1)
					{
						numErrors++;
						printf("ERROR(%d): Cannot use function '%s' as a variable.\n", parseTree -> linenum, parseTree -> ID);
					}
				} else {
				printf("ERROR(%d): Symbol '%s' is already declared at %d.\n", parseTree -> linenum, node -> ID, node -> linenum);
				}

				if(parseTree -> location == NULL)
							parseTree -> location = node -> location;
				parseTree -> size = node -> size;
				parseTree -> offset = node -> offset;

/**********************************************************************/
				

				


				//do nothing
				} else
				{
					printf("ERROR(%d): Symbol '%s' is not declared.\n", parseTree -> linenum, parseTree -> ID);
					numErrors++;
				}	
				break;
		case(AssignK):	
				//assigns the offset from the treeNode and calls semanalysis on each of the child nodes if they are not NULL
				if(print){printNode(parseTree);}
				parseTree -> offset = foffset ;
				foffset = foffset -1 ;
				semAnalysis(parseTree -> child[0]);
				if(parseTree -> child[1] != NULL)
				{
					semAnalysis(parseTree -> child[1]);
				}
				parseTree -> offset = foffset ;
				foffset = foffset -1 ;
				break;
		case(InitK):
				//calls semAnalysis on child nodes
				if(parseTree -> child[0] != NULL)
				{
					semAnalysis(parseTree -> child[0]);
				}
				if(print){printNode(parseTree);}	
				break;
		case(CallK):
				//changes the offset based on the current scope and ensures that each of the children/ids are declared
				if(print){printNode(parseTree);} 					
				parseTree -> offset = foffset ;
				foffset = foffset - 1 ;
				if(st -> lookup(parseTree ->ID) == NULL)
				{
					printf("ERROR(%d): Function '%s' is is not declared\n", parseTree -> linenum, parseTree -> ID, parseTree -> linenum);
					numErrors++;
				}		
				if(!strcmp(parseTree -> ID, "output"))
				{
					parseTree -> locationInMemory = 16;
				} else if(!strcmp(parseTree -> ID, "outputb"))
				{
					parseTree -> locationInMemory = 22;
				} else if(!strcmp(parseTree -> ID, "outputc"))
				{
					parseTree -> locationInMemory = 28;
				} else if(!strcmp(parseTree -> ID, "outnl"))
				{
					parseTree -> locationInMemory = 34;
				}
					
				else 
				{	
					void* temp1;
					temp1 = st->lookup(parseTree -> ID);
					TreeNode * node1;
					node1 = static_cast<TreeNode *>(temp1);
					if(node1 == NULL) printf("shit");	
					
					
					parseTree -> locationInMemory = node1 -> offset;
				} 
				semAnalysis(parseTree -> child[0]);
				if(print){printNode(parseTree);}
				break;
		case(BoolconstK):
				parseTree -> location = strdup("Global");
				parseTree -> offset = foffset ;
				foffset = foffset - 1 ;
				if(st -> lookup(parseTree -> ID))
					st -> insert(parseTree -> ID, parseTree);
				if(print){printNode(parseTree);}	
				break;
		case(CharconstK):
				if(!st -> lookup(parseTree -> ID))
				st -> insert(parseTree -> ID, parseTree);
				parseTree -> location = strdup("Global");
				parseTree -> offset = prevfoffset;
				prevfoffset = prevfoffset - 1;
				break;
}
//Find and set the output char* for the operator 
char* findOp(TreeNode * treeNode)
{
	char* printKind;
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
			printKind = strdup("-");
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
			printKind = strdup("************couldn't find the operator************");
			break;
		}
	return printKind;
}



//Find and set the expType Char* for the output
char* findType(TreeNode * treeNode)
{
	char * type;
	switch(treeNode -> expType)
	{
		case Void:
			type = strdup("void");
			break;
		case Int:
			type = strdup("int");
			break;	
		case Bool:
			type = strdup("bool");
			break;
		case Char:
			type = strdup("char");
			break;
		default:
			type = strdup("**********Couldn't find the expType*******************");
			break;
	}
	return type;
}

void checkLeaves_AndOr(TreeNode * parseTree)
{
	if(parseTree == NULL)
		return;

	checkLeaves_AndOr(parseTree -> child[0]);
	checkLeaves_AndOr(parseTree -> child[1]);
	if(parseTree -> kind.exp == CharconstK || parseTree -> kind.exp == ConstantK)
	{
		st -> insert(parseTree -> ID, parseTree);
	} else if(parseTree -> kind.exp == IdK)
	{
		if(st -> insert(parseTree -> ID, parseTree))
		{
		} else {	
			if( parseTree -> child[1] == NULL && parseTree -> child[0] == NULL)
			{
				void* temp;
				temp = st -> lookup(parseTree -> ID);
				TreeNode* node;
				node = static_cast<TreeNode *>(temp);
				if(node -> expType != Bool)
				{	
					printf("ERROR(%d): '&' requires operands of type bool but rhs is of type %s.\n", parseTree -> linenum, findType(node));
					numErrors++;
				}	

	
			}
		}
	}
}



//returns node id
char * getID(TreeNode * parseTree)
{
	while(parseTree -> ID == NULL )
	{
		if(print){printNode(parseTree);}	
		parseTree = parseTree -> child[0];
	}
	return parseTree -> ID;
}

//returns node ExpType
ExpType checkType(TreeNode * parseTree)
{
	while(parseTree -> expType == Void)
	{	
		if(parseTree -> child[0] != NULL)
		{
			parseTree = parseTree -> child[0];
		}
		else
		{
			break;
		}
	}
	return parseTree -> expType;
}

//converts enumerated exptype to char*
char* convExpType(ExpType expType)
{
	char * type;
	switch(expType)
	{
		case Void:
			type = strdup("void");
			break;
		case Int:
			type = strdup("int");
			break;	
		case Bool:
			type = strdup("bool");
			break;
		case Char:
			type = strdup("char");
			break;
		default:
			type = strdup("**********Couldn't find the expType*******************");
			break;
	}
	return type;	
}

//returns string representing the current scope based on the goffset
char * setMem()
{
	switch (goffset)
	{
		case 0:
			return strdup("Global");
		case 1:
			return strdup("Param");
		case 2:
			return strdup("Local");
		case 3:
			return strdup("Temp");
		case 4:
			return strdup("Static");
	}
}

//function to generate and ioTree then return a pointer to the head node of the ioTree
TreeNode * generateIOTree()
{
	
	TokenData *dummy;
	TreeNode *input;
	TreeNode *output;
	TreeNode *inputb;
	TreeNode *outputb;
	TreeNode *inputc;
	TreeNode *outputc;
	TreeNode *outnl;
	TreeNode *idummy;
	TreeNode *bdummy;
	TreeNode *cdummy;

	dummy = (TokenData *)malloc(sizeof(TokenData));
	dummy -> lineNum = -1;

	input = newDeclarationNode(FuncK, dummy);
	input -> expType = Int;
	input -> ioTree = 1;
	input -> attr.cvalue = strdup("input");
	input -> ioType = Input;

	inputb = newDeclarationNode(FuncK, dummy);
	inputb -> expType = Bool;
	inputb -> ioTree = 1;
	inputb -> ioType = Inputb;
	inputb -> attr.cvalue = strdup("inputb");
	input -> sibling = inputb;

	inputc = newDeclarationNode(FuncK, dummy);
	inputc -> expType = Char;
	inputc -> ioTree = 1;
	inputc -> ioType = Inputc;
	inputc -> attr.cvalue = strdup("inputc");
	inputb -> sibling = inputc;

	

	
	output = newDeclarationNode(FuncK, dummy);
	output -> expType = Void;
	output -> ioTree = 1;
	output -> ioType = Output;
	output -> attr.cvalue = strdup("output");
	idummy = newDeclarationNode(ParamK, dummy);
	idummy -> expType = Int;
	idummy -> ioTree = 1;
	idummy -> ioType = iDummy;
	idummy -> attr.cvalue = strdup("*dummy*");
	output -> child[0] = idummy;
	inputc -> sibling = output;

	outputb = newDeclarationNode(FuncK, dummy);
	outputb -> expType = Void;
	outputb -> ioTree = 1;
	outputb -> ioType = Outputb;
	outputb -> attr.cvalue = strdup("outputb");
	bdummy = newDeclarationNode(ParamK, dummy);
	bdummy -> expType = Bool;
	bdummy -> ioTree = 1;
	bdummy -> ioType = bDummy;
	bdummy -> attr.cvalue = strdup("*dummy*");
	outputb -> child[0] = bdummy;
	output -> sibling = outputb;

	outputc = newDeclarationNode(FuncK, dummy);
	outputc -> expType = Void;
	outputc -> ioTree = 1;
	outputc -> ioType = Outputc;
	outputc -> attr.cvalue = strdup("outputc");
	cdummy = newDeclarationNode(ParamK,dummy);
	cdummy -> expType = Char;
	cdummy -> ioTree = 1;
	cdummy -> ioType = cDummy;
	cdummy -> attr.cvalue = strdup("*dummy*");
	outputc -> child[0] = cdummy;
	outputb -> sibling = outputc;

	outnl - newDeclarationNode(FuncK, dummy);
	outnl -> expType = Void;
	outnl -> ioTree = 1;
	outnl -> ioType = Outnl;
	outnl -> attr.cvalue = strdup("outnl");
	outputc -> sibling = outnl;
	
	return input;
}




				
