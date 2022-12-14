BIN = parser
CC = g++
PROD = c-

SRCS = $(BIN).y $(BIN).l treeUtils.c ourgetopt.c symbolTable.cpp semantic.cpp yyerror.cpp codegen.cpp emitcode.cpp
HDRS = scanType.h treeUtils.h ourgetopt.h symbolTable.h semantic.h yyerror.h emitcode.h codegen.h
OBJS = emitcode.o lex.yy.o $(BIN).tab.o treeUtils.o ourgetopt.o symbolTable.o semantic.o yyerror.o  codegen.o

$(PROD) : $(OBJS) 
	$(CC) $(OBJS)  -o $(PROD)

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDRS)
	 flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

test : $(PROD)
	./$(PROD)    array1.c-
	
tar:
	tar -cvf $(BIN).tar $(SRCS) $(HDRS) makefile

clean : 
	rm $(OBJS) $(PROD) lex.yy.c parser.output parser2.tab.c parser2.output parser.tab.c parser.tab.h
