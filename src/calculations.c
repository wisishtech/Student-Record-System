#include "calculations.h"

// Calculate average marks for all students
float calculate_average_marks(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    float total = 0.0f;
    for (int i = 0; i < db->count; i++) {
        total += db->students[i].marks;
    }

    return total / db->count;
}

// Calculate comprehensive statistics
Statistics calculate_statistics(StudentDatabase *db) {
    Statistics stats = {0};

    if (!db || db->count == 0) {
        return stats;
    }

    stats.total_students = db->count;
    stats.highest_marks = db->students[0].marks;
    stats.lowest_marks = db->students[0].marks;

    float total_marks = 0.0f;

    for (int i = 0; i < db->count; i++) {
        float marks = db->students[i].marks;
        total_marks += marks;

        if (marks > stats.highest_marks) {
            stats.highest_marks = marks;
        }
        if (marks < stats.lowest_marks) {
            stats.lowest_marks = marks;
        }

        if (marks >= PASSING_MARKS) {
            stats.passed_students++;
        } else {
            stats.failed_students++;
        }
    }

    stats.average_marks = total_marks / db->count;
    stats.pass_percentage = (stats.total_students > 0) ?
                           ((float)stats.passed_students / stats.total_students) * 100.0f : 0.0f;

    return stats;
}

// Find student with highest marks
Student* find_highest_scorer(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return NULL;
    }

    Student *highest = &db->students[0];
    for (int i = 1; i < db->count; i++) {
        if (db->students[i].marks > highest->marks) {
            highest = &db->students[i];
        }
    }

    return highest;
}

// Find student with lowest marks
Student* find_lowest_scorer(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return NULL;
    }

    Student *lowest = &db->students[0];
    for (int i = 1; i < db->count; i++) {
        if (db->students[i].marks < lowest->marks) {
            lowest = &db->students[i];
        }
    }

    return lowest;
}

// Count students who passed
int count_passed_students(StudentDatabase *db) {
    if (!db) return 0;

    int count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].marks >= PASSING_MARKS) {
            count++;
        }
    }
    return count;
}

// Count students who failed
int count_failed_students(StudentDatabase *db) {
    if (!db) return 0;

    return db->count - count_passed_students(db);
}

// Calculate pass percentage
float calculate_pass_percentage(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    int passed = count_passed_students(db);
    return ((float)passed / db->count) * 100.0f;
}

// Swap two students
void swap_students(Student *a, Student *b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function for quicksort (marks-based)
int partition_marks(Student *arr, int low, int high, bool ascending) {
    float pivot = arr[high].marks;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        bool condition = ascending ? (arr[j].marks <= pivot) : (arr[j].marks >= pivot);
        if (condition) {
            i++;
            swap_students(&arr[i], &arr[j]);
        }
    }

    swap_students(&arr[i + 1], &arr[high]);
    return i + 1;
}

// Quicksort implementation for marks
void quicksort_marks(Student *arr, int low, int high, bool ascending) {
    if (low < high) {
        int pi = partition_marks(arr, low, high, ascending);
        quicksort_marks(arr, low, pi - 1, ascending);
        quicksort_marks(arr, pi + 1, high, ascending);
    }
}

// Bubble sort for names
void bubble_sort_names(Student *arr, int n, bool ascending) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int comparison = strcmp(arr[j].name, arr[j + 1].name);
            bool should_swap = ascending ? (comparison > 0) : (comparison < 0);

            if (should_swap) {
                swap_students(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Continuation of calculations.c - Main sorting function completion

// Main sorting function (continued)
void sort_students(StudentDatabase *db, SortCriteria criteria) {
    if (!db || db->count <= 1) {
        return;
    }

    switch (criteria) {
        case SORT_BY_MARKS_ASC:
            quicksort_marks(db->students, 0, db->count - 1, true);
            break;
        case SORT_BY_MARKS_DESC:
            quicksort_marks(db->students, 0, db->count - 1, false);
            break;
        case SORT_BY_NAME_ASC:
            bubble_sort_names(db->students, db->count, true);
            break;
        case SORT_BY_NAME_DESC:
            bubble_sort_names(db->students, db->count, false);
            break;
        case SORT_BY_ROLL_NUMBER_ASC:
            quicksort_roll_numbers(db->students, 0, db->count - 1, true);
            break;
        case SORT_BY_ROLL_NUMBER_DESC:
            quicksort_roll_numbers(db->students, 0, db->count - 1, false);
            break;
        default:
            // Default to sorting by marks in ascending order
            quicksort_marks(db->students, 0, db->count - 1, true);
            break;
    }
}

// Additional sorting functions for roll numbers
int partition_roll_numbers(Student *arr, int low, int high, bool ascending) {
    int pivot = arr[high].roll_number;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        bool condition = ascending ? (arr[j].roll_number <= pivot) : (arr[j].roll_number >= pivot);
        if (condition) {
            i++;
            swap_students(&arr[i], &arr[j]);
        }
    }

    swap_students(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort_roll_numbers(Student *arr, int low, int high, bool ascending) {
    if (low < high) {
        int pi = partition_roll_numbers(arr, low, high, ascending);
        quicksort_roll_numbers(arr, low, pi - 1, ascending);
        quicksort_roll_numbers(arr, pi + 1, high, ascending);
    }
}