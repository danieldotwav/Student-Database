#include <iostream>
#include <iomanip>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
//#include <cctype>
//#include <algorithm>
#include <vector>
#include "Student.h"
using namespace std;

const int NUMRECORDS = 11;
const int w = 5; // setw width

// Function Prototypes
int MenuSelection();
int SubMenuSelection();
void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY);
void SortBy(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY);
void PrintStudentRecords(int index, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY);
void PrintMasterRecords(string method, array<string, NUMRECORDS>lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY, array<string, NUMRECORDS> majorCOPY, array<double, NUMRECORDS> GPA_COPY);

int main() {
    // Variable Declaration
    string fileName{ "Data.txt" }, line;
    ifstream InFile(fileName);

    // Array Data
    array<string, NUMRECORDS> firstName, lastName, major;
    array<double, NUMRECORDS> GPA{ 0 };

    // Array Data (Copy for manipulation)
    array<string, NUMRECORDS> firstNameCOPY, lastNameCOPY, majorCOPY;
    array<double, NUMRECORDS> GPA_COPY{ 0 };

    int menuChoice{ 0 };
    int subMenuChoice{ 0 };
    int index{ 0 };

    // Create a vector to hold the student objects
    vector<Student> students;

    // Check to make sure the files open properly
    if (!InFile) {
        cout << "Unable to open " << fileName << endl;
        cout << "Terminating Program\n";
        exit(1);
    }

    // The first line of the Data.txt file is just format, so ignore it before processing data
    getline(InFile, line);
    line = "";
    int count{ 0 };
    string whitespace; // Used to skip the whitespace before major name

    while (!InFile.eof() && count < NUMRECORDS) {
        // Read the next line of the document into a string variable
        getline(InFile, line);
        // Convert the entire next line of the text document into a stringstream object
        stringstream input(line);

        // Populate the arrays
        getline(input, lastName[count], ',');
        getline(input, whitespace, ' '); // Removes space before firstName
        getline(input, firstName[count], ',');
        getline(input, whitespace, ' '); // Removes space before major
        getline(input, major[count], ',');
        input >> GPA[count];

        // The method below can be used to remove all whitespaces in a string. Not very userful in our case, but it does exist. 
        //firstName[count].erase(remove_if(firstName[count].begin(), firstName[count].end(), isspace));
        //major[count].erase(remove_if(major[count].begin(), major[count].end(), isspace));

        // Create a copy of the arrays for later use
        lastNameCOPY[count] = lastName[count];
        firstNameCOPY[count] = firstName[count];
        majorCOPY[count] = major[count];
        GPA_COPY[count] = GPA[count];

        // Increment count and repeat
        count++;
    }

    // Menu
    while (menuChoice != 3) {
        // Update User Selection
        menuChoice = MenuSelection();

        switch (menuChoice) {
        case 1: // Find Individual Student Records
            StudentLookup(index, lastNameCOPY, firstNameCOPY);
            PrintStudentRecords(index, lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
            break;
        case 2: // Print Entire Student Registry
            subMenuChoice = SubMenuSelection();
            switch (subMenuChoice) {
            case 1: // Alphabetical by Last Name
                SortBy("Last Name", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                PrintMasterRecords("Last Name", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                break;
            case 2: // Alphabetical by Major
                SortBy("Major", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                PrintMasterRecords("Major", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                break;
            case 3: // GPA (Highest to Lowest)
                SortBy("GPA", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                PrintMasterRecords("GPA", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                break;
            }
            break;
        case 3: // Exit
            cout << "Closing Menu...\n\n";
            break;
        }
    }

    // Close Files
    InFile.close();

    // Exit Prompt
    cout << "Terminating Program...\n";
    cout << "*sad computer noises*" << endl;
}
// Global Function Definitions

int MenuSelection() {
    int selection{ 0 };

    do {
        cout << "Please Select One of the Following Menu Options\n";
        cout << "------------------------------------------------\n";
        cout << "1. Find Individual Student Records\n";
        cout << "2. Print Entire Student Registry\n";
        cout << "3. Exit\n\n";
        cout << "Selection: ";
        cin >> selection;
        cout << endl;
    } while (selection != 1 && selection != 2 && selection != 3);

    return selection;
}

int SubMenuSelection() {
    int selection{ 0 };

    do {
        cout << "--Print Method--\n"
            << "Select One of the Following Menu Options\n";
        cout << "----------------------------------------\n";
        cout << "1. Alphabetically by Last Name\n";
        cout << "2. Alphabetically by Major\n";
        cout << "3. GPA (Highest to Lowest)\n\n";
        cout << "Selection: ";
        cin >> selection;
        cout << endl;
    } while (selection != 1 && selection != 2 && selection != 3);

    return selection;
}

void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY) {
    string lname, fname;
    bool lastNameMatch{ false };
    bool firstNameMatch{ false };

    cout << "Enter Student's Last Name: ";
    cin >> lname;
    // Loop through last name array to find match
    for (int i{ 0 }; i < NUMRECORDS; i++) {
        if (lname.compare(lastNameCOPY[i]) == 0) {
            lastNameMatch = true;
            index = i;
            break; // Exit loop once match is found
        }
    }
    // If the last name is in the record, check for first name
    if (lastNameMatch) {
        cout << "Enter Student's First Name: ";
        cin >> fname;
        cout << endl;

        if (fname.compare(firstNameCOPY[index]) == 0) {
            firstNameMatch = true;
        }
    }
    // If the first and last name don't match, set index to sentinel value of -999
    if (!lastNameMatch || !firstNameMatch) {
        index = -999;
    }
}

void SortBy(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY) {

    // Sort By Last Name (Bubble)
    if (method == "Last Name") {
        for (int i{ 0 }; i < NUMRECORDS; i++) {
            for (int j{ 0 }; j < NUMRECORDS - 1; j++) {
                if (lastNameCOPY[j] > lastNameCOPY[j + 1]) {
                    // Swap the elements in lastName array
                    string t = lastNameCOPY[j];
                    lastNameCOPY[j] = lastNameCOPY[j + 1];
                    lastNameCOPY[j + 1] = t;
                    // Swap the elements in the other arrays as well
                    // First Name
                    t = firstNameCOPY[j];
                    firstNameCOPY[j] = firstNameCOPY[j + 1];
                    firstNameCOPY[j + 1] = t;
                    // Major
                    t = majorCOPY[j];
                    majorCOPY[j] = majorCOPY[j + 1];
                    majorCOPY[j + 1] = t;
                    // GPA
                    double x = GPA_COPY[j];
                    GPA_COPY[j] = GPA_COPY[j + 1];
                    GPA_COPY[j + 1] = x;
                }
            }
        }
    }

    // Sort By Major (also Bubble)
    if (method == "Major") {
        for (int i{ 0 }; i < NUMRECORDS; i++) {
            for (int j{ 0 }; j < NUMRECORDS - 1; j++) {
                if (majorCOPY[j] > majorCOPY[j + 1]) {
                    // Swap the elements in lastName array
                    string t = majorCOPY[j];
                    majorCOPY[j] = majorCOPY[j + 1];
                    majorCOPY[j + 1] = t;
                    // Swap the elements in the other arrays as well
                    // First Name
                    t = firstNameCOPY[j];
                    firstNameCOPY[j] = firstNameCOPY[j + 1];
                    firstNameCOPY[j + 1] = t;
                    // Last Name
                    t = lastNameCOPY[j];
                    lastNameCOPY[j] = lastNameCOPY[j + 1];
                    lastNameCOPY[j + 1] = t;
                    // GPA
                    double x = GPA_COPY[j];
                    GPA_COPY[j] = GPA_COPY[j + 1];
                    GPA_COPY[j + 1] = x;
                }
            }
        }
    }

    // Sort By GPA (you guessed it, also Bubble)
    if (method == "GPA") {
        for (int i{ 0 }; i < NUMRECORDS; i++) {
            for (int j{ 0 }; j < NUMRECORDS - 1; j++) {
                if (GPA_COPY[j] < GPA_COPY[j + 1]) {
                    // Swap the elements in GPA array
                    double temp = GPA_COPY[j];
                    GPA_COPY[j] = GPA_COPY[j + 1];
                    GPA_COPY[j + 1] = temp;
                    // Swap the elements in the other arrays as well
                    // First Name
                    string t = firstNameCOPY[j];
                    firstNameCOPY[j] = firstNameCOPY[j + 1];
                    firstNameCOPY[j + 1] = t;
                    // Last Name
                    t = lastNameCOPY[j];
                    lastNameCOPY[j] = lastNameCOPY[j + 1];
                    lastNameCOPY[j + 1] = t;
                    // Major
                    t = majorCOPY[j];
                    majorCOPY[j] = majorCOPY[j + 1];
                    majorCOPY[j + 1] = t;
                }
            }
        }
    }

}

void PrintStudentRecords(int index, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY) {
    if (index == -999) {
        cout << "***No Student Record Found***\n\n";
    }
    else {
        string studentDocName = lastNameCOPY[index] + "_";
        studentDocName += firstNameCOPY[index] + "_Records.txt";
        ofstream OutFile(studentDocName);

        // Output Formatting
        OutFile << fixed << showpoint << setprecision(2);

        OutFile << "--------------------------------------------\n";

        OutFile << "Name:    " << lastNameCOPY[index] << ","
            << firstNameCOPY[index] << endl;
        OutFile << "Major:   " << setw(w) << majorCOPY[index] << endl;
        OutFile << "GPA:    " << setw(w) << GPA_COPY[index] << endl << endl; //The whitespace was apparently stored in the double variable somehow

        OutFile << "--------------------------------------------\n\n";

        // Let the user know the data printed successfully
        cout << "//////////////////////////////////////////////////\n";
        cout << "Data Exported Successfully to " << studentDocName << "\n";
        cout << "/////////////////////////////////////////\n\n";

        // Close files
        OutFile.close();
    }

}

void PrintMasterRecords(string method, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY, array<string, NUMRECORDS> majorCOPY, array<double, NUMRECORDS> GPA_COPY) {
    ofstream OutFile("Master_Records.txt");

    // Output Formatting
    OutFile << fixed << showpoint << setprecision(2);

    OutFile << "--------------------------------------------\n";

    if (method == "Last Name" || method == "Major") {
        OutFile << "Alphabetical by " << method << endl << endl;
    }
    else if (method == "GPA") {
        OutFile << method << " (Highest to Lowest)\n\n";
    }

    // Print arrays to verify all the information was stored properly
    for (int i{ 0 }; i < NUMRECORDS; i++) {
        OutFile << "Name:    " << lastNameCOPY[i] << ","
            << firstNameCOPY[i] << endl;
        OutFile << "Major:   " << setw(w) << majorCOPY[i] << endl;
        OutFile << "GPA:    " << setw(w) << GPA_COPY[i] << endl << endl;
    }

    OutFile << "--------------------------------------------\n\n";

    // Let the user know the data printed successfully
    cout << "//////////////////////////////////////////////////\n";
    cout << "Data Exported Successfully to Master_Records.txt\n";
    cout << "/////////////////////////////////////////\n\n";

    // Close Files
    OutFile.close();
}