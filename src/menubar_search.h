#ifndef __MENUBAR_SEARCH_H__
#define __MENUBAR_SEARCH_H__

#include "global.h"
#include "menubar_create_item.h"
#include "file_tabs.h"

#define SEARCH_MARK_START "search_start"
#define SEARCH_MARK_END "search_end"

// Callbacks
void find_cb(GtkMenuItem *menuitem, TextEditor *editor);

void create_search_menu(TextEditor *);

#endif