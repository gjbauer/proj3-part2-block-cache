all:
	clang -g -o cache_test main.c

clean:
	rm cache_test

open:
	gedit *.h *.c

.PHONY: clean open
