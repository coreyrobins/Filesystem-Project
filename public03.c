/*
 * CMSC 216, Spring 2012, Project #4, public test 3
 * Check whether mkdir and touch copy their input strings or not.
 */

#include <stdio.h>
#include "filesystem.h"
#include "memory-checking.h"

#define SIZE 8 * 1024

int main() {
  Filesystem fs;
  char c;
  int i;
  char arg[SIZE];  /* 8 K argument */

  setup_memory_checking();
  mkfs(&fs);

  for (c = '0'; c <= '4'; c++) {
    for (i = 0; i < SIZE - 1; i++)
      arg[i] = c;
    arg[SIZE - 1] = 0;
    touch(&fs, arg);
  }

  for (c = '5'; c <= '9'; c++) {
    for (i = 0; i < SIZE - 1; i++)
      arg[i] = c;
    arg[SIZE - 1] = 0;
    mkdir(&fs, arg);
    cd(&fs, arg);
  }

  printf("ls /\n");
  ls(fs, "/");
  printf("pwd\n");
  pwd(fs);

  rmfs(&fs);

  check_memory_leak();

  return 0;
}
