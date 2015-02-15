/*
 * CMSC 216, Spring 2012, Project #4, public test 1
 * Ensure that directory and file names are not stored in a fixed-size array.
 */

#include <stdio.h>
#include "filesystem.h"
#include "memory-checking.h"

#define BIG_SIZE 8 * 1024
#define SMALL_SIZE 64

int main() {
  Filesystem fs;
  int i;
  char dir1[BIG_SIZE];
  char dir2[SMALL_SIZE];
  char file1[BIG_SIZE];
  char file2[SMALL_SIZE];

  setup_memory_checking();
  mkfs(&fs);

  for (i = 0; i < BIG_SIZE - 1; i++)
    dir1[i] = 'u';
  dir1[BIG_SIZE - 1] = 0;
  mkdir(&fs, dir1);

  for (i = 0; i < SMALL_SIZE - 1; i++)
    dir2[i] = 'A';
  dir2[SMALL_SIZE - 1] = 0;
  mkdir(&fs, dir2);

  for (i = 0; i < BIG_SIZE - 1; i++)
    file1[i] = 'U';
  file1[BIG_SIZE - 1] = 0;
  touch(&fs, file1);

  for (i = 0; i < SMALL_SIZE - 1; i++)
    file2[i] = 'a';
  file2[SMALL_SIZE - 1] = 0;
  touch(&fs, file2);
  printf("ls\n");
  ls(fs, "");

  printf("ls\n");
  ls(fs, "");

  rmfs(&fs);

  check_memory_leak();

  return 0;
}
