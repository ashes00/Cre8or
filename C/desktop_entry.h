#ifndef DESKTOP_ENTRY_H
#define DESKTOP_ENTRY_H

#include <glib.h>

// Desktop entry types as per freedesktop.org specification
typedef enum {
    DESKTOP_TYPE_APPLICATION
} DesktopEntryType;

// Categories as per freedesktop.org specification
typedef struct {
    gboolean accessories;
    gboolean graphics;
    gboolean internet;
    gboolean office;
    gboolean other;
    gboolean programming;
    gboolean sound_video;
    gboolean system_tools;
    gboolean utilities;
} DesktopCategories;

// Main desktop entry structure
typedef struct {
    DesktopEntryType type;
    gchar *name;
    gchar *comment;
    gchar *exec_path;
    gchar *icon_path;
    gboolean terminal;
    DesktopCategories categories;
} DesktopEntry;

// Function prototypes
DesktopEntry* desktop_entry_new(void);
void desktop_entry_free(DesktopEntry *entry);
gchar* desktop_entry_generate_content(DesktopEntry *entry);
gboolean desktop_entry_validate(DesktopEntry *entry, gchar **error_msg);
gchar* desktop_entry_get_type_string(DesktopEntryType type);
gchar* desktop_entry_get_categories_string(DesktopCategories *categories);

// Category management
void desktop_entry_clear_categories(DesktopCategories *categories);
void desktop_entry_set_category(DesktopCategories *categories, const gchar *category, gboolean value);
gboolean desktop_entry_has_category(DesktopCategories *categories, const gchar *category);

#endif // DESKTOP_ENTRY_H 