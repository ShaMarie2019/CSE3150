## Overall
This program is ment to manage a list of students and their GPA using dynamically allocated arrays. The user is able to add students, update the GPAs, print student information, and computer the average GPA

## Pointers 
I used pointers throughout my program to manage memory and modify data. 

Example: 
- char** name, double* gpas are dynamically allocated arrays 
- double* gpaPtr is used in updateGPA() to directly modify a students GPA 
- nullptr is used to ensure safe pointer usuage before dereferencing

## Dynamic Memory Allocation 
Dynamic memory allocation is also used to cerate flexible arrays based on user input:
- new char*[capacity] for storing student names
- new double[capacity] for storing GPAs

Memory is released at the end using delete[]


## References
References are used to allow functions to modify variables directly without copying:

- int& size allows addStudent() to update the number of stored students.

This ensures changes persist outside the function scope.

## Const Usage
const char* name ensures student names are not modified in printStudent()

const double& gpa avoids unnecessary copying while protecting the value

## Exceptions
Finally exceptions were also included  for error handling:
"No students" is thrown when attempting to compute an average with no data

Exceptions are caught in `main()` using:
    catch (const char* msg)