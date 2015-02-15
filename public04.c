/*
 * CMSC 216, Spring 2012, Project #4, public test 4
 * Test rmfs() on an empty filesystem.
 */

#include <stdio.h>
#include "filesystem.h"
#include "memory-checking.h"

int main() {
  Filesystem fs;

  setup_memory_checking();

  mkfs(&fs);
  rmfs(&fs);

  check_memory_leak();

  return 0;
}
