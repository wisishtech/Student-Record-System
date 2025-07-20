/*
 * File: src/calculations.c
 * Path: SchoolRecordSystem/src/calculations.c
 * Description: Fixed statistical calculations and sorting algorithms
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 * 
 * Fixes:
 * - Fixed sorting algorithms for marks and roll numbers
 * - Added proper error handling
 * - Improved performance with optimized algorithms
 * - Added comprehensive statistics calculations
 */

#include "calculations.h"
#include <string.h>
#include <tgmath.h>

/* ============================================================================
 * STATISTICAL CALCULATION FUNCTIONS
 * ============================================================================ */

/**
 * Calculate average marks for all students
 * @param db Student database
 * @return Average marks or 0.0 if no students
 */
float calculate_average_marks(StudentDatabase *db) {
    if (!db || db->count == 0) {
        printf("Warning: No students found for average calculation\n");
        return 0.0f;
    }

    float total = 0.0f;
    int active_count = 0;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            total += db->students[i].marks;
            active_count++;
        }
    }

    if (active_count == 0) {
        printf("Warning: No active students found\n");
        return 0.0f;
    }

    float average = total / active_count;
    printf("Calculated average marks: %.2f from %d students\n", average, active_count);
    return average;
}

/**
 * Calculate comprehensive statistics for the database
 * @param db Student database
 * @return Statistics structure with calculated values
 */
Statistics calculate_statistics(StudentDatabase *db) {
    Statistics stats = {0}; // Initialize all fields to 0

    if (!db || db->count == 0) {
        printf("Warning: No students found for statistics calculation\n");
        return stats;
    }

    // Count active students first
    int active_count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            active_count++;
        }
    }

    if (active_count == 0) {
        printf("Warning: No active students found\n");
        return stats;
    }

    stats.total_students = active_count;
    
    // Initialize highest and lowest with first active student's marks
    bool first_found = false;
    float total_marks = 0.0f;

    for (int i = 0; i < db->count; i++) {
        if (!db->students[i].is_active) continue;
        
        float marks = db->students[i].marks;
        total_marks += marks;

        // Initialize highest and lowest on first active student
        if (!first_found) {
            stats.highest_marks = marks;
            stats.lowest_marks = marks;
            first_found = true;
        } else {
            if (marks > stats.highest_marks) {
                stats.highest_marks = marks;
            }
            if (marks < stats.lowest_marks) {
                stats.lowest_marks = marks;
            }
        }

        // Count pass/fail
        if (marks >= PASSING_MARKS) {
            stats.passed_students++;
        } else {
            stats.failed_students++;
        }
    }

   /*
 * File: src/calculations.c (Continued)
 * Path: SchoolRecordSystem/src/calculations.c
 * Description: Fixed statistical calculations and sorting algorithms (Part 2)
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 */

    // Calculate average and pass percentage
    stats.average_marks = total_marks / stats.total_students;
    stats.pass_percentage = (stats.total_students > 0) ?
                           ((float)stats.passed_students / stats.total_students) * 100.0f : 0.0f;

    printf("Statistics calculated: Total=%d, Average=%.2f, Passed=%d, Failed=%d, Pass%%=%.1f\n",
           stats.total_students, stats.average_marks, stats.passed_students,
           stats.failed_students, stats.pass_percentage);

    return stats;
}

/**
 * Find student with highest marks
 * @param db Student database
 * @return Pointer to student with highest marks or NULL if none found
 */
Student* find_highest_scorer(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return NULL;
    }

    Student *highest = NULL;
    float highest_marks = -1.0f;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].marks > highest_marks) {
            highest_marks = db->students[i].marks;
            highest = &db->students[i];
        }
    }

    if (highest) {
        printf("Highest scorer found: %s (%.2f marks)\n", highest->name, highest->marks);
    }

    return highest;
}

/**
 * Find student with lowest marks
 * @param db Student database
 * @return Pointer to student with lowest marks or NULL if none found
 */
Student* find_lowest_scorer(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return NULL;
    }

    Student *lowest = NULL;
    float lowest_marks = 101.0f; // Start with value higher than max possible

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].marks < lowest_marks) {
            lowest_marks = db->students[i].marks;
            lowest = &db->students[i];
        }
    }

    if (lowest) {
        printf("Lowest scorer found: %s (%.2f marks)\n", lowest->name, lowest->marks);
    }

    return lowest;
}

/**
 * Count students who passed
 * @param db Student database
 * @return Number of students who passed
 */
int count_passed_students(StudentDatabase *db) {
    if (!db) return 0;

    int count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].marks >= PASSING_MARKS) {
            count++;
        }
    }

    printf("Passed students count: %d\n", count);
    return count;
}

/**
 * Count students who failed
 * @param db Student database
 * @return Number of students who failed
 */
int count_failed_students(StudentDatabase *db) {
    if (!db) return 0;

    int passed = count_passed_students(db);
    int active_count = 0;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            active_count++;
        }
    }

    int failed = active_count - passed;
    printf("Failed students count: %d\n", failed);
    return failed;
}

/**
 * Calculate pass percentage
 * @param db Student database
 * @return Pass percentage
 */
float calculate_pass_percentage(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    int active_count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            active_count++;
        }
    }

    if (active_count == 0) {
        return 0.0f;
    }

    int passed = count_passed_students(db);
    float percentage = ((float)passed / active_count) * 100.0f;

    printf("Pass percentage calculated: %.2f%% (%d/%d)\n", percentage, passed, active_count);
    return percentage;
}

/* ============================================================================
 * SORTING HELPER FUNCTIONS
 * ============================================================================ */

/**
 * Swap two students in the array
 * @param a First student
 * @param b Second student
 */
void swap_students(Student *a, Student *b) {
    if (!a || !b || a == b) return;

    Student temp = *a;
    *a = *b;
    *b = temp;
}

/* ============================================================================
 * QUICKSORT IMPLEMENTATION FOR MARKS
 * ============================================================================ */

/**
 * Partition function for quicksort (marks-based) - FIXED VERSION
 * @param arr Array of students
 * @param low Starting index
 * @param high Ending index
 * @param ascending Sort order
 * @return Partition index
 */
int partition_marks(Student *arr, int low, int high, bool ascending) {
    if (!arr || low >= high) {
        return low;
    }

    float pivot = arr[high].marks;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        // Skip inactive students
        if (!arr[j].is_active) continue;

        bool condition = ascending ? (arr[j].marks <= pivot) : (arr[j].marks >= pivot);
        if (condition) {
            i++;
            swap_students(&arr[i], &arr[j]);
        }
    }

    swap_students(&arr[i + 1], &arr[high]);
    return i + 1;
}

/**
 * Quicksort implementation for marks - FIXED VERSION
 * @param arr Array of students
 * @param low Starting index
 * @param high Ending index
 * @param ascending Sort order
 */
void quicksort_marks(Student *arr, int low, int high, bool ascending) {
    if (!arr || low >= high) {
        return;
    }

    // Only sort if we have valid range and active students
    bool has_active = false;
    for (int i = low; i <= high; i++) {
        if (arr[i].is_active) {
            has_active = true;
            break;
        }
    }

    if (!has_active) {
        return;
    }

    int pi = partition_marks(arr, low, high, ascending);

    // Recursively sort elements before and after partition
    if (pi > low) {
        quicksort_marks(arr, low, pi - 1, ascending);
    }
    if (pi < high) {
        quicksort_marks(arr, pi + 1, high, ascending);
    }
}

/* ============================================================================
 * QUICKSORT IMPLEMENTATION FOR ROLL NUMBERS
 * ============================================================================ */

/**
 * Partition function for roll numbers - FIXED VERSION
 * @param arr Array of students
 * @param low Starting index
 * @param high Ending index
 * @param ascending Sort order
 * @return Partition index
 */
int partition_roll_numbers(Student *arr, int low, int high, bool ascending) {
    if (!arr || low >= high) {
        return low;
    }

    int pivot = arr[high].roll_number;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        // Skip inactive students
        if (!arr[j].is_active) continue;

        bool condition = ascending ? (arr[j].roll_number <= pivot) : (arr[j].roll_number >= pivot);
        if (condition) {
            i++;
            swap_students(&arr[i], &arr[j]);
        }
    }

    swap_students(&arr[i + 1], &arr[high]);
    return i + 1;
}

/**
 * Quicksort implementation for roll numbers - FIXED VERSION
 * @param arr Array of students
 * @param low Starting index
 * @param high Ending index
 * @param ascending Sort order
 */
void quicksort_roll_numbers(Student *arr, int low, int high, bool ascending) {
    if (!arr || low >= high) {
        return;
    }

    // Only sort if we have valid range and active students
    bool has_active = false;
    for (int i = low; i <= high; i++) {
        if (arr[i].is_active) {
            has_active = true;
            break;
        }
    }

    if (!has_active) {
        return;
    }

    int pi = partition_roll_numbers(arr, low, high, ascending);

    // Recursively sort elements before and after partition
    if (pi > low) {
        quicksort_roll_numbers(arr, low, pi - 1, ascending);
    }
    if (pi < high) {
        quicksort_roll_numbers(arr, pi + 1, high, ascending);
    }
}

/* ============================================================================
 * BUBBLE SORT IMPLEMENTATION FOR NAMES
 * ============================================================================ */

/**
 * Bubble sort for names - IMPROVED VERSION
 * @param arr Array of students
 * @param n Number of elements
 * @param ascending Sort order
 */
void bubble_sort_names(Student *arr, int n, bool ascending) {
    if (!arr || n <= 1) {
        return;
    }

    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            // Skip inactive students
            if (!arr[j].is_active || !arr[j + 1].is_active) continue;

            int comparison = strcmp(arr[j].name, arr[j + 1].name);
            bool should_swap = ascending ? (comparison > 0) : (comparison < 0);

            if (should_swap) {
                swap_students(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }

        // If no swapping occurred, array is sorted
        if (!swapped) {
            break;
        }
    }

    printf("Name sorting completed (%s)\n", ascending ? "ascending" : "descending");
}

/* ============================================================================
 * MAIN SORTING FUNCTION - FIXED VERSION
 * ============================================================================ */

/**
 * Main sorting function with improved error handling
 * @param db Student database
 * @param criteria Sorting criteria
 */
void sort_students(StudentDatabase *db, SortCriteria criteria) {
    if (!db || db->count <= 1) {
        printf("Warning: Database is empty or has only one student, no sorting needed\n");
        return;
    }

    // Check if we have any active students
    int active_count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            active_count++;
        }
    }

    if (active_count <= 1) {
        printf("Warning: Less than 2 active students, no sorting needed\n");
        return;
    }

    printf("Sorting %d students by ", active_count);

    switch (criteria) {
        case SORT_BY_MARKS_ASC:
            printf("marks (ascending)...\n");
            quicksort_marks(db->students, 0, db->count - 1, true);
            break;

        case SORT_BY_MARKS_DESC:
            printf("marks (descending)...\n");
            quicksort_marks(db->students, 0, db->count - 1, false);
            break;

        case SORT_BY_NAME_ASC:
            printf("name (ascending)...\n");
            bubble_sort_names(db->students, db->count, true);
            break;

        case SORT_BY_NAME_DESC:
            printf("name (descending)...\n");
            bubble_sort_names(db->students, db->count, false);
            break;

        case SORT_BY_ROLL_NUMBER_ASC:
            printf("roll number (ascending)...\n");
            quicksort_roll_numbers(db->students, 0, db->count - 1, true);
            break;

        case SORT_BY_ROLL_NUMBER_DESC:
            printf("roll number (descending)...\n");
            quicksort_roll_numbers(db->students, 0, db->count - 1, false);
            break;

        default:
            printf("unknown criteria, using marks ascending as default...\n");
            quicksort_marks(db->students, 0, db->count - 1, true);
            break;
    }

    // Mark database as changed
    mark_database_changed(db);
    printf("Sorting completed successfully\n");
}

/* ============================================================================
 * ENHANCED STATISTICS FUNCTIONS
 * ============================================================================ */

/**
 * Calculate median marks
 * @param db Student database
 * @return Median marks
 */
float calculate_median_marks(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    // Create a temporary array of active student marks
    float *marks = malloc(db->count * sizeof(float));
    if (!marks) {
        printf("Error: Memory allocation failed for median calculation\n");
        return 0.0f;
    }

    int active_count = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            marks[active_count] = db->students[i].marks;
            active_count++;
        }
    }

    if (active_count == 0) {
        free(marks);
        return 0.0f;
    }

    // Sort the marks array (simple bubble sort for small arrays)
    for (int i = 0; i < active_count - 1; i++) {
        for (int j = 0; j < active_count - i - 1; j++) {
            if (marks[j] > marks[j + 1]) {
                float temp = marks[j];
                marks[j] = marks[j + 1];
                marks[j + 1] = temp;
            }
        }
    }

    float median;
    if (active_count % 2 == 0) {
        // Even number of elements
        median = (marks[active_count / 2 - 1] + marks[active_count / 2]) / 2.0f;
    } else {
        // Odd number of elements
        median = marks[active_count / 2];
    }

    free(marks);
    printf("Median marks calculated: %.2f\n", median);
    return median;
}

/**
 * Calculate standard deviation
 * @param db Student database
 * @return Standard deviation
 */
float calculate_standard_deviation(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    float mean = calculate_average_marks(db);
    if (mean == 0.0f) {
        return 0.0f;
    }

    float sum_squared_diff = 0.0f;
    int active_count = 0;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active) {
            float diff = db->students[i].marks - mean;
            sum_squared_diff += diff * diff;
            active_count++;
        }
    }

    if (active_count <= 1) {
        return 0.0f;
    }

    float variance = sum_squared_diff / (active_count - 1);
    float std_dev = sqrt(variance);

    printf("Standard deviation calculated: %.2f\n", std_dev);
    return std_dev;
}

/**
 * Get grade distribution
 * @param db Student database
 * @param distribution Array to store distribution counts [A, B, C, D, E, F]
 */
void get_grade_distribution(StudentDatabase *db, int distribution[6]) {
    if (!db || !distribution) {
        return;
    }

    // Initialize distribution array
    for (int i = 0; i < 6; i++) {
        distribution[i] = 0;
    }

    for (int i = 0; i < db->count; i++) {
        if (!db->students[i].is_active) continue;

        float marks = db->students[i].marks;

        if (marks >= 90.0f) distribution[0]++;      // A
        else if (marks >= 80.0f) distribution[1]++; // B
        else if (marks >= 70.0f) distribution[2]++; // C
        else if (marks >= 60.0f) distribution[3]++; // D
        else if (marks >= 40.0f) distribution[4]++; // E
        else distribution[5]++;                     // F
    }

    printf("Grade distribution: A=%d, B=%d, C=%d, D=%d, E=%d, F=%d\n",
           distribution[0], distribution[1], distribution[2],
           distribution[3], distribution[4], distribution[5]);
}

/* ============================================================================
 * COURSE-RELATED STATISTICS
 * ============================================================================ */

/**
 * Calculate average GPA across all students
 * @param db Student database
 * @return Average GPA
 */
float calculate_average_gpa(StudentDatabase *db) {
    if (!db || db->count == 0) {
        return 0.0f;
    }

    float total_gpa = 0.0f;
    int students_with_courses = 0;

    for (int i = 0; i < db->count; i++) {
        if (db->students[i].is_active && db->students[i].course_count > 0) {
            total_gpa += db->students[i].gpa;
            students_with_courses++;
        }
    }

    if (students_with_courses == 0) {
        return 0.0f;
    }

    float average_gpa = total_gpa / students_with_courses;
    printf("Average GPA calculated: %.2f from %d students with courses\n",
           average_gpa, students_with_courses);

    return average_gpa;
}