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
        printf("Папка \"%s\" создана.\n", folder);
    } else {
        printf("Папка с именем \"%s\" уже существует.\n", folder);
    }
}


void save_note_to_file(const char *folder, const char *filename, Note note) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s.dat", folder, filename);

    struct stat st = {0};
    if (stat(filepath, &st) != -1) {
        printf("Заметка с именем \"%s\" уже существует.\n", filename);
        return;
    }

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

Note load_note_from_file(const char *filepath) {
    Note note;
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("Ошибка открытия файла для чтения.\n");
        exit(-1);
    }

    fread(&note.id, sizeof(int), 1, file);
    fread(note.created_timestamp, sizeof(char), 20, file);

    size_t text_length;
    fread(&text_length, sizeof(size_t), 1, file);

    note.text = (char *) malloc(text_length * sizeof(char));
    fread(note.text, sizeof(char), text_length, file);

    fclose(file);
    return note;
}

void free_note_text(Note *note) {
    free(note->text);
}

void list_notes(const char *folder) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder)) != NULL) {
        int i = 1;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s%s%s", folder, PATH_SEPARATOR, ent->d_name);

                struct stat st;
                char *dot = strrchr(ent->d_name, '.');

                if (dot && strcmp(dot + 1, "dat") == 0) {
                    if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode)) {
                        printf("%d. %s\n", i++, ent->d_name);
                    }
                }
            }
        }
        closedir(dir);
    } else {
        perror("Ошибка открытия директории");
    }
}

void list_folders(const char *folder) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder)) != NULL) {
        int i = 1;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s", folder, ent->d_name);

                struct stat st;
                if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
                    printf("%d. %s\n", i++, ent->d_name);
                }
            }
        }
        closedir(dir);
    } else {
        printf("Ошибка открытия директории");
    }
}

void add_note_to_folder(const char *folder) {
    Note note;

    note.id = time(NULL);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(note.created_timestamp, sizeof(note.created_timestamp), "%Y-%m-%d %H:%M", t);

    note.text = (char *) malloc(INITIAL_TEXT_LENGTH * sizeof(char));
    if (note.text == NULL) {
        printf("Ошибка выделения памяти для текста.\n");
        return;
    }

    printf("Введите текст заметки (Для завершения ввода комбинация клавиш Ctrl + O + Enter, для отмены действия Ctrl + Y + Enter):\n");
    getchar(); // Очистка буфера
    size_t length = 0;
    int c;
    while ((c = getchar()) != EOF && !(c == 15 && getchar() == 10)) { // Ctrl+O для завершения
        note.text[length++] = (char) c;
        if (length >= INITIAL_TEXT_LENGTH) {
            note.text = (char *) realloc(note.text, length + 1);
            if (note.text == NULL) {
                printf("Ошибка выделения памяти для текста.\n");
                return;
            }
        }
    }
    note.text[length] = '\0';

    char filename[256];
    printf("Введите название заметки: ");
    scanf("%s", filename);

    save_note_to_file(folder, filename, note);
    printf("Заметка добавлена.\n");

    free_note_text(&note);
}

void view_note_in_folder(const char *folder, const char *filename) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s", folder, filename);

    Note note = load_note_from_file(filepath);
    printf("Имя: %s\n", filename);
    printf("Дата создания: %s\n", note.created_timestamp);

    printf("\n%s\n", note.text);

    free_note_text(&note);
}

void navigate_folders(char *current_folder) {
    list_folders(current_folder);
    printf("Введите номер папки для перемещения: ");
    int folder_index;
    if (scanf("%d", &folder_index) != 1) {
        printf("Ошибка: введено не число.\n");
        while (getchar() != '\n'); // Очистка буфера ввода
        return;
    }

    DIR *dir = opendir(current_folder);
    if (dir) {
        struct dirent *ent;
        int i = 1;
        char selected_folder[256] = {0};
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                char potential_folder[512];
                snprintf(potential_folder, sizeof(potential_folder), "%s" PATH_SEPARATOR "%s", current_folder,
                         ent->d_name);

                struct stat st;
                if (stat(potential_folder, &st) == 0 && S_ISDIR(st.st_mode)) {
                    if (i == folder_index) {
                        strncpy(selected_folder, ent->d_name, sizeof(selected_folder) - 1);
                        selected_folder[sizeof(selected_folder) - 1] = '\0';
                        break;
                    }
                    i++;
                }
            }
        }

        closedir(dir);

        if (strlen(selected_folder) > 0) {
            size_t new_folder_length = strlen(current_folder) + strlen(PATH_SEPARATOR) + strlen(selected_folder) + 1;
            char *new_folder = malloc(new_folder_length);
            if (new_folder) {
                snprintf(new_folder, new_folder_length, "%s" PATH_SEPARATOR "%s", current_folder, selected_folder);
                strncpy(current_folder, new_folder, 256);
                current_folder[255] = '\0';
                printf("Перемещено в папку \"%s\".\n", current_folder);
                free(new_folder);
            } else {
                printf("Ошибка: недостаточно памяти для создания нового пути.\n");
            }
        } else {
            printf("Ошибка: неверный индекс папки.\n");
        }
    } else {
        perror("Ошибка открытия папки");
    }
}

void create_new_folder(char *current_folder) {
    char new_folder[256];
    printf("Введите название новой папки: ");
    scanf("%s", new_folder);

    char temp_folder[512];
    snprintf(temp_folder, sizeof(temp_folder), "%s" PATH_SEPARATOR "%s", current_folder, new_folder);

    create_folder(temp_folder);
}

void show_menu(const char *current_folder) {
    int flag;
    printf("Вы сейчас в папке \"%s\"\n", current_folder);
    printf("Меню:\n");
    printf("1. Открыть запись\n");
    printf("2. Добавить запись\n");
    flag = strcmp(current_folder, BASE_DIRECTORY);
    if (flag == 0) {
        printf("3. Переместиться в папку\n");
        printf("4. Добавить папку\n");
        printf("5. Выход\n");
    } else {
        printf("3. Переместиться назад\n");
        printf("4. Выход\n");
    }
}

int main() {
    system("chcp 1251 > nul");  /* поддержка русского языка */

    char current_folder[256] = BASE_DIRECTORY;

    create_folder("C:" PATH_SEPARATOR "Users" PATH_SEPARATOR "Public" PATH_SEPARATOR "console_notepad");
    create_folder(current_folder);

    while (1) {
        CLEAR_SCREEN();

        show_menu(current_folder);
        printf("Выберите опцию: ");

        int choice, flag;
        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: введено не число.\n");
            while (getchar() != '\n'); // Очистка буфера ввода
            continue;
        }

        CLEAR_SCREEN();

        switch (choice) {
            case 1: {
                list_notes(current_folder);
                printf("Введите номер записи для открытия: ");
                int note_index;
                if (scanf("%d", &note_index) != 1) {
                    printf("Ошибка: введено не число.\n");
                    while (getchar() != '\n');
                }

                DIR *dir;
                struct dirent *ent;
                if ((dir = opendir(current_folder)) != NULL) {
                    int i = 1;
                    while ((ent = readdir(dir)) != NULL) {
                        if (ent->d_name[0] != '.') {
                            char filepath[512];
                            snprintf(filepath, sizeof(filepath), "%s%s%s", current_folder, PATH_SEPARATOR, ent->d_name);

                            struct stat st;
                            char *dot = strrchr(ent->d_name, '.');

                            if (dot && strcmp(dot + 1, "dat") == 0) {
                                if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode)) {
                                    if (i == note_index) {
                                        view_note_in_folder(current_folder, ent->d_name);
                                        system("pause");
                                        break;
                                    }
                                    i++;
                                }
                            }
                        }
                    }
                    closedir(dir);
                } else {
                    perror("Ошибка открытия директории");
                }
                break;
            }
            case 2:
                add_note_to_folder(current_folder);
                system("pause");
                break;
            case 3:
                flag = strcmp(current_folder, BASE_DIRECTORY);
                if (flag == 0) {
                    navigate_folders(current_folder);
                    system("pause");
                } else {
                    strcpy(current_folder, BASE_DIRECTORY);
                    printf("Вы вернулись в базовую директорию\n");
                    system("pause");
                }
                break;
            case 4:
                flag = strcmp(current_folder, BASE_DIRECTORY);
                if (flag == 0) {
                    create_new_folder(current_folder);
                    system("pause");
                } else {
                    system("pause");
                    return 0;
                }
                break;

            case 5:
                flag = strcmp(current_folder, BASE_DIRECTORY);
                if (flag == 0) {
                    system("pause");
                    return 0;
                } else {
                    printf("Неверный выбор. Пожалуйста, выберите снова.\n");
                    system("pause");
                }
                break;
            default:
                printf("Неверный выбор. Пожалуйста, выберите снова.\n");
                system("pause");
                break;
        }
    }
}
