#include "wizard.h"
#include <string.h>

// Category names array
static const gchar *category_names[] = {
    "Utility", "Graphics", "Network", "Office", "Development",
    "AudioVideo", "System", "Settings", "Games"
};

// Global wizard state to prevent corruption
static WizardState *g_wizard_state = NULL;

WizardState* wizard_new(GtkWidget *parent_window) {
    WizardState *wizard = g_new0(WizardState, 1);
    
    wizard->window = parent_window;
    wizard->entry = desktop_entry_new();
    wizard->save_options = file_save_options_new();
    wizard->current_step = WIZARD_STEP_BASIC_INFO;
    wizard->preview_content = NULL;
    
    // Store in global variable
    g_wizard_state = wizard;
    
    // Create main container
    wizard->main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(wizard->main_container), 5);
    
    // Create step container
    wizard->step_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(wizard->main_container), wizard->step_container, TRUE, TRUE, 0);
    
    // Create navigation frame
    wizard->navigation_frame = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(wizard->navigation_frame), GTK_BUTTONBOX_END);
    gtk_box_pack_end(GTK_BOX(wizard->main_container), wizard->navigation_frame, FALSE, FALSE, 0);
    
    return wizard;
}

void wizard_free(WizardState *wizard) {
    if (wizard) {
        desktop_entry_free(wizard->entry);
        file_save_options_free(wizard->save_options);
        g_free(wizard->preview_content);
        g_free(wizard);
    }
}

void wizard_show(WizardState *wizard) {
    wizard_create_basic_info_step(wizard);
}

static void clear_step_container(WizardState *wizard) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(wizard->step_container));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    
    children = gtk_container_get_children(GTK_CONTAINER(wizard->navigation_frame));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

static void create_navigation_buttons(WizardState *wizard) {
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    GtkWidget *next_button = gtk_button_new_with_label("Next");
    
    gtk_widget_set_sensitive(back_button, wizard->current_step > WIZARD_STEP_BASIC_INFO);
    
    g_signal_connect(back_button, "clicked", G_CALLBACK(wizard_previous_step), wizard);
    g_signal_connect(next_button, "clicked", G_CALLBACK(wizard_next_step), wizard);
    
    gtk_container_add(GTK_CONTAINER(wizard->navigation_frame), back_button);
    gtk_container_add(GTK_CONTAINER(wizard->navigation_frame), next_button);
    
    gtk_widget_show_all(wizard->navigation_frame);
}



void wizard_create_basic_info_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_BASIC_INFO;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 1: Basic Information</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create form
    GtkWidget *form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(form_grid), 10);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), form_grid, FALSE, FALSE, 10);
    
    // Name field
    gtk_grid_attach(GTK_GRID(form_grid), gtk_label_new("Application Name:"), 0, 0, 1, 1);
    wizard->name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(form_grid), wizard->name_entry, 1, 0, 1, 1);
    
    // Comment field
    gtk_grid_attach(GTK_GRID(form_grid), gtk_label_new("Short Description:"), 0, 1, 1, 1);
    wizard->comment_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(form_grid), wizard->comment_entry, 1, 1, 1, 1);
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
    
    // Update UI with existing data
    if (wizard->entry->name) {
        gtk_entry_set_text(GTK_ENTRY(wizard->name_entry), wizard->entry->name);
    }
    if (wizard->entry->comment) {
        gtk_entry_set_text(GTK_ENTRY(wizard->comment_entry), wizard->entry->comment);
    }
}

void wizard_create_executable_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_EXECUTABLE;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 2: Executable</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create form
    GtkWidget *form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(form_grid), 10);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), form_grid, FALSE, FALSE, 10);
    
    // Executable field
    gtk_grid_attach(GTK_GRID(form_grid), gtk_label_new("Location:"), 0, 0, 1, 1);
    
    GtkWidget *exec_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    wizard->exec_entry = gtk_entry_new();
    wizard->exec_browse_button = gtk_button_new_with_label("Browse...");
    gtk_box_pack_start(GTK_BOX(exec_box), wizard->exec_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(exec_box), wizard->exec_browse_button, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(form_grid), exec_box, 1, 0, 1, 1);
    
    // Terminal checkbox (only for applications)
    wizard->terminal_check = gtk_check_button_new_with_label("Run in Terminal");
    gtk_grid_attach(GTK_GRID(form_grid), wizard->terminal_check, 0, 1, 2, 1);
    
    // Add helpful description
    GtkWidget *terminal_desc = gtk_label_new("Check this if the application needs to run in a terminal or shows errors when launched");
    gtk_label_set_line_wrap(GTK_LABEL(terminal_desc), TRUE);
    gtk_grid_attach(GTK_GRID(form_grid), terminal_desc, 0, 2, 2, 1);
    
    // Connect signals
    g_signal_connect(wizard->exec_browse_button, "clicked", 
                    G_CALLBACK(wizard_on_browse_executable), wizard);
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
    
    // Update UI with existing data
    if (wizard->entry->exec_path) {
        gtk_entry_set_text(GTK_ENTRY(wizard->exec_entry), wizard->entry->exec_path);
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wizard->terminal_check), wizard->entry->terminal);
}

void wizard_create_icon_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_ICON;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 3: Icon (Optional)</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create form
    GtkWidget *form_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(form_grid), 10);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), form_grid, FALSE, FALSE, 10);
    
    // Icon field
    gtk_grid_attach(GTK_GRID(form_grid), gtk_label_new("Icon File Location:"), 0, 0, 1, 1);
    
    GtkWidget *icon_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    wizard->icon_entry = gtk_entry_new();
    wizard->icon_browse_button = gtk_button_new_with_label("Browse...");
    gtk_box_pack_start(GTK_BOX(icon_box), wizard->icon_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(icon_box), wizard->icon_browse_button, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(form_grid), icon_box, 1, 0, 1, 1);
    
    // Connect signals
    g_signal_connect(wizard->icon_browse_button, "clicked", 
                    G_CALLBACK(wizard_on_browse_icon), wizard);
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
    
    // Update UI with existing data
    if (wizard->entry->icon_path) {
        gtk_entry_set_text(GTK_ENTRY(wizard->icon_entry), wizard->entry->icon_path);
    }
}

void wizard_create_categories_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_CATEGORIES;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 4: Categories (Optional)</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create description
    GtkWidget *desc_label = gtk_label_new("Select one or more categories for your application:");
    gtk_box_pack_start(GTK_BOX(wizard->step_container), desc_label, FALSE, FALSE, 10);
    
    // Create checkboxes in a grid
    GtkWidget *cat_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(cat_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(cat_grid), 20);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), cat_grid, FALSE, FALSE, 10);
    
    for (int i = 0; i < 9; i++) {
        wizard->category_checks[i] = gtk_check_button_new_with_label(category_names[i]);
        gtk_grid_attach(GTK_GRID(cat_grid), wizard->category_checks[i], i % 3, i / 3, 1, 1);
    }
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
}

void wizard_create_preview_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_PREVIEW;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 5: Preview and Edit</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create description
    GtkWidget *desc_label = gtk_label_new("Review and edit the .desktop file content:");
    gtk_box_pack_start(GTK_BOX(wizard->step_container), desc_label, FALSE, FALSE, 10);
    
    // Create text view with scrollbar
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled_window), 150);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), scrolled_window, TRUE, TRUE, 5);
    
    wizard->preview_text = gtk_text_view_new();
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(wizard->preview_text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(wizard->preview_text), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_window), wizard->preview_text);
    
    // Generate preview content
    wizard_generate_preview(wizard);
    
    // Connect text buffer signal
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(wizard->preview_text));
    g_signal_connect(buffer, "changed", G_CALLBACK(wizard_on_preview_changed), wizard);
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
}

void wizard_create_distribution_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_DISTRIBUTION;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Step 6: Distribution</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create description
    GtkWidget *desc_label = gtk_label_new("Choose where to save the .desktop file:");
    gtk_box_pack_start(GTK_BOX(wizard->step_container), desc_label, FALSE, FALSE, 10);
    
    // Create checkboxes
    GtkWidget *options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), options_box, FALSE, FALSE, 10);
    
    gchar *desktop_path = file_utils_get_desktop_directory();
    gchar *local_apps_path = file_utils_get_local_applications_directory();
    
    wizard->save_checkboxes[0] = gtk_check_button_new_with_label(
        g_strdup_printf("User's Desktop (%s)", desktop_path));
    wizard->save_checkboxes[1] = gtk_check_button_new_with_label(
        g_strdup_printf("User's Local Applications (%s)", local_apps_path));
    wizard->save_checkboxes[2] = gtk_check_button_new_with_label(
        "Save to custom location");
    
    gtk_box_pack_start(GTK_BOX(options_box), wizard->save_checkboxes[0], FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(options_box), wizard->save_checkboxes[1], FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(options_box), wizard->save_checkboxes[2], FALSE, FALSE, 5);
    
    // Custom path entry
    GtkWidget *custom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(custom_box), gtk_label_new("Custom path:"), FALSE, FALSE, 0);
    wizard->custom_path_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(custom_box), wizard->custom_path_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(options_box), custom_box, FALSE, FALSE, 5);
    
    gtk_widget_set_sensitive(wizard->custom_path_entry, FALSE);
    
    // Connect signals
    for (int i = 0; i < 3; i++) {
        g_signal_connect(wizard->save_checkboxes[i], "toggled", 
                        G_CALLBACK(wizard_on_save_option_changed), wizard);
    }
    
    g_free(desktop_path);
    g_free(local_apps_path);
    
    create_navigation_buttons(wizard);
    gtk_widget_show_all(wizard->step_container);
}

void wizard_create_complete_step(WizardState *wizard) {
    clear_step_container(wizard);
    wizard->current_step = WIZARD_STEP_COMPLETE;
    
    // Create title
    GtkWidget *title_label = gtk_label_new(NULL);
    gchar *title_markup = g_markup_printf_escaped("<span size='large' weight='bold'>Complete!</span>");
    gtk_label_set_markup(GTK_LABEL(title_label), title_markup);
    g_free(title_markup);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), title_label, FALSE, FALSE, 10);
    
    // Create message
    GtkWidget *msg_label = gtk_label_new("Your .desktop file has been created successfully!");
    gtk_box_pack_start(GTK_BOX(wizard->step_container), msg_label, FALSE, FALSE, 10);
    
    // Create buttons
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_CENTER);
    gtk_box_pack_start(GTK_BOX(wizard->step_container), button_box, FALSE, FALSE, 20);
    
    GtkWidget *new_button = gtk_button_new_with_label("Create Another");
    GtkWidget *quit_button = gtk_button_new_with_label("Quit");
    
    g_signal_connect(new_button, "clicked", G_CALLBACK(wizard_show), wizard);
    g_signal_connect_swapped(quit_button, "clicked", G_CALLBACK(gtk_widget_destroy), wizard->window);
    
    gtk_container_add(GTK_CONTAINER(button_box), new_button);
    gtk_container_add(GTK_CONTAINER(button_box), quit_button);
    
    gtk_widget_show_all(wizard->step_container);
}

void wizard_next_step(WizardState *wizard) {
    // Use global wizard state instead of parameter
    wizard = g_wizard_state;
    
    if (!wizard) {
        return;
    }
    
    gchar *error_msg = NULL;
    if (!wizard_validate_current_step(wizard, &error_msg)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(wizard->window),
                                                  GTK_DIALOG_MODAL,
                                                  GTK_MESSAGE_ERROR,
                                                  GTK_BUTTONS_OK,
                                                  "Validation Error");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", error_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        g_free(error_msg);
        return;
    }
    
    // Update entry data from current step before moving to next
    wizard_update_entry_from_current_step(wizard);
    
    switch (wizard->current_step) {

        case WIZARD_STEP_BASIC_INFO:
            wizard_create_executable_step(wizard);
            break;
        case WIZARD_STEP_EXECUTABLE:
            wizard_create_icon_step(wizard);
            break;
        case WIZARD_STEP_ICON:
            wizard_create_categories_step(wizard);
            break;
        case WIZARD_STEP_CATEGORIES:
            wizard_create_preview_step(wizard);
            break;
        case WIZARD_STEP_PREVIEW:
            wizard_create_distribution_step(wizard);
            break;
        case WIZARD_STEP_DISTRIBUTION:
            if (wizard_save_files(wizard, &error_msg)) {
                wizard_create_complete_step(wizard);
            } else {
                GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(wizard->window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_OK,
                                                          "Save Error");
                gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", error_msg);
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                g_free(error_msg);
            }
            break;
        default:
            break;
    }
}

void wizard_previous_step(WizardState *wizard) {
    // Use global wizard state instead of parameter
    wizard = g_wizard_state;
    
    switch (wizard->current_step) {
        case WIZARD_STEP_BASIC_INFO:
            // This is the first step, no previous
            break;
        case WIZARD_STEP_EXECUTABLE:
            wizard_create_basic_info_step(wizard);
            break;
        case WIZARD_STEP_ICON:
            wizard_create_executable_step(wizard);
            break;
        case WIZARD_STEP_CATEGORIES:
            wizard_create_icon_step(wizard);
            break;
        case WIZARD_STEP_PREVIEW:
            wizard_create_categories_step(wizard);
            break;
        case WIZARD_STEP_DISTRIBUTION:
            wizard_create_preview_step(wizard);
            break;
        default:
            break;
    }
}

void wizard_update_entry_from_current_step(WizardState *wizard) {
    switch (wizard->current_step) {

        case WIZARD_STEP_BASIC_INFO:
            g_free(wizard->entry->name);
            wizard->entry->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(wizard->name_entry)));
            g_free(wizard->entry->comment);
            wizard->entry->comment = g_strdup(gtk_entry_get_text(GTK_ENTRY(wizard->comment_entry)));
            break;
        case WIZARD_STEP_EXECUTABLE:
            g_free(wizard->entry->exec_path);
            wizard->entry->exec_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(wizard->exec_entry)));
            wizard->entry->terminal = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->terminal_check));
            break;
        case WIZARD_STEP_ICON:
            g_free(wizard->entry->icon_path);
            wizard->entry->icon_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(wizard->icon_entry)));
            break;
        case WIZARD_STEP_CATEGORIES:
            desktop_entry_clear_categories(&wizard->entry->categories);
            for (int i = 0; i < 9; i++) {
                if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->category_checks[i]))) {
                    desktop_entry_set_category(&wizard->entry->categories, category_names[i], TRUE);
                }
            }
            break;
        case WIZARD_STEP_PREVIEW:
            // Content is updated by text buffer callback
            break;
        case WIZARD_STEP_DISTRIBUTION:
            wizard->save_options->save_to_desktop = 
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[0]));
            wizard->save_options->save_to_local_apps = 
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[1]));
            wizard->save_options->save_to_custom = 
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[2]));
            if (wizard->save_options->save_to_custom) {
                g_free(wizard->save_options->custom_path);
                wizard->save_options->custom_path = 
                    g_strdup(gtk_entry_get_text(GTK_ENTRY(wizard->custom_path_entry)));
            }
            break;
        default:
            break;
    }
}

gboolean wizard_validate_current_step(WizardState *wizard, gchar **error_msg) {
    switch (wizard->current_step) {

        case WIZARD_STEP_BASIC_INFO:
            if (!gtk_entry_get_text(GTK_ENTRY(wizard->name_entry)) || 
                strlen(gtk_entry_get_text(GTK_ENTRY(wizard->name_entry))) == 0) {
                *error_msg = g_strdup("Application Name is required.");
                return FALSE;
            }
            return TRUE;
        case WIZARD_STEP_EXECUTABLE:
            if (!gtk_entry_get_text(GTK_ENTRY(wizard->exec_entry)) || 
                strlen(gtk_entry_get_text(GTK_ENTRY(wizard->exec_entry))) == 0) {
                *error_msg = g_strdup("Location is required.");
                return FALSE;
            }
            return TRUE;
        case WIZARD_STEP_ICON:
            // Icon is optional
            return TRUE;
        case WIZARD_STEP_CATEGORIES:
            // Categories are optional
            return TRUE;
        case WIZARD_STEP_PREVIEW:
            // Preview is always valid
            return TRUE;
        case WIZARD_STEP_DISTRIBUTION:
            gboolean save_to_desktop = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[0]));
            gboolean save_to_local_apps = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[1]));
            gboolean save_to_custom = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wizard->save_checkboxes[2]));
            

            
            if (!save_to_desktop && !save_to_local_apps && !save_to_custom) {
                *error_msg = g_strdup("Please select at least one save location.");
                return FALSE;
            }
            if (save_to_custom && 
                (!wizard->save_options->custom_path || 
                 strlen(wizard->save_options->custom_path) == 0)) {
                *error_msg = g_strdup("Please specify a custom path.");
                return FALSE;
            }
            return TRUE;
        default:
            return TRUE;
    }
}



void wizard_generate_preview(WizardState *wizard) {
    g_free(wizard->preview_content);
    wizard->preview_content = desktop_entry_generate_content(wizard->entry);
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(wizard->preview_text));
    gtk_text_buffer_set_text(buffer, wizard->preview_content, -1);
}

gboolean wizard_save_files(WizardState *wizard, gchar **error_msg) {
    // Use stored preview content instead of trying to access text view
    if (!wizard->preview_content) {
        wizard->preview_content = desktop_entry_generate_content(wizard->entry);
    }
    
    gchar *content = g_strdup(wizard->preview_content);
    gchar *filename = wizard->entry->name ? g_strdup(wizard->entry->name) : g_strdup("my_application");
    
    gboolean success = file_utils_save_desktop_file(content, filename, wizard->save_options, wizard->window, error_msg);
    
    g_free(content);
    g_free(filename);
    
    return success;
}

// Callback functions

void wizard_on_browse_executable(GtkButton *button, WizardState *wizard) {
    (void)button;  // Suppress unused parameter warning
    GtkWidget *dialog;
    gchar *filename = NULL;
    
    dialog = gtk_file_chooser_dialog_new("Select Executable",
                                       GTK_WINDOW(wizard->window),
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open", GTK_RESPONSE_ACCEPT,
                                       NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            // Validate the selected file
            gchar *exec_error = NULL;
            if (!file_utils_validate_executable(filename, &exec_error)) {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(wizard->window),
                                                               GTK_DIALOG_MODAL,
                                                               GTK_MESSAGE_ERROR,
                                                               GTK_BUTTONS_OK,
                                                               "Invalid Executable");
                gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(error_dialog), 
                                                       "The selected file is not a valid executable: %s", 
                                                       exec_error ? exec_error : "Unknown error");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
                if (exec_error) g_free(exec_error);
            } else {
                gtk_entry_set_text(GTK_ENTRY(wizard->exec_entry), filename);
            }
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
}

void wizard_on_browse_icon(GtkButton *button, WizardState *wizard) {
    (void)button;  // Suppress unused parameter warning
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Icon File",
                                                   GTK_WINDOW(wizard->window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                                   "_Open", GTK_RESPONSE_ACCEPT,
                                                   NULL);
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Image Files");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.xpm");
    gtk_file_filter_add_pattern(filter, "*.svg");
    gtk_file_filter_add_pattern(filter, "*.ico");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(wizard->icon_entry), filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void wizard_on_preview_changed(GtkTextBuffer *buffer, WizardState *wizard) {
    // Update the preview content when user edits the text
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    g_free(wizard->preview_content);
    wizard->preview_content = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

void wizard_on_save_option_changed(GtkToggleButton *button, WizardState *wizard) {
    if (button == GTK_TOGGLE_BUTTON(wizard->save_checkboxes[2])) { // Custom path checkbox
        gboolean active = gtk_toggle_button_get_active(button);
        gtk_widget_set_sensitive(wizard->custom_path_entry, active);
    }
} 