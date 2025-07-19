#include "gui.h"

// Global application data
static AppData *g_app = NULL;

// Initialize GTK and create the main application - IMPROVED VERSION
void init_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    g_app = create_app_data();
    if (!g_app) {
        g_print("Failed to create application data\n");
        return;
    }

    setup_main_window(g_app);
    show_welcome_dialog(g_app);

    // Improved data loading with fallback options
    char *default_file = get_default_filename();
    printf("Trying to load data from: %s\n", default_file);

    if (!load_from_file(g_app->db, default_file)) {
        printf("No existing data found. Offering to create sample data...\n");

        // Create a simple dialog to ask user
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(g_app->window),
                                                  GTK_DIALOG_MODAL,
                                                  GTK_MESSAGE_QUESTION,
                                                  GTK_BUTTONS_YES_NO,
                                                  "No existing student data found.\n\nWould you like to create some sample data to get started?");

        gtk_window_set_title(GTK_WINDOW(dialog), "Initialize Data");
        int response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if (response == GTK_RESPONSE_YES) {
            if (initialize_sample_data(g_app->db)) {
                show_message_dialog(g_app->window, GTK_MESSAGE_INFO, "Success",
                                  "Sample data created successfully!\n\nYou can now add, edit, or delete student records.");
            } else {
                show_message_dialog(g_app->window, GTK_MESSAGE_WARNING, "Warning",
                                  "Failed to create sample data.\n\nYou can still use the application by adding students manually.");
            }
        }
    }

    refresh_student_list(g_app);
    update_statistics_display(g_app);

    // Show data directory information
    char info_message[256];
    snprintf(info_message, sizeof(info_message),
             "Students loaded: %d | Data: %s",
             get_student_count(g_app->db),
             get_default_filename());

    set_status_message(g_app, info_message);

    gtk_widget_show_all(g_app->window);
    gtk_main();

    // Cleanup
    destroy_app_data(g_app);
}

// Create and initialize application data
AppData* create_app_data(void) {
    AppData *app = g_new0(AppData, 1);
    if (!app) {
        return NULL;
    }

    app->db = create_database();
    if (!app->db) {
        g_free(app);
        return NULL;
    }

    app->selected_student = NULL;
    strcpy(app->user_name, "User");

    return app;
}

// Destroy application data and free resources
void destroy_app_data(AppData *app) {
    if (app) {
        if (app->db) {
            // Auto-save before exit
            save_to_file(app->db, get_default_filename());
            destroy_database(app->db);
        }
        g_free(app);
    }
}

// Setup the main window
void setup_main_window(AppData *app) {
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "Student Record Management System");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 1000, 700);
    gtk_window_set_position(GTK_WINDOW(app->window), GTK_WIN_POS_CENTER);

    // Set window icon (if available)
    gtk_window_set_icon_name(GTK_WINDOW(app->window), "application-x-executable");

    g_signal_connect(app->window, "destroy", G_CALLBACK(on_window_destroy), app);

    // Create main container
    app->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(app->window), app->main_box);

    // Setup components
    setup_menu_bar(app);
    setup_toolbar(app);
    setup_notebook_tabs(app);
    setup_status_bar(app);
}

// Setup menu bar
void setup_menu_bar(AppData *app) {
    app->menu_bar = gtk_menu_bar_new();

    // File menu
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

    GtkWidget *save_item = gtk_menu_item_new_with_label("Save Records");
    GtkWidget *load_item = gtk_menu_item_new_with_label("Load Records");
    GtkWidget *export_item = gtk_menu_item_new_with_label("Export to CSV");
    GtkWidget *separator1 = gtk_separator_menu_item_new();
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), load_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), export_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    // Sort menu
    GtkWidget *sort_menu = gtk_menu_new();
    GtkWidget *sort_item = gtk_menu_item_new_with_label("Sort");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(sort_item), sort_menu);

    GtkWidget *sort_marks_asc = gtk_menu_item_new_with_label("Sort by Marks (Ascending)");
    GtkWidget *sort_marks_desc = gtk_menu_item_new_with_label("Sort by Marks (Descending)");
    GtkWidget *sort_name = gtk_menu_item_new_with_label("Sort by Name");
    GtkWidget *sort_roll = gtk_menu_item_new_with_label("Sort by Roll Number");

    gtk_menu_shell_append(GTK_MENU_SHELL(sort_menu), sort_marks_asc);
    gtk_menu_shell_append(GTK_MENU_SHELL(sort_menu), sort_marks_desc);
    gtk_menu_shell_append(GTK_MENU_SHELL(sort_menu), sort_name);
    gtk_menu_shell_append(GTK_MENU_SHELL(sort_menu), sort_roll);

    // Statistics menu
    GtkWidget *stats_menu = gtk_menu_new();
    GtkWidget *stats_item = gtk_menu_item_new_with_label("Statistics");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(stats_item), stats_menu);

    GtkWidget *calc_stats = gtk_menu_item_new_with_label("Calculate Statistics");
    gtk_menu_shell_append(GTK_MENU_SHELL(stats_menu), calc_stats);

    // Help menu
    GtkWidget *help_menu = gtk_menu_new();
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);

    GtkWidget *about_item = gtk_menu_item_new_with_label("About");
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);

    // Add menus to menu bar
    gtk_menu_shell_append(GTK_MENU_SHELL(app->menu_bar), file_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(app->menu_bar), sort_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(app->menu_bar), stats_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(app->menu_bar), help_item);

    gtk_box_pack_start(GTK_BOX(app->main_box), app->menu_bar, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_file_activate), app);
    g_signal_connect(load_item, "activate", G_CALLBACK(on_load_file_activate), app);
    g_signal_connect(export_item, "activate", G_CALLBACK(on_export_csv_activate), app);
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_window_destroy), app);
    g_signal_connect(sort_marks_asc, "activate", G_CALLBACK(on_sort_by_marks_asc_activate), app);
    g_signal_connect(sort_marks_desc, "activate", G_CALLBACK(on_sort_by_marks_desc_activate), app);
    g_signal_connect(sort_name, "activate", G_CALLBACK(on_sort_by_name_activate), app);
    g_signal_connect(calc_stats, "activate", G_CALLBACK(on_calculate_stats_activate), app);
    g_signal_connect(about_item, "activate", G_CALLBACK(on_about_activate), app);
}

// Setup toolbar
void setup_toolbar(AppData *app) {
    app->toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(app->toolbar), GTK_TOOLBAR_BOTH);

    // Add toolbar buttons
    GtkToolItem *add_button = gtk_tool_button_new(NULL, "Add Student");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(add_button), "list-add");

    GtkToolItem *remove_button = gtk_tool_button_new(NULL, "Remove Student");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(remove_button), "list-remove");

    GtkToolItem *separator = gtk_separator_tool_item_new();

    GtkToolItem *save_button = gtk_tool_button_new(NULL, "Save");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save_button), "document-save");

    GtkToolItem *load_button = gtk_tool_button_new(NULL, "Load");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(load_button), "document-open");

    gtk_toolbar_insert(GTK_TOOLBAR(app->toolbar), add_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(app->toolbar), remove_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(app->toolbar), separator, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(app->toolbar), save_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(app->toolbar), load_button, -1);

    gtk_box_pack_start(GTK_BOX(app->main_box), app->toolbar, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_file_activate), app);
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_file_activate), app);
}

// Setup notebook with tabs - FIXED VERSION
void setup_notebook_tabs(AppData *app) {
    app->notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(app->notebook), GTK_POS_TOP);

    // Create widgets first, then setup their content
    setup_student_list_view(app);
    setup_search_panel(app);
    setup_student_form(app);
    setup_statistics_panel(app);

    // Student Management Tab
    GtkWidget *student_tab = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(student_tab), 10);

    // Left panel for student list
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create search panel container
    GtkWidget *search_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(search_panel), 5);
    gtk_box_pack_start(GTK_BOX(search_panel), app->search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(search_panel), app->search_button, FALSE, FALSE, 0);

    // Add search panel to left panel
    gtk_box_pack_start(GTK_BOX(left_panel), search_panel, FALSE, FALSE, 0);

    // Create scrolled window for tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), app->tree_view);
    gtk_box_pack_start(GTK_BOX(left_panel), scrolled, TRUE, TRUE, 0);

    // Right panel for student form
    GtkWidget *right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create form frame and grid
    GtkWidget *form_frame = gtk_frame_new("Student Information");
    GtkWidget *form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(form_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(form_grid), 10);

    // Create labels
    GtkWidget *name_label = gtk_label_new("Name:");
    GtkWidget *roll_label = gtk_label_new("Roll Number:");
    GtkWidget *marks_label = gtk_label_new("Marks:");

    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    gtk_widget_set_halign(roll_label, GTK_ALIGN_START);
    gtk_widget_set_halign(marks_label, GTK_ALIGN_START);

    // Layout form elements
    gtk_grid_attach(GTK_GRID(form_grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), roll_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->roll_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), marks_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->marks_entry, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(form_grid), app->add_button, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->update_button, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->delete_button, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(form_grid), app->clear_button, 0, 6, 2, 1);

    gtk_container_add(GTK_CONTAINER(form_frame), form_grid);
    gtk_box_pack_start(GTK_BOX(right_panel), form_frame, FALSE, FALSE, 0);

    // Add panels to student tab
    gtk_box_pack_start(GTK_BOX(student_tab), left_panel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(student_tab), right_panel, FALSE, FALSE, 0);

    // Statistics Tab
    GtkWidget *stats_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(stats_tab), 10);
    gtk_box_pack_start(GTK_BOX(stats_tab), app->stats_label, TRUE, TRUE, 0);

    // Add tabs to notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), student_tab,
                            gtk_label_new("Student Management"));
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), stats_tab,
                            gtk_label_new("Statistics"));

    gtk_box_pack_start(GTK_BOX(app->main_box), app->notebook, TRUE, TRUE, 0);
}

// Setup student list view with tree view
void setup_student_list_view(AppData *app) {
    // Create list store
    app->list_store = gtk_list_store_new(NUM_COLS,
                                        G_TYPE_INT,    // Roll Number
                                        G_TYPE_STRING, // Name
                                        G_TYPE_FLOAT,  // Marks
                                        G_TYPE_STRING); // Status

    // Create tree view
    app->tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(app->list_store));

    // Create columns
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Roll Number column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Roll Number", renderer,
                                                     "text", COL_ROLL_NUMBER, NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_ROLL_NUMBER);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->tree_view), column);

    // Name column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer,
                                                     "text", COL_NAME, NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_NAME);
    gtk_tree_view_column_set_expand(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->tree_view), column);

    // Marks column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Marks", renderer,
                                                     "text", COL_MARKS, NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_MARKS);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->tree_view), column);

    // Status column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status", renderer,
                                                     "text", COL_STATUS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->tree_view), column);

    // Setup selection
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->tree_view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(on_student_selected), app);
}

// Setup student form - create widgets only
void setup_student_form(AppData *app) {
    // Create entry widgets
    app->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->name_entry), "Enter student name");

    app->roll_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->roll_entry), "Enter roll number");

    app->marks_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->marks_entry), "Enter marks (0-100)");

    // Create buttons
    app->add_button = gtk_button_new_with_label("Add Student");
    app->update_button = gtk_button_new_with_label("Update Student");
    app->delete_button = gtk_button_new_with_label("Delete Student");
    app->clear_button = gtk_button_new_with_label("Clear Form");

    // Initially disable update and delete buttons
    gtk_widget_set_sensitive(app->update_button, FALSE);
    gtk_widget_set_sensitive(app->delete_button, FALSE);

    // Connect signals
    g_signal_connect(app->add_button, "clicked", G_CALLBACK(on_add_student_clicked), app);
    g_signal_connect(app->update_button, "clicked", G_CALLBACK(on_update_student_clicked), app);
    g_signal_connect(app->delete_button, "clicked", G_CALLBACK(on_delete_student_clicked), app);
    g_signal_connect(app->clear_button, "clicked", G_CALLBACK(on_clear_form_clicked), app);
}

// Setup search panel - create widgets only
void setup_search_panel(AppData *app) {
    app->search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->search_entry), "Enter roll number to search");

    app->search_button = gtk_button_new_with_label("Search");

    g_signal_connect(app->search_button, "clicked", G_CALLBACK(on_search_clicked), app);
}

// Setup statistics panel
void setup_statistics_panel(AppData *app) {
    GtkWidget *stats_frame = gtk_frame_new("Statistics");
    GtkWidget *stats_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(stats_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(stats_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(stats_grid), 20);

    // Create labels for statistics
    app->total_label = gtk_label_new("Total Students: 0");
    app->average_label = gtk_label_new("Average Marks: 0.00");
    app->passed_label = gtk_label_new("Passed Students: 0");
    app->failed_label = gtk_label_new("Failed Students: 0");

    // Style the labels
    PangoAttrList *attrs = pango_attr_list_new();
    PangoAttribute *attr = pango_attr_size_new(14 * PANGO_SCALE);
    pango_attr_list_insert(attrs, attr);

    gtk_label_set_attributes(GTK_LABEL(app->total_label), attrs);
    gtk_label_set_attributes(GTK_LABEL(app->average_label), attrs);
    gtk_label_set_attributes(GTK_LABEL(app->passed_label), attrs);
    gtk_label_set_attributes(GTK_LABEL(app->failed_label), attrs);

    pango_attr_list_unref(attrs);

    // Layout
    gtk_grid_attach(GTK_GRID(stats_grid), app->total_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(stats_grid), app->average_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(stats_grid), app->passed_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(stats_grid), app->failed_label, 0, 3, 1, 1);

    gtk_container_add(GTK_CONTAINER(stats_frame), stats_grid);
    app->stats_label = stats_frame;
}

// Setup status bar
void setup_status_bar(AppData *app) {
    app->status_bar = gtk_statusbar_new();
    app->status_context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(app->status_bar),
                                                          "main");
    gtk_box_pack_start(GTK_BOX(app->main_box), app->status_bar, FALSE, FALSE, 0);

    set_status_message(app, "Ready");
}

// Event Handlers

// Window destroy handler
void on_window_destroy(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    // Auto-save before exit
    if (app && app->db) {
        save_to_file(app->db, get_default_filename());
    }

    gtk_main_quit();
}

// Add student button clicked
void on_add_student_clicked(GtkButton *button __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    if (!validate_input_fields(app)) {
        return;
    }

    const char *name = gtk_entry_get_text(GTK_ENTRY(app->name_entry));
    const char *roll_text = gtk_entry_get_text(GTK_ENTRY(app->roll_entry));
    const char *marks_text = gtk_entry_get_text(GTK_ENTRY(app->marks_entry));

    int roll_number = atoi(roll_text);
    float marks = atof(marks_text);

    if (add_student(app->db, name, roll_number, marks)) {
        refresh_student_list(app);
        update_statistics_display(app);
        clear_form_fields(app);
        set_status_message(app, "Student added successfully");
    } else {
        show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                          "Failed to add student. Roll number may already exist.");
    }
}

// Update student button clicked
void on_update_student_clicked(GtkButton *button __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    if (!app->selected_student || !validate_input_fields(app)) {
        return;
    }

    const char *name = gtk_entry_get_text(GTK_ENTRY(app->name_entry));
    const char *marks_text = gtk_entry_get_text(GTK_ENTRY(app->marks_entry));

    float marks = atof(marks_text);

    if (modify_student(app->db, app->selected_student->roll_number, name, marks)) {
        refresh_student_list(app);
        update_statistics_display(app);
        clear_form_fields(app);
        set_status_message(app, "Student updated successfully");
    } else {
        show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                          "Failed to update student");
    }
}

// Delete student button clicked
void on_delete_student_clicked(GtkButton *button __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    if (!app->selected_student) {
        return;
    }

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
                                              GTK_DIALOG_MODAL,
                                              GTK_MESSAGE_QUESTION,
                                              GTK_BUTTONS_YES_NO,
                                              "Are you sure you want to delete student '%s' (Roll: %d)?",
                                              app->selected_student->name,
                                              app->selected_student->roll_number);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES) {
        if (remove_student(app->db, app->selected_student->roll_number)) {
            refresh_student_list(app);
            update_statistics_display(app);
            clear_form_fields(app);
            set_status_message(app, "Student deleted successfully");
        } else {
            show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                              "Failed to delete student");
        }
    }
}

// Clear form button clicked
void on_clear_form_clicked(GtkButton *button __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;
    clear_form_fields(app);
    app->selected_student = NULL;
    set_status_message(app, "Form cleared");
}

// Search button clicked
void on_search_clicked(GtkButton *button __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    const char *search_text = gtk_entry_get_text(GTK_ENTRY(app->search_entry));
    if (strlen(search_text) == 0) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Warning",
                          "Please enter a roll number to search");
        return;
    }

    int roll_number = atoi(search_text);
    Student *student = find_student(app->db, roll_number);

    if (student) {
        populate_form_fields(app, student);
        app->selected_student = student;

        // Highlight the student in the tree view
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(app->list_store), &iter);

       // Continuation of gui.c - Event handlers and helper functions

        while (valid) {
            int row_roll;
            gtk_tree_model_get(GTK_TREE_MODEL(app->list_store), &iter,
                             COL_ROLL_NUMBER, &row_roll, -1);

            if (row_roll == roll_number) {
                GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->tree_view));
                gtk_tree_selection_select_iter(selection, &iter);

                GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(app->list_store), &iter);
                gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(app->tree_view), path, NULL, FALSE, 0.0, 0.0);
                gtk_tree_path_free(path);
                break;
            }

            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(app->list_store), &iter);
        }

        set_status_message(app, "Student found");
    } else {
        char message[100];
        snprintf(message, sizeof(message), "Student with roll number %d not found", roll_number);
        show_message_dialog(app->window, GTK_MESSAGE_INFO, "Not Found", message);
        set_status_message(app, "Student not found");
    }

    gtk_entry_set_text(GTK_ENTRY(app->search_entry), "");
}

// Student selected in tree view
void on_student_selected(GtkTreeSelection *selection, gpointer data) {
    AppData *app = (AppData*)data;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int roll_number;
        gtk_tree_model_get(model, &iter, COL_ROLL_NUMBER, &roll_number, -1);

        Student *student = find_student(app->db, roll_number);
        if (student) {
            populate_form_fields(app, student);
            app->selected_student = student;

            // Enable update and delete buttons
            gtk_widget_set_sensitive(app->update_button, TRUE);
            gtk_widget_set_sensitive(app->delete_button, TRUE);
        }
    } else {
        clear_form_fields(app);
        app->selected_student = NULL;

        // Disable update and delete buttons
        gtk_widget_set_sensitive(app->update_button, FALSE);
        gtk_widget_set_sensitive(app->delete_button, FALSE);
    }
}

// Menu event handlers
void on_save_file_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Student Records",
                                                   GTK_WINDOW(app->window),
                                                   GTK_FILE_CHOOSER_ACTION_SAVE,
                                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                                   "_Save", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "students.txt");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (save_to_file(app->db, filename)) {
            set_status_message(app, "File saved successfully");
        } else {
            show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                              "Failed to save file");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_load_file_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Load Student Records",
                                                   GTK_WINDOW(app->window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                                   "_Open", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (load_from_file(app->db, filename)) {
            refresh_student_list(app);
            update_statistics_display(app);
            clear_form_fields(app);
            set_status_message(app, "File loaded successfully");
        } else {
            show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                              "Failed to load file");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_export_csv_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Export to CSV",
                                                   GTK_WINDOW(app->window),
                                                   GTK_FILE_CHOOSER_ACTION_SAVE,
                                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                                   "_Export", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "students.csv");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (export_to_csv(app->db, filename)) {
            set_status_message(app, "Exported to CSV successfully");
        } else {
            show_message_dialog(app->window, GTK_MESSAGE_ERROR, "Error",
                              "Failed to export file");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_sort_by_marks_asc_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;
    sort_students(app->db, SORT_BY_MARKS_ASC);
    refresh_student_list(app);
    set_status_message(app, "Sorted by marks (ascending)");
}

void on_sort_by_marks_desc_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;
    sort_students(app->db, SORT_BY_MARKS_DESC);
    refresh_student_list(app);
    set_status_message(app, "Sorted by marks (descending)");
}

void on_sort_by_name_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;
    sort_students(app->db, SORT_BY_NAME_ASC);
    refresh_student_list(app);
    set_status_message(app, "Sorted by name");
}

void on_calculate_stats_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app = (AppData*)data;
    update_statistics_display(app);

    // Switch to statistics tab
    gtk_notebook_set_current_page(GTK_NOTEBOOK(app->notebook), 1);
    set_status_message(app, "Statistics calculated");
}

void on_about_activate(GtkMenuItem *menuitem __attribute__((unused)), gpointer data) {
    AppData *app __attribute__((unused)) = (AppData*)data;

    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),
                                     "Student Record Management System");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                 "A comprehensive student record management system built with GTK and C");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://github.com/");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), "Project Homepage");

    const char *authors[] = {"Your Name", NULL};
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Helper Functions

// Refresh the student list view
void refresh_student_list(AppData *app) {
    if (!app || !app->list_store || !app->db) {
        return;
    }

    // Clear existing data
    gtk_list_store_clear(app->list_store);

    // Add all students to the list
    for (int i = 0; i < app->db->count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(app->list_store, &iter);
        gtk_list_store_set(app->list_store, &iter,
                          COL_ROLL_NUMBER, app->db->students[i].roll_number,
                          COL_NAME, app->db->students[i].name,
                          COL_MARKS, app->db->students[i].marks,
                          COL_STATUS, app->db->students[i].status,
                          -1);
    }
}

// Clear form fields
void clear_form_fields(AppData *app) {
    if (!app) return;

    gtk_entry_set_text(GTK_ENTRY(app->name_entry), "");
    gtk_entry_set_text(GTK_ENTRY(app->roll_entry), "");
    gtk_entry_set_text(GTK_ENTRY(app->marks_entry), "");

    // Clear tree view selection
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->tree_view));
    gtk_tree_selection_unselect_all(selection);

    // Disable update and delete buttons
    gtk_widget_set_sensitive(app->update_button, FALSE);
    gtk_widget_set_sensitive(app->delete_button, FALSE);

    app->selected_student = NULL;
}

// Populate form fields with student data
void populate_form_fields(AppData *app, Student *student) {
    if (!app || !student) return;

    char roll_str[20];
    char marks_str[20];

    snprintf(roll_str, sizeof(roll_str), "%d", student->roll_number);
    snprintf(marks_str, sizeof(marks_str), "%.2f", student->marks);

    gtk_entry_set_text(GTK_ENTRY(app->name_entry), student->name);
    gtk_entry_set_text(GTK_ENTRY(app->roll_entry), roll_str);
    gtk_entry_set_text(GTK_ENTRY(app->marks_entry), marks_str);

    // Enable update and delete buttons
    gtk_widget_set_sensitive(app->update_button, TRUE);
    gtk_widget_set_sensitive(app->delete_button, TRUE);
}

// Update statistics display
void update_statistics_display(AppData *app) {
    if (!app || !app->db) return;

    Statistics stats = calculate_statistics(app->db);

    char total_text[100];
    char average_text[100];
    char passed_text[100];
    char failed_text[100];

    snprintf(total_text, sizeof(total_text), "Total Students: %d", stats.total_students);
    snprintf(average_text, sizeof(average_text), "Average Marks: %.2f", stats.average_marks);
    snprintf(passed_text, sizeof(passed_text), "Passed Students: %d (%.1f%%)",
             stats.passed_students, stats.pass_percentage);
    snprintf(failed_text, sizeof(failed_text), "Failed Students: %d", stats.failed_students);

    gtk_label_set_text(GTK_LABEL(app->total_label), total_text);
    gtk_label_set_text(GTK_LABEL(app->average_label), average_text);
    gtk_label_set_text(GTK_LABEL(app->passed_label), passed_text);
    gtk_label_set_text(GTK_LABEL(app->failed_label), failed_text);
}

// Show message dialog
void show_message_dialog(GtkWidget *parent, GtkMessageType type, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                              GTK_DIALOG_MODAL,
                                              type,
                                              GTK_BUTTONS_OK,
                                              "%s", message);

    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Validate input fields
bool validate_input_fields(AppData *app) {
    if (!app) return false;

    const char *name = gtk_entry_get_text(GTK_ENTRY(app->name_entry));
    const char *roll_text = gtk_entry_get_text(GTK_ENTRY(app->roll_entry));
    const char *marks_text = gtk_entry_get_text(GTK_ENTRY(app->marks_entry));

    // Check if fields are empty
    if (strlen(name) == 0) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Validation Error",
                          "Please enter student name");
        gtk_widget_grab_focus(app->name_entry);
        return false;
    }

    if (strlen(roll_text) == 0) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Validation Error",
                          "Please enter roll number");
        gtk_widget_grab_focus(app->roll_entry);
        return false;
    }

    if (strlen(marks_text) == 0) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Validation Error",
                          "Please enter marks");
        gtk_widget_grab_focus(app->marks_entry);
        return false;
    }

    // Validate roll number
    int roll_number = atoi(roll_text);
    if (roll_number <= 0) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Validation Error",
                          "Roll number must be a positive integer");
        gtk_widget_grab_focus(app->roll_entry);
        return false;
    }

    // Validate marks
    float marks = atof(marks_text);
    if (marks < 0 || marks > 100) {
        show_message_dialog(app->window, GTK_MESSAGE_WARNING, "Validation Error",
                          "Marks must be between 0 and 100");
        gtk_widget_grab_focus(app->marks_entry);
        return false;
    }

    return true;
}

// Set status message
void set_status_message(AppData *app, const char *message) {
    if (!app || !app->status_bar || !message) return;

    gtk_statusbar_pop(GTK_STATUSBAR(app->status_bar), app->status_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(app->status_bar), app->status_context_id, message);
}

// Show welcome dialog
void show_welcome_dialog(AppData *app) {
    if (!app) return;

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Welcome",
                                                   GTK_WINDOW(app->window),
                                                   GTK_DIALOG_MODAL,
                                                   "_OK", GTK_RESPONSE_OK,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);

    GtkWidget *welcome_label = gtk_label_new("Welcome to Student Record Management System");
    PangoAttrList *attrs = pango_attr_list_new();
    PangoAttribute *attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
    pango_attr_list_insert(attrs, attr);
    attr = pango_attr_size_new(16 * PANGO_SCALE);
    pango_attr_list_insert(attrs, attr);
    gtk_label_set_attributes(GTK_LABEL(welcome_label), attrs);
    pango_attr_list_unref(attrs);

    GtkWidget *name_label = gtk_label_new("Please enter your name:");
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Your name");

    gtk_grid_attach(GTK_GRID(grid), welcome_label, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        const char *user_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        if (strlen(user_name) > 0) {
            strncpy(app->user_name, user_name, MAX_NAME_LENGTH - 1);
            app->user_name[MAX_NAME_LENGTH - 1] = '\0';
        }
    }

    gtk_widget_destroy(dialog);

    // Update window title with user name
    char title[200];
    snprintf(title, sizeof(title), "Student Record Management System - Welcome %s", app->user_name);
    gtk_window_set_title(GTK_WINDOW(app->window), title);

    char status_msg[150];
    snprintf(status_msg, sizeof(status_msg), "Welcome %s! Ready to manage student records.", app->user_name);
    set_status_message(app, status_msg);
}