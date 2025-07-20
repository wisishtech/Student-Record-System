/*
 * File: include/calculations.h
 * Path: SchoolRecordSystem/include/calculations.h
 * Description: Enhanced calculations and sorting function declarations
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 */

#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include "student.h"
#include <math.h>

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

/* ============================================================================
 * BASIC STATISTICAL FUNCTIONS
 * ============================================================================ */

// Basic statistics calculations
float calculate_average_marks(StudentDatabase *db);
Statistics calculate_statistics(StudentDatabase *db);
Student* find_highest_scorer(StudentDatabase *db);
Student* find_lowest_scorer(StudentDatabase *db);
int count_passed_students(StudentDatabase *db);
int count_failed_students(StudentDatabase *db);
float calculate_pass_percentage(StudentDatabase *db);

/* ============================================================================
 * SORTING FUNCTIONS
 * ============================================================================ */

// Main sorting function
void sort_students(StudentDatabase *db, SortCriteria criteria);

// Helper functions for sorting
void quicksort_marks(Student *arr, int low, int high, bool ascending);
int partition_marks(Student *arr, int low, int high, bool ascending);
void quicksort_roll_numbers(Student *arr, int low, int high, bool ascending);
int partition_roll_numbers(Student *arr, int low, int high, bool ascending);
void bubble_sort_names(Student *arr, int n, bool ascending);
void swap_students(Student *a, Student *b);

/* ============================================================================
 * ENHANCED STATISTICAL FUNCTIONS
 * ============================================================================ */

// Advanced statistics
float calculate_median_marks(StudentDatabase *db);
float calculate_standard_deviation(StudentDatabase *db);
void get_grade_distribution(StudentDatabase *db, int distribution[6]);

// Course-related statistics
float calculate_average_gpa(StudentDatabase *db);

/* ============================================================================
 * COURSE STATISTICS (FOR FUTURE EXPANSION)
 * ============================================================================ */

// Course performance analysis
float calculate_course_average(StudentDatabase *db, const char *course_name);
int count_students_in_course(StudentDatabase *db, const char *course_name);
Student* find_top_performer_in_course(StudentDatabase *db, const char *course_name);

/* ============================================================================
 * GRADE ANALYSIS FUNCTIONS
 * ============================================================================ */

// Grade distribution analysis
int count_students_with_grade(StudentDatabase *db, char grade);
float get_grade_percentage(StudentDatabase *db, char grade);
void print_grade_report(StudentDatabase *db);

/* ============================================================================
 * PERFORMANCE METRICS
 * ============================================================================ */

// Performance tracking
float calculate_class_performance_index(StudentDatabase *db);
void generate_performance_summary(StudentDatabase *db, char *summary, size_t size);
bool is_student_above_average(Student *student, float class_average);

/* ============================================================================
 * RANKING FUNCTIONS
 * ============================================================================ */

// Student ranking
int get_student_rank_by_marks(StudentDatabase *db, int roll_number);
int get_student_rank_by_gpa(StudentDatabase *db, int roll_number);
void get_top_n_students(StudentDatabase *db, Student **top_students, int n);

/* ============================================================================
 * TREND ANALYSIS (PLACEHOLDERS FOR FUTURE IMPLEMENTATION)
 * ============================================================================ */

// Trend analysis (placeholder functions)
float calculate_improvement_rate(Student *student); // Based on course progression
void analyze_class_trends(StudentDatabase *db);
bool predict_student_success(Student *student);

/* ============================================================================
 * DATA VALIDATION AND INTEGRITY
 * ============================================================================ */

// Data validation for calculations
bool validate_calculation_data(StudentDatabase *db);
bool check_data_consistency(StudentDatabase *db);
void repair_data_inconsistencies(StudentDatabase *db);

/* ============================================================================
 * EXPORT AND REPORTING FUNCTIONS
 * ============================================================================ */

// Report generation
bool generate_statistics_report(StudentDatabase *db, const char *filename);
bool export_grade_distribution(StudentDatabase *db, const char *filename);
void print_detailed_statistics(StudentDatabase *db);

/* ============================================================================
 * MATHEMATICAL UTILITIES
 * ============================================================================ */

// Mathematical helper functions
double calculate_variance(float *values, int count, float mean);
double calculate_correlation(float *x_values, float *y_values, int count);
float calculate_percentile(float *sorted_values, int count, float percentile);

/* ============================================================================
 * SEARCH AND FILTER FUNCTIONS
 * ============================================================================ */

// Advanced search and filtering
int find_students_by_grade_range(StudentDatabase *db, float min_marks, float max_marks, Student **results);
int find_students_by_status(StudentDatabase *db, const char *status, Student **results);
int find_students_with_gpa_above(StudentDatabase *db, float min_gpa, Student **results);

/* ============================================================================
 * COMPARATIVE ANALYSIS
 * ============================================================================ */

// Comparison functions
int compare_student_performance(Student *student1, Student *student2);
float calculate_performance_gap(StudentDatabase *db); // Gap between highest and lowest
void analyze_performance_distribution(StudentDatabase *db);

/* ============================================================================
 * BENCHMARKING FUNCTIONS
 * ============================================================================ */

// Benchmarking and standards
bool meets_passing_standard(Student *student);
float calculate_class_benchmark(StudentDatabase *db);
void set_performance_targets(StudentDatabase *db, float target_average);

/* ============================================================================
 * ERROR HANDLING FOR CALCULATIONS
 * ============================================================================ */

// Error handling
typedef enum {
    CALC_SUCCESS = 0,
    CALC_ERROR_NULL_POINTER,
    CALC_ERROR_EMPTY_DATABASE,
    CALC_ERROR_INVALID_DATA,
    CALC_ERROR_DIVISION_BY_ZERO,
    CALC_ERROR_OUT_OF_RANGE
} CalculationError;

CalculationError validate_calculation_input(StudentDatabase *db);
const char* get_calculation_error_message(CalculationError error);

/* ============================================================================
 * CONSTANTS AND THRESHOLDS
 * ============================================================================ */

// Calculation constants
#define MIN_STUDENTS_FOR_STATS 1
#define MAX_REASONABLE_MARKS 100.0f
#define MIN_REASONABLE_MARKS 0.0f
#define DEFAULT_PERCENTILE_PRECISION 0.01f
#define PERFORMANCE_EXCELLENT_THRESHOLD 90.0f
#define PERFORMANCE_GOOD_THRESHOLD 75.0f
#define PERFORMANCE_AVERAGE_THRESHOLD 60.0f

// Grade boundaries
#define GRADE_A_THRESHOLD 90.0f
#define GRADE_B_THRESHOLD 80.0f
#define GRADE_C_THRESHOLD 70.0f
#define GRADE_D_THRESHOLD 60.0f
#define GRADE_E_THRESHOLD 40.0f
// Below 40.0f is Grade F

/* ============================================================================
 * MACROS FOR COMMON CALCULATIONS
 * ============================================================================ */

// Utility macros
#define IS_PASSING_GRADE(marks) ((marks) >= PASSING_MARKS)
#define GET_GRADE_POINTS(marks) (((marks) / 100.0f) * 4.0f)
#define ROUND_TO_DECIMAL_PLACES(value, places) (roundf((value) * powf(10, (places))) / powf(10, (places)))
#define PERCENTAGE_OF_TOTAL(part, total) (((total) > 0) ? (((float)(part) / (float)(total)) * 100.0f) : 0.0f)

/* ============================================================================
 * FUNCTION POINTER TYPES FOR CALLBACKS
 * ============================================================================ */

// Callback function types for flexible operations
typedef bool (*StudentFilterFunc)(Student *student, void *criteria);
typedef int (*StudentCompareFunc)(const Student *a, const Student *b);
typedef void (*StatisticsCallback)(const Statistics *stats, void *user_data);

/* ============================================================================
 * BATCH OPERATIONS
 * ============================================================================ */

// Batch processing functions
void apply_grade_curve(StudentDatabase *db, float curve_factor);
void normalize_marks(StudentDatabase *db, float target_average);
void calculate_relative_positions(StudentDatabase *db);

#endif // CALCULATIONS_H