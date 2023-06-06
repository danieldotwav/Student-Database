#ifndef STUDENT_H
#define STUDENT_H

// This file defines the Student class

#include <iostream>
#include <string>

class Student
{
private:
	std::string firstName;
	std::string lastName;
	std::string major;
	double GPA;

public:
	Student() {}; // default constructor
	Student(std::string, std::string, std::string, double); // parameterized constructor to manually set new student record
	void setFirstName(std::string fName) { firstName = fName; }
	void setLastName(std::string lName) { lastName = lName; }
	void setMajor(std::string major) { this->major = major; }
	void setGPA(double GPA) { this->GPA = GPA; }
	std::string getFirstName() { return firstName; }
	std::string getLastName() { return lastName; }
	std::string getMajor() { return major; }
	double getGPA() { return GPA; }

	void printRecord() const;
};


#endif