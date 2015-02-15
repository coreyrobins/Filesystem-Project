/* Corey Robins, 111185399, crobins@linux.grace.umd.edu */
/* This structure is the Filesystem and only
holds a pointer to the root directory, and a 
pointer to the current directory */
# if !defined(FILE_SYSTEM_INTERNALS_H)
#define FILE_SYSTEM_INTERNALS_H
#include "file-system-internals.h"
typedef struct filesystem {
  struct directory *root;
  struct directory *curr;
} Filesystem;

/* This is the File structure, which only holds
a pointer to the next_file in the list, and a 
pointer to the name of the file */
typedef struct file {
  struct file *next_file;
  char *file_name;
} File;

/* This is the Directory structure, which holds
a pointer to the next_sibling_directory, a pointer
to its subdirectory_list, a pointer to its parent
directory, a pointer to its file_list, and a pointer
to its name */
typedef struct directory {
  struct directory *next_sibling_directory;
  struct directory *subdirectory_list;
  struct directory *parent;
  struct file *file_list;
  char *directory_name;
} Directory;

#endif
