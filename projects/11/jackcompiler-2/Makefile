EXEC = jackcompiler
CC = clang++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++11

$(EXEC): main.cpp CompilationEngine.cpp JackTokenizer.cpp SymbolTable.cpp VMWriter.cpp
	$(CC) $(CFLAGS) main.cpp CompilationEngine.cpp JackTokenizer.cpp SymbolTable.cpp VMWriter.cpp -o $(EXEC)

clean:
	rm -f $(EXEC)