#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libnotify/notify.h>

#define APP_DIR "/.local/share/applications"
#define BUFFER_SIZE 1024

// UNINSTALL MENU SECTION
void send_notification(const char *summary, const char *body, const char *icon) {
    notify_init("AppImage Uninstaller");
    NotifyNotification* n = notify_notification_new(summary, body, icon);
    notify_notification_show(n, NULL);
    g_object_unref(G_OBJECT(n));
    notify_uninit();
}

int delete_file(const char *filepath) {
    if (remove(filepath) == 0) {
        return 0; // Success
    } else {
        return -1; // Failure
    }
}

char *get_exec_path_from_desktop_file(const char *appname) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char desktop_file_path[1024];

    // Construct the path to the desktop file within the user's home directory
    char *home_path = getenv("HOME");
    if (home_path == NULL) {
        printf("Error: HOME environment variable not set.\n");
        return NULL;
    }
    snprintf(desktop_file_path, sizeof(desktop_file_path), "%s/.local/share/applications/%s.desktop", home_path, appname);

    FILE *fp = fopen(desktop_file_path, "r");
    if (fp == NULL) {
        perror("Failed to open desktop file");
        return NULL;
    }

    char *exec_path = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, "Exec=", 5) == 0) {
            char *temp_path = strdup(line + 5); // +5 to skip "Exec="
            char *token = strtok(temp_path, " "); // Split the string on space
            if (token != NULL) {
                exec_path = strdup(token);
                free(temp_path);
                break; // Found the path, exit the loop
            }
            free(temp_path); // Ensure memory is freed if not exiting
        }
    }

    free(line);
    fclose(fp);
    // Handle replacement of $HOME in the path, if present
    if (exec_path && strstr(exec_path, "$HOME") != NULL) {
        char *home_in_path = strstr(exec_path, "$HOME");
        *home_in_path = '\0'; // Split the string at $HOME
        char resolved_path[1024];
        snprintf(resolved_path, sizeof(resolved_path), "%s%s", getenv("HOME"), home_in_path + strlen("$HOME"));
        free(exec_path);
        exec_path = strdup(resolved_path);
    }

    return exec_path;
}
// END UNINSTALL MENU SECTION

// DESKTOP FILE UPDATE SECTION
void update_desktop_file(const char* path, int* updatedCount) {
    FILE* file = fopen(path, "r+");
    if (!file) {
        printf("Could not open file: %s\n", path);
        return;
    }

    char buffer[BUFFER_SIZE];
    int appImageFound = 0;
    int actionsFound = 0;

    // Read file and check conditions
    while (fgets(buffer, BUFFER_SIZE, file)) {
        if (strstr(buffer, "Exec=") && strstr(buffer, ".AppImage")) {
            appImageFound = 1;
        }
        if (strstr(buffer, "Actions=Uninstall;")) {
            actionsFound = 1;
            break;
        }
    }

    // If .AppImage is found and Uninstall action is not present, append it
    if (appImageFound && !actionsFound) {
        (*updatedCount)++;
        fseek(file, 0, SEEK_END);
        // Extract the file name without path and extension
        char* filename = strrchr(path, '/') + 1; // Move past the last '/'
        char* dot = strrchr(filename, '.');
        if (dot != NULL) *dot = '\0'; // Terminate the string at the dot, removing the extension
        fprintf(file, "\nActions=Uninstall;\n\n[Desktop Action Uninstall]\nName=Uninstall Application\nExec=uninstall_appimage %s\n", filename);
    }

    fclose(file);
}

void update_desktop_files() {
    int updatedCount = 0;
    char* home = getenv("HOME");
    if (!home) {
        printf("Home directory not found.\n");
        return;
    }

    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s%s", home, APP_DIR);
    DIR* dir = opendir(path);
    if (!dir) {
        printf("Could not open directory: %s\n", path);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char* dot = strrchr(entry->d_name, '.');
            if (dot && !strcmp(dot, ".desktop")) {
                char filePath[BUFFER_SIZE];
                snprintf(filePath, BUFFER_SIZE, "%s/%s", path, entry->d_name);
                update_desktop_file(filePath, &updatedCount);
            }
        }
    }

    closedir(dir);
    if (updatedCount > 0) {
        char notificationMsg[256];
        snprintf(notificationMsg, sizeof(notificationMsg), "%d desktop files have been updated.", updatedCount);
        send_notification("Update Complete", notificationMsg, "dialog-information");
    } else {
        send_notification("Update Complete", "No desktop files were updated.", "dialog-information");
    }
}
// END DESKTOP FILE UPDATE SECTION

// MAIN CODE SECTION
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s {appname} or %s --update\n", argv[0], argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "--update") == 0) {
        // If the --update argument is passed, call the update_desktop_files function
        update_desktop_files();
    } else {
        // Otherwise, proceed with the existing logic to uninstall the app
        char *appname = argv[1];
        char *exec_path = get_exec_path_from_desktop_file(appname);
        char desktop_file_path[1024];

        if (exec_path == NULL) {
            send_notification("Uninstall Failed", "Failed to read desktop file.", "dialog-error");
            return 1;
        }

        snprintf(desktop_file_path, sizeof(desktop_file_path), "%s/.local/share/applications/%s.desktop", getenv("HOME"), appname);

        printf("AppName file resolved: %s\n", appname); // Debug print
        printf("Desktop file path resolved: %s\n", desktop_file_path); // Debug print
        printf("AppImage file path resolved: %s\n", exec_path); // Debug print

        // Deleting the actual AppImage file
        if (delete_file(exec_path) != 0) {
            free(exec_path);
            send_notification("Uninstall Failed", "Failed to delete AppImage file.", "dialog-error");
            return 1;
        }
        free(exec_path);

        // Deleting the desktop file
        snprintf(desktop_file_path, sizeof(desktop_file_path), "%s/.local/share/applications/%s.desktop", getenv("HOME"), appname);
        if (delete_file(desktop_file_path) != 0) {
            char failedMessage[1024];
            snprintf(failedMessage, sizeof(failedMessage), "Failed to delete desktop file for %s.", appname);
            send_notification("Uninstall Failed", failedMessage, "dialog-error");
            return 1;
        }

        char successMessage[1024];
        snprintf(successMessage, sizeof(successMessage), "%s AppImage have been uninstalled.", appname);
        send_notification("Uninstall Successful", successMessage, "dialog-information");
        return 0;
    }
}
