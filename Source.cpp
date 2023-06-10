#include <iostream>
#include <iomanip>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include "Student.h"
using namespace std;

//TODO:
// 1. Allow user to enter specified file destination (FIXED)
// 2. The documents print to the correct location, but they are empty files (FIXED)
// 3. Error handling:
//      a. The program should not loop infinitely when the user enters invalid characters



// Notes and Documentation
/*
6/8/2023: I attempted to save the file to a specified location by concatenating the file name to include the specified file path along with the document name (ex. C:\Desktop\FileName)
          However, it was more efficient to use Windows API to get the correct file path.
6/9/2023: The program now allows the user to select the destination of the data file.
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

const int NUMRECORDS = 11;
const int w = 5; // setw width

// Function Prototypes
int MenuSelection();
int SubMenuSelection();
void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY);
void SortBy(string method, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY);
int SetFileDestination(string& filePath, ofstream&);
string GetUserDefinedFilePath(ofstream&);
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
        std::cout << "Unable to open " << fileName << endl;
        std::cout << "Terminating Program\n";
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
            std::cout << "Closing Menu...\n\n";
            break;
        }
    }

    // Close Files
    InFile.close();

    // Exit Prompt
    std::cout << "Terminating Program...\n";
    std::cout << "*sad computer noises*" << endl;
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
        std::cout << endl;
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
        std::cout << endl;
    } while (selection != 1 && selection != 2 && selection != 3);

    return selection;
}

void StudentLookup(int& index, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY) {
    string lname, fname;
    bool lastNameMatch{ false };
    bool firstNameMatch{ false };

    std::cout << "Enter Student's Last Name: ";
    std::cin >> lname;
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
        std::cout << endl;

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

int SetFileDestination(string& filePath, ofstream& stream) {
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
        std::cout << "4. Type in your own file path\n\n";
        std::cout << "Selection: ";
        std::cin >> selection;
        std::cout << endl;
    } while (selection != 1 && selection != 2 && selection != 3 && selection != 4);

    switch (selection) {
        case 1:
            filePath = GetSpecifiedFolderPath(folderIDDesktop);
            return 0;
        case 2:
            filePath = GetSpecifiedFolderPath(folderIDDownloads);
            return 0;
        case 3:
            filePath = GetSpecifiedFolderPath(folderIDDocuments);
            return 0;
        case 4:
            filePath = GetUserDefinedFilePath(stream);
            return 1;
        default:
            std::cout << "ERROR: Invalid File Path Selection\n";
            exit(1);
    }
}

string GetUserDefinedFilePath(ofstream& stream) {

    string filePath;

    std::cout << "Enter the complete file path in the form:\n";
    std::cout << "C:\\Users\\username\\FileDestination\\FileName.txt\n";
    std::cout << "File Path = ";
    std::cin >> filePath;
    std::cout << endl << endl;

    // Fix the file path
    for (int i{ 0 }; filePath[i] != '\0'; i++) {
        if (filePath[i] == '\\') {
            filePath[i] = '/';
        }
    }
    
    stream = ofstream(filePath);
    if (stream.fail()) {
        std::cout << "ERROR: The Destination '" << filePath << "' is Invalid\n\n";
        return "";
    }
    else {
        return filePath;
    }
}

void PrintStudentRecords(int index, array<string, NUMRECORDS>& lastNameCOPY, array<string, NUMRECORDS>& firstNameCOPY, array<string, NUMRECORDS>& majorCOPY, array<double, NUMRECORDS>& GPA_COPY) {
    ofstream OutFile;
    string DocumentName;

    if (index == -999) {
        std::cout << "***No Student Record Found***\n\n";
    }
    else {
        // Determine the filepath
        string filePath;
        if ((SetFileDestination(filePath, OutFile)) == 1) {
            if (filePath == "") {
                // If the user enters an invalid path, send the file to their Documents folder
                const GUID folderIDDocuments = FOLDERID_Documents;
                filePath = GetSpecifiedFolderPath(folderIDDocuments);
                DocumentName = filePath + "\\Master_Records.txt";

                std::cout << "File will be exported to Documents folder\n";
                OutFile.open(DocumentName);
            }
        }
        else {
            // Update Document Name
            filePath += "\\";
            filePath += lastNameCOPY[index];
            filePath += "_";
            filePath += firstNameCOPY[index] + "_Records.txt";

            // TEST
            std::cout << "Selected File Destination: ";
            std::cout << filePath << endl << endl;
            OutFile = ofstream(filePath);
        }
        
        // Update Document Name
        string studentDocName = filePath + "\\";
        studentDocName += lastNameCOPY[index];
        studentDocName += "_";
        studentDocName += firstNameCOPY[index] + "_Records.txt";

        // TEST
        std::cout << "Selected File Destination: ";
        std::cout << filePath << endl << endl;
        ofstream OutFile(studentDocName.c_str());

        // Output Formatting
        OutFile << fixed << showpoint << setprecision(2);

        OutFile << "--------------------------------------------\n";

        OutFile << "Name:    " << lastNameCOPY[index] << ","
            << firstNameCOPY[index] << endl;
        OutFile << "Major:   " << setw(w) << majorCOPY[index] << endl;
        OutFile << "GPA:    " << setw(w) << GPA_COPY[index] << endl << endl; //The whitespace was apparently stored in the double variable somehow

        OutFile << "--------------------------------------------\n\n";

        // Let the user know the data printed successfully
        std::cout << "//////////////////////////////////////////////////\n";
        std::cout << "Data Exported Successfully to " << studentDocName << "\n";
        std::cout << "/////////////////////////////////////////\n\n";

        // Close files
        OutFile.close();
    }

}

void PrintMasterRecords(string method, array<string, NUMRECORDS> lastNameCOPY, array<string, NUMRECORDS> firstNameCOPY, array<string, NUMRECORDS> majorCOPY, array<double, NUMRECORDS> GPA_COPY) {
    ofstream OutFile;
    string DocumentName;

    // Determine the filepath
    string filePath;
    if ((SetFileDestination(filePath, OutFile)) == 1) {
        if (filePath == "") {
            // If the user enters an invalid path, send the file to their Documents folder
            const GUID folderIDDocuments = FOLDERID_Documents;
            filePath = GetSpecifiedFolderPath(folderIDDocuments);
            DocumentName = filePath + "\\Master_Records.txt";

            std::cout << "File will be exported to Documents folder\n";
            OutFile.open(filePath);
        }
    }
    else {
        // Update Document Name
        DocumentName = filePath + "\\Master_Records.txt";

        // TEST
        std::cout << "Selected file path: ";
        std::cout << filePath << endl << endl;
        // Note: When constructing the ofstream object, we pass DocumentName.c_str() as the argument, which converts the string to a c-style string
        //ofstream OutFile(DocumentName.c_str());
        OutFile.open(DocumentName);
    }

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
    std::cout << "//////////////////////////////////////////////////\n";
    std::cout << "Data Exported Successfully to Master_Records.txt\n";
    std::cout << "/////////////////////////////////////////\n\n";

    // Close Files
    OutFile.close();
}