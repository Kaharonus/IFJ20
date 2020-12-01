OBJS	= error.o lex_analyzer.o lex_token.o main.o symbol_table.o syntax_analyzer.o syntax_tree.o
SOURCE	= error.c lex_analyzer.c lex_token.c main.c symbol_table.c syntax_analyzer.c syntax_tree.c
HEADER	= symbol_table.h syntax_analyzer.h syntax_tree.h error.h lex_analyzer.h lex_token.h
OUT	= ifj
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

error.o: error.c
	$(CC) $(FLAGS) error.c 

lex_analyzer.o: lex_analyzer.c
	$(CC) $(FLAGS) lex_analyzer.c 

lex_token.o: lex_token.c
	$(CC) $(FLAGS) lex_token.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 

symbol_table.o: symbol_table.c
	$(CC) $(FLAGS) symbol_table.c 

syntax_analyzer.o: syntax_analyzer.c
	$(CC) $(FLAGS) syntax_analyzer.c 

syntax_tree.o: syntax_tree.c
	$(CC) $(FLAGS) syntax_tree.c 


clean:
	rm -f $(OBJS) $(OUT)
