all: smaz_test

smaz_test: smaz_test.c smaz.c
	gcc -o smaz_test -O2 -Wall -W -ansi -pedantic smaz.c smaz_test.c

check:
	./smaz_test

clean:
	rm -rf smaz_test
