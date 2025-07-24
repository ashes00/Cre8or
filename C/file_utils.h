#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <glib.h>
#include <gtk/gtk.h>
#include "desktop_entry.h"

// File save options
typedef struct {
    gboolean save_to_desktop;
    gboolean save_to_local_apps;
    gboolean save_to_custom;
    gchar *custom_path;
} FileSaveOptions;

// Function prototypes
gboolean file_utils_save_desktop_file(const gchar *content, const gchar *filename, 
                                     FileSaveOptions *options, GtkWidget *parent_window, gchar **error_msg);
gboolean file_utils_set_executable_permissions(const gchar *filepath, gchar **error_msg);
gboolean file_utils_mark_as_trusted(const gchar *filepath, gchar **error_msg);
gchar* file_utils_get_desktop_directory(void);
gchar* file_utils_get_local_applications_directory(void);
gboolean file_utils_ensure_directory_exists(const gchar *dirpath, gchar **error_msg);
gchar* file_utils_sanitize_filename(const gchar *name);
gboolean file_utils_file_exists(const gchar *filepath);
gboolean file_utils_validate_executable(const gchar *filepath, gchar **error_msg);
gboolean file_utils_confirm_overwrite(GList *existing_files, GtkWidget *parent_window);
gboolean file_utils_validate_custom_path(const gchar *path, gchar **error_msg);

// File type detection
typedef enum {
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_ELF,
    FILE_TYPE_PYTHON,
    FILE_TYPE_SHELL,
    FILE_TYPE_OTHER
} FileType;

FileType file_utils_detect_file_type(const gchar *filepath);

// File save options management
FileSaveOptions* file_save_options_new(void);
void file_save_options_free(FileSaveOptions *options);

#endif // FILE_UTILS_H 