#include "menubar.h"

void create_menubar(TextEditor *editor) {
    // Create menubar
    editor->menubar = gtk_menu_bar_new();

    // Add file menu
    create_file_menu(editor);

    // Add edit menu
    create_edit_menu(editor);

    // Add search menu
    create_search_menu(editor);

    // Add file menu
    documents_file_menu(editor);

    // Create help menu
    create_help_menu(editor);

    // Add to VBOX
    gtk_box_pack_start(GTK_BOX(editor->vbox), editor->menubar, FALSE, FALSE, 0);
}