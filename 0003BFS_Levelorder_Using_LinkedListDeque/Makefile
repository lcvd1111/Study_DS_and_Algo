vpath %c src
vpath %h include

EXE: main.o mylib.o test.o
	$(CC) $^ -I ./include -o $@

main.o: main.c mylib.h test.h
	$(CC) -c -I ./include $< -o $@

mylib.o: mylib.c mylib.h
	$(CC) -c -I ./include $< -o $@

test.o: test.c test.h mylib.h
	$(CC) -c -I ./include $< -o $@

.PHONY: clean
clean:
	rm EXE *.o

.PHONY: cleanall
cleanall:
	rm EXE *.o
