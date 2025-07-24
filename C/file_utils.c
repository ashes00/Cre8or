#include "file_utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <glib/gstdio.h>
#include <gio/gio.h>

FileSaveOptions* file_save_options_new(void) {
    FileSaveOptions *options = g_new0(FileSaveOptions, 1);
    options->save_to_desktop = FALSE;
    options->save_to_local_apps = FALSE;
    options->save_to_custom = FALSE;
    options->custom_path = NULL;
    return options;
}

void file_save_options_free(FileSaveOptions *options) {
    if (options) {
        g_free(options->custom_path);
        g_free(options);
    }
}

gchar* file_utils_get_desktop_directory(void) {
    return g_build_filename(g_get_home_dir(), "Desktop", NULL);
}

gchar* file_utils_get_local_applications_directory(void) {
    return g_build_filename(g_get_home_dir(), ".local", "share", "applications", NULL);
}

gboolean file_utils_ensure_directory_exists(const gchar *dirpath, gchar **error_msg) {
    if (!g_file_test(dirpath, G_FILE_TEST_IS_DIR)) {
        if (g_mkdir_with_parents(dirpath, 0755) != 0) {
            *error_msg = g_strdup_printf("Failed to create directory: %s", dirpath);
            return FALSE;
        }
    }
    return TRUE;
}

gchar* file_utils_sanitize_filename(const gchar *name) {
    if (!name) return g_strdup("my_application");
    
    gchar *sanitized = g_strdup(name);
    gchar *p = sanitized;
    
    // Replace spaces with underscores and remove invalid characters
    while (*p) {
        if (*p == ' ') {
            *p = '_';
        } else if (*p == '/' || *p == '\\' || *p == ':' || *p == '*' || 
                   *p == '?' || *p == '"' || *p == '<' || *p == '>' || *p == '|') {
            *p = '_';
        }
        p++;
    }
    
    // Ensure it's not empty
    if (strlen(sanitized) == 0) {
        g_free(sanitized);
        return g_strdup("my_application");
    }
    
    return sanitized;
}

gboolean file_utils_set_executable_permissions(const gchar *filepath, gchar **error_msg) {
    struct stat st;
    if (stat(filepath, &st) != 0) {
        *error_msg = g_strdup_printf("Failed to get file stats for: %s", filepath);
        return FALSE;
    }
    
    mode_t new_mode = st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH;
    if (chmod(filepath, new_mode) != 0) {
        *error_msg = g_strdup_printf("Failed to set executable permissions for: %s", filepath);
        return FALSE;
    }
    
    return TRUE;
}

gboolean file_utils_mark_as_trusted(const gchar *filepath, gchar **error_msg) {
    gchar *abs_path = g_canonicalize_filename(filepath, NULL);
    if (!abs_path) {
        *error_msg = g_strdup_printf("Failed to get absolute path for: %s", filepath);
        return FALSE;
    }
    
    // First, ensure the file is executable (this can help with trust)
    if (chmod(abs_path, S_IRWXU | S_IRGRP | S_IROTH) != 0) {
        // Warning - could not set executable permissions
    }
    
    // Use g_spawn_sync instead of system() to avoid command injection
    gchar *argv[] = {"/usr/bin/gio", "set", abs_path, "metadata::trusted", "true", NULL};
    gchar *stdout_buf = NULL;
    gchar *stderr_buf = NULL;
    gint exit_status = 0;
    GError *spawn_error = NULL;
    
    gboolean success = g_spawn_sync(NULL, argv, NULL, G_SPAWN_DEFAULT, 
                                   NULL, NULL, &stdout_buf, &stderr_buf, 
                                   &exit_status, &spawn_error);
    
    g_free(abs_path);
    g_free(stdout_buf);
    g_free(stderr_buf);
    
    if (!success) {
        *error_msg = g_strdup_printf("Failed to execute gio: %s", 
                                    spawn_error ? spawn_error->message : "Unknown error");
        if (spawn_error) g_error_free(spawn_error);
        return FALSE;
    }
    
    if (exit_status != 0) {
        *error_msg = g_strdup("Failed to mark file as trusted using gio");
        return FALSE;
    }
    
    // Also try using GIO's file attribute method as a backup
    GFile *file = g_file_new_for_path(abs_path);
    GError *attr_error = NULL;
    gboolean attr_success = g_file_set_attribute_string(file, "metadata::trusted", "true", 
                                                      G_FILE_QUERY_INFO_NONE, NULL, &attr_error);
    if (!attr_success) {
        if (attr_error) g_error_free(attr_error);
    }
    g_object_unref(file);
    
    return TRUE;
}

gboolean file_utils_save_desktop_file(const gchar *content, const gchar *filename, 
                                     FileSaveOptions *options, GtkWidget *parent_window, gchar **error_msg) {
    if (!content || !filename || !options) {
        *error_msg = g_strdup("Invalid parameters");
        return FALSE;
    }
    

    
    gchar *sanitized_filename = file_utils_sanitize_filename(filename);
    gchar *actual_filename = g_strdup_printf("%s.desktop", sanitized_filename);
    g_free(sanitized_filename);
    
    GList *target_paths = NULL;
    GList *existing_files = NULL;
    gint saved_count = 0;
    gboolean success = TRUE;
    GString *error_messages = g_string_new(NULL);
    
    // Add desktop path if requested
    if (options->save_to_desktop) {
        gchar *desktop_dir = file_utils_get_desktop_directory();
        gchar *desktop_path = g_build_filename(desktop_dir, actual_filename, NULL);
        target_paths = g_list_append(target_paths, desktop_path);
        g_free(desktop_dir);
    }
    
    // Add local applications path if requested
    if (options->save_to_local_apps) {
        gchar *local_apps_dir = file_utils_get_local_applications_directory();
        gchar *local_apps_path = g_build_filename(local_apps_dir, actual_filename, NULL);
        target_paths = g_list_append(target_paths, local_apps_path);
        g_free(local_apps_dir);
    }
    
    // Add custom path if requested
    if (options->save_to_custom && options->custom_path) {
        gchar *path_error = NULL;
        if (!file_utils_validate_custom_path(options->custom_path, &path_error)) {
            g_string_append_printf(error_messages, "Invalid custom path: %s\n", path_error ? path_error : "Unknown error");
            if (path_error) g_free(path_error);
            success = FALSE;
        } else {
            gchar *custom_path = g_build_filename(options->custom_path, actual_filename, NULL);
            target_paths = g_list_append(target_paths, custom_path);
        }
    }
    
    // Check for existing files
    for (GList *iter = target_paths; iter != NULL; iter = iter->next) {
        gchar *target_path = (gchar*)iter->data;
        if (file_utils_file_exists(target_path)) {
            existing_files = g_list_append(existing_files, g_strdup(target_path));
        }
    }
    
    // If there are existing files, show confirmation dialog
    if (existing_files) {
        if (!file_utils_confirm_overwrite(existing_files, parent_window)) {
            g_list_free_full(existing_files, g_free);
            g_list_free_full(target_paths, g_free);
            g_free(actual_filename);
            g_string_free(error_messages, TRUE);
            return FALSE;
        }
        g_list_free_full(existing_files, g_free);
    }
    
    // Save to all target paths
    for (GList *iter = target_paths; iter != NULL; iter = iter->next) {
        gchar *target_path = (gchar*)iter->data;
        
        // Ensure directory exists
        gchar *dir_path = g_path_get_dirname(target_path);
        if (!file_utils_ensure_directory_exists(dir_path, error_msg)) {
            g_string_append_printf(error_messages, "Failed to create directory: %s\n", dir_path);
            g_free(dir_path);
            success = FALSE;
            continue;
        }
        g_free(dir_path);
        
        // If file exists and user confirmed overwrite, just delete it
        // (no backup since user explicitly chose to overwrite)
        if (file_utils_file_exists(target_path)) {
            g_unlink(target_path);
        }
        
        // Write file content
        GError *write_error = NULL;
        if (!g_file_set_contents(target_path, content, -1, &write_error)) {
            g_string_append_printf(error_messages, "Failed to write file %s: %s\n", 
                                 target_path, write_error ? write_error->message : "Unknown error");
            if (write_error) g_error_free(write_error);
            success = FALSE;
            continue;
        }
        
        // Set executable permissions
        gchar *perm_error = NULL;
        if (!file_utils_set_executable_permissions(target_path, &perm_error)) {
            g_string_append_printf(error_messages, "Failed to set permissions for %s: %s\n", 
                                 target_path, perm_error ? perm_error : "Unknown error");
            if (perm_error) g_free(perm_error);
            success = FALSE;
            continue;
        }
        
        // Mark as trusted
        gchar *trust_error = NULL;
        if (!file_utils_mark_as_trusted(target_path, &trust_error)) {
            g_string_append_printf(error_messages, "Warning: Could not mark %s as trusted: %s\n", 
                                 target_path, trust_error ? trust_error : "Unknown error");
            if (trust_error) g_free(trust_error);
            // This is a warning, not a fatal error
        }
        
        saved_count++;
    }
    
    // Clean up
    g_list_free_full(target_paths, g_free);
    g_free(actual_filename);
    
    // Set error message if any errors occurred
    if (!success && error_messages->len > 0) {
        *error_msg = g_string_free(error_messages, FALSE);
    } else {
        g_string_free(error_messages, TRUE);
    }
    
    return success && saved_count > 0;
}

gboolean file_utils_file_exists(const gchar *filepath) {
    return g_file_test(filepath, G_FILE_TEST_EXISTS);
}

gboolean file_utils_validate_custom_path(const gchar *path, gchar **error_msg) {
    if (!path) {
        *error_msg = g_strdup("Path is NULL");
        return FALSE;
    }
    
    // Check for path traversal attempts
    if (g_strstr_len(path, -1, "..") != NULL) {
        *error_msg = g_strdup("Path traversal not allowed");
        return FALSE;
    }
    
    // Check for absolute paths (optional security measure)
    if (g_path_is_absolute(path)) {
        *error_msg = g_strdup("Absolute paths not allowed");
        return FALSE;
    }
    
    return TRUE;
}

gboolean file_utils_validate_executable(const gchar *filepath, gchar **error_msg) {
    if (!file_utils_file_exists(filepath)) {
        *error_msg = g_strdup_printf("Executable file does not exist: %s", filepath);
        return FALSE;
    }
    
    if (!g_file_test(filepath, G_FILE_TEST_IS_REGULAR)) {
        *error_msg = g_strdup_printf("Path is not a regular file: %s", filepath);
        return FALSE;
    }
    
    if (!g_file_test(filepath, G_FILE_TEST_IS_EXECUTABLE)) {
        *error_msg = g_strdup_printf("File is not executable: %s", filepath);
        return FALSE;
    }
    
    return TRUE;
}

gboolean file_utils_confirm_overwrite(GList *existing_files, GtkWidget *parent_window) {
    if (!existing_files) {
        return TRUE; // No existing files to worry about
    }
    
    // Count existing files
    gint count = g_list_length(existing_files);
    if (count == 0) {
        return TRUE;
    }
    
    // Create confirmation dialog
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                              GTK_DIALOG_MODAL,
                                              GTK_MESSAGE_QUESTION,
                                              GTK_BUTTONS_YES_NO,
                                              "Overwrite Existing Files?");
    
    // Build message text
    GString *message = g_string_new("The following files already exist:\n\n");
    
    for (GList *iter = existing_files; iter != NULL; iter = iter->next) {
        gchar *filepath = (gchar*)iter->data;
        g_string_append_printf(message, "• %s\n", filepath);
    }
    
    g_string_append(message, "\nDo you want to overwrite these files?");
    
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", message->str);
    g_string_free(message, TRUE);
    
    // Show dialog and get response
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    return (response == GTK_RESPONSE_YES);
}

FileType file_utils_detect_file_type(const gchar *filepath) {
    if (!filepath || !file_utils_file_exists(filepath)) {
        return FILE_TYPE_UNKNOWN;
    }
    
    // First check file extension for quick identification
    gchar *lower_path = g_ascii_strdown(filepath, -1);
    gboolean is_python = g_str_has_suffix(lower_path, ".py");
    gboolean is_shell = g_str_has_suffix(lower_path, ".sh");
    g_free(lower_path);
    
    if (is_python) {
        return FILE_TYPE_PYTHON;
    }
    if (is_shell) {
        return FILE_TYPE_SHELL;
    }
    
    // For non-extension files, check the file header (magic bytes)
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return FILE_TYPE_UNKNOWN;
    }
    
    unsigned char header[8];
    size_t bytes_read = fread(header, 1, 8, file);
    fclose(file);
    
    if (bytes_read < 4) {
        return FILE_TYPE_UNKNOWN;
    }
    
    // Check for ELF magic number (0x7f 0x45 0x4c 0x46)
    if (header[0] == 0x7f && header[1] == 0x45 && header[2] == 0x4c && header[3] == 0x46) {
        return FILE_TYPE_ELF;
    }
    
    // Check for Python shebang (#!/usr/bin/python or #!/usr/bin/env python)
    if (header[0] == '#' && header[1] == '!') {
        // Read more to check for python in shebang
        file = fopen(filepath, "r");
        if (file) {
            char line[256];
            if (fgets(line, sizeof(line), file)) {
                gchar *lower_line = g_ascii_strdown(line, -1);
                gboolean has_python = g_strstr_len(lower_line, -1, "python") != NULL;
                g_free(lower_line);
                fclose(file);
                if (has_python) {
                    return FILE_TYPE_PYTHON;
                }
            }
            fclose(file);
        }
    }
    
    // Check for shell shebang (#!/bin/bash, #!/bin/sh, etc.)
    if (header[0] == '#' && header[1] == '!') {
        file = fopen(filepath, "r");
        if (file) {
            char line[256];
            if (fgets(line, sizeof(line), file)) {
                gchar *lower_line = g_ascii_strdown(line, -1);
                gboolean has_shell = (g_strstr_len(lower_line, -1, "/bin/bash") != NULL ||
                                    g_strstr_len(lower_line, -1, "/bin/sh") != NULL ||
                                    g_strstr_len(lower_line, -1, "/bin/zsh") != NULL ||
                                    g_strstr_len(lower_line, -1, "/bin/dash") != NULL);
                g_free(lower_line);
                fclose(file);
                if (has_shell) {
                    return FILE_TYPE_SHELL;
                }
            }
            fclose(file);
        }
    }
    
    // Check if file is executable (might be a script without extension)
    if (g_file_test(filepath, G_FILE_TEST_IS_EXECUTABLE)) {
        // Check first line for shebang
        file = fopen(filepath, "r");
        if (file) {
            char line[256];
            if (fgets(line, sizeof(line), file)) {
                gchar *lower_line = g_ascii_strdown(line, -1);
                if (g_strstr_len(lower_line, -1, "python") != NULL) {
                    g_free(lower_line);
                    fclose(file);
                    return FILE_TYPE_PYTHON;
                } else if (g_strstr_len(lower_line, -1, "/bin/") != NULL) {
                    g_free(lower_line);
                    fclose(file);
                    return FILE_TYPE_SHELL;
                }
                g_free(lower_line);
            }
            fclose(file);
        }
    }
    
    return FILE_TYPE_OTHER;
} 