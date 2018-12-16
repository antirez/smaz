all: smaz_test smaz_tool

smaz_test: smaz_test.c smaz.c
	gcc -o smaz_test -O2 -Wall -W -ansi -pedantic smaz.c smaz_test.c

smaz_tool: smaz.c smaz_tool.c
	gcc -o smaz_tool -O2 -Wall -W -ansi -pedantic smaz.c smaz_tool.c

clean:
	rm -rf smaz_test smaz_tool
