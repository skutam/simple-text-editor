#ifndef __MENUBAR_EDIT_H__
#define __MENUBAR_EDIT_H__

#include "global.h"
#include "menubar_create_item.h"
#include "file_tabs.h"

// Callbacks
void cut_cb(GtkMenuItem *menuitem, TextEditor *editor);
void copy_cb(GtkMenuItem *menuitem, TextEditor *editor);
void paste_cb(GtkMenuItem *menuitem, TextEditor *editor);


void create_edit_menu(TextEditor *);

#endif