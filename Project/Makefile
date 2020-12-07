OBJS	= error.o expression_stack.o lex_analyzer.o lex_token.o main.o symtable.o syntax_analyzer.o syntax_tree.o
SOURCE	= error.c expression_stack.c lex_analyzer.c lex_token.c main.c symtable.c syntax_analyzer.c syntax_tree.c
HEADER	= error.h expression_stack.h lex_analyzer.h lex_token.h symtable.h syntax_analyzer.h syntax_tree.h
OUT	= ifj
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -lm

error.o: error.c
	$(CC) $(FLAGS) error.c 

expression_stack.o: expression_stack.c
	$(CC) $(FLAGS) expression_stack.c 

lex_analyzer.o: lex_analyzer.c
	$(CC) $(FLAGS) lex_analyzer.c 

lex_token.o: lex_token.c
	$(CC) $(FLAGS) lex_token.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 

symtable.o: symtable.c
	$(CC) $(FLAGS) symtable.c 

syntax_analyzer.o: syntax_analyzer.c
	$(CC) $(FLAGS) syntax_analyzer.c 

syntax_tree.o: syntax_tree.c
	$(CC) $(FLAGS) syntax_tree.c 


clean:
	rm -f $(OBJS) $(OUT)
