#include <iostream>
#include <cstring>   // for strlen, strcpy

// TODO: function prototypes

// TODO: implement addStudent
void addStudent(char* name, double gpa, char* names[], double gpas[], int& size, int capacity);
// TODO: implement updateGPA
void updateGPA(double* gpaPtr, double newGpa);
// TODO: implement printStudent
void printStudent(const char* name, const double& gpa);
// TODO: implement averageGPA
double averageGPA(const double gpas[], int size);

void addStudent(char* name, double gpa, char* names[], double gpas[], int& size, int capacity) {
    if (size >= capacity) {
        std::cout<< "List full\n";
        return;
    }

    names[size] = new char[strlen(name) + 1];
    strcpy(names[size], name);
    gpas[size] = gpa;
    size++;

    std::cout << "Student added\n";
}

void updateGPA(double* gpaPtr, double newGpa) {
    if (gpaPtr != nullptr) {
        *gpaPtr = newGpa;
    }
}

void printStudent(const char* name, const double& gpa) {
    std::cout << name << " " << gpa << "\n";
}

double averageGPA(const double gpas[], int size) {
    if (size == 0) {
        throw "No students";
        return 0.0;
    }

    double sum = 0;
    for (int i = 0; i < size; i++)
        sum += gpas[i];

    double avg = sum / size;
    std::cout << "Average GPA: " << avg << "\n";
    return avg;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./program <capacity>" << std::endl;
        return 1;
    }

    // VLAs, not covered
    //char* names[capacity];
    //double gpas[capacity];
    int capacity = std::stoi(argv[1]);

    char** names = new char*[capacity];
    double* gpas = new double[capacity];
    int size = 0;

    int choice;
    do {
        std::cout << "Menu:\n";
        std::cout << "1. Add student\n";
        std::cout << "2. Update GPA\n";
        std::cout << "3. Print all students\n";
        std::cout << "4. Compute average GPA\n";
        std::cout << "5. Quit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                // TODO: implement menu logic
                
                char temp[100];
                double gpa;

                if (size < capacity) {
                    std::cout << "Enter student name: ";
                    std::cin >> temp;

                    std::cout << "Enter GPA: ";
                    std::cin >> gpa;
                
                }
                addStudent(temp, gpa, names, gpas, size, capacity);
               
                break;
            }
            case 2: {
                // TODO: implement menu logic
                if (size == 0) {
                    std::cout << "No students\n";
                    break;
                }

                int idx;
                std::cout << "Enter index: ";
                std::cin >> idx;

                if (idx < 0 || idx >= size) {
                    std::cout << "Invalid index\n";
                    break;
                }

                double newGpa;
                std::cout << "Enter new GPA: ";
                std::cin >> newGpa;

                updateGPA(&gpas[idx], newGpa);
                break;
            }
            case 3: {
                // TODO: implement menu logic
                if (size == 0) {
                    std::cout << "No students\n";
                    break;
                }

                for (int i = 0; i < size; i++)
                    printStudent(names[i], gpas[i]);

                break;
            }
            case 4: {
                // TODO: implement menu logic
                try {
                    double avg = averageGPA(gpas, size);
                }catch (const char* msg) {
                    std::cout << msg << std::endl;
                }
                break;
            }
            case 5: {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            default: {
                std::cout << "Invalid choice" << std::endl;
            }
        }
    } while (choice != 5);

    // TODO: free memory
    for (int i = 0; i < size; i++) {
        delete[] names[i];
    }
    delete[] names;
    delete[] gpas;

    return 0;
}
