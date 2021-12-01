// Basic program skeleton for a Sketch File (.sk) Viewer
#include "displayfull.h"
#include "sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Allocate memory for a drawing state and initialise it
const byte KEEPOPERAND  = (byte)63;
state *newState() {
  state* instance = malloc(sizeof(state*));
  instance->x = 0;
  instance->y = 0;
  instance->tx = 0;
  instance->ty = 0;
  instance->tool = 0;
  instance->start = 0;
  instance->data = 0;
  instance->end = 0;
  return NULL; // this is a placeholder only
}

// Release all memory associated with the drawing state
void freeState(state *s) {
  free(s);
}

// Extract an opcode from a byte (two most significant bits).
int getOpcode(byte b) {
  return (int)(b >> 6); // this is a placeholder only
}

// Extract an operand (-32..31) from the rightmost 6 bits of a byte.
int getOperand(byte b) {
  return (int)(KEEPOPERAND & b); // this is a placeholder only
}

// Execute the next byte of the command sequence.
void obey(display *d, state *s, byte op) {
  //TO DO
}

// Draw a frame of the sketch file. For basic and intermediate sketch files
// this means drawing the full sketch whenever this function is called.
// For advanced sketch files this means drawing the current frame whenever
// this function is called.
unsigned long long getFileLen(FILE* byteFile) {
  unsigned long long len = 0;

    byteFile = fopen(argv[1], "rb");         
    fseek(byteFile, 0, SEEK_END);          
    filelen = ftell(byteFile);
}
bool processSketch(display *d, void *data, const char pressedKey) {

    //TO DO: OPEN, PROCESS/DRAW A SKETCH FILE BYTE BY BYTE, THEN CLOSE IT
    
    if (data == NULL) return (pressedKey == 27);
    state *s = (state*) data;
    char *filename = getName(d);
    for(i = 0; i < filelen; i++) {
       fread(buffer+i, 1, 1, fileptr); 
}
    show(d); 
    //AND TO RESET THE DRAWING STATE APART FROM THE 'START' FIELD AFTER CLOSING THE FILE

  return (pressedKey == 27);
}

// View a sketch file in a 200x200 pixel window given the filename
void view(char *filename) {
  display *d = newDisplay(filename, 200, 200);
  state *s = newState();
  run(d, s, processSketch);
  freeState(s);
  freeDisplay(d);
}

// Include a main function only if we are not testing (make sketch),
// otherwise use the main function of the test.c file (make test).
#ifndef TESTING
int main(int n, char *args[n]) {
  if (n != 2) { // return usage hint if not exactly one argument
    printf("Use ./sketch file\n");
    exit(1);
  } else view(args[1]); // otherwise view sketch file in argument
  return 0;
}
#endif
