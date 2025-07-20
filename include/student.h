/*
 * File: include/student.h
 * Path: SchoolRecordSystem/include/student.h
 * Description: Enhanced student data structures and function declarations
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define MAX_NAME_LENGTH 100
#define MAX_COURSE_NAME_LENGTH 50
#define MAX_STATUS_LENGTH 10
#define MAX_GRADE_LENGTH 3
#define INITIAL_CAPACITY 10
#define MAX_COURSES 10
#define PASSING_MARKS 40.0

// Course structure for individual course records
typedef struct {
    char course_name[MAX_COURSE_NAME_LENGTH];
    float course_score;
    char grade_letter[MAX_GRADE_LENGTH];  // A, B, C, D, E, F
    char status[MAX_STATUS_LENGTH];       // Pass or Fail
    bool is_active;                       // To track if course slot is used
} Course;

// Enhanced Student structure with course information
typedef struct {
    char name[MAX_NAME_LENGTH];
    int roll_number;
    float marks;                          // Overall average marks
    char status[MAX_STATUS_LENGTH];       // Overall Pass/Fail status
    Course courses[MAX_COURSES];          // Array of courses
    int course_count;                     // Number of active courses
    float gpa;                           // Grade Point Average
    bool is_active;                      // To track if student record is active
} Student;

// Dynamic student database structure
typedef struct {
    Student *students;
    int count;
    int capacity;
    char last_saved_file[256];           // Track last saved file path
    bool has_unsaved_changes;            // Track if changes need saving
} StudentDatabase;

// Error codes for better error handling
typedef enum {
    STUDENT_SUCCESS = 0,
    STUDENT_ERROR_NULL_POINTER,
    STUDENT_ERROR_INVALID_ROLL_NUMBER,
    STUDENT_ERROR_INVALID_MARKS,
    STUDENT_ERROR_DUPLICATE_ROLL_NUMBER,
    STUDENT_ERROR_STUDENT_NOT_FOUND,
    STUDENT_ERROR_MEMORY_ALLOCATION,
    STUDENT_ERROR_INVALID_COURSE_NAME,
    STUDENT_ERROR_MAX_COURSES_REACHED,
    STUDENT_ERROR_COURSE_NOT_FOUND
} StudentError;

// Function declarations for student operations
StudentDatabase* create_database(void);
void destroy_database(StudentDatabase *db);

// Basic student operations
StudentError add_student(StudentDatabase *db, const char *name, int roll_number, float marks);
StudentError remove_student(StudentDatabase *db, int roll_number);
StudentError modify_student(StudentDatabase *db, int roll_number, const char *name, float marks);
Student* find_student(StudentDatabase *db, int roll_number);
void display_all_students(StudentDatabase *db);
bool is_duplicate_roll_number(StudentDatabase *db, int roll_number);
void update_student_status(Student *student);
int get_student_count(StudentDatabase *db);
void clear_database(StudentDatabase *db);

// Enhanced course operations
StudentError add_course_to_student(StudentDatabase *db, int roll_number,
                                 const char *course_name, float course_score);
StudentError remove_course_from_student(StudentDatabase *db, int roll_number,
                                       const char *course_name);
StudentError modify_course_score(StudentDatabase *db, int roll_number,
                                const char *course_name, float new_score);
Course* find_course_in_student(Student *student, const char *course_name);
void calculate_student_gpa(Student *student);
void calculate_overall_marks(Student *student);

// Grade calculation functions
char* calculate_grade_letter(float score);
bool is_course_passing(float score);
void update_course_status(Course *course);

// Validation functions
bool is_valid_roll_number(int roll_number);
bool is_valid_marks(float marks);
bool is_valid_name(const char *name);
bool is_valid_course_name(const char *course_name);

// Error handling functions
const char* get_error_message(StudentError error);
void print_error(StudentError error);

// Database state management
void mark_database_changed(StudentDatabase *db);
bool has_unsaved_changes(StudentDatabase *db);
void mark_database_saved(StudentDatabase *db);

#endif // STUDENT_H