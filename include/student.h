#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define MAX_NAME_LENGTH 100
#define MAX_STATUS_LENGTH 10
#define INITIAL_CAPACITY 10
#define PASSING_MARKS 40.0

// Student structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int roll_number;
    float marks;
    char status[MAX_STATUS_LENGTH];
} Student;

// Dynamic student database structure
typedef struct {
    Student *students;
    int count;
    int capacity;
} StudentDatabase;

// Function declarations for student operations
StudentDatabase* create_database(void);
void destroy_database(StudentDatabase *db);
bool add_student(StudentDatabase *db, const char *name, int roll_number, float marks);
bool remove_student(StudentDatabase *db, int roll_number);
bool modify_student(StudentDatabase *db, int roll_number, const char *name, float marks);
Student* find_student(StudentDatabase *db, int roll_number);
void display_all_students(StudentDatabase *db);
bool is_duplicate_roll_number(StudentDatabase *db, int roll_number);
void update_student_status(Student *student);
int get_student_count(StudentDatabase *db);
void clear_database(StudentDatabase *db);

#endif // STUDENT_H