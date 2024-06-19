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
        printf("����� \"%s\" �������.\n", folder);
    } else {
        printf("����� � ������ \"%s\" ��� ����������.\n", folder);
    }
}


void save_note_to_file(const char *folder, const char *filename, Note note) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s.dat", folder, filename);

    struct stat st = {0};
    if (stat(filepath, &st) != -1) {
        printf("������� � ������ \"%s\" ��� ����������.\n", filename);
        return;
    }

    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        printf("������ �������� ����� ��� ������.\n");
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
        printf("������ �������� ����� ��� ������.\n");
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
        perror("������ �������� ����������");
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
        printf("������ �������� ����������");
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
        printf("������ ��������� ������ ��� ������.\n");
        return;
    }

    printf("������� ����� ������� (��� ���������� ����� ���������� ������ Ctrl + O + Enter, ��� ������ �������� Ctrl + Y + Enter):\n");
    getchar(); // ������� ������
    size_t length = 0;
    int c;
    while ((c = getchar()) != EOF && !(c == 15 && getchar() == 10)) { // Ctrl+O ��� ����������
        note.text[length++] = (char) c;
        if (length >= INITIAL_TEXT_LENGTH) {
            note.text = (char *) realloc(note.text, length + 1);
            if (note.text == NULL) {
                printf("������ ��������� ������ ��� ������.\n");
                return;
            }
        }
    }
    note.text[length] = '\0';

    char filename[256];
    printf("������� �������� �������: ");
    scanf("%s", filename);

    save_note_to_file(folder, filename, note);
    printf("������� ���������.\n");

    free_note_text(&note);
}

void view_note_in_folder(const char *folder, const char *filename) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s" PATH_SEPARATOR "%s", folder, filename);

    Note note = load_note_from_file(filepath);
    printf("���: %s\n", filename);
    printf("���� ��������: %s\n", note.created_timestamp);

    printf("\n%s\n", note.text);

    free_note_text(&note);
}

void navigate_folders(char *current_folder) {
    list_folders(current_folder);
    printf("������� ����� ����� ��� �����������: ");
    int folder_index;
    if (scanf("%d", &folder_index) != 1) {
        printf("������: ������� �� �����.\n");
        while (getchar() != '\n'); // ������� ������ �����
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
                printf("���������� � ����� \"%s\".\n", current_folder);
                free(new_folder);
            } else {
                printf("������: ������������ ������ ��� �������� ������ ����.\n");
            }
        } else {
            printf("������: �������� ������ �����.\n");
        }
    } else {
        perror("������ �������� �����");
    }
}

void create_new_folder(char *current_folder) {
    char new_folder[256];
    printf("������� �������� ����� �����: ");
    scanf("%s", new_folder);

    char temp_folder[512];
    snprintf(temp_folder, sizeof(temp_folder), "%s" PATH_SEPARATOR "%s", current_folder, new_folder);

    create_folder(temp_folder);
}

void show_menu(const char *current_folder) {
    int flag;
    printf("�� ������ � ����� \"%s\"\n", current_folder);
    printf("����:\n");
    printf("1. ������� ������\n");
    printf("2. �������� ������\n");
    flag = strcmp(current_folder, BASE_DIRECTORY);
    if (flag == 0) {
        printf("3. ������������� � �����\n");
        printf("4. �������� �����\n");
        printf("5. �����\n");
    } else {
        printf("3. ������������� �����\n");
        printf("4. �����\n");
    }
}

int main() {
    system("chcp 1251 > nul");  /* ��������� �������� ����� */

    char current_folder[256] = BASE_DIRECTORY;

    create_folder("C:" PATH_SEPARATOR "Users" PATH_SEPARATOR "Public" PATH_SEPARATOR "console_notepad");
    create_folder(current_folder);

    while (1) {
        CLEAR_SCREEN();

        show_menu(current_folder);
        printf("�������� �����: ");

        int choice, flag;
        if (scanf("%d", &choice) != 1) {
            printf("������: ������� �� �����.\n");
            while (getchar() != '\n'); // ������� ������ �����
            continue;
        }

        CLEAR_SCREEN();

        switch (choice) {
            case 1: {
                list_notes(current_folder);
                printf("������� ����� ������ ��� ��������: ");
                int note_index;
                if (scanf("%d", &note_index) != 1) {
                    printf("������: ������� �� �����.\n");
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
                    perror("������ �������� ����������");
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
                    printf("�� ��������� � ������� ����������\n");
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
                    printf("�������� �����. ����������, �������� �����.\n");
                    system("pause");
                }
                break;
            default:
                printf("�������� �����. ����������, �������� �����.\n");
                system("pause");
                break;
        }
    }
}
