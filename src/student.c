#include "student.h"
#include "memory_manager.h"

// Create a new student database
StudentDatabase* create_database(void) {
    StudentDatabase *db = (StudentDatabase*)safe_malloc(sizeof(StudentDatabase));
    if (!db) {
        return NULL;
    }

    db->students = (Student*)safe_malloc(INITIAL_CAPACITY * sizeof(Student));
    if (!db->students) {
        free(db);
        return NULL;
    }

    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    return db;
}

// Destroy the database and free memory
void destroy_database(StudentDatabase *db) {
    if (db) {
        if (db->students) {
            free(db->students);
            db->students = NULL;
        }
        free(db);
    }
}

// Update student status based on marks
void update_student_status(Student *student) {
    if (student) {
        if (student->marks >= PASSING_MARKS) {
            strcpy(student->status, "Pass");
        } else {
            strcpy(student->status, "Fail");
        }
    }
}

// Check if roll number already exists
bool is_duplicate_roll_number(StudentDatabase *db, int roll_number) {
    if (!db) return false;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].roll_number == roll_number) {
            return true;
        }
    }
    return false;
}

// Add a new student to the database
bool add_student(StudentDatabase *db, const char *name, int roll_number, float marks) {
    if (!db || !name || roll_number <= 0 || marks < 0 || marks > 100) {
        return false;
    }

    // Check for duplicate roll number
    if (is_duplicate_roll_number(db, roll_number)) {
        return false;
    }

    // Ensure capacity
    if (!ensure_capacity(db)) {
        return false;
    }

    // Add the student
    Student *new_student = &db->students[db->count];
    strncpy(new_student->name, name, MAX_NAME_LENGTH - 1);
    new_student->name[MAX_NAME_LENGTH - 1] = '\0';
    new_student->roll_number = roll_number;
    new_student->marks = marks;
    update_student_status(new_student);

    db->count++;
    return true;
}

// Find a student by roll number
Student* find_student(StudentDatabase *db, int roll_number) {
    if (!db) return NULL;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].roll_number == roll_number) {
            return &db->students[i];
        }
    }
    return NULL;
}

// Remove a student from the database
bool remove_student(StudentDatabase *db, int roll_number) {
    if (!db) return false;

    int index = -1;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].roll_number == roll_number) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return false; // Student not found
    }

    // Shift all elements after the removed student
    for (int i = index; i < db->count - 1; i++) {
        db->students[i] = db->students[i + 1];
    }

    db->count--;
    return true;
}

// Modify an existing student's information
bool modify_student(StudentDatabase *db, int roll_number, const char *name, float marks) {
    if (!db || !name || marks < 0 || marks > 100) {
        return false;
    }

    Student *student = find_student(db, roll_number);
    if (!student) {
        return false;
    }

    strncpy(student->name, name, MAX_NAME_LENGTH - 1);
    student->name[MAX_NAME_LENGTH - 1] = '\0';
    student->marks = marks;
    update_student_status(student);

    return true;
}

// Display all students (for console/debug output)
void display_all_students(StudentDatabase *db) {
    if (!db || db->count == 0) {
        printf("No students in the database.\n");
        return;
    }

    printf("\n%-15s %-30s %-10s %-10s\n", "Roll Number", "Name", "Marks", "Status");
    printf("%-15s %-30s %-10s %-10s\n", "----------", "----", "-----", "------");

    for (int i = 0; i < db->count; i++) {
        printf("%-15d %-30s %-10.2f %-10s\n",
               db->students[i].roll_number,
               db->students[i].name,
               db->students[i].marks,
               db->students[i].status);
    }
    printf("\nTotal students: %d\n", db->count);
}

// Get the number of students in the database
int get_student_count(StudentDatabase *db) {
    return db ? db->count : 0;
}

// Clear all students from the database
void clear_database(StudentDatabase *db) {
    if (db) {
        db->count = 0;
    }
}