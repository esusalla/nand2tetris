EXEC = jackcompiler
CC = clang++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++11

$(EXEC): main.cpp CompilationEngine.cpp JackTokenizer.cpp
	$(CC) $(CFLAGS) main.cpp CompilationEngine.cpp JackTokenizer.cpp -o $(EXEC)

clean:
	rm -f $(EXEC)