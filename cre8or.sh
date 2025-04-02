#!/bin/bash

# Set default values for fields
type_value="Application"
name_value=""
comment_value=""
exec_value=""
icon_value=""
terminal_value="false"
categories_value="Utility;Development"

# Function to generate .desktop content
generate_desktop_content() {
    echo -e "[Desktop Entry]\nType=$type_value\nName=$name_value\nComment=$comment_value\nExec=$exec_value\nIcon=$icon_value\nTerminal=$terminal_value\nCategories=$categories_value"
}

# Function to display Zenity form for inputs
get_user_input() {
    # Type selection
    type_value=$(zenity --list --title="Select Type" --column="Type" "Application" "Link" "Directory")
    if [[ -z "$type_value" ]]; then exit 1; fi

    # Name entry
    name_value=$(zenity --entry --title="Name" --text="Enter application name")
    if [[ -z "$name_value" ]]; then exit 1; fi

    # Comment entry
    comment_value=$(zenity --entry --title="Comment" --text="Enter a short description")
    if [[ -z "$comment_value" ]]; then exit 1; fi

    # Executable file selection
    exec_value=$(zenity --file-selection --title="Select Executable")
    if [[ -z "$exec_value" ]]; then exit 1; fi

    # Icon file selection
    icon_value=$(zenity --file-selection --title="Select Icon")
    if [[ -z "$icon_value" ]]; then exit 1; fi

    # Terminal selection
    terminal_value=$(zenity --list --title="Run in Terminal?" --column="Terminal" "false" "true")
    if [[ -z "$terminal_value" ]]; then exit 1; fi

    # Categories selection
    categories_value=$(zenity --list --title="Select Categories" --text="Select categories for the application" \
        --checklist --column="Select" --column="Category" \
        FALSE "Accessories" \
        FALSE "Graphics" \
        FALSE "Internet" \
        FALSE "Office" \
        FALSE "Other" \
        FALSE "Programming" \
        FALSE "Sound & Video" \
        FALSE "System Tools" \
        FALSE "Utilities" \
        --separator=";")
    
    # If no categories are selected, set default
    if [[ -z "$categories_value" ]]; then
        categories_value="Utility"
    fi
}

# Function to handle the "Generate" button
generate_button_action() {
    desktop_content=$(generate_desktop_content)
    zenity --text-info --title="Desktop File Preview" --width=400 --height=300 --filename=<(echo "$desktop_content")

    zenity --question --text="Do you want to create the .desktop file?" --ok-label="Yes" --cancel-label="No"
    if [ $? -eq 0 ]; then
        download_button_action
    fi
}

# Function to handle the "Download" button
download_button_action() {
    filename=$(zenity --entry --title="Save .desktop File" --text="Enter the filename (e.g., test.desktop)" --entry-text="test.desktop")
    
    user_name=$(whoami)
    desktop_path="/home/$user_name/Desktop/$filename"
    applications_path="/home/$user_name/.local/share/applications/$filename"

    # Save file to desktop
    echo "$(generate_desktop_content)" > "$desktop_path"
    chmod +x "$desktop_path"

    # Function C: Mark as trusted (Using gio for GNOME-based desktops)
    gio set "$desktop_path" metadata::trusted true
    chmod +x "$desktop_path" # Ensure executable permissions

    # Function D: Copy to ~/.local/share/applications/ and mark as trusted
    mkdir -p "/home/$user_name/.local/share/applications/"
    cp "$desktop_path" "$applications_path"
    gio set "$applications_path" metadata::trusted true
    chmod +x "$applications_path"

    zenity --info --title="File Saved" --text="Your .desktop file has been saved and marked as trusted.\n\n- Desktop: $desktop_path\n- Applications Menu: $applications_path"
}

# Function to handle the "Reset" button
reset_button_action() {
    type_value="Application"
    name_value=""
    comment_value=""
    exec_value=""
    icon_value=""
    terminal_value="false"
    categories_value="Utility;Development"
    get_user_input
}

# Function to handle the "Quit" button
quit_button_action() {
    exit 1
}

# Main loop with a Zenity window for button interactions
while true; do
    get_user_input

    action=$(zenity --list --title="Desktop File Creator" --column="Actions" "Generate" "Download" "Reset" "Quit" --height=250 --width=300)

    case "$action" in
        "Generate") generate_button_action ;;
        "Download") download_button_action ;;
        "Reset") reset_button_action ;;
        "Quit") quit_button_action ;;
        *) exit 1 ;;
    esac
done

