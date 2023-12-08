all: test

test: main.cpp
	g++ main.cpp -I /usr/include/SDL2 -lSDL2 -o bin/test

run: test
	./bin/test

clean:
	rm -f test