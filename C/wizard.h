#ifndef WIZARD_H
#define WIZARD_H

#include <gtk/gtk.h>
#include "desktop_entry.h"
#include "file_utils.h"

// Wizard step enumeration
typedef enum {
    WIZARD_STEP_BASIC_INFO,
    WIZARD_STEP_EXECUTABLE,
    WIZARD_STEP_ICON,
    WIZARD_STEP_CATEGORIES,
    WIZARD_STEP_PREVIEW,
    WIZARD_STEP_DISTRIBUTION,
    WIZARD_STEP_COMPLETE
} WizardStep;

// Wizard state structure
typedef struct {
    GtkWidget *window;
    GtkWidget *main_container;
    GtkWidget *step_container;
    GtkWidget *navigation_frame;
    
    DesktopEntry *entry;
    FileSaveOptions *save_options;
    
    WizardStep current_step;
    gchar *preview_content;
    
    // UI elements for each step
    GtkWidget *name_entry;
    GtkWidget *comment_entry;
    GtkWidget *exec_entry;
    GtkWidget *exec_browse_button;
    GtkWidget *icon_entry;
    GtkWidget *icon_browse_button;
    GtkWidget *terminal_check;
    GtkWidget *category_checks[9];
    GtkWidget *preview_text;
    GtkWidget *save_checkboxes[3];
    GtkWidget *custom_path_entry;
} WizardState;

// Function prototypes
WizardState* wizard_new(GtkWidget *parent_window);
void wizard_free(WizardState *wizard);
void wizard_show(WizardState *wizard);
void wizard_next_step(WizardState *wizard);
void wizard_previous_step(WizardState *wizard);

gboolean wizard_validate_current_step(WizardState *wizard, gchar **error_msg);
void wizard_generate_preview(WizardState *wizard);
void wizard_update_entry_from_current_step(WizardState *wizard);
gboolean wizard_save_files(WizardState *wizard, gchar **error_msg);

// Step-specific functions
void wizard_create_basic_info_step(WizardState *wizard);
void wizard_create_executable_step(WizardState *wizard);
void wizard_create_icon_step(WizardState *wizard);
void wizard_create_categories_step(WizardState *wizard);
void wizard_create_preview_step(WizardState *wizard);
void wizard_create_distribution_step(WizardState *wizard);
void wizard_create_complete_step(WizardState *wizard);

// Callback functions
void wizard_on_browse_executable(GtkButton *button, WizardState *wizard);
void wizard_on_browse_icon(GtkButton *button, WizardState *wizard);
void wizard_on_preview_changed(GtkTextBuffer *buffer, WizardState *wizard);
void wizard_on_save_option_changed(GtkToggleButton *button, WizardState *wizard);

#endif // WIZARD_H 