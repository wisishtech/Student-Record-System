/*
 * File: src/student.c
 * Path: SchoolRecordSystem/src/student.c
 * Description: Enhanced student data management with course support
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 */

#include "student.h"
#include "memory_manager.h"

/* ============================================================================
 * DATABASE MANAGEMENT FUNCTIONS
 * ============================================================================ */

/**
 * Create a new student database with initial capacity
 * @return Pointer to new database or NULL on failure
 */
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

    // Initialize database properties
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    db->has_unsaved_changes = false;
    strcpy(db->last_saved_file, "");

    // Initialize all student records as inactive
    for (int i = 0; i < INITIAL_CAPACITY; i++) {
        db->students[i].is_active = false;
        db->students[i].course_count = 0;

        // Initialize all course slots as inactive
        for (int j = 0; j < MAX_COURSES; j++) {
            db->students[i].courses[j].is_active = false;
        }
    }

    printf("Database created successfully with capacity: %d\n", INITIAL_CAPACITY);
    return db;
}

/**
 * Destroy the database and free all allocated memory
 * @param db Database to destroy
 */
void destroy_database(StudentDatabase *db) {
    if (db) {
        if (db->students) {
            printf("Freeing memory for %d students\n", db->count);
            free(db->students);
            db->students = NULL;
        }
        free(db);
        printf("Database destroyed successfully\n");
    }
}

/* ============================================================================
 * STUDENT MANAGEMENT FUNCTIONS
 * ============================================================================ */

/**
 * Update student status based on overall marks
 * @param student Student to update
 */
void update_student_status(Student *student) {
    if (!student) return;

    if (student->marks >= PASSING_MARKS) {
        strcpy(student->status, "Pass");
    } else {
        strcpy(student->status, "Fail");
    }
}

/**
 * Check if roll number already exists in database
 * @param db Database to check
 * @param roll_number Roll number to check
 * @return true if duplicate found, false otherwise
 */
bool is_duplicate_roll_number(StudentDatabase *db, int roll_number) {
    if (!db) return false;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].roll_number == roll_number) {
            return true;
        }
    }
    return false;
}

/**
 * Add a new student to the database
 * @param db Database to add to
 * @param name Student name
 * @param roll_number Student roll number
 * @param marks Student marks
 * @return StudentError code
 */
StudentError add_student(StudentDatabase *db, const char *name, int roll_number, float marks) {
    // Input validation
    if (!db || !name) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    if (!is_valid_roll_number(roll_number)) {
        return STUDENT_ERROR_INVALID_ROLL_NUMBER;
    }

    if (!is_valid_marks(marks)) {
        return STUDENT_ERROR_INVALID_MARKS;
    }

    if (!is_valid_name(name)) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    // Check for duplicate roll number
    if (is_duplicate_roll_number(db, roll_number)) {
        return STUDENT_ERROR_DUPLICATE_ROLL_NUMBER;
    }

    // Ensure capacity
    if (!ensure_capacity(db)) {
        return STUDENT_ERROR_MEMORY_ALLOCATION;
    }

    // Add the student
    Student *new_student = &db->students[db->count];

    // Initialize student data
    strncpy(new_student->name, name, MAX_NAME_LENGTH - 1);
    new_student->name[MAX_NAME_LENGTH - 1] = '\0';
    new_student->roll_number = roll_number;
    new_student->marks = marks;
    new_student->course_count = 0;
    new_student->gpa = 0.0f;
    new_student->is_active = true;

    // Initialize all courses as inactive
    for (int i = 0; i < MAX_COURSES; i++) {
        new_student->courses[i].is_active = false;
        strcpy(new_student->courses[i].course_name, "");
        new_student->courses[i].course_score = 0.0f;
        strcpy(new_student->courses[i].grade_letter, "");
        strcpy(new_student->courses[i].status, "");
    }

    update_student_status(new_student);
    db->count++;
    mark_database_changed(db);

    printf("Student added successfully: %s (Roll: %d, Marks: %.2f)\n",
           name, roll_number, marks);

    return STUDENT_SUCCESS;
}

/**
 * Find a student by roll number
 * @param db Database to search
 * @param roll_number Roll number to find
 * @return Pointer to student or NULL if not found
 */
Student* find_student(StudentDatabase *db, int roll_number) {
    if (!db) return NULL;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].roll_number == roll_number) {
            return &db->students[i];
        }
    }
    return NULL;
}

/**
 * Remove a student from the database
 * @param db Database to remove from
 * @param roll_number Roll number of student to remove
 * @return StudentError code
 */
StudentError remove_student(StudentDatabase *db, int roll_number) {
    if (!db) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    int index = -1;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].roll_number == roll_number) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return STUDENT_ERROR_STUDENT_NOT_FOUND;
    }

    // Shift all elements after the removed student
    for (int i = index; i < db->count - 1; i++) {
        db->students[i] = db->students[i + 1];
    }

    // Mark the last slot as inactive
    db->students[db->count - 1].is_active = false;
    db->count--;
    mark_database_changed(db);

    printf("Student with roll number %d removed successfully\n", roll_number);
    return STUDENT_SUCCESS;
}

/**
 * Modify an existing student's information
 * @param db Database containing the student
 * @param roll_number Roll number of student to modify
 * @param name New name
 * @param marks New marks
 * @return StudentError code
 */
StudentError modify_student(StudentDatabase *db, int roll_number, const char *name, float marks) {
    if (!db || !name) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    if (!is_valid_marks(marks)) {
        return STUDENT_ERROR_INVALID_MARKS;
    }

    if (!is_valid_name(name)) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    Student *student = find_student(db, roll_number);
    if (!student) {
        return STUDENT_ERROR_STUDENT_NOT_FOUND;
    }

    // Update student information
    strncpy(student->name, name, MAX_NAME_LENGTH - 1);
    student->name[MAX_NAME_LENGTH - 1] = '\0';
    student->marks = marks;

    // Recalculate overall status
    calculate_overall_marks(student);
    update_student_status(student);
    mark_database_changed(db);

    printf("Student updated successfully: %s (Roll: %d, Marks: %.2f)\n",
           name, roll_number, marks);

    return STUDENT_SUCCESS;
}

/* ============================================================================
 * COURSE MANAGEMENT FUNCTIONS
 * ============================================================================ */

/**
 * Add a course to a student's record
 * @param db Database containing the student
 * @param roll_number Student's roll number
 * @param course_name Name of the course
 * @param course_score Score in the course
 * @return StudentError code
 */
StudentError add_course_to_student(StudentDatabase *db, int roll_number,
                                 const char *course_name, float course_score) {
    if (!db || !course_name) {
        return STUDENT_ERROR_NULL_POINTER;
    }

    if (!is_valid_course_name(course_name)) {
        return STUDENT_ERROR_INVALID_COURSE_NAME;
    }

    if (!is_valid_marks(course_score)) {
        return STUDENT_ERROR_INVALID_MARKS;
    }

    Student *student = find_student(db, roll_number);
    if (!student) {
        return STUDENT_ERROR_STUDENT_NOT_FOUND;
    }

    if (student->course_count >= MAX_COURSES) {
        return STUDENT_ERROR_MAX_COURSES_REACHED;
    }

    // Check if course already exists
    if (find_course_in_student(student, course_name)) {
        return STUDENT_ERROR_DUPLICATE_ROLL_NUMBER; // Reusing error code for duplicate course
    }

    // Find first inactive course slot
    int course_index = -1;
    for (int i = 0; i < MAX_COURSES; i++) {
        if (!student->courses[i].is_active) {
            course_index = i;
            break;
        }
    }

    if (course_index == -1) {
        return STUDENT_ERROR_MAX_COURSES_REACHED;
    }

    // Add the course
    Course *new_course = &student->courses[course_index];
    strncpy(new_course->course_name, course_name, MAX_COURSE_NAME_LENGTH - 1);
    new_course->course_name[MAX_COURSE_NAME_LENGTH - 1] = '\0';
    new_course->course_score = course_score;
    strcpy(new_course->grade_letter, calculate_grade_letter(course_score));
    new_course->is_active = true;

    update_course_status(new_course);
    student->course_count++;

    // Recalculate student's overall performance
    calculate_student_gpa(student);
    calculate_overall_marks(student);
    update_student_status(student);
    mark_database_changed(db);

    printf("Course added successfully: %s (Score: %.2f, Grade: %s)\n",
           course_name, course_score, new_course->grade_letter);

    return STUDENT_SUCCESS;
}

/**
 * Find a course in a student's record
 * @param student Student to search
 * @param course_name Course name to find
 * @return Pointer to course or NULL if not found
 */
Course* find_course_in_student(Student *student, const char *course_name) {
    if (!student || !course_name) return NULL;

    for (int i = 0; i < MAX_COURSES; i++) {
        if (student->courses[i].is_active &&
            strcmp(student->courses[i].course_name, course_name) == 0) {
            return &student->courses[i];
        }
    }
    return NULL;
}

/**
 * Calculate student's GPA based on all courses
 * @param student Student to calculate GPA for
 */
void calculate_student_gpa(Student *student) {
    if (!student || student->course_count == 0) {
        if (student) student->gpa = 0.0f;
        return;
    }

    float total_points = 0.0f;
    int active_courses = 0;

    for (int i = 0; i < MAX_COURSES; i++) {
        if (student->courses[i].is_active) {
            // Convert percentage to GPA points (0-4 scale)
            float points = (student->courses[i].course_score / 100.0f) * 4.0f;
            total_points += points;
            active_courses++;
        }
    }

    student->gpa = (active_courses > 0) ? (total_points / active_courses) : 0.0f;
}

/**
 * Calculate overall marks as average of all courses
 * @param student Student to calculate overall marks for
 */
void calculate_overall_marks(Student *student) {
    if (!student) return;

    if (student->course_count == 0) {
        // Keep original marks if no courses added
        return;
    }

    float total_marks = 0.0f;
    int active_courses = 0;

    for (int i = 0; i < MAX_COURSES; i++) {
        if (student->courses[i].is_active) {
            total_marks += student->courses[i].course_score;
            active_courses++;
        }
    }

    if (active_courses > 0) {
        student->marks = total_marks / active_courses;
    }
}

/* ============================================================================
 * GRADE CALCULATION FUNCTIONS
 * ============================================================================ */

/**
 * Calculate grade letter based on score
 * @param score Numerical score
 * @return Grade letter (A-F)
 */
char* calculate_grade_letter(float score) {
    static char grade[3];

    if (score >= 90.0f) strcpy(grade, "A");
    else if (score >= 80.0f) strcpy(grade, "B");
    else if (score >= 70.0f) strcpy(grade, "C");
    else if (score >= 60.0f) strcpy(grade, "D");
    else if (score >= 40.0f) strcpy(grade, "E");
    else strcpy(grade, "F");

    return grade;
}

/**
 * Check if a course score is passing
 * @param score Course score to check
 * @return true if passing, false otherwise
 */
bool is_course_passing(float score) {
    return score >= PASSING_MARKS;
}

/**
 * Update course status based on score
 * @param course Course to update
 */
void update_course_status(Course *course) {
    if (!course) return;

    if (is_course_passing(course->course_score)) {
        strcpy(course->status, "Pass");
    } else {
        strcpy(course->status, "Fail");
    }
}

/* ============================================================================
 * VALIDATION FUNCTIONS
 * ============================================================================ */

/**
 * Validate roll number
 * @param roll_number Roll number to validate
 * @return true if valid, false otherwise
 */
bool is_valid_roll_number(int roll_number) {
    return roll_number > 0 && roll_number <= 999999; // Reasonable range
}

/**
 * Validate marks/score
 * @param marks Marks to validate
 * @return true if valid, false otherwise
 */
bool is_valid_marks(float marks) {
    return marks >= 0.0f && marks <= 100.0f;
}

/**
 * Validate name
 * @param name Name to validate
 * @return true if valid, false otherwise
 */
bool is_valid_name(const char *name) {
    if (!name || strlen(name) == 0 || strlen(name) >= MAX_NAME_LENGTH) {
        return false;
    }

    // Check for valid characters (letters, spaces, hyphens, apostrophes)
    for (size_t i = 0; i < strlen(name); i++) {
        char c = name[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              c == ' ' || c == '-' || c == '\'')) {
            return false;
        }
    }

    return true;
}

/**
 * Validate course name
 * @param course_name Course name to validate
 * @return true if valid, false otherwise
 */
bool is_valid_course_name(const char *course_name) {
    if (!course_name || strlen(course_name) == 0 || strlen(course_name) >= MAX_COURSE_NAME_LENGTH) {
        return false;
    }
    return true;
}

/* ============================================================================
 * ERROR HANDLING FUNCTIONS
 * ============================================================================ */

/**
 * Get error message for error code
 * @param error Error code
 * @return Error message string
 */
const char* get_error_message(StudentError error) {
    switch (error) {
        case STUDENT_SUCCESS:
            return "Operation completed successfully";
        case STUDENT_ERROR_NULL_POINTER:
            return "Invalid input: NULL pointer provided";
        case STUDENT_ERROR_INVALID_ROLL_NUMBER:
            return "Invalid roll number: Must be positive and within range";
        case STUDENT_ERROR_INVALID_MARKS:
            return "Invalid marks: Must be between 0 and 100";
        case STUDENT_ERROR_DUPLICATE_ROLL_NUMBER:
            return "Duplicate roll number: Roll number already exists";
        case STUDENT_ERROR_STUDENT_NOT_FOUND:
            return "Student not found with the specified roll number";
        case STUDENT_ERROR_MEMORY_ALLOCATION:
            return "Memory allocation failed";
        case STUDENT_ERROR_INVALID_COURSE_NAME:
            return "Invalid course name provided";
        case STUDENT_ERROR_MAX_COURSES_REACHED:
            return "Maximum number of courses reached for this student";
        case STUDENT_ERROR_COURSE_NOT_FOUND:
            return "Course not found for the specified student";
        default:
            return "Unknown error occurred";
    }
}

/**
 * Print error message
 * @param error Error code
 */
void print_error(StudentError error) {
    if (error != STUDENT_SUCCESS) {
        fprintf(stderr, "Error: %s\n", get_error_message(error));
    }
}

/* ============================================================================
 * DATABASE STATE MANAGEMENT
 * ============================================================================ */

/**
 * Mark database as having unsaved changes
 * @param db Database to mark
 */
void mark_database_changed(StudentDatabase *db) {
    if (db) {
        db->has_unsaved_changes = true;
    }
}

/**
 * Check if database has unsaved changes
 * @param db Database to check
 * @return true if has unsaved changes, false otherwise
 */
bool has_unsaved_changes(StudentDatabase *db) {
    return db ? db->has_unsaved_changes : false;
}

/**
 * Mark database as saved
 * @param db Database to mark
 */
void mark_database_saved(StudentDatabase *db) {
    if (db) {
        db->has_unsaved_changes = false;
    }
}

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * Display all students (for console/debug output)
 * @param db Database to display
 */
void display_all_students(StudentDatabase *db) {
    if (!db || db->count == 0) {
        printf("No students in the database.\n");
        return;
    }

    printf("\n%-15s %-30s %-10s %-10s %-10s %-10s\n",
           "Roll Number", "Name", "Marks", "Status", "Courses", "GPA");
    printf("%-15s %-30s %-10s %-10s %-10s %-10s\n",
           "----------", "----", "-----", "------", "-------", "---");

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            printf("%-15d %-30s %-10.2f %-10s %-10d %-10.2f\n",
                   db->students[i].roll_number,
                   db->students[i].name,
                   db->students[i].marks,
                   db->students[i].status,
                   db->students[i].course_count,
                   db->students[i].gpa);
        }
    }
    printf("\nTotal active students: %d\n", db->count);
}

/**
 * Get the number of students in the database
 * @param db Database to count
 * @return Number of students
 */
int get_student_count(StudentDatabase *db) {
    return db ? db->count : 0;
}

/**
 * Clear all students from the database
 * @param db Database to clear
 */
void clear_database(StudentDatabase *db) {
    if (db) {
        // Mark all students as inactive
        for (int i = 0; i < db->count; i++) {
            db->students[i].is_active = false;
            db->students[i].course_count = 0;

            // Mark all courses as inactive
            for (int j = 0; j < MAX_COURSES; j++) {
                db->students[i].courses[j].is_active = false;
            }
        }

        db->count = 0;
        mark_database_changed(db);
        printf("Database cleared successfully\n");
    }
}