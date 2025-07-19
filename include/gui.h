#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "student.h"
#include "file_operations.h"
#include "calculations.h"
#include "memory_manager.h"

// GUI structure to hold all widgets and data
typedef struct {
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
    
    // Search functionality
    GtkWidget *search_entry;
    GtkWidget *search_button;
    
    // Statistics display
    GtkWidget *stats_label;
    GtkWidget *average_label;
    GtkWidget *total_label;
    GtkWidget *passed_label;
    GtkWidget *failed_label;
    
    // Status bar
    GtkWidget *status_bar;
    guint status_context_id;
    
    // User info
    char user_name[MAX_NAME_LENGTH];
    
    // Data
    StudentDatabase *db;
    Student *selected_student;
} AppData;

// GUI function declarations
void init_gui(int argc, char *argv[]);
AppData* create_app_data(void);
void destroy_app_data(AppData *app);
void setup_main_window(AppData *app);
void setup_menu_bar(AppData *app);
void setup_toolbar(AppData *app);
void setup_notebook_tabs(AppData *app);
void setup_student_list_view(AppData *app);
void setup_student_form(AppData *app);
void setup_search_panel(AppData *app);
void setup_statistics_panel(AppData *app);
void setup_status_bar(AppData *app);

// Event handlers
void on_window_destroy(GtkWidget *widget, gpointer data);
void on_add_student_clicked(GtkButton *button, gpointer data);
void on_update_student_clicked(GtkButton *button, gpointer data);
void on_delete_student_clicked(GtkButton *button, gpointer data);
void on_clear_form_clicked(GtkButton *button, gpointer data);
void on_search_clicked(GtkButton *button, gpointer data);
void on_student_selected(GtkTreeSelection *selection, gpointer data);
void on_save_file_activate(GtkMenuItem *menuitem, gpointer data);
void on_load_file_activate(GtkMenuItem *menuitem, gpointer data);
void on_export_csv_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_marks_asc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_marks_desc_activate(GtkMenuItem *menuitem, gpointer data);
void on_sort_by_name_activate(GtkMenuItem *menuitem, gpointer data);
void on_calculate_stats_activate(GtkMenuItem *menuitem, gpointer data);
void on_about_activate(GtkMenuItem *menuitem, gpointer data);

// Helper functions
void refresh_student_list(AppData *app);
void clear_form_fields(AppData *app);
void populate_form_fields(AppData *app, Student *student);
void update_statistics_display(AppData *app);
void show_message_dialog(GtkWidget *parent, GtkMessageType type, const char *title, const char *message);
bool validate_input_fields(AppData *app);
void set_status_message(AppData *app, const char *message);
void show_welcome_dialog(AppData *app);

// Column enumeration for tree view
enum {
    COL_ROLL_NUMBER,
    COL_NAME,
    COL_MARKS,
    COL_STATUS,
    NUM_COLS
};

#endif // GUI_H