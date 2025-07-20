/*
 * File: include/gui.h
 * Path: SchoolRecordSystem/include/gui.h
 * Description: Enhanced GUI interface with course management and fixed functionality
 * Author: Wisdom Chimezie
 * Date: July 20, 2025
 * Version: 2.0.0
 */

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <math.h>
#include "student.h"
#include "file_operations.h"
#include "calculations.h"
#include "memory_manager.h"

// GUI structure to hold all widgets and data
typedef struct {
    // Main window components
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *menu_bar;
    GtkWidget *toolbar;
    GtkWidget *notebook;

    // Student list view
    GtkWidget *tree_view;
    GtkListStore *list_store;

    // Add/Edit student form
    GtkWidget *name_entry;
    GtkWidget *roll_entry;
    GtkWidget *marks_entry;
    GtkWidget *add_button;
    GtkWidget *update_button;
    GtkWidget *delete_button;
    GtkWidget *clear_button;

    // Toolbar buttons (now functional)
    GtkWidget *toolbar_add_button;
    GtkWidget *toolbar_remove_button;
    GtkWidget *toolbar_save_button;
    GtkWidget *toolbar_load_button;

    // Search functionality
    GtkWidget *search_entry;
    GtkWidget *search_button;
    GtkWidget *search_clear_button;

    // Course management
    GtkWidget *course_name_entry;
    GtkWidget *course_score_entry;
    GtkWidget *add_course_button;
    GtkWidget *remove_course_button;
    GtkWidget *course_tree_view;
    GtkListStore *course_list_store;

    // Statistics display
    GtkWidget *stats_label;
    GtkWidget *average_label;
    GtkWidget *total_label;
    GtkWidget *passed_label;
    GtkWidget *failed_label;
    GtkWidget *gpa_label;
    GtkWidget *median_label;
    GtkWidget *std_dev_label;

    // Grade distribution chart area
    GtkWidget *grade_chart_area;

    // Student details dialog
    GtkWidget *details_dialog;
    GtkWidget *details_tree_view;
    GtkListStore *details_list_store;

    // Status bar and progress
    GtkWidget *status_bar;
    GtkWidget *progress_bar;
    guint status_context_id;

    // User info and welcome
    char user_name[MAX_NAME_LENGTH];
    GtkWidget *welcome_label;
    bool welcome_shown;

    // Data and state
    StudentDatabase *db;
    Student *selected_student;
    Course *selected_course;
    bool is_editing_mode;
    bool auto_save_enabled;

    // Error handling
    GtkWidget *error_dialog;
    char last_error_message[256];

    // Loop control for multiple student input
    bool input_loop_active;
    int students_added_in_session;
} AppData;

// GUI initialization and setup functions
void init_gui(int argc, char *argv[]);
AppData* create_app_data(void);
void destroy_app_data(AppData *app);
void setup_main_window(AppData *app);
void setup_menu_bar(AppData *app);
void setup_toolbar(AppData *app);
void setup_notebook_tabs(AppData *app);

// Component setup functions
void setup_student_list_view(AppData *app);
void setup_student_form(AppData *app);
void setup_search_panel(AppData *app);
void setup_statistics_panel(AppData *app);
void setup_course_management_panel(AppData *app);
void setup_status_bar(AppData *app);
void setup_welcome_display(AppData *app);

// Enhanced event handlers - main window
void on_window_destroy(GtkWidget *widget, gpointer data);
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);

// Student management event handlers
void on_add_student_clicked(GtkButton *button, gpointer data);
void on_update_student_clicked(GtkButton *button, gpointer data);
void on_delete_student_clicked(GtkButton *button, gpointer data);
void on_clear_form_clicked(GtkButton *button, gpointer data);
void on_student_selected(GtkTreeSelection *selection, gpointer data);
void on_student_row_activated(GtkTreeView *tree_view, GtkTreePath *path,
                             GtkTreeViewColumn *column, gpointer data);

// Toolbar event handlers (now functional)
void on_toolbar_add_clicked(GtkButton *button, gpointer data);
void on_toolbar_remove_clicked(GtkButton *button, gpointer data);
void on_toolbar_save_clicked(GtkButton *button, gpointer data);
void on_toolbar_load_clicked(GtkButton *button, gpointer data);

// Search event handlers
void on_search_clicked(GtkButton *button, gpointer data);
void on_search_clear_clicked(GtkButton *button, gpointer data);
void on_search_entry_changed(GtkEntry *entry, gpointer data);

// Course management event handlers
void on_add_course_clicked(GtkButton *button, gpointer data);
void on_remove_course_clicked(GtkButton *button, gpointer data);
void on_course_selected(GtkTreeSelection *selection, gpointer data);

// Menu event handlers
void on_save_file_activate(GtkMenuItem *menuitem, gpointer data);
void on_load_file_activate(GtkMenuItem *menuitem, gpointer data);
void on_export_csv_activate(GtkMenuItem *menuitem, gpointer data);
void on_import_csv_activate(GtkMenuItem *menuitem, gpointer data);
void on_create_backup_activate(GtkMenuItem *menuitem, gpointer data);

// Sorting menu handlers (fixed)
void on_sort_by_marks_asc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_marks_desc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_name_asc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_name_desc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_roll_asc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_roll_desc_activate(GtkMenuItem *menuitem, gpointer data);

// Statistics and help handlers
void on_calculate_stats_activate(GtkMenuItem *menuitem, gpointer data);
void on_show_grade_distribution_activate(GtkMenuItem *menuitem, gpointer data);
void on_about_activate(GtkMenuItem *menuitem, gpointer data);
void on_help_activate(GtkMenuItem *menuitem, gpointer data);

// Display and refresh functions
void refresh_student_list(AppData *app);
void refresh_course_list(AppData *app, Student *student);
void update_statistics_display(AppData *app);
void update_welcome_display(AppData *app);

// Form management functions
void clear_form_fields(AppData *app);
void populate_form_fields(AppData *app, Student *student);
void clear_course_form_fields(AppData *app);
void populate_course_form_fields(AppData *app, Course *course);

// Validation functions
bool validate_input_fields(AppData *app);
bool validate_course_input_fields(AppData *app);
void show_validation_error(AppData *app, const char *field_name, const char *message);

// Dialog functions
void show_message_dialog(GtkWidget *parent, GtkMessageType type,
                        const char *title, const char *message);
void show_error_dialog(AppData *app, const char *title, const char *message);
void show_success_dialog(AppData *app, const char *title, const char *message);
gboolean show_confirmation_dialog(GtkWidget *parent, const char *title, const char *message);

// Student details dialog
void show_student_details_dialog(AppData *app, Student *student);
void create_student_details_dialog(AppData *app);
void populate_student_details(AppData *app, Student *student);

// Enhanced welcome system
void show_welcome_dialog(AppData *app);
void show_welcome_message_in_app(AppData *app);
void setup_user_preferences(AppData *app);

// Progress and status functions
void set_status_message(AppData *app, const char *message);
void show_progress(AppData *app, const char *message);
void hide_progress(AppData *app);
void update_progress(AppData *app, double fraction);

// Auto-save and loop management
void enable_auto_save(AppData *app);
void disable_auto_save(AppData *app);
gboolean auto_save_timer_callback(gpointer data);
void start_input_loop(AppData *app);
void stop_input_loop(AppData *app);
gboolean continue_input_loop_dialog(AppData *app);

// Error handling and recovery
void handle_database_error(AppData *app, StudentError error);
void show_error_details(AppData *app, StudentError error, const char *context);
bool attempt_error_recovery(AppData *app, StudentError error);

// Data formatting functions
void format_marks_display(float marks, char *buffer, size_t buffer_size);
void format_gpa_display(float gpa, char *buffer, size_t buffer_size);
const char* format_grade_letter(float score);

// Enhanced statistics display
void display_advanced_statistics(AppData *app);
void show_grade_distribution_chart(AppData *app);
void create_grade_distribution_window(AppData *app);

// Theme and appearance
void apply_theme(AppData *app);
void set_widget_styles(AppData *app);

// Column enumeration for student tree view
enum {
    COL_ROLL_NUMBER,
    COL_NAME,
    COL_MARKS,
    COL_STATUS,
    COL_COURSES,
    COL_GPA,
    NUM_STUDENT_COLS
};

// Column enumeration for course tree view
enum {
    COURSE_COL_NAME,
    COURSE_COL_SCORE,
    COURSE_COL_GRADE,
    COURSE_COL_STATUS,
    NUM_COURSE_COLS
};

// Constants for formatting
#define MARKS_DECIMAL_PLACES 2
#define GPA_DECIMAL_PLACES 2
#define MAX_STATUS_MESSAGE_LENGTH 256
#define AUTO_SAVE_INTERVAL_SECONDS 30

// Error message constants
#define ERROR_TITLE_VALIDATION "Validation Error"
#define ERROR_TITLE_DATABASE "Database Error"
#define ERROR_TITLE_FILE_OPERATION "File Operation Error"
#define ERROR_TITLE_MEMORY "Memory Error"

#endif // GUI_H