# Student Record System

![School Record System](./data/srs.png)


## Project Overview

A comprehensive Student Record Management System built in C with GTK GUI framework. This application provides a user-friendly interface for managing student information including adding, modifying, deleting, searching, and analyzing student records with persistent file storage.

## Features

### Core Functionality
- **Student Registration**: Add new students with name, roll number, and marks
- **Record Management**: View, modify, and delete existing student records
- **Search System**: Quick search functionality by roll number
- **Pass/Fail Analysis**: Automatic determination based on configurable threshold (default: 40 marks)

### Advanced Features
- **File Persistence**: Save and load records from text files
- **Statistical Analysis**: Calculate and display average marks for all students
- **Sorting Capabilities**: Sort records by marks (ascending/descending) or by name
- **Dynamic Memory Management**: Efficient memory allocation and deallocation
- **User Authentication**: Personalized welcome system with user name input

### GUI Components
- Modern GTK-based graphical user interface
- Intuitive navigation with menu system
- Data entry forms with validation
- Tabular display of student records
- Dialog boxes for confirmations and alerts

## Project Structure

```
StudentRecordSystem/
├── src/
│   ├── main.c              # Main application entry point
│   ├── student.c           # Student data structure and operations
│   ├── gui.c               # GTK GUI implementation
│   ├── file_operations.c   # File I/O operations
│   ├── calculations.c      # Statistical calculations and sorting
│   └── memory_manager.c    # Dynamic memory management
│   data/
│   └── students.txt        # Default data file for student records
├── include/
│   ├── student.h           # Student structure and function declarations
│   ├── gui.h               # GUI function declarations
│   ├── file_operations.h   # File operation declarations
│   ├── calculations.h      # Calculation function declarations
│   └── memory_manager.h    # Memory management declarations
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
└── .gitignore             # Git ignore file
```

## Technical Specifications

### Development Environment
- **IDE**: JetBrains CLion
- **Language**: C (C99 standard)
- **GUI Framework**: GTK 3.0
- **Platform**: macOS (MacBook Pro 2017)
- **Build System**: CMake

### System Requirements
- macOS 10.12 or later
- GTK 3.0 development libraries
- CMake 3.10 or later
- Xcode Command Line Tools

### Installation Prerequisites

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install GTK and CMake
brew install gtk+3 cmake pkg-config

# Install Xcode Command Line Tools
xcode-select --install
```

## Data Structures

### Student Record
```c
typedef struct {
    char name[100];
    int roll_number;
    float marks;
    char status[10];  // "Pass" or "Fail"
} Student;

typedef struct {
    Student *students;
    int count;
    int capacity;
} StudentDatabase;
```

## Key Algorithms

### Memory Management
- Dynamic array resizing with realloc()
- Automatic memory cleanup on program exit
- Memory leak prevention with proper free() calls

### Sorting Algorithms
- Quick Sort implementation for marks-based sorting
- Bubble Sort for name-based sorting
- Configurable ascending/descending order

### File Format
```
Student Name,Roll Number,Marks,Status
John Doe,101,85.5,Pass
Jane Smith,102,32.0,Fail
```

## Complexity Analysis

### Time Complexity
- **Add Student**: O(1) amortized
- **Search by Roll Number**: O(n) linear search
- **Sort by Marks**: O(n log n) using quicksort
- **Calculate Average**: O(n)
- **File Operations**: O(n) for reading/writing all records

### Space Complexity
- **Memory Usage**: O(n) where n is the number of students
- **Dynamic Allocation**: Grows/shrinks based on actual usage
- **File Storage**: Linear growth with number of records

## Build Instructions

### Using CLion
1. Open CLion and select "Open Project"
2. Navigate to the project directory and select CMakeLists.txt
3. CLion will automatically configure the project
4. Build using Ctrl+F9 (Cmd+F9 on Mac)
5. Run using Shift+F10

### Using Command Line
```bash
mkdir build
cd build
cmake ..
make
./StudentRecordSystem
```

## Usage Instructions

1. **First Run**: Enter your name when prompted for personalized experience
2. **Add Students**: Use the "Add Student" button to register new students
3. **View Records**: Browse all records in the main table view
4. **Search**: Use roll number to quickly find specific students
5. **Sort**: Choose sorting criteria from the menu
6. **Save/Load**: Records are automatically saved; use File menu for manual operations
7. **Statistics**: View average marks and pass/fail statistics

## Error Handling

- Input validation for all user entries
- Memory allocation failure handling
- File I/O error management
- Invalid roll number detection
- Duplicate entry prevention

## Future Enhancements

- Grade point average (GPA) calculations
- Student photo management
- Export to Excel/PDF formats
- Advanced search with multiple criteria
- Backup and restore functionality
- Multi-user support with access control

## Authors

**Wisdom Chimezie**
- Primary Developer
- Email: [wisdom.chimezie@miva.edu.ng]
- GitHub: [Wisdom Alex]

## License

This project is created for educational purposes. Feel free to use and modify for learning and academic projects.

## Acknowledgments

- GTK Development Team for the excellent GUI framework
- JetBrains for CLion IDE
- Stack Overflow community for troubleshooting assistance

## Version History

- **v1.0.0** - Initial release with core functionality
- **v1.1.0** - Added GUI interface
- **v1.2.0** - Implemented file operations and sorting
- **v1.3.0** - Enhanced memory management and error handling

---

*Last Updated: July 19, 2025*