# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.

parse: parse.o scan.o
	g++ -o parse parse.o scan.o

clean:
	rm *.o parse

test: parse
	./parse < ./test.txt
test1: parse
	./parse < ./test1.txt
test2: parse
	./parse < ./test2.txt
test3: parse
	./parse < ./test3.txt

parse.o: scan.h
scan.o: scan.h
