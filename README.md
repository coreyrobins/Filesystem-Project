CMSC 216, Spring 2012, Project #4, Public Tests

* Overview:

  There are 9 public tests. The first five public tests (public01 -
  public05) are C programs that call your functions, and do memory checks.
  The last four (public06 - public09) are specified with input files that
  contain the input commands your implementation will be tested against
  with the driver program we have supplied.

* How to run the tests

  Compile your filesystem.c source file, the publicXX.c files and driver.c,
  then link them into 6 programs (public01, public02, public03, public04,
  public05, driver), using the Makefile that you write for this project.
  Your Makefile MUST have rules to make all 6 executable programs.  Note
  that the program named driver is used to execute public tests 06 - 09.

  A given test (say public02 for example), is run by running the executable
  program (driver, or public01, public02, etc.).  For the tests that use
  the driver program, redirect input from the corresponding input file and
  comparing the resulting output with the given output file. This can be
  achieved by running the following command for a C program public test:

  public02 | diff - public02.output

  For tests that use the driver, the command looks like:

  driver < public07.input | diff - public07.output

  A test is successful if the command does not produce any output.  If
  there are differences between your output and the expected output, diff
  will show where these occur.

* Test descriptions

  The nine public tests are:

  public01:       Test that directory and file names are not stored in a
		  fixed-size array.

  public02:       Test pwd extensively, ensuring that fixed-length strings are
		  not used to merge the path parts.

  public03:       Check whether mkdir and touch copy their input strings
		  properly.

  public04:       Test rmfs() on an empty filesystem.

  public05:       Check some error conditions for re_name().

  public06:       Test rmfs() followed by mkfs().

  public07:       Test the basic operation of rm().

  public08:       Test a special case for rm(), the first file and
		  directory created in a directory.

  public09:	  Test rm() on a non-empty directory.
