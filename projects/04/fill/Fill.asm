// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

(KEYDOWN)
    @SCREEN
    D=A
    @i
    M=D
    @KBD
    D=M    
    @WHITE
    D;JEQ

(BLACK)
    @color
    M=-1
    @PAINT
    0;JMP

(WHITE)
    @color
    M=0

(PAINT)
    @i
    D=M
    @KBD
    D=D-A
    @KEYDOWN
    D;JGE
    @color
    D=M
    @i
    A=M
    M=D
    D=A+1
    @i
    M=D
    @PAINT
    0;JMP
