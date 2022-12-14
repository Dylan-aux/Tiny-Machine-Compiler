#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "codegen.h"
#include "semantic.h"

extern int litLoc;
extern int emitLoc;
extern int goffset;
int Main = 0;
extern SymbolTable * st; 
funcList * functions = (funcList*)malloc(sizeof(funcList));
funcList *variables = (funcList*)malloc(sizeof(funcList));
varList * variables = (varList*)malloc(sizeof(varList));
int tOffset = 0;

void codeGenIO(TreeNode * ioTree)
{
	while(ioTree != NULL && ioTree -> attr.cvalue != NULL)
	{
		emitComment((char *)" ");
		emitComment( (char*) "** ** ** ** ** ** ** ** ** ** ** **");
		emitComment( (char*) "FUNCTION", ioTree -> attr.cvalue);
	
		ioTree -> offset = emitSkip(0);
	
		emitRM( (char*)"ST", AC, -1, FP, (char *)"Store return address");
	

		switch(ioTree -> ioType)
		{
			case Input:
				emitRO((char*)"IN", RT, RT, RT, (char*)"Grab int input");
				break;
			case Output:
				emitRM((char*)"LD", AC, -2, FP, (char*)"Load parameter");
				emitRO((char*)"OUT", AC, AC, AC, (char*)"Output integer");
//				emitRM((char*)"LDC", RT, 0, 6, (char*)"Set return to 0");
//LDC removed from current version of Tiny Machine but if/when the functionality is reintroduced this is where/how it should be implemented in output
				break;
			case Inputb:
				emitRO((char*)"INB", RT, RT, RT, (char*)"Grab bool input");
				break;
			case Outputb:
				emitRM((char*)"LD", AC, -2, FP, (char*)"Load parameter");
				emitRO((char*)"OUTB", AC, AC, AC, (char*)"Output bool");
//				emitRM((char*)"LDC", RT, 0, 6, (char*)"Set return to 0");
				break;
			case Inputc:
				emitRO((char*)"INC",RT, RT, RT, (char*)"Grab char input");
				break;
			case Outputc:
				emitRM((char*)"LD", AC, -2, FP, (char*)"Load parameter");
				emitRO((char*)"OUTC", AC, AC, AC, (char*)"Output char");
//				emitRM((char*)"LDC", RT, 0, 6, (char*)"Set return to 0");
				break;
			case Outnl:
				emitRO((char*)"OUTNL", AC, AC, AC, (char*)"Output a new line");
				break;
			default:
				emitComment((char*)"ERROR in ioTree type values. ioType not recognized");
				break;
		}
	emitRM((char*)"LD", AC, -1, FP, (char*)"Load return address");
	emitRM((char*)"LD", FP, 0, FP, (char*)"Adjust fp");
	emitGoto(0, AC, (char*)"Return");
	emitComment((char*)"END FUNCTION", ioTree -> attr.cvalue);
	ioTree = ioTree -> sibling;
	
	}

		emitComment( (char*) "** ** ** ** ** ** ** ** ** ** ** **");
	
}


//This loop fed the head node pointer of the parseTree and is used sent each node through the output based on its typing
//starting with its main type then sent to another function that will break it down based on the subtype
//these subtype functions are: declGen(parseTree), stmtGen(parseTree), and expGen(parseTree) which are subsequently defined

void mainLoop(TreeNode *parseTree)
{
//used to jump  from node to node down the tree so that all functions and variables can be properly defined in output
	while (parseTree != NULL)
	
	{
		switch(parseTree -> nodeKind)
		{
			case DeclK:
					declGen(parseTree);
					break;
			case StmtK:
					stmtGen(parseTree);
					break;
			case ExpK:	
					expGen(parseTree);
					break;
			default:
					printf("Incorrect nodeKind \n\n\n");
					break;
		}	
	parseTree = parseTree -> sibling;

	

	}

}

//takes a specific parseTree  and the corresponding ioTree for a given program
void codeGen(TreeNode * parseTree, TreeNode * ioTree)
{

	int init;
	init = emitSkip(1);
	codeGenIO(ioTree);
	
	
	mainLoop(parseTree);

	//used to allow for declaration then later definition of a function or variable, notates the location in memory
	backPatchAJumpToHere((char*)"JMP", PC, init, (char*)"Jump to init [backpatch]");


	//function generates the ioTree output as well as anything set at the global scope 
	emitComment((char*)"INIT");
	emitRM((char*)"LD", 0,0,0, (char*)"Set the global pointer");
	emitRM((char*)"LDA", FP, goffset, GP, (char*)"set first frame at end of globals");
	emitRM((char*)"ST", FP, 0, FP, (char*)"store old fp (point to self)");


	emitComment((char*)"INIT GLOBALS AND STATICS");
	
	st -> applyToAllGlobal(codeGenGlobal);


	emitComment((char*)"END INIT GLOBALS AND STATICS");


	emitRM((char*)"LDA", AC, 1, PC, (char*)"Return address in ac");
	emitRMAbs((char*)"JMP", PC, Main, (char*)"Jump to main");
	emitRO((char*)"HALT", 0,0,0, (char*)"DONE!"); 

	emitComment((char*)"END INIT");

}


//emit functions for the various declKind subtypes 
//these cases are handled in a switch statement that corresponds to each possible declKind subtype 
//each case requires unit emit functions for the machine code of the Tiny Machine

void declGen(TreeNode * parseTree)
{
	switch(parseTree -> kind.decl)
	{
		case(VarK):
		if(parseTree -> isArray == 1)
		{
				emitRM((char*)"LDC", AC, parseTree -> size, AC, (char*)"Load size of array", parseTree -> ID);
				emitRM((char*)"ST", AC, parseTree -> offset , GP, (char*)"Save size of array", parseTree -> ID);
				tOffset += parseTree -> offset;
		} else if(parseTree -> child[0] != NULL)
		{
			tOffset--;
			mainLoop(parseTree -> child[0]);
			emitRM((char*)"ST", AC, parseTree -> offset--, FP, (char*)"Store global variable", parseTree -> ID);
		} else
		{
			tOffset--;
		}
			


				break;
		case(FuncK):
		
				countSize(parseTree);
				tOffset = -1* (parseTree -> size);

	
				emitComment((char*)"FUNCTION", parseTree -> ID);
				if(!strcmp(parseTree ->ID, "main")) Main = emitSkip(0);
				parseTree -> locationInMemory = emitSkip(0);
				emitRM((char*)"ST", AC, -1, FP, (char*)"Store return address");
				mainLoop(parseTree -> child[0]);
				mainLoop(parseTree -> child[1]);

				emitComment((char*)"Add standard closing in case there is no return statement");
				emitRM((char*)"LDC", RT, 0, AC3, (char*)"Set return value to 0");
				emitRM((char*)"LD", AC, -1, FP, (char*)"Load return address");
				emitRM((char*)"LD",FP, 0, FP, (char*)"Adjust fp");
				emitRM((char*)"LDA", PC, 0, AC, (char*)"Return");
			

				emitComment((char*)"END FUNCTION", parseTree -> ID);
			
				if(functions -> ID == NULL)
				{
					//sets a NULL funcID to the current parsetreeNode function id and uses this ID to find its location in memory if only a declaration has been made
					functions -> ID = parseTree -> ID;
					functions -> locationInMemory = parseTree -> locationInMemory;	
				} else 
				{
					//creates a list of functions if there are valid function IDs to allow for the prosessing of each location in memory corresponding
					//to the declaration and definition locations of each of the present funcions
					funcList * p = (funcList *)malloc(sizeof(funcList));
					p = functions;
					funcList * t = (funcList *)malloc(sizeof(funcList));
					t -> ID = strdup(parseTree -> ID);
					t -> locationInMemory = parseTree -> locationInMemory;
					while(p -> next != NULL)
					{
						p = p -> next;
					}
					p -> next = t;	
				}	
				
				break;
		case(ParamK):
				if(!strcmp(parseTree -> location, "Local"))
				{
					emitRM((char*)"ST", AC, parseTree -> offset, FP, (char*)"Store variable", parseTree -> ID);
				}

				break;
		default:
			printf("*****************In declAnalyze couldn't find valid declKind****************\n");
				break;
	}


}

//emit functions for the various stmtKind subtypes 
//these cases are handled in a switch statement that corresponds to each possible declKind subtype 
//each case requires unit emit functions for the machine code of the Tiny Machine
void stmtGen(TreeNode * parseTree)
{
	int temp1, temp2, temp3;
	//Loc variables are relevant locations in memory
	//currLoc is the current location, breakLoc is the break point location, and skipLoc is the skip location for a given statement
	int currLoc, breakLoc, skipLoc;


	switch(parseTree -> kind.stmt)
	{
		case(NullK):
			//does nothing but does not produce and error as NullK is a valid statement in the Tiny Machine language
				break;
		case(IfK):
				emitComment((char*)"IF");
				//first if handles if there are three children for the ifK node
				//ie. if, then, elif statements (similar to if -> then -> else statements in c)
				if(parseTree -> child[2] != NULL)
				{
					mainLoop(parseTree -> child[0]);
					temp1 = emitSkip(1);
					emitComment((char*)"THEN");
					mainLoop(parseTree -> child[1]);
					emitBackup(temp1);
					emitRMAbs((char*)"JZR", AC, temp1+1, (char*)"Jump around the THEN if false [backpatch]");
					emitLoc = temp1+3;
					
					emitComment((char*)"ELSE");
					temp1 = emitSkip(1);
					mainLoop(parseTree ->child[2]);
					emitBackup(temp1);
					emitRMAbs((char*)"LDA", PC, temp1 +3, (char*)"Jump around the ELSE [backpatch]");
					emitLoc = temp1+3;
				}				
				//handles the case that there is no elif 
				else if(parseTree -> child[1] != NULL)
				{
					mainLoop(parseTree -> child[0]);
					temp1 = emitSkip(1);
					emitComment((char*)"THEN");
					mainLoop(parseTree -> child[1]);
					emitBackup(temp1);
					emitRMAbs((char*)"JZR", AC, temp1, (char*)"Jump around the THEN if false [backpatch]");
					emitLoc = temp1+3;
				}

				emitComment((char*)"END IF");
				break;
		case(WhileK):
				
				emitComment((char*)"WHILE");
				//currLoc set to the current memory location
				currLoc = emitSkip(0); 
				expGen(parseTree -> child[0]);
				
				emitRM((char*)"JNZ", AC, 1, PC, (char*)"Jump to while part");
				emitComment((char*)"DO");
				
				//skipLoc becomes the old breakLoc and the breakLoc becomes the next memory location to denote where
				//the program skipped from and to where it needs to return in memory
				skipLoc = breakLoc;
				breakLoc = emitSkip(1);
				//runs the mainLoop function for the children of the current node
				//ie the new scope of the while statement
				mainLoop(parseTree -> child[1]);
				emitGotoAbs(currLoc, (char*)"go to beginning of loop");
				backPatchAJumpToHere(breakLoc, (char*)"Jump past loop [backpatch]");
				breakLoc = skipLoc;
				emitComment((char*)"END WHILE");
				

				break;
		case(IterationK):
		//IterationK calles the mainLoop and each of the child nodes of the IterationK node
				mainLoop(parseTree -> child[0]);	
				mainLoop(parseTree -> child[1]);
				mainLoop(parseTree -> child[2]);
				break;
		case(CompoundK):
		//CompoundK works similarly to IterationK except that it also tracks the offset locations of the compound
				emitComment((char*)"COMPOUND");
				parseTree -> size = tOffset;
				mainLoop(parseTree -> child[0]);
				emitComment((char*)"Compound Body");
				mainLoop(parseTree -> child[1]);

				tOffset = parseTree -> size;
				//Offset set to the size of the current compoundK node and its subsequent code
				emitComment((char*)"END COMPOUND");
				break;
		case(ReturnK):
		//calls mainLoop() on the child of the return and loads the necassary memory locations for end of program or current scope
				emitComment((char*)"RETURN");
				if(parseTree -> child[0] != NULL)
				{
					mainLoop(parseTree -> child[0]);
				}
				emitRM((char*)"LD", AC, -1, FP, (char*)"Load return address");
				emitRM((char*)"LD", FP, 0, FP, (char*)"Adjust fp");
				emitRM((char*)"JMP", PC, 0, AC, (char*)"Return");
				


				break;
		case(BreakK):
		//uses breakLoc to find the memory location of the break
				emitGotoAbs(breakLoc, (char*)"break me!");
				break;
		default:
				printf("In stmtAnalyze, could not find valid StmtKind************\n");
	}

}

//emit functions for the various expKind subtypes 
//these cases are handled in a switch statement that corresponds to each possible declKind subtype 
//each case requires unit emit functions for the machine code of the Tiny Machine	
void expGen(TreeNode * parseTree)
{
	
	funcList * p = (funcList *)malloc(sizeof(funcList));
	funcList * q = (funcList *)malloc(sizeof(funcList));
	//create two function lists for expressions at the current and subsequent treeNodes

	switch(parseTree -> kind.exp)
	{
		case(OpK):
		//OpK runs the mainLoop on the first child node to ensure that variables and values present in the lower nodes are loaded into memory first
		// then runs 2 initial checks to emit the first Offsets needed by the Tiny Machine for any subsequent values
		//it then uses a switch based on the type of operator present in the node
		//This switch case starts with binary operators then checks for unary operators
				mainLoop(parseTree -> child[0]);
				
				if(parseTree -> attr.op == mulOp && parseTree -> value ==1)
				{
					int placeholderx;
				}
				else 
				{
				emitRM((char*)"ST", AC, tOffset--, FP, (char*)"OpK 1");
				}
				if(parseTree -> child[1] != NULL)
				{
			
					mainLoop(parseTree -> child[1]);
				}
				emitRM((char*)"LD", AC1, ++tOffset, FP, (char*)"Load address of base of array", parseTree -> ID);
				

				//switch case to analyze the operator for the current node
				switch(parseTree -> attr.op)
				{
					case plusOp:
							//simple adder
							emitRO((char*)"ADD", AC, AC1, AC, (char*)"Op +");
							break;
					case minusOp:
							//similar to the adder except that is also checks if there is a child
							//if a child node is present then it subtracts else it changes the sign of the indicated value
							//from either positive to negative or negative to positive
							if(parseTree -> child[1] == NULL)
							{
								emitRM((char*)"LDC", AC1, 0, AC1, (char*)"");
								emitRO((char*)"SUB", AC, AC1, AC, (char*)"Op -");
		
							} else {
							emitRO((char*)"SUB", AC, AC1, AC, (char*)"Op -");
							}

							break;
					case mulOp:
							//mulOp runs several checks to determine of the variable for muiltiplication is local or global, if local
							//the value is either stored as a param or a child which is checked then the value is pointed to in memory
							//otherwise the global value is taken
							if(parseTree -> child[0] != NULL)
							{
								if(!strcmp(parseTree -> child[0] -> location, "Local"))
								{
									if(parseTree -> child[0] ->  kind.decl == ParamK)
									{
										emitRM((char*)"LD", AC3, parseTree -> child[0] ->offset, FP, (char*)"Idkcase param node array");
									} else
									{
										emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, FP, (char*)"IDKcase local node array");
									}
								} else
								if(!strcmp(parseTree -> child[0] -> location, "Global"))
								{
									emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, GP, (char*)"IDKcase global node array");
								}
								emitRM("LD", AC, 1, AC, (char*)"Load array size");
							} else {

							emitRO((char*)"MUL", AC, AC1, AC, (char*)"Op *");
							}
							break;
					case divOp:
					//simple division operator
							emitRO((char*)"DIV", AC, AC1, AC, (char*)"Op /");
							break;
					case eeqOp:
					//simple comparasin operator
							emitRO((char*)"TEQ", AC, AC1, AC, (char*)"Op ==");	
							break;
					case andOp:
					//and operator
							emitRO((char*)"AND", AC, AC1, AC, (char*)"Op AND");
							break;
					case orOp:
					//or operator
							emitRO((char*)"OR", AC, AC1, AC, (char*)"Op OR");
							break;
					case noteqOp:
					//!=
							emitRO((char*)"TNE", AC, AC1, AC, (char*)"Op !=");
							break;
					case lesseqOp:
					//<=
							emitRO((char*)"TLE", AC, AC1, AC, (char*)"Op <=");
							break;
					case lessOp:
					//<
							emitRO((char*)"TLT", AC, AC1, AC, (char*)"Op < ");
							break;
					case greateqOp:
					//>=
							emitRO((char*)"TGE", AC, AC1, AC, (char*)"Op >=");
							break;
					case greatOp:
					//>
							emitRO((char*)"TGT", AC, AC1, AC, (char*)"Op >");
							break;
					case percOp:
					//%
							emitRO((char*)"DIV", AC2, AC1, AC, (char*)"Op %");
							emitRO((char*)"MUL", AC2, AC, AC2, (char*)"");
							emitRO((char*)"SUB", AC, AC1, AC2, (char*)"");
							break;

					//Unary section of operators of the switch case
					case notOp:
					//notOp ensures values in the child not are present then Xors 
							mainLoop(parseTree -> child[0]);
							emitRM((char*)"LDC", AC1, 1,AC1, (char*)"");
							emitRO((char*)"XOR", AC, AC, AC1, (char*)"Op |");
							break;
					
					case leftbrackOp:
					//[
					//leftbrackOp checks for the presence of valid child node, handling the case in which there is no child node
					//then checks for the presence of an array and whether the values are locally or globally declared
					//this is then used to find the offset and address in memory for the values
						if(parseTree -> child[0] != NULL)
						{
							expGen(parseTree -> child[0]);
							emitRO((char*)"SUB", AC3, AC3, AC, (char*)"Compute offset of value");
							emitRM((char*)"LD", AC, 0, AC3, (char*)"Load array element");
						}  else
						if(parseTree -> isArray == 1)
						{
							if(!strcmp(parseTree -> child[0] -> location, "Local"))
							{
								if(parseTree -> child[0] -> kind.decl == ParamK)
								{
									emitRM((char*)"LD", AC3, parseTree -> child[0] -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
								} else 
								{
									emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
								}
							} else
							if(!strcmp(parseTree -> child[0] -> location, "Global"))
							{
									emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, GP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
							}
						} else {
							
							if(!strcmp(parseTree -> location, "Global"))
							{
								emitRM((char*)"LD", AC, parseTree -> offset, GP, (char*)"load from lbrack");
							} else if(!strcmp(parseTree -> location, "Local"))
							{
								emitRM((char*)"LD", AC, parseTree -> offset, FP, (char*)"Load from lbrack");
							}			
		
							}
							break;
					default:
							printf("*************************no valid values in OpK\n\n\n\n********\n\n");
							emitComment((char*)"Broke in switch case for opK");
				}
				break;
		case(AssignK):
		//AssignK first checks to if there is and IDk (ID node) present in the assignment and makes sure that the child node of the id
		//has a valid ID then calls expGen on the child ->child so that the IDs are loaded into memory
		//it then checks locations and if it is an operator type that will require the children to be loaded into memory first
		//and uses a switch case to emit the proper function to the Tiny Machine based on the type of assignment 
				emitComment("EXPRESSION AssignK")
				if(parseTree -> child[0] -> kind.exp == IdK && parseTree -> child[0] -> child[0] != NULL)
				{
					expGen(parseTree -> child[0] -> child[0]);
					emitRM((char*)"ST", AC, tOffset--, FP, (char*)"Store Array Index");
				}

				if(parseTree -> attr.op == eqOp)
				{
					expGen(parseTree -> child[1]);
				}
				if(parseTree -> attr.op == incOp || parseTree -> attr.op == decOp)
				{
					mainLoop(parseTree -> child[0]);
				}		
				if(!strcmp(parseTree -> child[0] -> location, "Global"))
				{
					emitRM((char*)"ST", AC, parseTree -> child[0] -> offset, GP, (char*)"Store variable", parseTree -> child[0] ->ID);
				
				} else 

				{
					emitRM((char*)"ST", AC, parseTree -> child[0] -> offset, FP, (char*)"Store variable", parseTree -> child[0] ->ID);
				}
				
				switch(parseTree -> attr.op)
				{
					case eqOp:
					//=
						break;
					case incOp:
					//++
							emitRM((char*)"LDA", AC, 1, AC, (char*)"NNNEEEEDS CAHVNE Increment value of");
							break;
					case decOp:	
					//--
							emitRM((char*)"LDA", AC, -1, AC, (char*)"NNNEEEEDS CAHVNE Decrement value of");
							break;
					case addassOp:
					//+=
							emitRO((char*)"ADD", AC, AC, AC1, (char*)"Op +=");
							break;
					case subassOp:
					//-=
							emitRO((char*)"SUB", AC, AC, AC1, (char*)"Op -=");	
							break;
					case mulassOp:
					//*=
							emitRO((char*)"MUL", AC, AC, AC1, (char*)"Op *=");
							break;
					case divassOp:
					// /=
							emitRO((char*)"DIV", AC, AC, AC1, (char*)"Op /=");
							break;
					default:
							printf("********Invalid ASSIGNK node***********\n");
							emitComment((char*)"Broke on the assignK switrch");
							break;
				}
		
				if(parseTree -> child[0] -> kind.exp == IdK && parseTree -> child[0] -> child[0] != NULL)
				{
					emitRM((char*)"LD", AC1, tOffset + 1, FP, (char*)"Array indexi");
					if(!strcmp(parseTree -> child[0] -> location, "Local"))
					{
						if(parseTree -> child[0] -> kind.decl == ParamK)
						{
							emitRM((char*)"LD", AC3, parseTree -> child[0] -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
						} else 
						{
							emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
						}
					} else
					if(!strcmp(parseTree -> child[0] -> location, "Global"))
					{
							emitRM((char*)"LDA", AC3, parseTree -> child[0] -> offset, GP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
					}
				
					emitRO((char*)"SUB", AC3, AC3, AC1, (char*)"Compute offset of value");		
					emitRM((char*)"ST", AC, 0, AC3, (char*)"Store variable", parseTree -> child[0] -> ID);
				}		
				break;
		case(InitK):
		//InitK calls mainloop on the children of the current node as long as there is a valid child node
				if(parseTree -> child[0] != NULL)
				{
					mainLoop(parseTree -> child[0]);
				}
				break;
		case(CallK):
		//CallK checks the parameters of the call and assigns offsets for the Tiny Machine memory addresses
		//it then checks for the presence of parameters in the child node and copies the node and sends it into mainloop
		// then loads up the corresponding memory locations using the initial offsets
				emitComment((char*)"EXPRESSION");
				emitComment((char*)"CALL", parseTree -> ID);
					
				emitRM((char*)"ST", FP, tOffset, FP, (char*)"Store fp in ghost fram for output");

				int prevTOffset;
				prevTOffset = tOffset;
				tOffset = tOffset -2;
	
				if(parseTree -> child[0] != NULL)
				{	
					TreeNode * temp = (TreeNode*)malloc(sizeof(TreeNode));
					temp = parseTree -> child[0];					

					while(temp != NULL)
					{
						emitComment((char*)"Param");
						mainLoop(temp);
						emitRM((char*)"ST", AC, tOffset, FP, (char*)"Push parameter");
						char* x = parseTree -> ID;
						emitComment((char*)"Param end", x);
						temp = temp -> sibling;
						tOffset--;
					}
						
				}  
					
				emitComment((char*)"Param end", parseTree -> ID); 
				emitRM((char*)"LDA", FP, prevTOffset, FP, (char*)"Load address of new frame");
				emitRM((char*)"LDA", AC, 1, PC, (char*)"Return address in AC");
		
				p = functions;

				while(p != NULL && p -> ID != NULL)
				{
					if(!strcmp(parseTree -> ID, "This"))
					{
						parseTree -> locationInMemory = p -> locationInMemory;
					}
					p = p-> next;
				}
				
			
		
				emitRMAbs((char*)"JMP", PC, parseTree -> locationInMemory, (char*)"CALL", parseTree -> ID);
				emitRM((char*)"LDA", AC, 0, RT, (char*)"Save the result in ac");
				emitComment((char*)"Call end", parseTree -> ID);

				tOffset = prevTOffset;

				break;
		case(BoolconstK):
		//Boolean constant
				emitComment((char*)"EXPRESSION");
				emitRM((char*)"LDC",AC, parseTree ->attr.boolvalue , AC3, (char*)"Load Boolean constant");
				break;

		case(CharconstK):
		//Character Constant
				emitComment((char*)"EXPRESSION");
				char x;
				x  = parseTree -> attr.cvalue[1];
				int temp;
				temp = (int)x;	
				emitRM((char*)"LDC", AC, x, AC3, (char*)"Load char constant");		


				break;
		case(ConstantK):
		//numeric constant
				emitComment((char*)"EXPRESSION");
				if(parseTree -> isBool == 1)
				{
				emitRM((char*)"LDC",AC, parseTree ->attr.boolvalue , AC3, (char*)"Load Boolean constant");
				} else {
				emitRM((char*)"LDC",AC, parseTree ->value , AC3, (char*)"Load integer constant");
				}
				break; 

		case(IdK):
		//IdK case first checks that the children are not null and checks whether or not the id
		//in the node is associated with an array, if so, it will send the memory location of the parameter
		//then it will check for local and global assignments and load the corresponding values from memory
				if(parseTree -> child[0] != NULL)
				{
					expGen(parseTree -> child[0]);

					if(!strcmp(parseTree -> location, "Local"))
					{
						if(parseTree -> kind.decl == ParamK)
						{
							emitRM((char*)"LD", AC3, parseTree -> offset, FP, (char*)"Idkcase param node array");
						} else
						{
							emitRM((char*)"LDA", AC3, parseTree -> offset, FP, (char*)"IDKcase local node array");
						}
					} else
					if(!strcmp(parseTree -> location, "Global"))
					{
						emitRM((char*)"LDA", AC3, parseTree -> offset, GP, (char*)"IDKcase global node array");
					}




					emitRO((char*)"SUB", AC3, AC3, AC, (char*)"Compute offset of value");
					emitRM((char*)"LD", AC, 0, AC3, (char*)"Load array element");
				}  else
				if(parseTree -> isArray == 1)
				{
					if(!strcmp(parseTree -> location, "Local"))
					{
						if(parseTree -> kind.decl == ParamK)
						{
							emitRM((char*)"LD", AC3, parseTree -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
						} else 
						{
							emitRM((char*)"LDA", AC3, parseTree -> offset, FP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
						}
					} else
					if(!strcmp(parseTree -> child[0] -> location, "Global"))
					{
							emitRM((char*)"LDA", AC3, parseTree -> offset, GP, (char*)"load address of base of array", parseTree -> child[0] -> ID);
					}
				} 	
				if(!strcmp(parseTree -> location, "Global"))
				{
					emitRM((char*)"LD", AC, parseTree -> offset, GP, (char*)"load from IDK NODE");
				} else if(!strcmp(parseTree -> location, "Local"))
				{
					emitRM((char*)"LD", AC, parseTree -> offset, FP, (char*)"Load from IDk node");
				}
				break;	

	}

}

//codeGenGlobal loads the global variables and IDs of the given tree
void codeGenGlobal(std::string str1, void* pointer)
{
	TreeNode * aux = (TreeNode*)malloc(sizeof(TreeNode));
	aux = static_cast<TreeNode *>(pointer);

	if(aux == NULL || aux -> ioTree == 1|| aux -> kind.decl == FuncK)
	{
		return;
	}
	if(aux -> isArray == 1)
	{
		if(!strcmp(aux -> location, "Global"))
		{
			emitRM((char*)"LDC", AC, aux -> size, AC, (char*)"Load size of global array", aux -> ID);
			emitRM((char*)"ST", AC, aux -> offset + 1, GP, (char*)"Save size of global array", aux -> ID);
		}
	} else if(aux -> child[0] != NULL)
	{
		mainLoop(aux -> child[0]);
		emitRM((char*)"ST", AC, aux -> offset, GP, (char*)"Store global variable", aux -> ID);
	}
}

	
	

//calculates the tree size of the current node
void countSize(TreeNode* parseTree)
{
	int temp = 0;
	TreeNode * p = (TreeNode*)malloc(sizeof(TreeNode));
	p = parseTree;
	if(p -> child[0] != NULL) 
	{
		p = p -> child[0];
		temp++;
		while(p -> sibling != NULL)
		{
			temp++;
			p = p->sibling;
		}
	}
	parseTree -> size = 2+temp;
}




