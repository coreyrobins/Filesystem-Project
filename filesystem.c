/* Corey Robins, 111185399, crobins@linux.grace.umd.edu */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"

/* These are all the helper methods and variables, described in detail
   where the functions are implemented */
static int find_directory(Filesystem **files,
		   Directory *directories, const char arg[]);
static int search_file(File *files, const char arg[]);
static int search_directory(Directory *files, const char arg[]);
static void print_all(Directory files);
static int add_directory(Filesystem filesystem, Directory **files,
		  const char arg[]);
static int add_file(File **files, const char arg[]);
static int delete_file(File **files, const char arg[]);
static int delete_directory(Directory **files);
static int delete_directory_from_list(Directory **files, const char arg[]);
static int add_full_directory(Directory **files, Directory *new_item);
static void pwd_helper(Filesystem files, char target[]);
static int search_all_sub(Directory directories, char target[]);
static Directory *temp_delete;
static char *temp_name;
static Directory *temp_directory;
static Directory *reset_curr;
static Directory *reset_parent_sub;

/* This function allocations memory for the root directory, and sets all of
 its contents to NULL, except for the parent directory, and since we are in the
 root directory, the parent directory is just the root directory, and the
 current directory is currently the root directory */
void mkfs(Filesystem *files) {
  if (files == NULL) {
    return;
  }
  files->root = malloc(sizeof(Directory));
  if (files->root != NULL) {
    files->root->next_sibling_directory = NULL;
    files->root->subdirectory_list = NULL;
    files->root->parent = files->root;
    files->root->file_list = NULL;
    files->root->directory_name = NULL;
    files->curr = files->root;
  } else {
    printf("Memory allocation failed!\n");
    exit(1);
  }
}

/* This function takes in a pointer to a filesystem as well as the name
 of the file to add */
int touch(Filesystem *files, const char arg[]) {
  /* if the Filesystem parameter is NULL, returns 0 */
  if (files == NULL) {
    return 0;
  }
  /* if the name is NULL, then it returns 0 */
  if (arg == NULL) {
    return 0;
  }
  if (strcmp(arg, "") == 0) {
    return -1;
  }
  /* If arg is '..', '.', or '/', then it is not a valid file_name,
     and therefore 0 is returned with no effect */
  if (((arg[0] == '.' && arg[1] == '.' && !arg[2]) || 
       (arg[0] == '.' && !arg[1]) || (arg[0] == '/' && !arg[1]))) {
    return 0;
  }
  /* I called these helper methods to see if there is already a file or
     directory in the current directory with the same name as arg,
     in which case it returns 0 */
  if ((search_file(files->curr->file_list, arg) == 1) || 
      (search_directory(files->curr->subdirectory_list, arg) == 1)) {
    return 0;
  }
  /* If the file_list of the current directory is NULL (meaning that there
     are no files currently in the directory), then I allocate space 
     for the file_list of the current directory, NULL check it, and then 
     again allocate memory for the new file's name, and again NULL checked it.
     Finally, I made arg the name for the first file in the file_list of the
     current directory */
  if (files->curr->file_list == NULL) {
    files->curr->file_list = malloc(sizeof(File));
    if (files->curr->file_list == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    files->curr->file_list->next_file = NULL;
    files->curr->file_list->file_name = malloc(strlen(arg) + 1);
    if (files->curr->file_list->file_name == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    strcpy(files->curr->file_list->file_name, arg);
    return 0;
  }
  /* If the argument is NULL (the first value is 0), then it returns -1
     and this is clearly not a valid argument name */
  if (arg[0] == '0') {
    return -1;
  }
  /* If it made it through to here, meaning that it is a valid file name,
     and the file_list of the current directory is not empty, then I called
     the helper function add_file(), which adds the file to the file_list
     (see below) */
  add_file(&(files->curr->file_list), arg);
  return 0;
}

/* This function adds a new directory to the subdirectory_list of the
 current directory */
int mkdir(Filesystem *files, const char arg[]) {
  /* If the Filesystem parameter is NULL, then return 0 */
  if (files == NULL) {
    return 0;
  }
  /* If the name parameter is NULL, then return 0 */
  if (arg == NULL) {
    return 0;
  }
  if (strcmp(arg, "") == 0) {
    return -1;
  }
  /* If the arg parameter is ".." or "." or "/", then these are non-valid
     directory names and therefore -2 is returned */
  if (((arg[0] == '.' && arg[1] == '.' && !arg[2]) || 
       (arg[0] == '.' && !arg[1]) || (arg[0] == '/' && !arg[1]))) {
    return -2;
  }
  /* search_file() and search_directory() are again two helper methods,
     and they return whether or not the name parameter is the same name of
     a file or directory in the file_list or subdirectory_list of the
     current directory. If these return true, then print -2 because it is
     not a valid directory name */
  if ((search_file(files->curr->file_list, arg) == 1) || 
      (search_directory(files->curr->subdirectory_list, arg) == 1)) {
    return -2;
  }
  /* I checked if the current directory's subdirectory_list was NULL,
     meaning that the new directory will be the first in the list,
     then I added it to the subdirectory_list of the current directory */
  if (files->curr->subdirectory_list == NULL) {
    /* Need to allocate space for the directory list since it is NULL,
       then NULL check it to make sure that the memory allocation succeeded,
       if it did, then I allocate space for the directory_name to be stored
       for the subdirectory_list, NULL checked the allocation, set the name
       of the first directory in the subdirectory_list  to the name
       of the arg parameter, set this new directory's parent to the 
       current directory, and returned 1 as a success */
    files->curr->subdirectory_list = malloc(sizeof(Directory));
    if (files->curr->subdirectory_list == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    files->curr->subdirectory_list->directory_name = malloc(strlen(arg) + 1);
    if (files->curr->subdirectory_list->directory_name == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    strcpy(files->curr->subdirectory_list->directory_name, arg);
    files->curr->subdirectory_list->parent = files->curr;
    files->curr->subdirectory_list->next_sibling_directory = NULL;
    files->curr->subdirectory_list->file_list = NULL;
    files->curr->subdirectory_list->subdirectory_list = NULL;
    return 0;
  }
  /* If the subdirectory_list of the current directory is not NULL,
     then I call the helper function add_directory */
  add_directory(*(files), &(files->curr->subdirectory_list), arg);
  return 0;
}

/* This function changes the current directory to the specified directory, if
 that directory is valid */
int cd(Filesystem *files, const char arg[]) {
  /* If the Filesystem parameter is NULL, then return 0 because this is
     obviously not valid */
  if (files == NULL) {
    return 0;
  }
  /* If the arg name is ".." then it sets the current directory to the 
     parent directory and returns 1 */
  if (arg[0] == '.' && arg[1] == '.' && !arg[2]) {
    files->curr = files->curr->parent;
    return 0;
  }
  /* If the arg name is "." then it has no effect because this is saying
     change directory to the current directory, so it returns 1 */
  if ((arg[0] == '.' && !arg[1]) || arg == NULL) {
    return 0;
  }
  /* If the arg name is "/" then it sets the current directory to the root
     directory and returns 1 */
  if (arg[0] == '/' && !arg[1]) {
    files->curr = files->root;
    return 0;
  }
  /* If the arg name is not in the file_list or the subdirectory_list of the
     current directory, then return -1 because the arg name is not valid */
  if (!(search_file((files->curr->file_list), arg)) && 
      !(search_directory((files->curr->subdirectory_list), arg))) {
    return -1;
  }
  /* If the arg is the name of a file in the file_list, then I return -2
     because this is not the name of a directory */
  if (search_file((files->curr->file_list), arg)) {
    return -2;
  }
  /* I then called this helper function which changes the current directory
     to a directory in the subdirectory_list. See below */
  if (find_directory(&files, files->curr->subdirectory_list, arg)) {
    return 0;
  }
  return 1;
}

/* This function prints out the elements (in increasing lexicographic order) of
 the specified directory or file */
int ls(Filesystem files, const char arg[]) {
  /* I created these two variables so that I did not have to modify the files
     parameter */
  Directory *temp_directories;
  Filesystem *temp_filesystem;
  if (arg == NULL) {
    return 0;
  }
  /* If the arg name was ".." then I printed out all the elements of the
     parent directory of the current directory */
  if (arg[0] == '.' && arg[1] == '.' && !arg[2]) {
    print_all(*(files.curr->parent));
    return 0;
  }
  /* If the arg name was "." or an empty string, then I called the helper
     function print_all() (see below) and printed out all the elements of the
     current directory */
  if ((arg[0] == '.' && !arg[1]) || (strcmp(arg, "") == 0)) {
    print_all(*(files.curr));
    return 0;
  }
  /* If the arg name was "/" then I printed out all the elements of the root
     directory */
  if (arg[0] == '/' && !arg[1]) {
    print_all(*(files.root));
    return 0;
  }
  /* If the arg name is not in the subdirectory_list of the current directory
     nor the file_list of the current directory, then it will return -1 */
  if (!(search_file(files.curr->file_list, arg)) && 
      !(search_directory(files.curr->subdirectory_list, arg))) {
    return -1;
  }
  /* If the arg name matched the file name, then I printed just the name
     of that file */
  if (search_file(files.curr->file_list, arg)) {
    printf("%s\n", arg);
    return 0;
  }
  /* If the name of the directory was found in the subdirectory_list of the
     current directory, the effect is to print every element of the
     subdirectory specified in the parameter. First I allocated space for
     temp_directories and temp_filesystem, set them equal to the current
     directory's directory list and the filesystem parameter (respectively),
     called find_directory() (see below) on the temp_filesystem and 
     temp_directories variables, and then called print_all on the 
     temp_directories. I did this so that I did not modify the Filesystem 
     parameter, and I called find_directory() so that I could set the current
     directory of temp_directories equal to the directory specified
     in the arg name parameter */
  if (search_directory(files.curr->subdirectory_list, arg)) {
    temp_directories = malloc(sizeof(Directory));
    if (temp_directories == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    temp_directories = files.curr->subdirectory_list;
    temp_filesystem = &files;
    find_directory(&(temp_filesystem), temp_directories, arg);
    print_all(*(temp_directories));
    return 0;
  }
  return 1;
}

/* This function prints out a path from the root to the current directory */
void pwd(Filesystem files) {
  /* I set target to keep track of the current directory name, so that
     the program knows when to stop printing out the parent directories */
  char *target = files.curr->directory_name;
  /* First I checked if the current directory was the root directory,
     in which case I just printed out a "/", a new line, and returned */
  if (files.root == files.curr) {
    printf("/\n");
    return;
  }
  /* If the current directory is an immediate subdirectory of the root
     directory, then I printed out a "/", then the name of the current 
     directory, a new line, and then returned */
  if (files.curr->parent == files.root) {
    printf("/");
    printf("%s\n", files.curr->directory_name);
    return;
  }
  /* I used reset_curr to keep track of the current directory, because in the
     process of creating a path to the current directory, I will be changing
     filesystem.curr because it made it easier to implement */
  reset_curr = files.curr;
  /* I iterated up every level in the filesystem until we were at the 
     immediate sublevel of the root directory */
  while (files.curr->parent != files.root) {
    files.curr = files.curr->parent;
  }
  /* I then called the helper function pwd_helper(), explained below */
  pwd_helper(files, target);
  /* I then set curr (which was changed at this point to make the pwd()
     function easier to implement) back to the original current directory,
     because at this point, pwd() is completed, except that filesystem.curr
     has been changed, so we must change it back */
  files.curr = reset_curr;
}

/* This is a helper function for pwd() */
static void pwd_helper(Filesystem files, char target[]) {
  /* If the current directory is NOT an immediate subdirectory of the root
     directory, then I set the parent's subdirectory_list equal to the 
     variable reset_parent_sub. I did this because the 
     subdirectory_list will be modified, and I needed to reset it back after
     each function call so that the subdirectory_list went back to 
     what it was supposed to be */
  if (files.curr->parent != files.root) {
    files.curr->parent->subdirectory_list = reset_parent_sub;
  }
  /* Here I checked if we were at the target directory, and if we were,
     then I printed out the name of the target directory, a new line,
     and returned, because pwd() is complete */
  if (files.curr->directory_name != NULL && 
      (strcmp(files.curr->directory_name, target) == 0)) {
    printf("/%s\n", target);
    return;
  }
  /* If we made it here, then I needed to print out a "/", the name of the
     current directory, and I set the variable temp_directory to
     the subdirectory_list of the current directory (explained later in
     search_all_sub()), and set reset_parent_sub to the subdirectory_list of
     the current directory, because there is a chance that it will get 
     modified, so I needed to make sure that afterwards it was set back to
     what it was originally, so I needed to keep track of it */
  printf("/");
  printf("%s", files.curr->directory_name);
  temp_directory = files.curr->subdirectory_list;
  reset_parent_sub = files.curr->subdirectory_list;
  /* I then did a while loop to iterate through the subdirectories of the
     current directory to figure out which path I needed to take. I used a 
     while loop to check if the subdirectory_list was not NULL, then called
     the helper function search_all_sub(), which iterated through each 
     subdirectory of the directory being searched to see if it contains
     the target directory. If it did, then it broke out of the loop,
     and if not, I tested the next subdirectory, and set temp_directory
     equal to the subdirectory_list of the current directory (explained
     in the function search_all_sub() below */
  while (files.curr->subdirectory_list != NULL && 
	 !search_all_sub(*(files.curr->subdirectory_list), target)) {
    files.curr->subdirectory_list = 
      files.curr->subdirectory_list->next_sibling_directory;
    temp_directory = files.curr->subdirectory_list;
  }
  /* Once I found which subdirectory of the current directory contained
     the target directory, I set files.curr equal to that subdirectory, 
     and recursively called pwd_helper() on the modified filesystem */
  files.curr = files.curr->subdirectory_list;
  pwd_helper(files, target);
}

/* This function iterates through each subdirectory of the current directory
 to find the target directory */
static int search_all_sub(Directory directories, char target[]) {
  /* If the target directory was the current directory, return true */
  if (strcmp(directories.directory_name, target) == 0) {
    return 1;
  }
  /* I then called search_directory() on the subdirectory_list of the
     current directory, and returned true if one of them contained the target */
  if (directories.subdirectory_list != NULL && 
      search_directory(directories.subdirectory_list, target)) {
    return 1;
  }
  /* If no immediate subdirectory of the current directory contained the 
     target, then I recursively called search_all_sub() on each of the
     subdirectories of the current directory */
  if (directories.subdirectory_list != NULL &&
      search_all_sub(*(directories.subdirectory_list), target)) {
    return 1;
  }
  /* Before I called search_all_sub() on the sibling directories of the 
     current directory, I needed to make sure that the current directory
     was not the temp_directory (i.e. the top level from pwd_helper()), because
     I did not want to check those sibling directories or this statement
     woudl be vacuously true. That being said, I made sure the current directory
     was NOT equal to temp_directory, and made sure that the next sibling
     directory was not NULL, and then called search_all_sub() on the next
     sibling directory */
  if ((strcmp(temp_directory->directory_name, directories.directory_name) != 0) 
      && directories.next_sibling_directory != NULL &&
      search_all_sub(*(directories.next_sibling_directory), target)) {
    return 1;
  }
  /* If it got to here, then it is clearly false, so returned 0 */
  return 0;
}

/* This is a helper function for cd(), and it is also used in ls(). This 
  function first iterates through the list of directories until the directory
  is found. Then it makes sure that the directory was found, and if so, it sets
  the current directory in the files parameter to be set to this directory */
static int find_directory(Filesystem **files, Directory *directories, 
			  const char arg[]) {
  while (directories != NULL && 
	 (strcmp(directories->directory_name, arg) < 0)) {
    directories = directories->next_sibling_directory;
  }
  if (strcmp(directories->directory_name, arg) == 0) {
    (*files)->curr = directories;
    return 0;
  }
  return 1;
}  

/* This function searches for a file in the list of files, and returns
   1 if the file was found, and 0 if not */
static int search_file(File *files, const char arg[]) {
  while(files != NULL && (strcmp(files->file_name, arg) != 0)) {
    files = files->next_file;
  }
  if (files == NULL) {
    return 0;
  }
  if (files != NULL) {
    return 1;
  }
  return -1;
}

/* This function searches for a directory in the list of directories,
   and returns 1 if it was found, and 0 if not */
static int search_directory(Directory *files, const char arg[]) {
  while (files != NULL && (strcmp(files->directory_name, arg) != 0)) {
    files = files->next_sibling_directory;
  }
  if (files == NULL) {
    return 0;
  }
  if (files != NULL) {
    return 1;
  }
  return -1;
}

/* This function is a helper function for the ls() function. In this 
   function, I first used a while loop to check whether or not both 
   the file_list and the subdirectory_list were NULL each time, and then
   within the while loop I checked if the file_list was NULL, and if it was,
   I created a nested while loop to iterate through the subdirectory_list,
   because there was no longer a need to go through the file_list. In the next
   if statement, I checked and did the reverse. If both lists were not NULL,
   then I needed to print the elements out in increasing lexicographic order.
   Therefore, I checked which name came before the other, and printed whichever
   one was first, and incremented it to point to the next file or directory. */
static void print_all(Directory files) {
  while (files.file_list != NULL || files.subdirectory_list != NULL) {
    if (files.file_list == NULL) {
      while (files.subdirectory_list != NULL) {
	printf("%s/\n", files.subdirectory_list->directory_name);
	files.subdirectory_list = files.subdirectory_list->
	  next_sibling_directory;
      }
    } else if (files.subdirectory_list == NULL) {
      while (files.file_list != NULL) {
	printf("%s\n", files.file_list->file_name);
	files.file_list = files.file_list->next_file;
      }
    } else if (strcmp(files.file_list->file_name, 
		      files.subdirectory_list->directory_name) < 0) {
      printf("%s\n", files.file_list->file_name);
      files.file_list = files.file_list->next_file;
    } else {
      printf("%s/\n", files.subdirectory_list->directory_name);
      files.subdirectory_list = files.subdirectory_list->next_sibling_directory;
    }
  }
}

/* This is a helper function for touch(). The reason I needed to take in a 
   pointer to a pointer was so that files was changed */
static int add_file(File **files, const char arg[]) {
  /* First I declared pointer, prev, and new_item, and allocated space
     for new_item (and its name), and set the name of the new_item equal to
     the name of the current directory */
  File *current = *files, *prev = NULL, *new_item;
  new_item = malloc(sizeof(*new_item));
  if (new_item == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  new_item->file_name = malloc(strlen(arg) + 1);
  if (new_item->file_name == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  strcpy(new_item->file_name, arg);
  /* In this if statement I checked to see if the next_file was equal to null,
     meaning that the file_list had only one element. Then I checked to see 
     where the new_item needs to go lexicographically, and if the new_item was 
     to go before the current item, then I set the next_file of the new_item
     equal to current, and current's next_file to null, and I had the file list
     point to the new_item */
  if (current->next_file == NULL) {
    if (strcmp(current->file_name, arg) > 0) {
      new_item->next_file = current;
      current->next_file = NULL;
      (*files) = new_item;
      return 0;
    }
  }
  /* If the new_item should go before the current item, then I had the next
     file of the new_item point to the current item, and had the files
     param point to the new_item as the new head of the list */
  if (strcmp(current->file_name, arg) > 0) {
    new_item->next_file = current;
    (*files) = new_item;
    return 0;
  }
  /* I used this while loop to iterate through the list of files and see
     where I needed to insert the file */
  while (current != NULL) {   
    /* At the beginning, I checked if this was the correct place to insert
       the new file. If it was the correct place, then I set the next_file of
       the new_item equal to the next_file of current, and set the next_file
       of current equal to the new_item, and I returned 1. If this was not the
       correct place for it, then I set prev equal to current, and set
       current equal to its next_file (traversing through the list).
       I also needed to check and see if the new_item needed to be the last
       element in the list */
    if (strcmp(current->file_name, arg) < 0 && (current->next_file == NULL || 
       (strcmp(current->next_file->file_name, arg) > 0))) {
      new_item->next_file = current->next_file;
      current->next_file = new_item;
      return 0;
    } else {
      prev = current;
      current = current->next_file;
    }
  }
  /* If prev was NULL by the end of this, then I set files to point to the
     new_item. If not, then I set prev's next_file equal to the new_item */
  if (prev == NULL) {
    *files = new_item;
  } else {
    prev->next_file = new_item;
  }
  return 1;
}

/* This is a helper function for the mkdir() function. This is almost completely
   the same as the add_file() function, with a few small differences (such as 
  the names of the variables). Instead of explaining both functions, I will only
   point out differences between the functions, so please see add_file() for a 
   full description of the function */
static int add_directory(Filesystem filesystem, Directory **files, 
			 const char arg[]) {
  Directory *current = *files, *prev = NULL, *new_item;
  new_item = malloc(sizeof(*new_item));
  if (new_item == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  new_item->directory_name = malloc(strlen(arg) + 1);
  if (new_item->directory_name == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  strcpy(new_item->directory_name, arg);
  if (current->next_sibling_directory == NULL) {
    if (strcmp(current->directory_name, arg) > 0) {
      new_item->next_sibling_directory = current;
      current->next_sibling_directory = NULL;
      new_item->subdirectory_list = NULL;
      new_item->file_list = NULL;
      /* Here, since Directory is similar to a doubly linked list, I needed
	 to set the parent of the new_item equal to the parent of the current
	 item, which is essentially the only difference between the functions */
      new_item->parent = current->parent;
      (*files) = new_item;
      return 0;
    }
    if (strcmp(current->directory_name, arg) < 0) {
      new_item->next_sibling_directory = NULL;
      current->next_sibling_directory = new_item;
      /* Again, I needed to set new_item's parent */
      new_item->parent = current->parent;
      new_item->subdirectory_list = NULL;
      new_item->file_list = NULL;
      return 0;
    }
  }
  if (strcmp(current->directory_name, arg) > 0) {
    new_item->next_sibling_directory = current;
    /* Same thing with the parent directory */
    new_item->parent = current->parent;
    (*files) = new_item;
    new_item->subdirectory_list = NULL;
    new_item->file_list = NULL;
    return 0;
  }
  while (current != NULL) {    
    if (strcmp(current->directory_name, arg) < 0 && 
	(current->next_sibling_directory == NULL || 
	 (strcmp(current->next_sibling_directory->directory_name, arg) > 0))) {
      new_item->next_sibling_directory = current->next_sibling_directory;
      current->next_sibling_directory = new_item;
      /* Again with the parent directory */
      new_item->parent = current->parent;
      new_item->subdirectory_list = NULL;
      new_item->file_list = NULL;
      return 0;
    } else {
      prev = current;
      current = current->next_sibling_directory;
    }
  }
  if (prev == NULL) {
    *files = new_item;
  } else {
    prev->next_sibling_directory = new_item;
  }
  return 1;
}

/* This function removes an entire filesystem. I made sure files was not NULL
 first, and just returned if it were (because otherwise I'd get a segfault).
 Then I called delete_directory() (explained below) on the root directory,
 set files.root equal to NULL, and files.curr equal to NULL, and by this
 point I had removed everything from the filesystem */
void rmfs(Filesystem *files) {
  if (files == NULL) {
    return;
  }
  delete_directory(&(files->root));
  files->root = NULL;
  files->curr = NULL;
}

/* This function removes either a file or a directory, depending on what
 arg refers to. If it is a directory, then it deletes the directory and 
 everything inside it */
int rm(Filesystem *files, const char arg[]) {
  Directory *current = files->curr->subdirectory_list;
  /* If files were equal to NULL, then I returned 1 because this is a 
     failure */
  if (files == NULL) {
    return 1;
  }
  if (arg == NULL) {
    return 0;
  }
  /* If arg was an empty string, then I returned -3 */
  if (strcmp(arg, "") == 0) {
    return -3;
  }
  /* If arg was "..", ".", or "/", then this was already not a valid name,
     so I returned -2 */
  if (((arg[0] == '.' && arg[1] == '.' && !arg[2]) || 
       (arg[0] == '.' && !arg[1]) || (arg[0] == '/' && arg[1]))) {
    return -2;
  }
  /* I then called search_file() and search_directory() on the file_list
     and the subdirectory_list of the current directory, and if both of these
     returned 0, then arg does not exist within the current directory,
     and I returned -1 */
  if (!search_file(files->curr->file_list, arg) && 
      !search_directory(files->curr->subdirectory_list, arg)) {
    return -1;
  }
  /* If the file was contained in the file_list of the current directory,
     then I called the helper function delete_file() and returned 0 */
  if (search_file(files->curr->file_list, arg)) {
    delete_file(&(files->curr->file_list), arg);
    return 0;
  }
  /* If the directory was contained in the subdirectory_list of the current 
     directory, then I first checked to make sure current was not NULL, 
     and then allocated space for temp_name (reason explained below),
     called delete_directory_from_list() and then delete_directory(),
     and both functions are explained below */
  if (search_directory(files->curr->subdirectory_list, arg)) {
    if (current == NULL) {
      return 0;
    }
    temp_name = malloc(strlen(arg) + 1);
    if (temp_name == NULL) {
      printf("Memory Allocation Failed!\n");
      exit(1);
    }
    strcpy(temp_name, arg);
    delete_directory_from_list(&(files->curr->subdirectory_list), arg);
    delete_directory(&(temp_delete));
    return 0;
  }
  return 1;
}

/* This function renames a file or directory */
int re_name(Filesystem *files, const char arg1[], const char arg2[]) {
  if (files == NULL) {
    return 1;
  }
  if (arg1 == NULL || arg2 == NULL) {
    return 0;
  }
  /* If arg1 or arg2 was ".." "." or "/" then this was not valid
     and I returned -3 */
  if (((arg1[0] == '.' && arg1[1] == '.' && !arg1[2]) || 
       (arg1[0] == '.' && !arg1[1]) || (arg1[0] == '/' && !arg1[1]))) {
    return -3;
  }
  if (((arg2[0] == '.' && arg2[1] == '.' && !arg2[2]) || 
       (arg2[0] == '.' && !arg2[1]) || (arg2[0] == '/' && !arg2[1]))) {
    return -3;
  }
  /* If arg1 or arg2 was an empty string, then I returned -2 */
  if ((strcmp(arg1, "") == 0) || (strcmp(arg2, "") == 0)) {
    return -2;
  }
  /* If arg1 was not conainted in neither the file list nor the subdirectory
     list then I returned -1 */
  if (!search_file(files->curr->file_list, arg1) && 
      !search_directory(files->curr->subdirectory_list, arg1)) {
    return -1;
  }
  /* If arg1 and arg2 did not have the same names, but arg2 is an existing
     name in either the file list or the subdirectory_list, then I returned
     -3 */
  if ((strcmp(arg1, arg2) != 0) && (search_file(files->curr->file_list, arg2) 
     || search_directory(files->curr->subdirectory_list, arg2))) {
    return -3;
  }
  /* If arg1 and arg2 were the same name, and they were contained in either
     the file list or the subdirectory list, then I returned -4 */
  if ((strcmp(arg1, arg2) == 0) && (search_file(files->curr->file_list, arg1) 
     || search_directory(files->curr->subdirectory_list, arg1))) {
    return -4;
  }
  /* If the file was contained in the file list, then I called delete_file()
     on arg1 and then called touch() so that the file would be re-added to the
     list in the correct place */
  if (search_file(files->curr->file_list, arg1)) {
    delete_file(&(files->curr->file_list), arg1);
    touch(files, arg2);
    return 0;
  }
  /* If arg1 was a directory in the subdirectory list, then I called
     delete_directory_from_list() which kept a pointer to that directory,
     but removed it temporarily from the list, and then allocated
     space for the new name of that directory (now arg2), and called
     add_full_directory() which added the directory and everything
     contained within it to the subdirectory list of the current directory */
  if (search_directory(files->curr->subdirectory_list, arg1)) {
    delete_directory_from_list(&(files->curr->subdirectory_list), arg1);
    temp_delete->directory_name = malloc(strlen(arg2) + 1);
    if (temp_delete->directory_name == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    strcpy(temp_delete->directory_name, arg2);
    add_full_directory(&(files->curr->subdirectory_list), temp_delete);
  }
  return 1;
}

/* This function is a helper function for rm() if arg was the name of a file */
static int delete_file(File **files, const char arg[]) {
  /* These are the local variables */
  File *current = *files, *prev = NULL, *temp_file = (*files);
  /* I first checked if the current file was the same as arg, and the next
     file in the list was NULL, then I freed the name of the file, and then
     freed the file_list */
  if ((strcmp((*files)->file_name, arg) == 0) && 
      (*files)->next_file == NULL) {
    free((*files)->file_name);
    (*files)->file_name = NULL;
    free((*files));
    (*files) = NULL;
    return 0;
  }
  /* If it's the first element in the list, but there were other elements
     after it, then I set files equal to the next_file, freed the file
     to be deleted (which is why I needed temp_file), and returned 0 */
  if (strcmp((*files)->file_name, arg) == 0) {
    (*files) = (*files)->next_file;
    free(temp_file->file_name);
    temp_file->file_name = NULL;
    free(temp_file);
    temp_file = NULL;
    return 0;
  }
  /* If it was not the first file in the list, then I iterated through
     the list to find it, setting prev to current and current to the next
     file */
  while (current != NULL && (strcmp(current->file_name, arg) != 0)) {   
    prev = current;
    current = current->next_file;
  }
  /* This if statement checks if it was the LAST file in the list */
  if ((strcmp(current->file_name, arg) == 0) && current->next_file == NULL) {
    free(current->file_name);
    current->file_name = NULL;
    free(current->next_file);
    current->next_file = NULL;
    prev->next_file = NULL;
    free(current);
    current = NULL;
    return 0;
  }
  /* This if statement checks if it is in the MIDDLE of the list, and
     deletes the file accordingly */
  if (strcmp(current->file_name, arg) == 0) {
    prev->next_file = current->next_file;
    free(current->file_name);
    current->file_name = NULL;
    free(current);
    current = NULL;
    return 0;
  }
  return 1;
}		

/* This is another helper function for rm(), except that it removes
 a directory and all of its contents */
static int delete_directory(Directory **files) {
  if (files == NULL) {
    return 0;
  }
  /* First I iterated through the file_list deleting every file */
  while ((*files)->file_list != NULL) {
    delete_file(&((*files)->file_list), (*files)->file_list->file_name);
  }
  /* Then I recursively called delete_directory() on all of the 
     subdirectories of the current directory */
  while ((*files)->subdirectory_list != NULL) {
    delete_directory(&((*files)->subdirectory_list));
  }
  /* If the temp_name was NULL, then I made a while loop to iterate
     through the sibling directories of the current directory, recursively
     deleting the contents of each one */
  if (temp_name == NULL) {
    while ((*files)->next_sibling_directory != NULL) {
      delete_directory(&((*files)->next_sibling_directory));
    }
  } else {
    /* If this was not the case, then I made sure that the current 
       directory name was not NULL, and that the current directory name
       is not the same as the temp_name, and again recursively deleted
       the contents of the sibling directories */
    while ((*files)->next_sibling_directory != NULL && 
	   (*files)->directory_name != NULL && 
	   strcmp((*files)->directory_name, temp_name) != 0) {
      delete_directory(&((*files)->next_sibling_directory));
    }
  }
  /* Here I made sure the temp_name was not NULL, and then checked
     if either the current directory name was NULL or if the current
     directory name is the same as the temp_name, and if none of these
     were the case, then I freed temp_name because I no longer needed it
     and it was taking up unnecessary space */
  if (temp_name != NULL && ((*files)->directory_name == NULL || 
			    strcmp((*files)->directory_name, temp_name) == 0)) {
    free(temp_name);
    temp_name = NULL;
  }
  /* Here I set the parent equal to NULL, freed the now empty file_list,
     freed the current directory name, and finally freed the directory,
     and set everything equal to NULL to finish the job */
  (*files)->parent = NULL;
  free((*files)->file_list);
  (*files)->file_list = NULL;
  (*files)->next_sibling_directory = NULL;
  free((*files)->directory_name);
  (*files)->directory_name = NULL;
  free(*files);
  *files = NULL;
  return 0;
}	 	

/* This is a helper function for re_name(), and I needed it to simply delete the
 directory and its contents from the list without actually deleting the
 directory, because I just wanted to insert it in order but in a different 
 spot in the list */
static int delete_directory_from_list(Directory **files, const char arg[]) {
  Directory *prev = NULL, *current = *files;
  /* Iterating through finding the target directory */
  while (current != NULL && (strcmp(current->directory_name, arg) != 0)) {
    prev = current;
    current = current->next_sibling_directory;
  }
  /* Once I found it, I set temp_delete to current so that I still
     have the pointer to the directory's contents, and freed the no longer
     necessary directory_name */
  temp_delete = current;
  free(temp_delete->directory_name);
  temp_delete->directory_name = NULL;
  /* I made sure prev was not NULL (meaning it was not the first item in 
     the list), and if it wasn't, then I checked if current was NULL, and if
     it wasn't, then I set the next_sibling_directory to prev equal to NULL,
     and if not, set it equal to the next sibling directory */
  if (prev != NULL) {
    if (current == NULL) {
      prev->next_sibling_directory = NULL;
    } else {
      prev->next_sibling_directory = current->next_sibling_directory;
    }
  } else {
    /* If it was the first element in the list, I just removed
       the first element from the list by having the pointer to the 
       subdirectory_list point at the second (now first) element */
    (*files) = current->next_sibling_directory;
  }
  return 0;
}

/* This is a helper function for re_name() so that once I've taken the
 directory out of the list, I can insert it with its new name into a 
 proper spot in the list */
static int add_full_directory(Directory **files, Directory *new_item) {
  Directory *current = *files, *prev = NULL;
  /* If directories was NULL, meaning that there is nothing in the list,
     then I had the directories list point to the new_item being added */
  if (current == NULL) {
    (*files) = new_item;
    new_item->next_sibling_directory = NULL;
    return 0;
  }
  /* If there was only one item in the list, then I needed to figure out
     if the new_item should go before it or after, which is what the 
     strcmp() functions are for. I inserted it lexicographically */
  if (current->next_sibling_directory == NULL) {
    if (strcmp(current->directory_name, new_item->directory_name) > 0) {
      new_item->next_sibling_directory = current;
      current->next_sibling_directory = NULL;
      (*files) = new_item;
      return 0;
    }
    if (strcmp(current->directory_name, new_item->directory_name) < 0) {
      new_item->next_sibling_directory = NULL;
      current->next_sibling_directory = new_item;
      return 0;
    }
  }
  /* In this if statement, I checked to see if the new_item belonged
     at the very start of the list (lexicographically speaking), and 
     if so, then I inserted it to the beginning of the list */
  if (strcmp(current->directory_name, new_item->directory_name) > 0) {
    new_item->next_sibling_directory = current;
    (*files) = new_item;
    return 0;
  }
  /* If it made it all the way here, then I needed to find the place to 
     put the new_item, somewhere in the middle of the list or at the end.
     So I used a while loop to iterate through and find the necessary spot,
     and then set the prev, current, and next_sibling_directory accordingly */
  while (current != NULL) {    
    if (strcmp(current->directory_name, new_item->directory_name) < 0 && 
	(current->next_sibling_directory == NULL || 
	 (strcmp(current->next_sibling_directory->directory_name, 
		 new_item->directory_name) > 0))) {
      new_item->next_sibling_directory = current->next_sibling_directory;
      current->next_sibling_directory = new_item;
      return 0;
    } else {
      prev = current;
      current = current->next_sibling_directory;
    }
  }
  if (prev == NULL) {
    *files = new_item;
  } else {
    prev->next_sibling_directory = new_item;
  }
  return 1;
}
