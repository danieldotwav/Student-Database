#include <iostream>
#include <iomanip>
#include "Student.h"
using namespace std;

const int w = 5; //width in setw

Student::Student(std::string, std::string, std::string, double) {
	this->firstName = firstName;
	this->lastName = lastName;
	this->major = major;
	this->GPA = GPA;
}

void Student::printRecord() const {
	// Output Formatting
	cout << fixed << showpoint << setprecision(2);
	
	cout << "Name:  " << setw(w) << lastName << ", " << firstName << endl;
	cout << "Major: " << setw(w) << major << endl;
	cout << "GPA:   " << setw(w) << GPA << endl << endl;

}