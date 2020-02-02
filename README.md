# Sorer Project

Sorer Project creates an executable that will read in a file that holds SOR data
in a row format and let the user query the information in the file.

## Design Philosophy

We want sorer to be fast. The speed of the executable is important. The program
is designed with the idea of eventually extending it and using it to process big
data scale input files.

We are also concerned with assessing the well-formedness of the file that gets
ingested. We want be able to quickly assess if the input file contains well
formed sor data. This becomes important when we try to process real world data
in actual applications.

Due to the above two concerns, we have decided that speed and correctness are
the most important criteria for us.

## Choices Made

We chose to implement sorer in C++. We restricted ourselves to CwC subset of C++
as much as possible in order to get the speed benefit of using a systems
languages while avoid some of C++'s overwhelming number of features and
pitfalls.

To manage the complexity of the project, we designed a number of classes to
handle some of the code logic for us.

We rely on the kernel to manage paging for us to decrease the code complexity
and potentially improve performance.

At this stage, we mainly work with offsets of a memory mapped file to avoid
excessive copying of strings. We also try as much as possible to avoid random
accesses in the file to improve cache and preempting effectiveness.

## Using the Program

The user must specify a file, and a query for the program to run. Otherwise, the
program is not required to do any work and will simply terminate.

The user can make one of three queries during each run of the program:  
-print_col_type  
-print_col_idx  
-is_missing_idx.  
