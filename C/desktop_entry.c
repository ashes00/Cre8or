#include "desktop_entry.h"
#include "file_utils.h"
#include <string.h>
#include <stdio.h>

DesktopEntry* desktop_entry_new(void) {
    DesktopEntry *entry = g_new0(DesktopEntry, 1);
    entry->type = DESKTOP_TYPE_APPLICATION;
    entry->terminal = FALSE;
    desktop_entry_clear_categories(&entry->categories);
    return entry;
}

void desktop_entry_free(DesktopEntry *entry) {
    if (entry) {
        g_free(entry->name);
        g_free(entry->comment);
        g_free(entry->exec_path);
        g_free(entry->icon_path);
        g_free(entry);
    }
}

gchar* desktop_entry_get_type_string(DesktopEntryType type) {
    switch (type) {
        case DESKTOP_TYPE_APPLICATION:
            return "Application";
        default:
            return "Application";
    }
}

void desktop_entry_clear_categories(DesktopCategories *categories) {
    categories->accessories = FALSE;
    categories->graphics = FALSE;
    categories->internet = FALSE;
    categories->office = FALSE;
    categories->other = FALSE;
    categories->programming = FALSE;
    categories->sound_video = FALSE;
    categories->system_tools = FALSE;
    categories->utilities = FALSE;
}

void desktop_entry_set_category(DesktopCategories *categories, const gchar *category, gboolean value) {
    if (g_strcmp0(category, "Utility") == 0) {
        categories->accessories = value;
    } else if (g_strcmp0(category, "Graphics") == 0) {
        categories->graphics = value;
    } else if (g_strcmp0(category, "Network") == 0) {
        categories->internet = value;
    } else if (g_strcmp0(category, "Office") == 0) {
        categories->office = value;
    } else if (g_strcmp0(category, "Development") == 0) {
        categories->programming = value;
    } else if (g_strcmp0(category, "AudioVideo") == 0) {
        categories->sound_video = value;
    } else if (g_strcmp0(category, "System") == 0) {
        categories->system_tools = value;
    } else if (g_strcmp0(category, "Settings") == 0) {
        categories->utilities = value;
    } else if (g_strcmp0(category, "Games") == 0) {
        categories->other = value;
    }
}

gboolean desktop_entry_has_category(DesktopCategories *categories, const gchar *category) {
    if (g_strcmp0(category, "Accessories") == 0) {
        return categories->accessories;
    } else if (g_strcmp0(category, "Graphics") == 0) {
        return categories->graphics;
    } else if (g_strcmp0(category, "Internet") == 0) {
        return categories->internet;
    } else if (g_strcmp0(category, "Office") == 0) {
        return categories->office;
    } else if (g_strcmp0(category, "Other") == 0) {
        return categories->other;
    } else if (g_strcmp0(category, "Programming") == 0) {
        return categories->programming;
    } else if (g_strcmp0(category, "Sound & Video") == 0) {
        return categories->sound_video;
    } else if (g_strcmp0(category, "System Tools") == 0) {
        return categories->system_tools;
    } else if (g_strcmp0(category, "Utilities") == 0) {
        return categories->utilities;
    }
    return FALSE;
}

gchar* desktop_entry_get_categories_string(DesktopCategories *categories) {
    GString *cat_string = g_string_new(NULL);
    gboolean first = TRUE;
    
    if (categories->accessories) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Utility");
        first = FALSE;
    }
    if (categories->graphics) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Graphics");
        first = FALSE;
    }
    if (categories->internet) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Network");
        first = FALSE;
    }
    if (categories->office) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Office");
        first = FALSE;
    }
    if (categories->other) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Utility");
        first = FALSE;
    }
    if (categories->programming) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Development");
        first = FALSE;
    }
    if (categories->sound_video) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "AudioVideo");
        first = FALSE;
    }
    if (categories->system_tools) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "System");
        first = FALSE;
    }
    if (categories->utilities) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Settings");
        first = FALSE;
    }
    if (categories->other) {
        if (!first) g_string_append(cat_string, ";");
        g_string_append(cat_string, "Games");
        first = FALSE;
    }
    
    if (cat_string->len > 0) {
        g_string_append(cat_string, ";");
    }
    
    return g_string_free(cat_string, FALSE);
}

gboolean desktop_entry_validate(DesktopEntry *entry, gchar **error_msg) {
    if (!entry->name || strlen(entry->name) == 0) {
        *error_msg = g_strdup("Application Name is required.");
        return FALSE;
    }
    
    switch (entry->type) {
        case DESKTOP_TYPE_APPLICATION:
            if (!entry->exec_path || strlen(entry->exec_path) == 0) {
                *error_msg = g_strdup("Executable Location is required for Type 'Application'.");
                return FALSE;
            }
            break;
    }
    
    return TRUE;
}

gchar* desktop_entry_generate_content(DesktopEntry *entry) {
    GString *content = g_string_new(NULL);
    
    // Header
    g_string_append(content, "[Desktop Entry]\n");
    g_string_append(content, "Version=1.0\n");
    
    // Type
    g_string_append_printf(content, "Type=%s\n", desktop_entry_get_type_string(entry->type));
    
    // Name
    g_string_append_printf(content, "Name=%s\n", entry->name);
    
    // Comment
    if (entry->comment && strlen(entry->comment) > 0) {
        g_string_append_printf(content, "Comment=%s\n", entry->comment);
    }
    
    // Type-specific fields
    switch (entry->type) {
        case DESKTOP_TYPE_APPLICATION:
            if (entry->exec_path) {
                // Validate executable path
                gchar *exec_error = NULL;
                if (!file_utils_validate_executable(entry->exec_path, &exec_error)) {
                    g_free(exec_error);
                }
                
                // Detect file type and generate appropriate Exec line
                FileType file_type = file_utils_detect_file_type(entry->exec_path);
                
                switch (file_type) {
                    case FILE_TYPE_PYTHON:
                        if (entry->terminal) {
                            g_string_append_printf(content, "Exec=gnome-terminal -- python3 \"%s\"\n", entry->exec_path);
                        } else {
                            g_string_append_printf(content, "Exec=python3 \"%s\"\n", entry->exec_path);
                        }
                        break;
                    case FILE_TYPE_SHELL:
                        if (entry->terminal) {
                            // For shell scripts in terminal, use gnome-terminal to keep it open
                            g_string_append_printf(content, "Exec=gnome-terminal -- bash -c \"%s; exec bash\"\n", entry->exec_path);
                        } else {
                            // For shell scripts without terminal, run directly
                            g_string_append_printf(content, "Exec=bash \"%s\"\n", entry->exec_path);
                        }
                        break;
                    case FILE_TYPE_ELF:
                        g_string_append_printf(content, "Exec=\"%s\"\n", entry->exec_path);
                        break;
                    case FILE_TYPE_OTHER:
                        // For other types, try to use the file directly
                        g_string_append_printf(content, "Exec=\"%s\"\n", entry->exec_path);
                        break;
                    case FILE_TYPE_UNKNOWN:
                    default:
                        // Fallback to direct execution
                        g_string_append_printf(content, "Exec=\"%s\"\n", entry->exec_path);
                        break;
                }
            }
            g_string_append_printf(content, "Terminal=%s\n", entry->terminal ? "true" : "false");
            break;
    }
    
    // Icon
    if (entry->icon_path && strlen(entry->icon_path) > 0) {
        g_string_append_printf(content, "Icon=%s\n", entry->icon_path);
    }
    
    // Categories
    gchar *categories = desktop_entry_get_categories_string(&entry->categories);
    if (strlen(categories) > 0) {
        g_string_append_printf(content, "Categories=%s\n", categories);
    }
    g_free(categories);
    
    return g_string_free(content, FALSE);
} 