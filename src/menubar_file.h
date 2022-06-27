#ifndef __MENUBAR_FILE_H__
#define __MENUBAR_FILE_H__

#include "global.h"
#include "menubar_create_item.h"
#include "file_tabs.h"

#define HEADER_HEIGHT 20.0f

// Callbacks
void new_item_cb(GtkMenuItem *menuitem, TextEditor *editor);
void open_item_cb(GtkMenuItem *menuitem, TextEditor *editor);
void save_item_cb(GtkMenuItem *menuitem, TextEditor *editor);
void print_item_cb(GtkMenuItem *menuitem, TextEditor *editor);
void close_item_cb(GtkMenuItem *menuitem, TextEditor *editor);

void create_file_menu(TextEditor *);

gboolean write_file(TextEditor *editor, TextEditor_File *file);
gboolean check_for_save(TextEditor *editor);
#endif