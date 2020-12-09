OBJS	= expression_stack.o lex_token.o symtable.o error.o lex_analyzer.o semantic_analyzer.o syntax_tree.o code_generation.o garbage_collector.o main.o syntax_analyzer.o
SOURCE	= expression_stack.c lex_token.c symtable.c error.c lex_analyzer.c semantic_analyzer.c syntax_tree.c code_generation.c garbage_collector.c main.c syntax_analyzer.c
HEADER	= code_generation.h garbage_collector.h syntax_analyzer.h expression_stack.h lex_token.h symtable.h error.h lex_analyzer.h semantic_analyzer.h syntax_tree.h
OUT	= a.out
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

expression_stack.o: expression_stack.c
	$(CC) $(FLAGS) expression_stack.c 

lex_token.o: lex_token.c
	$(CC) $(FLAGS) lex_token.c 

symtable.o: symtable.c
	$(CC) $(FLAGS) symtable.c 

error.o: error.c
	$(CC) $(FLAGS) error.c 

lex_analyzer.o: lex_analyzer.c
	$(CC) $(FLAGS) lex_analyzer.c 

semantic_analyzer.o: semantic_analyzer.c
	$(CC) $(FLAGS) semantic_analyzer.c 

syntax_tree.o: syntax_tree.c
	$(CC) $(FLAGS) syntax_tree.c 

code_generation.o: code_generation.c
	$(CC) $(FLAGS) code_generation.c 

garbage_collector.o: garbage_collector.c
	$(CC) $(FLAGS) garbage_collector.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 

syntax_analyzer.o: syntax_analyzer.c
	$(CC) $(FLAGS) syntax_analyzer.c 


clean:
	rm -f $(OBJS) $(OUT)
