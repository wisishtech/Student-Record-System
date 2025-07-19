#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "student.h"

// Default file path
#define DEFAULT_DATA_FILE "data/students.txt"

// File operation function declarations
bool save_to_file(StudentDatabase *db, const char *filename);
bool load_from_file(StudentDatabase *db, const char *filename);
bool create_backup(StudentDatabase *db, const char *backup_filename);
bool file_exists(const char *filename);
void create_data_directory(void);
bool export_to_csv(StudentDatabase *db, const char *filename);
char* get_default_filename(void);

// New improved functions
char* get_data_directory_path(void);
char* get_data_file_path(const char *filename);
bool initialize_sample_data(StudentDatabase *db);

#endif // FILE_OPERATIONS_H