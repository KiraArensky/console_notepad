// struct.h
#ifndef CONSOLE_NOTEPAD_STRUCT_H_
#define CONSOLE_NOTEPAD_STRUCT_H_

typedef struct Note {
    int id;
    char created_timestamp[20];
    char edited_timestamp[20];
    char *text;
} Note;

#endif  // CONSOLE_NOTEPAD_STRUCT_H_
