#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "student.h"

// Enumeration for sorting criteria
typedef enum {
    SORT_BY_MARKS_ASC,
    SORT_BY_MARKS_DESC,
    SORT_BY_NAME_ASC,
    SORT_BY_NAME_DESC,
    SORT_BY_ROLL_NUMBER_ASC,
    SORT_BY_ROLL_NUMBER_DESC
} SortCriteria;

// Statistics structure
typedef struct {
    float average_marks;
    float highest_marks;
    float lowest_marks;
    int total_students;
    int passed_students;
    int failed_students;
    float pass_percentage;
} Statistics;

// Function declarations for calculations and sorting
float calculate_average_marks(StudentDatabase *db);
Statistics calculate_statistics(StudentDatabase *db);
void sort_students(StudentDatabase *db, SortCriteria criteria);
Student* find_highest_scorer(StudentDatabase *db);
Student* find_lowest_scorer(StudentDatabase *db);
int count_passed_students(StudentDatabase *db);
int count_failed_students(StudentDatabase *db);
float calculate_pass_percentage(StudentDatabase *db);

// Helper functions for sorting
void quicksort_marks(Student *arr, int low, int high, bool ascending);
int partition_marks(Student *arr, int low, int high, bool ascending);
void quicksort_roll_numbers(Student *arr, int low, int high, bool ascending);
int partition_roll_numbers(Student *arr, int low, int high, bool ascending);
void bubble_sort_names(Student *arr, int n, bool ascending);
void swap_students(Student *a, Student *b);

#endif // CALCULATIONS_H