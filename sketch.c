// Basic program skeleton for a Sketch File (.sk) Viewer
#include "displayfull.h"
#include "sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Allocate memory for a drawing state and initialise it
const byte KEEPOPERAND  = (byte)63;
const int MAXBYTE = (1 << 8) - 1;
//debugger functions
void printState(state* s) {
  printf("x: %d\n", s->x);
  printf("y: %d\n", s->y);
  printf("tx: %d\n", s->tx);
  printf("ty: %d\n", s->ty);
  printf("tool: %u\n", s->tool);
  //printf("start: %u\n", s->start);
  printf("data: 0x%08x\n", s->data);
  //printf("end: %d\n", s->end);
}

void printOpcode(int opcode) {
  switch(opcode) {
    case DX:
      printf("opcode: DX\n");
      break;
    case DY:
      printf("opcode: DY\n");
      break;
    case TOOL:
      printf("opcode: TOOL\n");
      break;
    case DATA:
      printf("opcode: DATA\n");
      break;
  }
}
//debugger functions END
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
//helper functions
unsigned int getData(state* s) {
  int output = s->data;
  s->data = 0;
  return output;
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
  if (b & (byte)pow(2, 5)) output = (b | (~KEEPOPERAND)); //set leading bits to 1 if input is two's complement negative 
  else output = b & KEEPOPERAND; //if two's complement is positive then set leading bits to 0
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

void runDY(display* d, state* s, int operand) {
  s->ty += operand;
  draw(s, d);
}


void changeTOOL(display* d, state* s, int operand) {
  switch(operand) {
    case COLOUR:
    {
      colour(d, (int)getData(s));
      break;
    }
    case TARGETX:
      s->tx = getData(s);
      break;

    case TARGETY:
      s->ty = getData(s);
      break;

    case SHOW:
      show(d);
      break;
    
    case PAUSE:
      pause(d, getData(s));
      break;

    case NEXTFRAME:
      s->end = true;
      break;

    default:
      s->tool = operand;
      break;
  }
}

void runDATA(state* s, int operand) {
  byte input = (byte) operand;
  if (operand < 0) {
    input = ~input + 1; //convert negative two's complement int to positive two's complement int 
    input = ((~input) & KEEPOPERAND) + 1; //convert positive two's complement int to negative two's complement 6 bit number by setting the 2 leading bits to 0
  }
  //if positive then no change needs to be made
  s->data = (s->data << 6) | input;
}
// Execute the next byte of the command sequence.
void obey(display *d, state *s, byte op) {
  int opcode = getOpcode(op);
  int operand = getOperand(op);
  //printOpcode(opcode);
  //printf("operand: %d\n", operand);
  switch (opcode) {
    case DX:
      runDX(s, operand);
      break;
    case DY:
      runDY(d, s, operand);
      break;
    case TOOL:
      changeTOOL(d, s, operand);
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
    int index = 0;
    while (!feof(byteFile) && !s->end) {
      if (index >= s->start) {
        obey(d, s, currentByte);
      }
      if (s->end) {
        s->start = index+1;
      }
      currentByte = fgetc(byteFile);
      index++;
    }
    show(d);
    if (!s->end) s->start = 0; 
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
