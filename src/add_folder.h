#ifndef CONSOLE_NOTEPAD_ADD_FOLDER_H
#define CONSOLE_NOTEPAD_ADD_FOLDER_H

void add_folder() {
    char name_folder[51], command_str[256];

    printf("Input folder name: ");
    fflush(stdin);
    scanf("%[^\n]s", name_folder);
    snprintf(command_str, sizeof(command_str), "mkdir ..\\all_notes\\%s", name_folder);

    system(command_str);

    printf("Folder %s create", name_folder);
}

#endif //CONSOLE_NOTEPAD_ADD_FOLDER_H
