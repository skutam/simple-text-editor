#ifndef __MENUBAR_DOCUMENTS_H__
#define __MENUBAR_DOCUMENTS_H__

#include "global.h"
#include "menubar_create_item.h"
#include "menubar_file.h"

void documents_file_menu(TextEditor *);

void close_all_cb(GtkMenuItem *menuitem, TextEditor *editor);
#endif