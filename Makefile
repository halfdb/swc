swc: parser.c lex.c table.c compiler.c interface.c
	gcc machine.c lex.c parser.c table.c compiler.c interface.c -g -o swc

parser.c parser.h: sw_parser.y types.h machine.h table.h compiler.h
	bison sw_parser.y --output=parser.c --defines=parser.h

lex.c: sw_lex.l parser.h types.h machine.h table.h compiler.h
	flex -8 --outfile=lex.c sw_lex.l

clean:
	-rm parser.c parser.h lex.c swc
