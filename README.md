# Student-Database
A C++ program for managing student records including names, majors, and GPAs. This program provides various features for data manipulation and sorting. It allows users to find individual student records, print the entire student registry, and sort the records by last name, major, or GPA. Additionally, it offers options to save the records to different file destinations, such as the desktop, downloads folder, or documents folder.

## **Features**
- Find individual student records by last name and first name.
- Print the entire student registry sorted alphabetically by last name, major, or GPA.
- Save student records to a specified file destination.
- Efficient sorting using merge sort algorithm.
- Flexible file destination selection.
- Error handling for invalid input.
- Future plans include adding more advanced features like calculating average GPA, class ranking, and university ranking.

## **How to Use**
### 1. **Clone the Repository:**
  Clone this GitHub repository to your local machine using the following command:
  ```sh
  git clone <https://github.com/danieldotwav/Student-Database>
  ```

### 2. **Compile the Program:**

  Navigate to the directory where you've cloned the repository using your terminal or command prompt.
  Compile the C++ program using a C++ compiler. For example, you can use g++ on Unix-based systems:
  * *g++ main.cpp -o student_records* *
  This command will compile the program and create an executable named student_records.

### 3. **Execute the compiled program:**
   ```sh
   ./student_records
   ```
  
### 4. **Main Menu:**

  The program will display a main menu with the following options:

  `1. Find Individual Student Records`: Use this option to search for a specific student's record by last name and first name.
  
  `2. Print Entire Student Registry`: Select this option to print the entire student registry with sorting options.
  
  `3. Exit:` Choose this option to exit the program.

### 5. **Find Individual Student Records:**

  If you select option 1, the program will prompt you to enter the last name and first name of the student you're looking for.
  It will then display the student's record if found, including their last name, first name, major, and GPA.

### 6. **Print Entire Student Registry:**

  If you choose option 2, the program will present a sub-menu with sorting options:

  `1. Alphabetically by Last Name`: Sort and print the student registry by last name.
  
  `2. Alphabetically by Major`: Sort and print the student registry by major.
 
  `3. GPA (Highest to Lowest)`: Sort and print the student registry by GPA.
  Select one of these sorting methods to print the student records accordingly.

### 7. **Save Records to File:**

  After sorting and printing, the program will ask you to select a file destination:

  `1. Print to 'Desktop'`: Save the records to your desktop.
  
  `2. Print to 'Downloads'`: Save the records to your downloads folder.
  
  `3. Print to 'Documents'`: Save the records to your documents folder.
  Choose the desired file destination, and the program will save the records in a text file with the appropriate format.

### 8. **Error Handling:**

  The program incorporates error handling for invalid input. If you provide incorrect input, it will display error messages and guide you to enter valid information.

### 9. **Exit the Program:**

  To exit the program, select option 3 from the main menu. The program will close, and you'll return to your command prompt or terminal.

### 10. **Future Enhancements:**

  The program has future plans to include advanced features such as calculating the average GPA, class ranking, and university ranking. Keep an eye out for updates to this repository for these additional functionalities.
