#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>  // для функции _mkdir

#include "src\\add_note.h"
#include "src\\open_note.h"
#include "src\\add_folder.h"
#include "src\\open_folder.h"

#define BASE_DIRECTORY "..\\all_notes\\"

//void add_folder() {
//    char folder_name[256], full_path[512];
//
//    printf("Введите имя папки: ");
//    fgets(folder_name, sizeof(folder_name), stdin);
//
//    folder_name[strcspn(folder_name, "\n")] = 0;
//
//    snprintf(full_path, sizeof(full_path), "%s%s", baseFolderPath, folder_name);
//
//    if (_mkdir(full_path) == 0) {
//        printf("Папка \"%s\" успешно создана.\n", full_path);
//    } else {
//        perror("Ошибка при создании папки");
//    }
//}

void show_menu(char *directory) {
    printf("1. Open note\n");
    printf("2. Add note\n");


    printf("3. Open folder\n");
    printf("4. Add folder\n");
    printf("0. Exit\n");
}

int main() {
    int choice;
    char directory[256] = BASE_DIRECTORY;
    while (1) {
        printf("Select an option:");
        show_menu(directory);

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                open_note(directory);
                break;
            case 2:
                add_note(directory);
                break;
            case 3:
                open_folder(directory);
                break;
            case 4:
                add_folder();
                break;
            case 0:
                system("pause");
                return 0;
            default:
                printf("Error\n");
        }
    }
}
