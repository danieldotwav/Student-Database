#include <iostream>
#include <iomanip>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include "Student.h"
using namespace std;

//TODO:
// 1. Allow user to enter specified file destination (FIXED)
// 2. The documents print to the correct location, but they are empty files (FIXED)
// 3. Error handling:
//      a. The program should not loop infinitely when the user enters invalid characters (FIXED)
// 4. Caclulate the average GPA for individual students and display their relative rankings in their individual records.
//      a. Relative rankings include Major Ranking, Class Ranking, and Overall University Ranking (Involves statistics and predictive models / machine learning most likely)
//      b. Add a feature that allows the administrator to return the "Top 10" students by GPA, filtered by Major, Class, or University
// 5. Create a separate list that uses Major Codes rather than the Department name. EX. ANTHRO (Major Code) rather than Anthropology (Dept. Name)
// 6. To make the program more realistic, each student should have a unique 4-Digit ID number. That ID number can provide another method for administrators to look up students.
//      a. Create the option to seach for students by: 1. Name // 2. Student ID // 3. Major Code - return a list of all students in the specified major and then ask admin to enter a name from the given list (ADDITONALLY: Display the total number of students majoring in that specific subject)
// 7. Replace the bubble sort with a more efficient sorting algorith (one that's recursive)
// 8. Create a document that lists Department Name, Abbreviation, Major Name, Abbreviation, Code, Division, and School
// 
// If a user-entered name does not currently exist in the database, create a similarity index using stats that computes a percent similarity based on the user's input and the existing students in the database.


// Notes and Documentation
/*
6/8/2023:  I attempted to save the file to a specified location by concatenating the file name to include the specified file path along with the document name (ex. C:\Desktop\FileName)
           However, it was more efficient to use Windows API to get the correct file path.
6/9/2023:  The program now allows the user to select the destination of the data file.
           The program will no longer enter an infinite loop when the user enters invalid characters when prompted for input.
6/10/2023: Removed the ability for the user to input a specific file destination. In order to implement this feature, you need to first check that the user-entered destination is valid, and if it is not then either send the user back to the menu or generate the file in a known user location.
           I replaced the bubble sort mechanism with a merge sort algorithm that recursively divides the array into smaller subarrays until the base case is reached (when "low" becomes greater than or equal to "high")
           Note: Arrays are not good for storing and processing large amounts of data, as they are allocated on the stack which is much more limited in size than data that is allocated on the heap
6/11/2023: All array containers have been changed to vectors for scalability. Stack overflow is no longer an issue for processing records with thousands of lines.
           All functions now accomodate vectors, and the sorting algorithm has been redesigned take in four parameters instead of 5. The first parameter is the vector whose contents are being sorted alphabetically (or chronologically in the case of GPA) and the other three parameters are the 
           ...vectors whose index orders change in tandem with the sorted veector of interest.






*/

#ifdef _WIN32
#include <shlobj.h>
#endif

string GetSpecifiedFolderPath(const GUID& folderID) {
    string specifiedPath;

#ifdef _WIN32
    PWSTR path;
    if (SUCCEEDED(SHGetKnownFolderPath(folderID, 0, nullptr, &path))) {
        wchar_t* wPath = path;
        size_t convertedChars = 0;
        size_t bufferSize = wcslen(wPath) + 1;
        char pathBuffer[MAX_PATH];
        wcstombs_s(&convertedChars, pathBuffer, bufferSize, wPath, _TRUNCATE);
        specifiedPath = pathBuffer;
        CoTaskMemFree(path);
    }
#else
    const char* homeDir = std::getenv("HOME");
    if (homeDir != nullptr) {
        switch (folderID) {
            case FOLDERID_Desktop:
                specifiedPath = string(homeDir) + "\\Desktop\\";
                break;
            case FOLDERID_Downloads:
                specifiedPath = string(homeDir) + "\\Downloads\\";
                break;
            default:
                break;
        }
    }
#endif

    return specifiedPath;
}

constexpr int NUMRECORDS = 800;
constexpr int NUMMAJORS = 153;
const int w = 5; // setw width

// Function Prototypes
int MenuSelection();
int SubMenuSelection();
void SortBy(string method, vector<string>& lastNameCOPY, vector<string>& firstNameCOPY, vector<string>& majorCOPY, vector<string>& GPA_COPY);
void StudentLookup(int& index, vector<string> lastNameCOPY, vector<string> firstNameCOPY);
void SetFileDestination(string& filePath);
void PrintStudentRecords(int index, vector<string>& lastNameCOPY, vector<string>& firstNameCOPY, vector<string>& majorCOPY, vector<string>& GPA_COPY);
void PrintMasterRecords(string method, vector<string> lastNameCOPY, vector<string> firstNameCOPY, vector<string> majorCOPY, vector<string> GPA_COPY);
double GetAvgGPA(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY);
void sortVectors(std::vector<std::string>& mainVector, std::vector<std::string>& vector2, std::vector<std::string>& vector3, std::vector<std::string>& vector4);

int main() {
    // Store File Names in String Objects
    std::string studentDataName{ "Student_Data.txt" };
    std::string majorInfoFileName{ "Undergraduate_Majors_and_Pre-Majors.txt" };

    // Open Files
    ifstream studentDataFile(studentDataName);
    ifstream majorFile(majorInfoFileName);

    // Check for Files Opening Properly
    if (!studentDataFile) {
        std::cout << "Unable to open " << studentDataName << std::endl;
        std::cout << "Terminating Program\n";
        exit(1);
    }
    if (!majorFile) {
        std::cout << "Unable to open " << majorInfoFileName << std::endl;
        std::cout << "Terminating Program\n";
        exit(1);
    }

    // Store Data in Vectors (memory allocated on the heap)
    vector<string> firstName, lastName, major, GPA;
    vector<string> dept, deptAbbr, majorName, majorAbbr, majorCode, division, school;

    // Copy Student Data for Processing
    vector<string> firstNameCOPY, lastNameCOPY, majorCOPY, GPA_COPY;

    int menuChoice{ 0 };
    int subMenuChoice{ 0 };
    int index{ 0 };

    // The first line of the files are just categories, so ignore them before processing data
    string line;
    getline(studentDataFile, line);
    line = "";
    getline(majorFile, line);
    line = "";

    int studentCount{ 0 }; // Keep track of number of records (in case of future changes)
    int majorCount{ 0 }; // Keep track of number of majors (in case of future changes)
    string whitespace; // Used to skip the whitespaces
    string word;
    
    // Read data from Student_Data File
    while (!studentDataFile.eof()) {
        // Read the next line of the document into a string variable
        getline(studentDataFile, line);
        // Convert the entire next line of the text document into a stringstream object
        stringstream input(line);

        // Resize Vectors Before Storing New Data
        lastName.resize(studentCount + 1);
        firstName.resize(studentCount + 1);
        major.resize(studentCount + 1);
        GPA.resize(studentCount + 1);
        lastNameCOPY.resize(studentCount + 1);
        firstNameCOPY.resize(studentCount + 1);
        majorCOPY.resize(studentCount + 1);
        GPA_COPY.resize(studentCount + 1);

        // Populate Vectors
        getline(input, lastName[studentCount], ',');
        getline(input, whitespace, ' '); // Removes space before firstName
        getline(input, firstName[studentCount], ',');
        getline(input, whitespace, ' '); // Removes space before major
        getline(input, major[studentCount], ',');
        getline(input, whitespace, ' '); // Removes space before GPA
        getline(input, GPA[studentCount], ',');

        // Create a copy of the arrays for later use
        lastNameCOPY[studentCount] = lastName[studentCount];
        firstNameCOPY[studentCount] = firstName[studentCount];
        majorCOPY[studentCount] = major[studentCount];
        GPA_COPY[studentCount] = GPA[studentCount];

        // Increment count and repeat
        studentCount++;
    }
    line = "";

    // Read data from Major Data File
    while (!majorFile.eof() && majorCount < NUMMAJORS) {
        getline(majorFile, line);
        stringstream input(line);

        // Resize Vectors Before Storing New Data
        dept.resize(majorCount + 1);
        deptAbbr.resize(majorCount + 1);
        majorName.resize(majorCount + 1);
        majorAbbr.resize(majorCount + 1);
        majorCode.resize(majorCount + 1);
        division.resize(majorCount + 1);
        school.resize(majorCount + 1);

        // Populate Arrays
        getline(input, dept[majorCount], ';');
        getline(input, whitespace, ' '); // Removes space before deptAbbr
        getline(input, deptAbbr[majorCount], ';');
        getline(input, whitespace, ' '); // Removes space before...
        getline(input, majorName[majorCount], ';');
        getline(input, whitespace, ' ');
        getline(input, majorAbbr[majorCount], ';');
        getline(input, whitespace, ' ');
        getline(input, majorCode[majorCount], ';');
        getline(input, whitespace, ' ');
        getline(input, division[majorCount], ';');
        getline(input, whitespace, ' ');
        getline(input, school[majorCount], ';');

        majorCount++;
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
                sortVectors(lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                PrintMasterRecords("Last Name", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                break;
            case 2: // Alphabetical by Major
                sortVectors(majorCOPY, lastNameCOPY, firstNameCOPY, GPA_COPY);
                PrintMasterRecords("Major", lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
                break;
            case 3: // GPA (Highest to Lowest)
                sortVectors(GPA_COPY, majorCOPY, lastNameCOPY, firstNameCOPY);
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
    studentDataFile.close();
    majorFile.close();

    // Exit Prompt
    std::cout << "Terminating Program...\n";
    std::cout << "*sad computer noises*" << std::endl;
}
// Global Function Definitions

int MenuSelection() {
    int selection{ 0 };

    do {
        std::cout << "Please Select One of the Following Menu Options\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "1. Find Individual Student Records\n";
        std::cout << "2. Print Entire Student Registry\n";
        std::cout << "3. Exit\n\n";
        std::cout << "Selection: ";
        std::cin >> selection;
        std::cout << std::endl;
        
        // Handle invalid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "ERROR: INVALID INPUT\n\n";
        }
    } while (selection != 1 && selection != 2 && selection != 3);

    return selection;
}

int SubMenuSelection() {
    int selection{ 0 };

    do {
        std::cout << "--Print Method--\n"
            << "Select One of the Following Menu Options\n";
        std::cout << "----------------------------------------\n";
        std::cout << "1. Alphabetically by Last Name\n";
        std::cout << "2. Alphabetically by Major\n";
        std::cout << "3. GPA (Highest to Lowest)\n\n";
        std::cout << "Selection: ";
        std::cin >> selection;
        std::cout << std::endl;

        // Handle invalid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "ERROR: INVALID INPUT\n\n";
        }
    } while (selection != 1 && selection != 2 && selection != 3);

    return selection;
}

void StudentLookup(int& index, vector<string> lastNameCOPY, vector<string> firstNameCOPY) {
    string lname, fname;
    bool lastNameMatch{ false };
    bool firstNameMatch{ false };

    std::cout << "Enter Student's Last Name: ";
    std::cin >> lname;
    std::cout << std::endl;
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
        std::cout << "Enter Student's First Name: ";
        std::cin >> fname;
        std::cout << std::endl;

        if (fname.compare(firstNameCOPY[index]) == 0) {
            firstNameMatch = true;
        }
        else {
            index = -999; // If first name doesn't match, reset index to -999
        }
    }
    if (!lastNameMatch || !firstNameMatch) { 
        index = -999; // If either the first or last name don't match, set index to sentinel value of -999
    }
}

// Note: The time complexity of the bubble sort is O(n^2) making it very inefficient for processing large amounts of data
void SortBy(string method, vector<string>& lastNameCOPY, vector<string>& firstNameCOPY, vector<string>& majorCOPY, vector<string>& GPA_COPY) {

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
                    string x = GPA_COPY[j];
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
                    string x = GPA_COPY[j];
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
                    string temp = GPA_COPY[j];
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

void SetFileDestination(string& filePath) {
    int selection;
    const GUID folderIDDesktop = FOLDERID_Desktop;
    const GUID folderIDDownloads = FOLDERID_Downloads;
    const GUID folderIDDocuments = FOLDERID_Documents;

    do {
        std::cout << "--File Path Selection--\n"
            << "Select One of the Following Menu Options\n";
        std::cout << "----------------------------------------\n";
        std::cout << "1. Print to 'Desktop'\n";
        std::cout << "2. Print to 'Downloads'\n";
        std::cout << "3. Print to 'Documents'\n";
        std::cout << "Selection: ";
        std::cin >> selection;
        std::cout << std::endl;

        // Handle invalid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "ERROR: INVALID INPUT\n\n";
        }
    } while (selection != 1 && selection != 2 && selection != 3 && selection != 4);

    switch (selection) {
        case 1:
            filePath = GetSpecifiedFolderPath(folderIDDesktop);
            break;
        case 2:
            filePath = GetSpecifiedFolderPath(folderIDDownloads);
            break;
        case 3:
            filePath = GetSpecifiedFolderPath(folderIDDocuments);
            break;
        default:
            std::cout << "ERROR: Invalid File Path Selection\n";
            exit(1);
    }
}

double GetAvgGPA(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY) {
    double avgGPA{ 0 };
    double totalGPA{ 0 };

    if (method == "Major") {
        // Loop Through List and gather only the GPA's of students with the same major

        return avgGPA;
    }
    else if (method == "Class") {
        //
        return avgGPA;
    }
    else if (method == "University") {
        for (int i{ 0 }; i < NUMRECORDS; i++) {
            //totalGPA += GPA_COPY[i];
        }
        avgGPA = (totalGPA / NUMRECORDS);
        return avgGPA;
    }
}

void PrintStudentRecords(int index, vector<string>& lastNameCOPY, vector<string>& firstNameCOPY, vector<string>& majorCOPY, vector<string>& GPA_COPY) {
    string DocumentName;

    if (index == -999) {
        std::cout << "***ERROR: NO STUDENT RECORD FOUND***\n\n";
    }
    else { // Student Record was successfully found
        string filePath;
        SetFileDestination(filePath);

        // Update Document Name
        filePath += "\\";
        filePath += lastNameCOPY[index];
        filePath += "_";
        filePath += firstNameCOPY[index] + "_Records.txt";

        // Display file location to user
        std::cout << "Selected File Destination: ";
        std::cout << filePath << std::endl << std::endl;
        
        // Open the output file and initialize it with filePath as the name
        ofstream OutFile(filePath);

        // Output Formatting
        OutFile << fixed << showpoint << setprecision(2);

        OutFile << "--------------------------------------------\n";

        OutFile << "Name:    " << lastNameCOPY[index] << ", "
            << firstNameCOPY[index] << std::endl;
        OutFile << "Major:   " << setw(w) << majorCOPY[index] << std::endl;
        OutFile << "GPA:    " << setw(w) << GPA_COPY[index] << std::endl << std::endl; //The whitespace was apparently stored in the double variable somehow
        // Print Student's Major Ranking: Average GPA for (Insert_Users_Major) is ...
        // Print Student's Class Ranking (Class of 20XX):
        // Print Student's Overall University Ranking: 
        OutFile << "--------------------------------------------\n\n";

        // Let the user know the data printed successfully
        std::cout << "//////////////////////////////////////////////////\n";
        std::cout << "Data Exported Successfully to " << filePath << "\n";
        std::cout << "/////////////////////////////////////////\n\n";

        // Close files
        OutFile.close();
    }

}

void PrintMasterRecords(string method, vector<string> lastNameCOPY, vector<string> firstNameCOPY, vector<string> majorCOPY, vector<string> GPA_COPY) {
    string DocumentName;

    // Determine the filepath
    string filePath;
    SetFileDestination(filePath);
    
    // Update document name
    filePath += "\\Master_Records.txt";

    // Display file location to user
    std::cout << "Selected File Path: ";
    std::cout << filePath << std::endl << std::endl;

    // Open the output file and initialize it with filePath as the name
    ofstream OutFile(filePath);

    // Output Formatting
    OutFile << fixed << showpoint << setprecision(2);

    OutFile << "--------------------------------------------\n";

    if (method == "Last Name" || method == "Major") {
        OutFile << "Alphabetical by " << method << std::endl << std::endl;
    }
    else if (method == "GPA") {
        OutFile << method << " (Highest to Lowest)\n\n";
    }

    // Print information to data file
    for (int i{ 0 }; i < NUMRECORDS; i++) {
        OutFile << "Name:    " << lastNameCOPY[i] << ", "
            << firstNameCOPY[i] << std::endl;
        OutFile << "Major:   " << setw(w) << majorCOPY[i] << std::endl;
        OutFile << "GPA:    " << setw(w) << GPA_COPY[i] << std::endl << std::endl;
    }

    OutFile << "--------------------------------------------\n\n";

    // Let the user know the data printed successfully
    std::cout << "//////////////////////////////////////////////////\n";
    std::cout << "Data Exported Successfully to Master_Records.txt\n";
    std::cout << "/////////////////////////////////////////\n\n";

    // Close Files
    OutFile.close();
}

void sortVectors(std::vector<std::string>& mainVector, std::vector<std::string>& vector2, std::vector<std::string>& vector3, std::vector<std::string>& vector4) {
    // Create an index vector to keep track of the original positions
    std::vector<int> index(mainVector.size());
    for (int i = 0; i < index.size(); i++) {
        index[i] = i;
    }

    // Sort the main vector while preserving the corresponding order in the index vector
    std::sort(index.begin(), index.end(), [&](int a, int b) {
        return mainVector[a] < mainVector[b];
        });

    // Rearrange the main vector based on the sorted index vector
    std::vector<std::string> sortedVector(mainVector.size());
    for (int i = 0; i < sortedVector.size(); i++) {
        sortedVector[i] = mainVector[index[i]];
    }

    // Rearrange the other vectors based on the sorted index vector
    std::vector<std::string> sortedVector2(vector2.size());
    std::vector<std::string> sortedVector3(vector3.size());
    std::vector<std::string> sortedVector4(vector4.size());

    for (int i = 0; i < index.size(); i++) {
        sortedVector2[i] = vector2[index[i]];
        sortedVector3[i] = vector3[index[i]];
        sortedVector4[i] = vector4[index[i]];
    }

    // Update the original vectors with the sorted values
    mainVector = sortedVector;
    vector2 = sortedVector2;
    vector3 = sortedVector3;
    vector4 = sortedVector4;
}