//=======================================================================
// Building and Installing
//=======================================================================

For minimal installation (without additional features; baseline project
requirements), use:

Windows:
g++ -o CourseManagement.exe CourseManagement_minimal.cpp -mconsole

Linux:
g++ -o CourseManagement CourseManagement_minimal.cpp

For the latest stable release, use CourseManagement_v1.2.cpp instead
of CourseManagement_minimal.cpp:

Windows:
g++ -o CourseManagement.exe CourseManagement_v1.2.cpp -mconsole

Linux:
g++ -o CourseManagement CourseManagement_v1.2.cpp

//=======================================================================
// Build Options and Versions
//=======================================================================

Minimal / Baseline Version (CourseManagement_minimal.cpp)
Targets the core CS-300 project requirements. This build is static:
the in-memory catalog is read-only during runtime (no add/remove).

Key characteristics:
- Loads course data from a CSV file into a hash table
- Prints the full course list in alphanumeric order
- Prints detailed information for a single course, including
prerequisites

Menu options:
1. Load Data Structure
2. Print Course List
3. Print Course
9. Exit

Original Interactive Prototype (CourseManagement.cpp)
An intermediate step that adds timing and improved validation but does
NOT yet support adding/removing courses from the menu.

Key characteristics:
- Same hash-table core and CSV-loading logic as the minimal version
- Measures and prints performance metrics (clock ticks and seconds)
for loading, printing the course list, and printing course info
- Input is case-insensitive for lookups (course numbers converted
to uppercase)
- Improved validation and error messages for malformed CSV files

Menu options:
1. Load Data Structure
2. Print Course List
3. Print Course
9. Exit

Use this version if you want performance metrics and better robustness
but do not need interactive add/remove yet.

Latest Stable Release (CourseManagement_v1.2.cpp) [RECOMMENDED]
In addition to everything in the minimal and prototype versions, this
release adds full dynamic course management and improved file selection.

Additional key characteristics:
- Auto directory scan with numbered CSV file selection
- Interactive add and remove of courses at runtime
- Prerequisite validation on add; dependency warning on remove
- Optional prerequisite cleanup after course removal
- Timing metrics for all five menu operations

Menu options:
1. Load Data Structure
2. Print Course List
3. Print Course
4. Add Course              (NEW)
5. Remove Course           (NEW)
9. Exit

//=======================================================================
// NEW FEATURES OVERVIEW  (v1.2 only)
//=======================================================================

The system now supports dynamic course management with two new menu
options:

OPTION 4: ADD COURSE

Add new courses to the system interactively
Validate that all prerequisites exist before adding
Prevent duplicate course numbers
Display performance metrics
OPTION 5: REMOVE COURSE

Remove courses from the system
Check for dependent courses (courses that require this as a prerequisite)
Warn and confirm before removing if dependencies exist
Optionally strip removed course from other courses' prerequisite lists
Display performance metrics
//=======================================================================
// OPTION 4: ADD COURSE - DETAILED GUIDE
//=======================================================================

PURPOSE:
Add new courses to the in-memory catalog dynamically without editing
the CSV file.

WORKFLOW:

Select option 4 from the menu.
Enter course number (normalized to uppercase, whitespace trimmed).
Enter course title.
Enter prerequisites (comma-separated), or press Enter for none.
VALIDATION:

Course number cannot be empty
Course number must be unique (no duplicates)
Course title cannot be empty
All prerequisites must already exist in the system; if any are missing the add is aborted and the data structure is unchanged
EXAMPLE SESSION:
What would you like to do? 4
Enter course number: CSCI500
Enter course title: Advanced Data Structures
Enter prerequisites (comma-separated, or press Enter for none): CSCI200,CSCI300

Course CSCI500 added successfully. Time to add: 0 clock ticks Time to add: 0.000022 seconds
FEATURES:

Case-insensitive input (csci500 → CSCI500)
Automatic whitespace trimming
Immediate prerequisite validation
Performance timing display

//=======================================================================
// OPTION 5: REMOVE COURSE - DETAILED GUIDE
//=======================================================================

PURPOSE:
Remove courses from the system when they are no longer offered or
needed.

WORKFLOW:

Select option 5 from the menu.
Enter the course number to remove (case-insensitive, trimmed).
Program checks whether the course exists; prints error if not.
Program scans all courses to see if this course is listed as a prerequisite.
If dependents exist, it prints a warning and lists affected courses.
User confirms removal by typing "yes"; anything else cancels.
If removal succeeds, the program offers optional prerequisite cleanup:
If confirmed, all references to the removed course are stripped from other courses' prerequisite lists.
If skipped, dependent courses may still reference the removed course.
EXAMPLE SESSION (WITH DEPENDENCIES):
What would you like to do? 5
Enter course number to remove: CSCI100


//=======================================================================
// USE CASES AND EXAMPLES
//=======================================================================

USE CASE 1: ADDING A NEW ELECTIVE COURSE
Scenario: Department wants to offer a new elective course.

Steps:

Load existing data (Option 1).
Add new course (Option 4): Course: CSCI490 Title: Special Topics in Computer Science Prerequisites: CSCI200
Verify addition (Option 2 - see it in the list).
Check details (Option 3 with CSCI490).
Result: New elective is immediately available in the system for advising.

USE CASE 2: ADDING A COURSE WITH MULTIPLE PREREQUISITES
Scenario: Adding an advanced course that requires multiple background
courses.

Steps:

Load data (Option 1).
Verify prerequisites exist (Option 3 for CSCI301, CSCI350).
Add new course (Option 4): Course: CSCI410 Title: Software Engineering Project Prerequisites: CSCI301,CSCI350
Verify (Option 3 with CSCI410 shows both prerequisites).
USE CASE 3: REMOVING A DEPRECATED COURSE
Scenario: Course is no longer offered and has no dependencies.

Steps:

Load data (Option 1).
Try Option 5; if no dependency warning appears, confirm removal.
Verify removal (Option 2 should no longer show the course).
USE CASE 4: ATTEMPTING TO REMOVE A PREREQUISITE COURSE
Scenario: Try to remove a foundational course.

Steps:

Load data (Option 1).
Try to remove (Option 5 with CSCI100).
System warns: "CSCI100 is a prerequisite for: CSCI101".
Decision point:
Type "no" to cancel and keep the course.
Type "yes" to remove anyway, then optionally clean up prerequisite references.
USE CASE 5: BUILDING A COURSE SEQUENCE
Scenario: Adding multiple related courses in sequence.

Steps:

Load data (Option 1).
Add foundation course (Option 4: CSCI510, no prerequisites).
Add intermediate course (Option 4: CSCI520, prerequisite CSCI510).
Add advanced course (Option 4: CSCI530, prerequisite CSCI520).
Print list (Option 2) to see the complete sequence.
Test search (Option 3) on each to verify prerequisites.
//=======================================================================
// DATA INTEGRITY AND VALIDATION
//=======================================================================

VALIDATION RULES:

ADD COURSE:
Course number must be unique and non-empty.
Course title must be non-empty.
All prerequisites must exist before insertion.
Failed validation does not modify the data structure.
REMOVE COURSE:
Course must exist in the system.
User must confirm if course has dependents.
Removal is atomic: either fully succeeds or not at all.
Optional global cleanup removes all stale prerequisite references.
GENERAL:
Operations are designed to be atomic with no partial state changes on error.
Memory for removed nodes is properly deallocated (no leaks).

//=======================================================================
// FUTURE ENHANCEMENTS (NOT YET IMPLEMENTED)
//=======================================================================

Potential additions for future versions:
- Edit course title without removing/re-adding
- Modify prerequisites without removing the course
- Bulk add courses from a file
- Export modified course list back to CSV
- Undo last add/remove operation
- Course history / audit log
- Automatic prerequisite cleanup on removal (currently optional)
