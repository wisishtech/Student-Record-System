#include "file_operations.h"
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

// Get the application's data directory path
char* get_data_directory_path(void) {
    static char data_path[512];
    static bool initialized = false;

    if (!initialized) {
        // Try to get the executable's directory first
        char exe_path[512];
        ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);

        if (len == -1) {
            // Fallback for macOS and other systems
            // Use current working directory
            if (getcwd(data_path, sizeof(data_path)) != NULL) {
                strcat(data_path, "/data");
            } else {
                // Last resort - use home directory
                const char *home = getenv("HOME");
                if (home) {
                    snprintf(data_path, sizeof(data_path), "%s/.student_records", home);
                } else {
                    strcpy(data_path, "./data");
                }
            }
        } else {
            exe_path[len] = '\0';
            char *dir = dirname(exe_path);
            snprintf(data_path, sizeof(data_path), "%s/data", dir);
        }

        initialized = true;
        printf("Data directory path: %s\n", data_path);
    }

    return data_path;
}

// Create data directory if it doesn't exist
void create_data_directory(void) {
    char *data_dir = get_data_directory_path();
    struct stat st = {0};

    if (stat(data_dir, &st) == -1) {
        if (mkdir(data_dir, 0755) == 0) {
            printf("Created data directory: %s\n", data_dir);
        } else {
            fprintf(stderr, "Failed to create data directory: %s (errno: %d)\n", data_dir, errno);
            // Try creating in current directory as fallback
            if (mkdir("data", 0755) == 0) {
                printf("Created fallback data directory: ./data\n");
            } else {
                fprintf(stderr, "Failed to create fallback data directory\n");
            }
        }
    } else {
        printf("Data directory exists: %s\n", data_dir);
    }
}

// Check if file exists
bool file_exists(const char *filename) {
    if (!filename) return false;

    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Get default filename with full path
char* get_default_filename(void) {
    static char full_path[512];
    static bool initialized = false;

    if (!initialized) {
        char *data_dir = get_data_directory_path();
        snprintf(full_path, sizeof(full_path), "%s/students.txt", data_dir);
        initialized = true;
    }

    return full_path;
}

// Get a filename relative to data directory
char* get_data_file_path(const char *filename) {
    static char full_path[512];
    char *data_dir = get_data_directory_path();
    snprintf(full_path, sizeof(full_path), "%s/%s", data_dir, filename);
    return full_path;
}

// Save student records to file
bool save_to_file(StudentDatabase *db, const char *filename) {
    if (!db || !filename) {
        fprintf(stderr, "Invalid parameters for save_to_file\n");
        return false;
    }

    // Ensure data directory exists
    create_data_directory();

    printf("Attempting to save to: %s\n", filename);

    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file for writing: %s (errno: %d)\n", filename, errno);

        // Try saving to current directory as fallback
        char fallback_name[256];
        snprintf(fallback_name, sizeof(fallback_name), "./students_backup.txt");
        printf("Trying fallback location: %s\n", fallback_name);

        file = fopen(fallback_name, "w");
        if (!file) {
            fprintf(stderr, "Failed to open fallback file: %s\n", fallback_name);
            return false;
        }
    }

    // Write header
    fprintf(file, "Roll Number,Name,Marks,Status\n");

    // Write student data
    for (int i = 0; i < db->count; i++) {
        fprintf(file, "%d,%s,%.2f,%s\n",
                db->students[i].roll_number,
                db->students[i].name,
                db->students[i].marks,
                db->students[i].status);
    }

    fclose(file);
    printf("Successfully saved %d student records to %s\n", db->count, filename);
    return true;
}

// Load student records from file
bool load_from_file(StudentDatabase *db, const char *filename) {
    if (!db || !filename) {
        fprintf(stderr, "Invalid parameters for load_from_file\n");
        return false;
    }

    printf("Attempting to load from: %s\n", filename);

    // Check if file exists first
    if (!file_exists(filename)) {
        printf("File %s not found.\n", filename);

        // Try to find the file in current directory
        if (file_exists("students.txt")) {
            printf("Found students.txt in current directory, loading...\n");
            filename = "students.txt";
        } else if (file_exists("data/students.txt")) {
            printf("Found data/students.txt, loading...\n");
            filename = "data/students.txt";
        } else {
            printf("No existing data file found. Starting with empty database.\n");
            return false;
        }
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file for reading: %s (errno: %d)\n", filename, errno);
        return false;
    }

    char line[512];
    char name[MAX_NAME_LENGTH];
    int roll_number;
    float marks;
    char status[MAX_STATUS_LENGTH];
    int loaded_count = 0;
    int line_count = 0;

    // Skip header line if it exists
    if (fgets(line, sizeof(line), file) != NULL) {
        line_count++;
        // Check if this is actually a header or data
        if (strstr(line, "Roll Number") || strstr(line, "Name") || strstr(line, "Marks")) {
            printf("Skipping header line\n");
        } else {
            // This is actually data, parse it
            rewind(file);
        }
    }

    // Clear existing data
    clear_database(db);

    // Read student data
    while (fgets(line, sizeof(line), file)) {
        line_count++;
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Skip empty lines
        if (strlen(line) == 0) continue;

        printf("Parsing line %d: %s\n", line_count, line);

        // Try different parsing methods
        int parsed = 0;

        // Method 1: Standard CSV parsing
        if (sscanf(line, "%d,%99[^,],%f,%9s", &roll_number, name, &marks, status) == 4) {
            parsed = 1;
        }
        // Method 2: Try with spaces after commas
        else if (sscanf(line, "%d, %99[^,], %f, %9s", &roll_number, name, &marks, status) == 4) {
            parsed = 1;
        }
        // Method 3: Try manual parsing for quoted names
        else {
            char *token = strtok(line, ",");
            if (token) {
                roll_number = atoi(token);
                token = strtok(NULL, ",");
                if (token) {
                    // Remove quotes if present
                    if (token[0] == '"') {
                        token++;
                        char *end_quote = strrchr(token, '"');
                        if (end_quote) *end_quote = '\0';
                    }
                    strncpy(name, token, MAX_NAME_LENGTH - 1);
                    name[MAX_NAME_LENGTH - 1] = '\0';

                    token = strtok(NULL, ",");
                    if (token) {
                        marks = atof(token);
                        token = strtok(NULL, ",");
                        if (token) {
                            strncpy(status, token, MAX_STATUS_LENGTH - 1);
                            status[MAX_STATUS_LENGTH - 1] = '\0';
                            parsed = 1;
                        }
                    }
                }
            }
        }

        if (parsed) {
            printf("Parsed: Roll=%d, Name='%s', Marks=%.2f, Status='%s'\n",
                   roll_number, name, marks, status);

            if (add_student(db, name, roll_number, marks)) {
                loaded_count++;
                printf("Successfully added student %d\n", loaded_count);
            } else {
                printf("Failed to add student with roll number %d\n", roll_number);
            }
        } else {
            printf("Failed to parse line: %s\n", line);
        }
    }

    fclose(file);
    printf("Successfully loaded %d student records from %s\n", loaded_count, filename);
    return loaded_count > 0;
}

// Create backup of current data
bool create_backup(StudentDatabase *db, const char *backup_filename) {
    if (!db || !backup_filename) {
        return false;
    }

    char *backup_path = get_data_file_path(backup_filename);
    return save_to_file(db, backup_path);
}

// Export to CSV format with additional formatting
bool export_to_csv(StudentDatabase *db, const char *filename) {
    if (!db || !filename) {
        return false;
    }

    // Ensure data directory exists
    create_data_directory();

    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file for export: %s\n", filename);
        return false;
    }

    // Write enhanced header
    fprintf(file, "Roll Number,Student Name,Marks Obtained,Pass/Fail Status,Grade\n");

    // Write student data with grades
    for (int i = 0; i < db->count; i++) {
        char grade;
        if (db->students[i].marks >= 90) grade = 'A';
        else if (db->students[i].marks >= 80) grade = 'B';
        else if (db->students[i].marks >= 70) grade = 'C';
        else if (db->students[i].marks >= 60) grade = 'D';
        else if (db->students[i].marks >= 40) grade = 'E';
        else grade = 'F';

        fprintf(file, "%d,\"%s\",%.2f,%s,%c\n",
                db->students[i].roll_number,
                db->students[i].name,
                db->students[i].marks,
                db->students[i].status,
                grade);
    }

    fclose(file);
    printf("Successfully exported %d student records to %s\n", db->count, filename);
    return true;
}

// Initialize data with sample data if no file exists
bool initialize_sample_data(StudentDatabase *db) {
    if (!db) return false;

    printf("Creating sample data...\n");

    // Sample student data
    struct {
        char name[MAX_NAME_LENGTH];
        int roll;
        float marks;
    } sample_students[] = {
        {"Wisdom Chimezie", 101, 85.90},
        {"Jane Smith", 102, 92.75},
        {"Bob Johnson", 103, 78.25},
        {"Alice Brown", 104, 67.50},
        {"Charlie Wilson", 105, 34.00},
        {"Diana Davis", 106, 88.75},
        {"Edward Miller", 107, 45.25},
        {"Fiona Garcia", 108, 91.00},
        {"George Martinez", 109, 29.75},
        {"Helen Rodriguez", 110, 73.50}
    };

    int sample_count = sizeof(sample_students) / sizeof(sample_students[0]);
    int added_count = 0;

    for (int i = 0; i < sample_count; i++) {
        if (add_student(db, sample_students[i].name, sample_students[i].roll, sample_students[i].marks)) {
            added_count++;
        }
    }

    printf("Added %d sample students\n", added_count);

    // Save the sample data
    char *default_file = get_default_filename();
    if (save_to_file(db, default_file)) {
        printf("Sample data saved to %s\n", default_file);
        return true;
    }

    return false;
}