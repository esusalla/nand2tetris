EXEC = vmtranslator
CC = clang++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++11

$(EXEC): main.cpp CodeWriter.cpp Parser.cpp
	$(CC) $(CFLAGS) main.cpp CodeWriter.cpp Parser.cpp -o $(EXEC)

clean:
	rm -f $(EXEC)