#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#define CLEAR_SCREEN() system("cls")
#define PATH_SEPARATOR "\\"
#define BASE_DIRECTORY "C:" PATH_SEPARATOR "Users" PATH_SEPARATOR "Public" PATH_SEPARATOR "console_notepad" PATH_SEPARATOR "all_notes"
#else
#include <sys/types.h>
#include <unistd.h>
#define CLEAR_SCREEN() system("clear")
#define PATH_SEPARATOR "/"
#define BASE_DIRECTORY "console_notepad" PATH_SEPARATOR "all_notes"
#endif

#define INITIAL_TEXT_LENGTH 256

typedef struct {
    int id;
    char created_timestamp[20];
    char *text;
} Note;

void create_folder(const char *folder) {
    struct stat st = {0};
    if (stat(folder, &st) == -1) {
        mkdir(folder, 0777);
    } else {
        printf("Папка с именем \"%s\" уже существует.\n", folder);
    }
}

void save_note_to_file(const char *folder, const char *filename, Note note) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s.dat", folder, filename);

    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи.\n");
        return;
    }

    fwrite(&note.id, sizeof(int), 1, file);
    fwrite(note.created_timestamp, sizeof(char), 20, file);

    size_t text_length = strlen(note.text) + 1;
    fwrite(&text_length, sizeof(size_t), 1, file);
    fwrite(note.text, sizeof(char), text_length, file);

    fclose(file);
}

Note create_note(const char *text) {
    Note note;

    note.id = time(NULL);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(note.created_timestamp, sizeof(note.created_timestamp), "%Y-%m-%d %H:%M", t);

    note.text = (char *) malloc((strlen(text) + 1) * sizeof(char));
    if (note.text != NULL) {
        strcpy(note.text, text);
    }

    return note;
}

void create_test_notes(const char *folder) {
    char note_text[3][50] = {
            "Это тестовая заметка 1",
            "Это тестовая заметка 2",
            "Это тестовая заметка 3"
    };

    for (int i = 0; i < 3; i++) {
        char filename[20];
        snprintf(filename, sizeof(filename), "test_note_%d", i + 1);
        Note note = create_note(note_text[i]);
        save_note_to_file(folder, filename, note);
        free(note.text);
    }
}

void create_test_folders_with_notes(const char *base_folder) {
    char folder_names[3][20] = {
            "folder1",
            "folder2",
            "folder3"
    };

    for (int i = 0; i < 3; i++) {
        char folder_path[512];
        snprintf(folder_path, sizeof(folder_path), "%s" PATH_SEPARATOR "%s", base_folder, folder_names[i]);
        create_folder(folder_path);
        create_test_notes(folder_path);
    }
}

int main() {
    system("chcp 1251 > nul");  /* поддержка русского языка */

    create_folder("C:" PATH_SEPARATOR "Users" PATH_SEPARATOR "Public" PATH_SEPARATOR "console_notepad");
    create_folder(BASE_DIRECTORY);

    create_test_notes(BASE_DIRECTORY);

    create_test_folders_with_notes(BASE_DIRECTORY);

    printf("Тестовые заметки и папки успешно созданы.\n");

    system("pause");
    return 0;
}
