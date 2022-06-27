#include "menubar_edit.h"

void cut_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get selection
    GtkClipboard  *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    
    // Get currently selected file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_child);

    // No file selected, or no selection
    if (file == NULL || clipboard == NULL)
    {
        return;
    }
    
    // Cut out from buffer
    gtk_text_buffer_cut_clipboard(file->buffer, clipboard, TRUE);
}

void copy_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get selection
    GtkClipboard  *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    
    // Get currently selected file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_child);

    // No file selected, or no selection
    if (file == NULL || clipboard == NULL)
    {
        return;
    }
    
    // Cut out from buffer
    gtk_text_buffer_copy_clipboard(file->buffer, clipboard);
}

void paste_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get selection
    GtkClipboard  *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    
    // Get currently selected file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_child);

    // No file selected, or no selection
    if (file == NULL || clipboard == NULL)
    {
        return;
    }
    
    // Cut out from buffer
    gtk_text_buffer_paste_clipboard(file->buffer, clipboard, NULL, TRUE);
}

static void delete_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get currently selected file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_child);

    // No file selected, or no selection
    if (file == NULL)
    {
        return;
    }
    
    // Cut out from buffer
    gtk_text_buffer_delete_selection(file->buffer, FALSE, TRUE);
}

static void select_all_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get currently selected file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_child);
	GtkTextIter start, end;

    // No file selected, or no selection
    if (file == NULL)
    {
        return;
    }
    
    gtk_text_buffer_get_start_iter(file->buffer, &start);
	gtk_text_buffer_get_end_iter(file->buffer, &end);
    gtk_text_buffer_select_range(file->buffer, &start, &end);
}

void create_edit_menu(TextEditor *editor) {
    // Create edit menu item
    GtkWidget *edit = gtk_menu_item_new_with_mnemonic(_("_Edit"));
    gtk_menu_shell_append(GTK_MENU_SHELL(editor->menubar), edit);
    
    // Create menu item
    GtkWidget *editmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), editmenu);

    // Add edit items
    GtkWidget *cut = create_icon_item(_("_Cut"), "edit-cut", editor->accel_group, GDK_KEY_x, GDK_CONTROL_MASK);
    GtkWidget *copy = create_icon_item(_("_Copy"), "edit-copy", editor->accel_group, GDK_KEY_c, GDK_CONTROL_MASK);
    GtkWidget *paste = create_icon_item(_("_Paste"), "edit-paste", editor->accel_group, GDK_KEY_v, GDK_CONTROL_MASK);
    GtkWidget *delete = create_icon_item(_("_Delete"), "edit-delete", editor->accel_group, GDK_KEY_d, GDK_CONTROL_MASK);
    GtkWidget *select_all = create_icon_item(_("_SelectAll"), "edit-select-all", editor->accel_group, GDK_KEY_a, GDK_CONTROL_MASK);

    // Append items to menu
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), cut);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), copy);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), paste);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), delete);
    gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), select_all);

    editor->menubar_struct.edit_cut = cut;
    editor->menubar_struct.edit_copy = copy;
    editor->menubar_struct.edit_paste = paste;
    editor->menubar_struct.edit_delete = delete;
    editor->menubar_struct.edit_select_all = select_all;

    // Disable cut, copy, paste, delete and select_all
    gtk_widget_set_sensitive(GTK_WIDGET(cut), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(copy), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(paste), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(delete), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(select_all), FALSE);

    // Add signals
	g_signal_connect(G_OBJECT(cut), "activate", G_CALLBACK(cut_cb), editor);
	g_signal_connect(G_OBJECT(copy), "activate", G_CALLBACK(copy_cb), editor);
	g_signal_connect(G_OBJECT(paste), "activate", G_CALLBACK(paste_cb), editor);
	g_signal_connect(G_OBJECT(delete), "activate", G_CALLBACK(delete_cb), editor);
	g_signal_connect(G_OBJECT(select_all), "activate", G_CALLBACK(select_all_cb), editor);
	
}