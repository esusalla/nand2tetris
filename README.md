# nand2tetris
Ground-up implementation of the general-purpose computing system described in [The Elements of Computing Systems](https://mitpress.mit.edu/books/elements-computing-systems) ([nand2tetris](https://www.nand2tetris.org/))

### Chapters 1 - 5
Focused on using a hardware description language (HDL) to implement a variety of combinational and sequential circuits. Earlier chapters begin with basic logic gates and move on to multiplexers / demultiplexers, adders, and an ALU before eventually culminating in a functioning computer that brings all the pieces together.

### Chapters 6 - 13
Geared towards developing the build tools and software for the system, providing specifications for writing an assembler (chapter 6), VM translator (chapters 7 and 8), and compiler (chapters 10 and 11) for the Jack programming language. Programs written in Jack can then be passed through the build process (compiler -> VM translator -> assembler) and executed on the target architecture built in the first half of the book.