/*
 * CMSC 216, Spring 2012, Project #4, public test 2
 * Test pwd extensively, ensuring that fixed-length strings are not used to
 * merge the path parts.
 */

#include <stdio.h>
#include <string.h>
#include "filesystem.h"
#include "memory-checking.h"

#define BUFLENGTH 150

int main() {
  Filesystem fs;
  int i;
  char buf[BUFLENGTH];
  char format_str[BUFLENGTH];

  setup_memory_checking();

  /* this string is longer than 80 characters, so it's broken up into two */
  strcpy(format_str, "ThisIsALongDirectoryNameForPublicTest2ThisIsFor");
  strcat(format_str, "CheckingAFixedLengthBuffer%d");

  mkfs(&fs);

  for (i = 0; i < 100; i++) {
    sprintf(buf, format_str, i);
    printf("mkdir %s\n", buf);
    mkdir(&fs, buf);
    printf("cd %s\n", buf);
    cd(&fs, buf);
    printf("pwd\n");
    pwd(fs);
  }

  rmfs(&fs);

  check_memory_leak();

  return 0;
}
