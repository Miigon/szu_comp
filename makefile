CC=c++
CFLAGS=-I.
DEPS = lexer.hpp typeutils.hpp
OBJ = main.o lexer.o

%.o: %.cpp $(DEPS)
	$(CC) -c -std=c++17 -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)