#ifndef __FILE_TABS_H__
#define __FILE_TABS_H__

#include "global.h"
#include "statusbar.h"

void open_file_in_tab(TextEditor *editor, const gchar *filename, const gchar *filepath);
void new_file_in_tab(TextEditor *editor);
void create_filetabs(TextEditor *editor);
void close_open_file_in_tab(TextEditor *editor);
TextEditor_File * get_file(TextEditor *editor, GtkWidget *vbox);
void close_file(TextEditor *editor, GtkWidget *vbox);
#endif