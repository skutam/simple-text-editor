#ifndef __STATUSBAR_H__
#define __STATUSBAR_H__

#include "global.h"

typedef enum {
    STATUSBAR_FILE_OPENING,
    STATUSBAR_FILE_SAVING,
    STATUSBAR_FILE_LOADING,
    STATUSBAR_FILE_OPENED,
    STATUSBAR_EMPTY
} STATUSBAR_TYPES;

void set_col_line_text_from_file(TextEditor *editor, TextEditor_File *file);
void set_statustext(TextEditor *editor, STATUSBAR_TYPES type, const gchar *text);

void reset_default_status(TextEditor *editor);

void create_statusbar(TextEditor *editor);

#endif