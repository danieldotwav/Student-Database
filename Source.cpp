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
void SortBy(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY);
void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY);
void SetFileDestination(string& filePath);
void PrintStudentRecords(int index, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY);
void PrintMasterRecords(string method, array<string, NUMRECORDS>lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY, array<string, NUMRECORDS> majorCOPY, array<string, NUMRECORDS> GPA_COPY);
double GetAvgGPA(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY);

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

    // Store Data in Arrays
    array<string, NUMRECORDS> firstName, lastName, major; // Student Data
    array<string, NUMRECORDS> GPA{ 0 }; // NOTE: We only need GPA to be a double datatype when performing calculation
    array<string, NUMMAJORS> dept, deptAbbr, majorName, majorAbbr, majorCode, division, school; // University Data

    // Copy Student Data for Processing
    array<string, NUMRECORDS> firstNameCOPY, lastNameCOPY, majorCOPY;
    array<string, NUMRECORDS> GPA_COPY{ 0 };

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

    // Read data from Student_Data File 
    while (!studentDataFile.eof() && studentCount < NUMRECORDS) {
        // Read the next line of the document into a string variable
        getline(studentDataFile, line);
        // Convert the entire next line of the text document into a stringstream object
        stringstream input(line);

        // Populate Arrays
        getline(input, lastName[studentCount], ',');
        getline(input, whitespace, ' '); // Removes space before firstName
        getline(input, firstName[studentCount], ',');
        getline(input, whitespace, ' '); // Removes space before major
        getline(input, major[studentCount], ',');
        input >> GPA[studentCount];

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
    studentDataFile.close();

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

void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY) {
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

void MergeArrays(const std::array<std::string, NUMRECORDS>& srcArr1, const std::array<std::string, NUMRECORDS>& srcArr2,
    const std::array<std::string, NUMRECORDS>& srcArr3, const std::array<std::string, NUMRECORDS>& srcArr4,
    std::array<std::string, NUMRECORDS>& destArr1, std::array<std::string, NUMRECORDS>& destArr2,
    std::array<std::string, NUMRECORDS>& destArr3, std::array<std::string, NUMRECORDS>& destArr4,
    int low, int mid, int high) {
    int leftSize = mid - low + 1;
    int rightSize = high - mid;

    // Copy data to temporary arrays
    std::copy_n(srcArr1.begin() + low, leftSize, destArr1.begin() + low);
    std::copy_n(srcArr2.begin() + low, leftSize, destArr2.begin() + low);
    std::copy_n(srcArr3.begin() + low, leftSize, destArr3.begin() + low);
    std::copy_n(srcArr4.begin() + low, leftSize, destArr4.begin() + low);

    // Merge the temporary arrays back into the original arrays
    int i = low, j = mid + 1, k = low;
    while (i <= mid && j <= high) {
        if (destArr1[i] <= srcArr1[j]) {
            destArr1[k] = srcArr1[i];
            destArr2[k] = srcArr2[i];
            destArr3[k] = srcArr3[i];
            destArr4[k] = srcArr4[i];
            i++;
        }
        else {
            destArr1[k] = srcArr1[j];
            destArr2[k] = srcArr2[j];
            destArr3[k] = srcArr3[j];
            destArr4[k] = srcArr4[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements from the left subarray, if any
    while (i <= mid) {
        destArr1[k] = srcArr1[i];
        destArr2[k] = srcArr2[i];
        destArr3[k] = srcArr3[i];
        destArr4[k] = srcArr4[i];
        i++;
        k++;
    }

    // No need to copy the remaining elements from the right subarray,
    // as they will already be in their correct positions in destArr
}

void MergeSortArrays(std::array<std::string, NUMRECORDS>& arr1, std::array<std::string, NUMRECORDS>& arr2,
    std::array<std::string, NUMRECORDS>& arr3, std::array<std::string, NUMRECORDS>& arr4) {
    std::array<std::string, NUMRECORDS> tempArr1, tempArr2, tempArr3, tempArr4;

    // Copy the original arrays to temporary arrays
    std::copy(arr1.begin(), arr1.end(), tempArr1.begin());
    std::copy(arr2.begin(), arr2.end(), tempArr2.begin());
    std::copy(arr3.begin(), arr3.end(), tempArr3.begin());
    std::copy(arr4.begin(), arr4.end(), tempArr4.begin());

    int currSize = 1;
    int n = NUMRECORDS;
    while (currSize < n) {
        int low = 0;
        while (low < n - 1) {
            int mid = (low + currSize - 1) < (n - 1) ? (low + currSize - 1) : (n - 1);
            int high = (low + 2 * currSize - 1) < (n - 1) ? (low + 2 * currSize - 1) : (n - 1);
            MergeArrays(tempArr1, tempArr2, tempArr3, tempArr4, arr1, arr2, arr3, arr4, low, mid, high);
            low += 2 * currSize;
        }
        currSize *= 2;
    }
}

void SortBy(std::string method, std::array<std::string, NUMRECORDS>& lastNameCOPY,
    std::array<std::string, NUMRECORDS>& firstNameCOPY, std::array<std::string, NUMRECORDS>& majorCOPY,
    std::array<std::string, NUMRECORDS>& GPA_COPY) {

    if (method == "Last Name") {
        MergeSortArrays(lastNameCOPY, firstNameCOPY, majorCOPY, GPA_COPY);
    }
    else if (method == "Major") {
        MergeSortArrays(majorCOPY, firstNameCOPY, lastNameCOPY, GPA_COPY);
    }
    else if (method == "GPA") {
        // Convert GPA values to doubles and then sort accordingly
    }
}

/*
// Note: The time complexity of the bubble sort is O(n^2) making it very inefficient for processing large amounts of data
void SortBy(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY) {

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
*/


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

void PrintStudentRecords(int index, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<string, NUMRECORDS>& GPA_COPY) {
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

void PrintMasterRecords(string method, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY, array<string, NUMRECORDS> majorCOPY, array<string, NUMRECORDS> GPA_COPY) {
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