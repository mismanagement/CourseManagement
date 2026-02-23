//============================================================================
// Name        : CourseManagement.cpp
// Author      : Erik Olsen
// Version     : 1.0
// Course      : CS-300
// Description : Course Management System for ABCU Academic Advisors
//               Uses hash table for O(1) course lookups and O(n) loading
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <limits>
#include <chrono>

#include <filesystem>
// Utility function to list CSV files in the current directory
std::vector<std::string> getCsvFilesInCurrentDir() {
    std::vector<std::string> csvFiles;
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (name.size() >= 4 && name.substr(name.size() - 4) == ".csv") {
                csvFiles.push_back(name);
            }
        }
    }
    return csvFiles;
}
#include <iomanip>
// Remove a prerequisite from all courses in the hash table

using namespace std;
using namespace std:: chrono;


//=============================================================================
//Course Structure Definition
//=============================================================================

// Structure to represent a single course

struct Course {
    string courseNumber; // Unique course identifier (e.g., "CSCI300")
    string courseTitle; // Course name
    vector<string> prerequisites; // List of prerequisite course numbers

    // Default constructor
    Course() {
        courseNumber = "";
        courseTitle = "";
    }
};
//=============================================================================
// Hash Table Class Definition
//=============================================================================

// Hash table implementation for storing courses
// Uses chaining for collision resolution

class HashTable {
private:
    // Structure for nodes in the chain
    struct Node {
        Course course;
        Node* next;

        Node(Course aCourse) : course(aCourse), next(nullptr) {}
    };

    vector<Node*> table; // Hash table buckets
    unsigned int tableSize; // Size of the hash table


    /**
     * This hash function determines the bucket index
     * @param key The course number to hash
     * @return The bucket index
     */
    unsigned int hash(const string& key) {
        unsigned int hashValue = 0;
        for (char ch : key) {
            hashValue = hashValue * 31 + ch;
        }
        return hashValue % tableSize;
    }

public:
    /**
     * Constructor to initialize the hash table with a specified size
     * @param size The size of the hash table (default 179 - prime number)
     */
    HashTable(unsigned int size = 179) {
        tableSize = size;
        table.resize(tableSize, nullptr);
    }

    /**
     * Destructor to clean up memory used by the hash table
     */
    ~HashTable() {
        for (unsigned int i=0; i < tableSize; ++i) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }

    /**
     * Inserts a course into the hash table
     * @param course The course to insert
     */
    void insert(Course course) {
        unsigned int index = hash(course.courseNumber);

        // Create a new node
        Node* newNode = new Node(course);

        // If bucket is empty, insert at head
        if (table[index] == nullptr) {
            table[index] = newNode;
        } else {
            // Chain at the beginning for efficiency
            newNode->next = table[index];
            table[index] = newNode;
        }
    }

    /**
     * Search for a course by its course number
     * @param courseNumber The course number to search for
     * @return Pointer to the course if found, nullptr otherwise
     */
    Course* search(const string& courseNumber) {
        unsigned int index = hash(courseNumber);
        Node* current = table[index];

        // Traverse the chain at this bucket
        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return &(current->course);
            }
            current = current->next;
        }

        return nullptr; // Course not found
    }


    /**
     * Get all courses in the hash table
     * @return A vector of all courses
     */
    vector<Course> getAllCourses() {
        vector<Course> allCourses;

        // Iterate through all buckets
        for (unsigned int i = 0; i < tableSize; ++i) {
            Node* current = table[i];

            // Traverse the chain and collect all courses
            while (current != nullptr) {
                allCourses.push_back(current->course);
                current = current->next;
            }
        }

        return allCourses;
    }
/**
 * Remove a course from the hash table
 * @param courseNumber The course number to remove
 * @return True if the course was removed, false if not found
 */
bool remove(const string& courseNumber) {
    unsigned int index = hash(courseNumber);
    Node* current = table[index];
    Node* prev = nullptr;

    // Traverse the chain at this bucket
    while (current != nullptr) {
        if (current->course.courseNumber == courseNumber) {
            // Course found, remove it
            if (prev == nullptr) {
                // Removing head of the chain
                table[index] = current->next;
            } else {
                // Removing from middle or end of the chain
                prev->next = current->next;
            }
            delete current;
            return true; // Course removed
        }
        prev = current;
        current = current->next;
    }

    return false; // Course not found


}
};

//=============================================================================
// Utility Functions
//=============================================================================

/**
 * Converts a string to uppercase
 * @param str The string to convert
 * @return The uppercase version of the string
 */
string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}   
// Remove a prerequisite from all courses in the hash table
void removePrerequisiteFromAllCourses(HashTable& hashTable, const string& removedCourseNumber) {
    vector<Course> allCourses = hashTable.getAllCourses();
    for (auto& course : allCourses) {
        auto& prereqs = course.prerequisites;
        size_t oldSize = prereqs.size();
        prereqs.erase(remove(prereqs.begin(), prereqs.end(), removedCourseNumber), prereqs.end());
        if (prereqs.size() != oldSize) {
            // Only update if a prerequisite was actually removed
            hashTable.remove(course.courseNumber);
            hashTable.insert(course);
        }
    }
}
/**
 * Trims leading and trailing whitespace from a string
 * @param str The string to trim
 * @return The trimmed string
 */
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos)
            return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

/**
 * Split a string by a delimiter
 * @param str The string to split
 * @param delimiter The delimiter character
 * @return A vector of tokens
 */

 vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
 }

 //=============================================================================
 // Core Functionality Functions
 //=============================================================================

 /**
  * Load course data from CSV file into hash table
  * @param filename Path to the CSV file
  * @param hashTable Reference to the hash table
  * @return True if loading is successful, false otherwise
  */
 bool loadCourseData(const string& filename, HashTable& hashTable) {
    // Start timing
    auto startTime = high_resolution_clock::now();
    clock_t startTicks = clock();

    ifstream file(filename);

    // Check if file opened successfully
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << "'" << endl;
        return false;
    } else { 
        cout << filename << " loaded successfully!" << endl;
    }
 

 // Temporary vector to store courses for validation
 vector<Course> tempCourses;
 string line;
 int lineNumber = 0;
 
 // Read file line by line
 while (getline(file, line)) {
    lineNumber++;

    // Skip empty lines
    if (trim(line).empty()) {
        continue;
    }

    // Parse the line
    vector<string> tokens = split(line, ',');

    // Validate the number of tokens (at least course number and title)
    if (tokens.size() < 2) {
        cerr << "Error: Line " << lineNumber << " - Invalid format (missing course number or title)" << endl;
        cerr << "Line: " << line << endl;
        file.close();
        return false;
    }

    // Create a Course object
    Course course;
    course.courseNumber = toUpperCase(tokens[0]);
    course.courseTitle = tokens[1];

    // Extract prerequisites if they exist (tokens from index 2 onward)
    for (size_t i = 2; i < tokens.size(); ++i) {
        if (!tokens[i].empty()) {
            course.prerequisites.push_back(toUpperCase(tokens[i]));
        }
    }

    tempCourses.push_back(course);
 }

file.close();

// Validate that all prerequisites exist as courses
for (const auto& course : tempCourses) {
    for (const auto& prereq : course.prerequisites) {
        bool found = false;

        // Search for prerequisite in the temporary course list
        for (const auto& existingCourse : tempCourses) {
            if (existingCourse.courseNumber == prereq) {
                found = true;
                break;
            }
        }

        if (!found) {
            cerr << "Error: Prerequisite '" << prereq << "' for course '"
                 << course.courseNumber << "' does not exist in the course list." << endl;
            return false;
        }
    }
}

// All validations passed, insert courses into the hash table
for (const auto& course : tempCourses) {
    hashTable.insert(course);
}

// End timing
auto endTime = high_resolution_clock::now();
clock_t endTicks = clock();

// Calculate elapsed time
auto duration = duration_cast<microseconds>(endTime - startTime);
double seconds = duration.count() / 1000000.0;
clock_t ticks = endTicks - startTicks;

// Display performance metrics
cout << tempCourses.size() << " courses loaded." << endl;
cout << "Time to load: " << ticks << " clock ticks" << endl;
cout << "Time to load: " << fixed << setprecision(6) << seconds << " seconds" << endl;

return true;
}

/**
 * Print all courses in alphanumeric order
 * @param hashTable Reference to the hash table containing courses
 */

void printCourseList(HashTable& hashTable) {
    // Start timing
    auto startTime = high_resolution_clock::now();
    clock_t startTicks = clock();

    // Get all courses from hash table
    vector<Course> courses = hashTable.getAllCourses();
    
    // Check if courses are loaded
    if (courses.empty()) {
        cerr << "No courses loaded. Please load data first (Option 1)." << endl;
        return;
    } else {
        cout << "Courses loaded successfully!" << endl;
    }

    // Sort courses alphanumerically by course number
    sort(courses.begin(), courses.end(), 
        [](const Course& a, const Course& b) {
            return a.courseNumber < b.courseNumber;
        });
        
    // Print header
    cout << "Here is a sample schedule:" << endl;
    cout << endl;

    // Print each course
    for (const auto& course : courses) {
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }

    // End timing
    auto endTime = high_resolution_clock::now();
    clock_t endTicks = clock();

    // Calculate elapsed time
    auto duration = duration_cast<microseconds>(endTime - startTime);
    double seconds = duration.count() / 1000000.0;
    clock_t ticks = endTicks - startTicks;

    cout << "Time to print: " << ticks << " clock ticks" << endl;
    cout << "Time to print: " << fixed << setprecision(6) << seconds << " seconds" << endl;
    cout << endl;
}

/**
 * Search for and print a specific course's information
 * @param hashTable Reference to the hash table containing courses
 * @param courseNumber The course number to search for
 */
void printCourseInformation(HashTable& hashTable, const string& courseNumber) {
    // Start timing
    auto startTime = high_resolution_clock::now();
    clock_t startTicks = clock();

    // Validate input is not empty or whitespace
    string trimmedCourse = trim(courseNumber);
    if (trimmedCourse.empty()) {
        cout << "Error: Course number cannot be empty. Please enter a valid course number." << endl;
        return;
    }

    // Convert to uppercase for case-insensitive search
    string searchNumber = toUpperCase(trimmedCourse);

    // Search for the course
    Course* course = hashTable.search(searchNumber);
    if (course == nullptr) {
        cout << "Error: Course '" << trimmedCourse << "' not found. Please enter a valid course number." << endl;
        return;
    }

    // Print course information
    cout << course->courseNumber << ", " << course->courseTitle << endl;

    // Print prerequisites if they exist
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            cout << course->prerequisites[i];
            if (i < course->prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }

    // End timing
    auto endTime = high_resolution_clock::now();
    clock_t endTicks = clock();

    // Calculate elapsed time
    auto duration = duration_cast<microseconds>(endTime - startTime);
    double seconds = duration.count() / 1000000.0;
    clock_t ticks = endTicks - startTicks;

    cout << "Time to print: " << ticks << " clock ticks" << endl;
    cout << "Time to print: " << fixed << setprecision(6) << seconds << " seconds" << endl;
}

/**
 * Add a new course to the system
 * @param hashTable Reference to the hash table containing courses
 */
void addCourse(HashTable& hashTable) {
    // Start timing
    auto startTime = high_resolution_clock::now();
    clock_t startTicks = clock();

    Course newCourse;
    string input;

    // Get course number
    cout << "Enter course number (e.g., CSCI300): ";
    getline(cin, input);
    newCourse.courseNumber = toUpperCase(trim(input));

    // Validate course number is not empty
    if (newCourse.courseNumber.empty()) {
        cout << "Error: Course number cannot be empty." << endl;
        return;
    }

    // Check if course already exists
    if (hashTable.search(newCourse.courseNumber) != nullptr) {
        cout << "Error: Course " << newCourse.courseNumber << " already exists." << endl;
        return;
    }

    // Get course title
    cout << "Enter course title: ";
    getline(cin, newCourse.courseTitle);

    // Validate course title is not empty
    if (newCourse.courseTitle.empty()) {
        cout << "Error: Course title cannot be empty." << endl;
        return;
    }

    // Get prerequisites
    cout << "Enter prerequisites (comma-separated, leave blank or type 'none' if none): ";
    getline(cin, input);

    string prereqInput = toUpperCase(trim(input));
    if (!prereqInput.empty() && prereqInput != "NONE") {
        vector<string> prereqs = split(input, ',');
        // Validate each prerequisite exists in the hash table
        // If a prerequisite does not exist, show an error and do not add the course
        for (const auto& prereq : prereqs) {
            string prereqNumber = toUpperCase(trim(prereq));
            if (prereqNumber == "NONE" || prereqNumber.empty()) {
                continue;
            }
            if (hashTable.search(prereqNumber) == nullptr) {
                cout << "Error: Prerequisite '" << prereqNumber << "' does not exist in the course list." << endl;
                cout << "Course not added. Please add prerequisites first." << endl;
                return;
            }
            newCourse.prerequisites.push_back(prereqNumber);
        }
    }

    // Add the course to the hash table
    hashTable.insert(newCourse);
    cout << "Course '" << newCourse.courseNumber << "' added successfully!" << endl;
}
/**
 * Remove a course from the system
 * @param hashTable Reference to the hash table containing courses
 */
void removeCourse(HashTable& hashTable) {
    // Start timing
    auto startTime = high_resolution_clock::now();
    clock_t startTicks = clock();
    
    string courseNumber;
    
    cout << "Enter course number to remove: ";
    getline(cin, courseNumber);
    
    string searchNumber = toUpperCase(trim(courseNumber));
    
    // Validate input
    if (searchNumber.empty()) {
        cout << "Error: Course number cannot be empty." << endl;
        return;
    }
    
    // Check if course exists
    Course* course = hashTable.search(searchNumber);
    if (course == nullptr) {
        cout << "Error: Course " << courseNumber << " not found." << endl;
        return;
    }
    
    // Check if this course is a prerequisite for other courses
    vector<Course> allCourses = hashTable.getAllCourses();
    vector<string> dependentCourses;
    
    for (const auto& c : allCourses) {
        for (const auto& prereq : c.prerequisites) {
            if (prereq == searchNumber) {
                dependentCourses.push_back(c.courseNumber);
                break;
            }
        }
    }
    
    // Warn if course is a prerequisite
    if (!dependentCourses.empty()) {
        cout << endl;
        cout << "WARNING: " << searchNumber << " is a prerequisite for:" << endl;
        for (const auto& dep : dependentCourses) {
            cout << "  - " << dep << endl;
        }
        cout << endl;
        cout << "Removing this course will affect these courses." << endl;
        cout << "Are you sure you want to continue? (yes/no): ";
        
        string confirmation;
        getline(cin, confirmation);
        
        if (toUpperCase(trim(confirmation)) != "YES") {
            cout << "Course removal cancelled." << endl;
            return;
        }
    }
    
    // Remove the course
    bool removed = hashTable.remove(searchNumber);
    
    // End timing
    auto endTime = high_resolution_clock::now();
    clock_t endTicks = clock();
    
    // Calculate elapsed time
    auto duration = duration_cast<microseconds>(endTime - startTime);
    double seconds = duration.count() / 1000000.0;
    clock_t ticks = endTicks - startTicks;
    
    if (removed) {
        removePrerequisiteFromAllCourses(hashTable, searchNumber);
        cout << endl;
        cout << "Course " << searchNumber << " removed successfully." << endl;
        cout << "WARNING: All prerequisites referencing this course will be automatically removed from other courses." << endl;
        cout << "Do you want to proceed with prerequisite cleanup? (yes/no): ";
        string cleanupConfirm;
        getline(cin, cleanupConfirm);
        if (toUpperCase(trim(cleanupConfirm)) == "YES") {
            removePrerequisiteFromAllCourses(hashTable, searchNumber);
            cout << "Prerequisite cleanup completed." << endl;
        } else {
            cout << "Prerequisite cleanup skipped. Some courses may still reference this course as a prerequisite." << endl;
        }
        cout << "Time to remove: " << ticks << " clock ticks" << endl;
        cout << "Time to remove: " << fixed << setprecision(6) << seconds << " seconds" << endl;
    } else {
        cout << "Error: Failed to remove course." << endl;
    }
}
// Display the main menu and get user choice
void displayMenu() {
    cout << "1. Load Data File." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "4. Add Course." << endl;
    cout << "5. Remove Course." << endl;
    cout << "9. Exit." << endl;
    cout << endl;
    cout << "What would you like to do? ";
}

//=============================================================================
// Main Function
//============================================================================= 

// Entry point of the program
int main() {
    // Create hash table to store courses
    HashTable courseHashTable;

    // Flag to track if data is loaded
    bool dataLoaded = false;

    int choice =0;
    string filename;
    string courseNumber;

    cout << "Welcome to the ABCU Course Management System!" << endl;
    cout << endl;

    // Main loop for user interaction
    while (choice != 9) {
        displayMenu();

        // Get user input with validation
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number for your menu choice." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Clear input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Process menu choice
        switch (choice) {
            case 1:
                // Load data structure from CSV file
                {
                    std::vector<std::string> csvFiles = getCsvFilesInCurrentDir();
                    if (csvFiles.empty()) {
                        cout << "No CSV files found in the current directory." << endl;
                        break;
                    }
                    cout << "Available CSV files in the current directory:" << endl;
                    for (size_t i = 0; i < csvFiles.size(); ++i) {
                        cout << (i + 1) << ". " << csvFiles[i] << endl;
                    }
                    cout << endl;
                    cout << "Enter the number of the file to load: ";
                    size_t fileChoice = 0;
                    cin >> fileChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (fileChoice < 1 || fileChoice > csvFiles.size()) {
                        cout << "Invalid selection." << endl;
                        break;
                    }
                    filename = csvFiles[fileChoice - 1];
                }
                if (loadCourseData(filename, courseHashTable)) {
                    dataLoaded = true;
                }
                cout << endl;
                break;

            case 2:
                // Print course list
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                } else {
                    printCourseList(courseHashTable);
                }
                break;

            case 3:
                // Print specific course information
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                } else {
                    cout << "What course do you want to know about? ";
                    getline(cin, courseNumber);
                    printCourseInformation(courseHashTable, courseNumber);
                    cout << endl;
                }
                break;
            
            case 4:
                // Add a new course
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                } else {
                    addCourse(courseHashTable);
                    cout << endl;
                }
                break;

            case 5:
                // Remove a course
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                } else {
                    removeCourse(courseHashTable);
                    cout << endl;
                }
                break;
                
            case 9:
                // Exit the program
                cout << "Thank you for using the ABCU Course Management System. Goodbye!" << endl;
                break;

            default:
                // Handle invalid menu choice
                cout << choice << " is not a valid option. Please select 1, 2, 3, or 9." << endl;
                cout << endl;
                break;
                
        }
    }

    return 0;
}