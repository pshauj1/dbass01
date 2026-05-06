# dbass01
CSE321 Project #1 Implementation and Analysis of B-tree Index Structures

# File Structure
- test.cc: Main benchmark execution file containing the test logic. 

- b_tree.cc / .h: Implementation of the standard B-tree.  

- b_star_tree.cc / .h: Implementation of the B*-tree.

- b_plus_tree.cc / .h: Implementation of the B+-tree.

- struct.cc / .h: Basic node structures and common tree logic.  

- std_record.h: Student record data structure.  

- Makefile: Build automation script.  

- student.csv: Dataset containing 100,000 records. 

# Prerequisites

Compiler: g++ (GCC)  

C++ Standard: C++17 or higher  

Build System: make

Dataset: Ensure student.csv is in the same directory as the executable.

# How to Build & Run

## Compilation
Navigate to the project directory and run the make command. This will compile the source code using the -O3 optimization flag.

- make

## Execution
Run the generated benchmark binary to start the tests:

- ./benchmark

When the program runs, the results for Insertion (d=3, 5, 10), Point Search, Range Query, and Deletion are displayed sequentially on the screen.
For each step, the execution time, node_util, split count, and whether structural integrity is maintained are displayed in the console.

## Cleanup
To remove object files and the executable, use:

- make clean

# Main logic of test.cc

**You can conduct experiments by adjusting the parameters of this function.**

- **main()**: Reads the student.csv file, stores it in a vector (db) of StudentRecord structures, and calls each benchmark function.  
- **test_degree(degree, data_size, db)**: Creates three types of trees with the specified degree ($d$).  It inserts 100,000 data points and records the elapsed time and the number of splits. 
- **test_randomsearch(degree, data_size, db)**:
Generates 10,000 random target IDs using the mt19937 engine.  It searches for these keys in each tree to measure the average search time.  test_query(degree, data_size, db): Extracts data with student IDs ranging from 202083137 to 202150000 using the range_query() function.
- **test_randomdelete(degree, data_size, db, count)**: Deletes randomly selected records and measures the time taken, Delete the number of items specified by the `count` variable
## helper function of test.cc
- **check_integrity_bt / check_integrity_bp**: After the deletion operation is complete, these functions recursively check whether all nodes comply with the B-tree minimum key rule ($(d-1)/2$) to verify structural integrity.
- **cal_avg_weight**: Calculates the average weight of female students within that range by referencing the returned list of RIDs.

