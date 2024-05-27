#ifndef CONSOLE_NOTEPAD_OPEN_FOLDER_H
#define CONSOLE_NOTEPAD_OPEN_FOLDER_H

void open_folder(char *directory) {
    char name_folder[51];

    printf("Input folder name: ");
    fflush(stdin);
    scanf("%[^\n]s", name_folder);

    snprintf(directory, sizeof(directory), "%s%s", directory, name_folder);

    printf("Folder %s is open", directory);
}

#endif //CONSOLE_NOTEPAD_OPEN_FOLDER_H
