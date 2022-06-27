#include "menubar_documents.h"

static void save_all_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Remember actual selected file
    GtkWidget *curr_selected_file = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    for(int i = 0; i < MAX_OPEN_TABS; i++) {
        // All files are saved
        if (editor->files[i] == NULL)
        {
            break;
        }

        // Buffer was modified, save is needed
        if (gtk_text_buffer_get_modified(editor->files[i]->buffer))
        {
            // Change selected file
            gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[i]->vbox);

            // Write file            
            write_file(editor, editor->files[i]);
        }
    }

    // After all files are saved, change to the original selected file
    gtk_stack_set_visible_child(GTK_STACK(editor->stack), curr_selected_file);
}

void close_all_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    int i = 0;

    while (TRUE) {
        // When there is file
        if (editor->files[i] != NULL) {
            // Is there something to be saved ? 
            if (
                gtk_text_buffer_get_modified(editor->files[i]->buffer) &&
                gtk_text_buffer_get_char_count(editor->files[i]->buffer) > 0
            ) {
                // Ask user if he wants to save file
                if (!check_for_save(editor))
                {
                    i++;
                    continue;
                }
            }

            // Switch to file
            gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[i]->vbox);

            // Close file
            close_open_file_in_tab(editor);
            continue;
        }

        // All files have been saved or closed
        break;
    }
}

static void prev_document_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    GtkWidget *visible_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // Select first, when no child is visible
    if (visible_child == NULL)
    {
        // Skip when there are no files
        if (editor->files[0] == NULL)
        {
            return;
        }

        // Set first child as visible
        gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[0]->vbox);
        return;
    }

    gboolean next = FALSE;

    // Loop through all files, and check which is visible
    for (int i = MAX_OPEN_TABS - 1; i >= 0; i--) {
        if (next == TRUE && editor->files[i] != NULL)
        {
            // Set visible child
            gtk_widget_show_all(GTK_WIDGET(editor->files[i]->vbox));
            gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[i]->vbox);
            return;
        }
        
        // Find which files is visible, when found show next in row
        if (editor->files[i] != NULL && editor->files[i]->vbox == visible_child)
        {
            next = TRUE;
            continue;
        }
    }
}

static void next_document_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    GtkWidget *visible_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // Select first, when no child is visible
    if (visible_child == NULL)
    {
        // Skip when there are no files
        if (editor->files[editor->new_file_index - 1] == NULL)
        {
            return;
        }

        // Set first child as visible
        gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[0]->vbox);
        return;
    }

    gboolean next = FALSE;

    // Loop through all files, and check which is visible
    for (int i = 0; i < MAX_OPEN_TABS; i++) {
        if (next == TRUE && editor->files[i] != NULL)
        {
            // Set visible child
            gtk_widget_show_all(GTK_WIDGET(editor->files[i]->vbox));
            gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[i]->vbox);
            return;
        }
        
        // Find which files is visible, when found show next in row
        if (editor->files[i] != NULL && editor->files[i]->vbox == visible_child)
        {
            next = TRUE;
            continue;
        }
    }
}

void documents_file_menu(TextEditor *editor) {
    // Create documents menu item
    GtkWidget *documents = gtk_menu_item_new_with_mnemonic(_("_Documents"));
    gtk_menu_shell_append(GTK_MENU_SHELL(editor->menubar), documents);
    
    // Create menu item
    GtkWidget *documentsmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(documents), documentsmenu);

    // Add file items
    GtkWidget *save_all = create_icon_item(_("_SaveAll"), "document-save", editor->accel_group, GDK_KEY_l, GDK_CONTROL_MASK | GDK_SHIFT_MASK);
    GtkWidget *close_all = create_icon_item(_("_CloseAll"), "window-close", editor->accel_group, GDK_KEY_w, GDK_CONTROL_MASK | GDK_SHIFT_MASK);
    GtkWidget *prev_document = create_item(_("_PrevDocument"), editor->accel_group, GDK_KEY_r, GDK_CONTROL_MASK);
    GtkWidget *next_document = create_item(_("_NextDocument"), editor->accel_group, GDK_KEY_t, GDK_CONTROL_MASK);

    // Append items to menu
    gtk_menu_shell_append(GTK_MENU_SHELL(documentsmenu), save_all);
    gtk_menu_shell_append(GTK_MENU_SHELL(documentsmenu), close_all);
    gtk_menu_shell_append(GTK_MENU_SHELL(documentsmenu), gtk_separator_menu_item_new());
    gtk_menu_shell_append(GTK_MENU_SHELL(documentsmenu), prev_document);
    gtk_menu_shell_append(GTK_MENU_SHELL(documentsmenu), next_document);

    editor->menubar_struct.documents_save_all = save_all;
    editor->menubar_struct.documents_close_all = close_all;
    editor->menubar_struct.documents_prev_document = prev_document;
    editor->menubar_struct.documents_next_document = next_document;

    // Set sensitive to false
    gtk_widget_set_sensitive(GTK_WIDGET(save_all), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(close_all), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(prev_document), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(next_document), FALSE);

    // Add signals
	g_signal_connect(G_OBJECT(save_all), "activate", G_CALLBACK(save_all_cb), editor);
	g_signal_connect(G_OBJECT(close_all), "activate", G_CALLBACK(close_all_cb), editor);
	g_signal_connect(G_OBJECT(prev_document), "activate", G_CALLBACK(prev_document_cb), editor);
	g_signal_connect(G_OBJECT(next_document), "activate", G_CALLBACK(next_document_cb), editor);

}