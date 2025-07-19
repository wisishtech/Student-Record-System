#include "memory_manager.h"
#include <stdio.h>

// Safe malloc with error checking
void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Memory allocation failed for size %zu\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Safe realloc with error checking
void* safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "Memory reallocation failed for size %zu\n", size);
        free(ptr); // Free the original pointer
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

// Safe free that sets pointer to NULL
void safe_free(void **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

// Resize the database capacity
bool resize_database(StudentDatabase *db, int new_capacity) {
    if (!db || new_capacity < db->count) {
        return false;
    }
    
    Student *new_students = (Student*)safe_realloc(db->students, 
                                                   new_capacity * sizeof(Student));
    if (!new_students) {
        return false;
    }
    
    db->students = new_students;
    db->capacity = new_capacity;
    return true;
}

// Ensure the database has enough capacity for new students
bool ensure_capacity(StudentDatabase *db) {
    if (!db) return false;
    
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        return resize_database(db, new_capacity);
    }
    
    return true;
}

// Cleanup all resources associated with the database
void cleanup_resources(StudentDatabase *db) {
    if (db) {
        if (db->students) {
            free(db->students);
            db->students = NULL;
        }
        db->count = 0;
        db->capacity = 0;
    }
}