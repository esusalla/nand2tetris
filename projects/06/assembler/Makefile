EXEC = assembler
CC = clang++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++11

$(EXEC): main.cpp Code.cpp Parser.cpp
	$(CC) $(CFLAGS) main.cpp Code.cpp Parser.cpp -o $(EXEC)

clean:
	rm -f $(EXEC)