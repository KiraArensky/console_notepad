#ifndef CONSOLE_NOTEPAD_ADD_NOTE_H
#define CONSOLE_NOTEPAD_ADD_NOTE_H

#include <time.h>
#include "struct.h"

void save_note_to_file(Note note, char *directory) {
    char filename[256];

    snprintf(filename, sizeof(filename), "%s\\note_%d.dat", directory, note.id);
    printf("%s", filename);

    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        printf("Error file open\n");
        return;
    }

    fwrite(&note.id, sizeof(int), 1, file);

    fwrite(note.created_timestamp, sizeof(char), 20, file);
    fwrite(note.edited_timestamp, sizeof(char), 20, file);

    size_t text_length = strlen(note.text) + 1;
    fwrite(&text_length, sizeof(size_t), 1, file);
    fwrite(note.text, sizeof(char), text_length, file);

    fclose(file);
}

void add_note(char *directory) {
    Note note;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    note.id = time(NULL);

    strftime(note.created_timestamp, sizeof(note.created_timestamp), "%Y-%m-%d %H:%M", t);
    strcpy(note.edited_timestamp, note.created_timestamp);

    note.text = (char *) malloc(256 * sizeof(char));
    if (note.text == NULL) {
        printf("Memory allocation error for text \n");
        return;
    }

    printf("Input text: ");
    getchar();  // Очистка буфера
    size_t length = 0;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        note.text[length++] = (char) c;
        if (length >= 256) {
            note.text = (char *) realloc(note.text, length + 1);
            if (note.text == NULL) {
                printf("Memory allocation error for text!\n");
                return;
            }
        }
    }
    note.text[length] = '\0';

    save_note_to_file(note, directory);
    printf("All done\n");

    free(note.text);
}

#endif //CONSOLE_NOTEPAD_ADD_NOTE_H