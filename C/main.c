#include <gtk/gtk.h>
#include <glib.h>
#include "wizard.h"

// Global window reference for About dialog
static GtkWidget *g_main_window = NULL;

static void on_window_destroy(GtkWidget *widget, gpointer data) {
    (void)widget;  // Suppress unused parameter warning
    (void)data;    // Suppress unused parameter warning
    gtk_main_quit();
}

static void show_about_dialog(GtkWidget *parent) {
    (void)parent;  // Suppress unused parameter warning
    GtkWidget *dialog = gtk_about_dialog_new();
    
    // Use global window reference
    if (g_main_window && GTK_IS_WINDOW(g_main_window)) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(g_main_window));
    }
    
    // Set the logo
    gchar *logo_path = g_build_filename("images", "robot-icon2.png", NULL);
    if (g_file_test(logo_path, G_FILE_TEST_EXISTS)) {
        GError *logo_error = NULL;
        GdkPixbuf *logo = gdk_pixbuf_new_from_file(logo_path, &logo_error);
        if (logo) {
            gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), logo);
            g_object_unref(logo);
        } else if (logo_error) {
            g_error_free(logo_error);
        }
    }
    g_free(logo_path);
    
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Cre8or");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "2.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "© 2025");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
        "Linux Desktop Entry Creator\n\n"
        "A wizard-based tool for creating .desktop files according to the "
        "freedesktop.org Desktop Entry Specification.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
        "https://github.com/ashes00/");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), 
        "https://github.com/ashes00/");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void create_menu_bar(GtkWidget *container, GtkWidget *window) {
    GtkWidget *menu_bar = gtk_menu_bar_new();
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *help_menu = gtk_menu_new();
    
    // File menu
    GtkWidget *file_menu_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
    
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect_swapped(quit_item, "activate", G_CALLBACK(gtk_widget_destroy), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
    
    // Help menu
    GtkWidget *help_menu_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu_item);
    
    GtkWidget *about_item = gtk_menu_item_new_with_label("About");
    g_signal_connect(about_item, "activate", G_CALLBACK(show_about_dialog), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);
    
    // Removed duplicate Quit button from top right (already available in File menu)
    
    gtk_box_pack_start(GTK_BOX(container), menu_bar, FALSE, FALSE, 0);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Cre8or");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    // Store window reference globally
    g_main_window = window;
    
    // Set window icon if available
    GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
    if (gtk_icon_theme_has_icon(icon_theme, "applications-development")) {
        gtk_window_set_icon_name(GTK_WINDOW(window), "applications-development");
    }
    
    // Create main container
    GtkWidget *main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_container);
    
    // Create menu bar
    create_menu_bar(main_container, window);
    
    // Create wizard
    WizardState *wizard = wizard_new(window);
    gtk_box_pack_start(GTK_BOX(main_container), wizard->main_container, TRUE, TRUE, 0);
    
    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);
    
    // Show the wizard
    wizard_show(wizard);
    gtk_widget_show_all(window);
    
    // Start GTK main loop
    gtk_main();
    
    // Cleanup
    wizard_free(wizard);
    
    return 0;
} 