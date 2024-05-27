#include <stdlib.h>
#include <stdio.h>
#include "struct.h"


int load_note_from_file(const char *filename, Note *note) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error file open.\n");
        return 0;
    }

    fread(&note->id, sizeof(int), 1, file);
    fread(note->created_timestamp, sizeof(char), 20, file);
    fread(note->edited_timestamp, sizeof(char), 20, file);

    size_t text_length;
    fread(&text_length, sizeof(size_t), 1, file);

    note->text = (char *)malloc(text_length * sizeof(char));
    fread(note->text, sizeof(char), text_length, file);

    fclose(file);
    return 1;
}

void view_note_by_id() {
    int id, check_flag;
    Note note[1];

    printf("Input ID: ");
    scanf("%d", &id);

    char filename[100];
    snprintf(filename, sizeof(filename), "..\\all_notes\\note_%d", id);

    check_flag = load_note_from_file(filename, note);

    if (!check_flag) {
        return;
    }

    printf("ID: %d\n", note->id);
    printf("Create: %s\n", note->created_timestamp);
    printf("Change: %s\n", note->edited_timestamp);
    printf("Text: %s\n", note->text);
}