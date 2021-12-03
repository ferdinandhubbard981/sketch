// Basic program skeleton for a Sketch File (.sk) Viewer
#include "displayfull.h"
#include "sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Allocate memory for a drawing state and initialise it
const byte KEEPOPERAND  = (byte)63;
state *newState() {
  state* s = malloc(sizeof(state));
  s->x = 0;
  s->y = 0;
  s->tx = 0;
  s->ty = 0;
  s->tool = LINE;
  s->start = 0;
  s->data = 0;
  s->end = 0;
  return s;
}
//reset everything other than start
void resetState(state* s) {
  s->x = 0;
  s->y = 0;
  s->tx = 0;
  s->ty = 0;
  s->tool = LINE;
  s->data = 0;
  s->end = 0;
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
  int output = 0;
  if (b & (byte)pow(2, 5)) output = (b | (~KEEPOPERAND));
  else output = b & KEEPOPERAND;
  return output;
}
void draw(state* s, display* d) {
  switch(s->tool) {
    case NONE:
      break;
    case LINE:
      line(d, s->x, s->y, s->tx, s->ty);
      break;
    case BLOCK:
      block(d, s->x, s->y, s->tx - s->x, s->ty - s->y);
      break;
  }
  s->x = s->tx;
  s->y = s->ty;
}

void runDX(state* s, int operand) {
  s->tx += operand;
}

void runDY(state* s, int operand, display* d) {
  s->ty += operand;
  draw(s, d);
}

void runTOOL(state* s, int operand) {
  s->tool = operand;

}

void runDATA(state* s, int operand) {
  
}
// Execute the next byte of the command sequence.
void obey(display *d, state *s, byte op) {
  int opcode = getOpcode(op);
  int operand = getOperand(op);
  switch (opcode) {
    case DX:
      runDX(s, operand);
      break;
    case DY:
      runDY(s, operand, d);
      break;
    case TOOL:
      runTOOL(s, operand);
      break;
    case DATA:
      runDATA(s, operand);
      break;
  }


}

// Draw a frame of the sketch file. For basic and intermediate sketch files
// this means drawing the full sketch whenever this function is called.
// For advanced sketch files this means drawing the current frame whenever
// this function is called.

bool processSketch(display *d, void *data, const char pressedKey) {
    //TO DO: OPEN, PROCESS/DRAW A SKETCH FILE BYTE BY BYTE, THEN CLOSE IT
    if (data == NULL) return (pressedKey == 27);
    state *s = (state*) data;
    char *filename = getName(d);
    FILE* byteFile = fopen(filename, "rb");
    byte currentByte = fgetc(byteFile);
    
    while (!feof(byteFile)) {
      obey(d, s, currentByte);
      currentByte = fgetc(byteFile);
    }
    
    show(d); 
    resetState(s);

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
