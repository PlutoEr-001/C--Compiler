parser: Mips.c IR.c error.c symbol_table.c sdt.c ast.c main.c sytanx.tab.c 
	gcc $^ -g -o  parser -lfl -ly

sytanx.tab.h: sytanx.y lex.yy.c
	bison -d sytanx.y

sytanx.tab.c: sytanx.y lex.yy.c
	bison -d sytanx.y

lex.yy.c: lexical.l sytanx.tab.h
	flex lexical.l
clean:
	rm -f *.tab.c *.yy.c *.tab.h parser *.output

