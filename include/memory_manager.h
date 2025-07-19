#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "student.h"

// Memory management function declarations
bool resize_database(StudentDatabase *db, int new_capacity);
void* safe_malloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
void safe_free(void **ptr);
void cleanup_resources(StudentDatabase *db);
bool ensure_capacity(StudentDatabase *db);

#endif // MEMORY_MANAGER_H