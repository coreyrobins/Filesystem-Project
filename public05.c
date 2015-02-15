/*
 * CMSC 216, Spring 2012, Project #4, public test 5
 * Check some error conditions for re_name().
 */

#include <stdio.h>
#include "filesystem.h"
#include "memory-checking.h"

int main() {
  Filesystem fs;

  setup_memory_checking();

  mkfs(&fs);

  mkdir(&fs, "d1");
  touch(&fs, "f1");

  if (re_name(&fs, "d2", "d3") == -1 && re_name(&fs, "", "d3") == -2 &&
      re_name(&fs, ".", "d3") == -3 && re_name(&fs, "d1", "..") == -3 &&
      re_name(&fs, "f1", "f1") == -4)
    printf("Passed.\n");
  else printf("Failed.\n");

  rmfs(&fs);

  check_memory_leak();

  return 0;
}
