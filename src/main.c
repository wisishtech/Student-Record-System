#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "gui.h"
#include "student.h"
#include "file_operations.h"
#include "calculations.h"
#include "memory_manager.h"

// Global variables for cleanup
static StudentDatabase *g_database = NULL;

// Signal handler for graceful shutdown
void signal_handler(int signal) {
    printf("\nReceived signal %d. Saving data and exiting...\n", signal);

    if (g_database) {
        save_to_file(g_database, get_default_filename());
        destroy_database(g_database);
    }

    exit(0);
}

// Setup signal handlers
void setup_signal_handlers(void) {
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // Termination signal

#ifdef SIGQUIT
    signal(SIGQUIT, signal_handler);  // Quit signal (Unix)
#endif
}

// Print program banner
void print_banner(void) {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              Student Record Management System                ║\n");
    printf("║                     Version 1.0.0                           ║\n");
    printf("║                Built with GTK and C                         ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// Print help information
void print_help(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Student Record Management System - A comprehensive GUI application\n");
    printf("for managing student information with sorting, searching, and statistics.\n\n");
    printf("Options:\n");
    printf("  -h, --help     Display this help message\n");
    printf("  -v, --version  Display version information\n");
    printf("  --console      Run in console mode (text-based interface)\n");
    printf("  --data FILE    Specify custom data file (default: data/students.txt)\n");
    printf("\nFeatures:\n");
    printf("  • Add, modify, and delete student records\n");
    printf("  • Search students by roll number\n");
    printf("  • Sort records by marks, name, or roll number\n");
    printf("  • Calculate statistics (average, pass/fail rates)\n");
    printf("  • Import/Export data to/from CSV files\n");
    printf("  • Automatic data persistence\n");
    printf("\nExamples:\n");
    printf("  %s                    # Start GUI application\n", program_name);
    printf("  %s --console          # Run in console mode\n", program_name);
    printf("  %s --data mydata.txt  # Use custom data file\n", program_name);
    printf("\n");
}

// Print version information
void print_version(void) {
    printf("Student Record Management System v1.0.0\n");
    printf("Built with GTK 3.0 and C99\n");
    printf("Copyright (c) 2025 - Educational Project\n");
}

// Console mode for testing without GUI
void console_mode(const char *data_file) {
    printf("Starting in console mode...\n");
    printf("Data file: %s\n\n", data_file);

    StudentDatabase *db = create_database();
    if (!db) {
        fprintf(stderr, "Failed to create database\n");
        return;
    }

    g_database = db;

    // Load existing data
    if (load_from_file(db, data_file)) {
        printf("Loaded existing data from %s\n", data_file);
    } else {
        printf("Starting with empty database\n");
    }

    char input[256];
    int choice;

    while (1) {
        printf("\n=== Student Record Management System ===\n");
        printf("1. Display all students\n");
        printf("2. Add student\n");
        printf("3. Search student\n");
        printf("4. Calculate statistics\n");
        printf("5. Sort students by marks\n");
        printf("6. Save to file\n");
        printf("7. Exit\n");
        printf("Enter your choice (1-7): ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        choice = atoi(input);

        switch (choice) {
            case 1:
                display_all_students(db);
                break;

            case 2: {
                char name[MAX_NAME_LENGTH];
                int roll;
                float marks;

                printf("Enter student name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';

                printf("Enter roll number: ");
                fgets(input, sizeof(input), stdin);
                roll = atoi(input);

                printf("Enter marks: ");
                fgets(input, sizeof(input), stdin);
                marks = atof(input);

                if (add_student(db, name, roll, marks)) {
                    printf("Student added successfully!\n");
                } else {
                    printf("Failed to add student. Roll number may already exist.\n");
                }
                break;
            }

           // Continuation of main.c - Console mode and main function

            case 3: {
                printf("Enter roll number to search: ");
                fgets(input, sizeof(input), stdin);
                int roll = atoi(input);

                Student *student = find_student(db, roll);
                if (student) {
                    printf("\nStudent Found:\n");
                    printf("Name: %s\n", student->name);
                    printf("Roll Number: %d\n", student->roll_number);
                    printf("Marks: %.2f\n", student->marks);
                    printf("Status: %s\n", student->status);
                } else {
                    printf("Student with roll number %d not found.\n", roll);
                }
                break;
            }

            case 4: {
                Statistics stats = calculate_statistics(db);
                printf("\n=== Statistics ===\n");
                printf("Total Students: %d\n", stats.total_students);
                printf("Average Marks: %.2f\n", stats.average_marks);
                printf("Highest Marks: %.2f\n", stats.highest_marks);
                printf("Lowest Marks: %.2f\n", stats.lowest_marks);
                printf("Passed Students: %d\n", stats.passed_students);
                printf("Failed Students: %d\n", stats.failed_students);
                printf("Pass Percentage: %.1f%%\n", stats.pass_percentage);

                Student *top_student = find_highest_scorer(db);
                if (top_student) {
                    printf("Top Scorer: %s (Roll: %d, Marks: %.2f)\n",
                           top_student->name, top_student->roll_number, top_student->marks);
                }
                break;
            }

            case 5: {
                printf("Sort by: 1) Marks Ascending 2) Marks Descending\n");
                printf("Enter choice (1-2): ");
                fgets(input, sizeof(input), stdin);
                int sort_choice = atoi(input);

                if (sort_choice == 1) {
                    sort_students(db, SORT_BY_MARKS_ASC);
                    printf("Students sorted by marks (ascending)\n");
                } else if (sort_choice == 2) {
                    sort_students(db, SORT_BY_MARKS_DESC);
                    printf("Students sorted by marks (descending)\n");
                } else {
                    printf("Invalid choice\n");
                }
                break;
            }

            case 6:
                if (save_to_file(db, data_file)) {
                    printf("Data saved successfully to %s\n", data_file);
                } else {
                    printf("Failed to save data\n");
                }
                break;

            case 7:
                printf("Saving data and exiting...\n");
                save_to_file(db, data_file);
                destroy_database(db);
                g_database = NULL;
                return;

            default:
                printf("Invalid choice. Please enter 1-7.\n");
                break;
        }
    }

    destroy_database(db);
    g_database = NULL;
}

// Check system requirements
bool check_system_requirements(void) {
    printf("Checking system requirements...\n");

    // Check if GTK is available (basic check)
    printf("  - GTK libraries: ");

#ifdef GTK_MAJOR_VERSION
    printf("Found (GTK %d.%d)\n", GTK_MAJOR_VERSION, GTK_MINOR_VERSION);
#else
    printf("Not found or version unknown\n");
    return false;
#endif

    // Check data directory
    printf("  - Data directory: ");
    create_data_directory();
    printf("Ready\n");

    printf("System requirements check completed.\n\n");
    return true;
}

// Initialize application
bool initialize_application(void) {
    printf("Initializing Student Record Management System...\n");

    // Setup signal handlers for graceful shutdown
    setup_signal_handlers();

    // Check system requirements
    if (!check_system_requirements()) {
        fprintf(stderr, "System requirements not met\n");
        return false;
    }

    printf("Application initialized successfully.\n\n");
    return true;
}

// Main function
int main(int argc, char *argv[]) {
    bool console_mode_flag = false;
    bool show_help = false;
    bool show_version = false;
    char *data_file = NULL;

    // Print banner
    print_banner();

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            show_version = true;
        } else if (strcmp(argv[i], "--console") == 0) {
            console_mode_flag = true;
        } else if (strcmp(argv[i], "--data") == 0 && i + 1 < argc) {
            data_file = argv[++i];
        } else {
            printf("Unknown option: %s\n", argv[i]);
            printf("Use --help for usage information.\n");
            return 1;
        }
    }

    // Handle help and version flags
    if (show_help) {
        print_help(argv[0]);
        return 0;
    }

    if (show_version) {
        print_version();
        return 0;
    }

    // Set default data file if not specified
    if (!data_file) {
        data_file = get_default_filename();
    }

    // Initialize application
    if (!initialize_application()) {
        return 1;
    }

    // Run in appropriate mode
    if (console_mode_flag) {
        console_mode(data_file);
    } else {
        printf("Starting GUI mode...\n");
        printf("Data file: %s\n", data_file);
        printf("Use Ctrl+C to exit safely.\n\n");

        // Initialize and run GUI
        init_gui(argc, argv);
    }

    printf("Thank you for using Student Record Management System!\n");
    return 0;
}

// Additional utility functions for main

// Cleanup function called at exit
void cleanup_at_exit(void) {
    if (g_database) {
        printf("Performing cleanup...\n");
        save_to_file(g_database, get_default_filename());
        destroy_database(g_database);
        g_database = NULL;
    }
}

// Register cleanup function
__attribute__((constructor))
void register_cleanup(void) {
    atexit(cleanup_at_exit);
}