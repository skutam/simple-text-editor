#include "menubar_search.h"

void find_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Clear statusbar
    reset_default_status(editor);
  
    // Get currently open file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    // Get file
	TextEditor_File *file = get_file(editor, selected_child);

    // No file selected
    if (file == NULL)
    {
        return;
    }

    GtkWidget *content, *entry, *dialog;
    const gchar *text;
	GtkTextIter iter;
	GtkTextIter pstart,pend;
    GtkTextMark *search_end;
	gboolean found, res;

    // Create find dialog
    dialog = gtk_dialog_new_with_buttons(
        _("Search"),
        GTK_WINDOW(editor->window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_OK", GTK_RESPONSE_OK,
        "_CANCEL", GTK_RESPONSE_CANCEL,
        NULL
    );

    // Get dialog content
    #if GTK_MAJOR_VERSION >= 3 || GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION >= 14
	    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    #else
        content = GTK_DIALOG(dialog)->vbox;
    #endif

    // Create new entry
    entry = gtk_entry_new();

    // When we already got search text defined, use it
    if (file->search_text)
    {
        gtk_entry_set_text(GTK_ENTRY(entry), file->search_text);
    }

    // Append and show entry in dialog
    gtk_container_add(GTK_CONTAINER(content), entry);
	gtk_widget_show_all(dialog);

    // Press OK when ENTER is pressed
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

    // Run dialog
	res = gtk_dialog_run(GTK_DIALOG(dialog));

    // Found was closed
    if (res != GTK_RESPONSE_OK)
    {
        // Close widget
        gtk_widget_destroy(dialog);
        return;
    }

    // Get entry data from dialog
	text = gtk_entry_get_text(GTK_ENTRY(entry));

    // Get start itter
    gtk_text_buffer_get_start_iter(file->buffer, &iter);

    // Find search text
    found = gtk_text_iter_forward_search(
        &iter,
        text,
        GTK_TEXT_SEARCH_CASE_INSENSITIVE,
        &pstart,
        &pend,
        NULL
    );

    // When we find something, highlight it
    if (found)
    {
		gtk_text_buffer_select_range(file->buffer, &pstart, &pend);

        // Free search text, when defined
        if (file->search_text)
        {
            g_free(file->search_text);
        }

        // Update search text
        file->search_text = g_strdup(text);

        // Create mark in buffer
        gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_START, &pstart, FALSE);
        search_end = gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_END, &pend, FALSE);

        // Scroll to mark
        gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(file->text_view), search_end);
    }

    // Close widget
    gtk_widget_destroy(dialog);
    
    // Show widget when nothing was found
    if (!found)
    {
        info_message(editor, _("Not found"));
    }
}

static void find_next_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get currently open file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    // Get file
	TextEditor_File *file = get_file(editor, selected_child);

    // No file selected
    if (file == NULL)
    {
        return;
    }

    GtkTextMark *search_end;
	GtkTextIter iter;
	GtkTextIter pstart,pend;
	gboolean found;

    // Get mark
	search_end = gtk_text_buffer_get_mark(file->buffer, SEARCH_MARK_END);
	
    // No mark previously selected
    if (search_end == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(editor->statusbar),
			editor->statusbar_context_id, _("Cannot continue"));
		return;
	}

    // Get itter at mark
	gtk_text_buffer_get_iter_at_mark(file->buffer, &iter, search_end);

    // Search next input
	found = gtk_text_iter_forward_search(
        &iter,
        file->search_text,
        GTK_TEXT_SEARCH_CASE_INSENSITIVE,
        &pstart,
        &pend,
        NULL
    );

    // When found highlight found text
    if (found) {
        gtk_text_buffer_select_range(file->buffer, &pstart, &pend);
        gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_START, &pstart, FALSE);
        gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_END, &pend, FALSE);
		gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(file->text_view), search_end);
    } else {
        info_message(editor, _("Not found"));
    }
}

static void find_previous_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    // Get currently open file
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    // Get file
	TextEditor_File *file = get_file(editor, selected_child);

    // No file selected
    if (file == NULL)
    {
        return;
    }

    GtkTextMark *search_start;
	GtkTextIter iter;
	GtkTextIter pstart,pend;
	gboolean found;

    // Get mark
	search_start = gtk_text_buffer_get_mark(file->buffer, SEARCH_MARK_START);
	
    // No mark previously selected
    if (search_start == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(editor->statusbar),
			editor->statusbar_context_id, _("Cannot continue"));
		return;
	}

    // Get itter at mark
	gtk_text_buffer_get_iter_at_mark(file->buffer, &iter, search_start);

    // Search next input
	found = gtk_text_iter_backward_search(
        &iter,
        file->search_text,
        GTK_TEXT_SEARCH_CASE_INSENSITIVE,
        &pstart,
        &pend,
        NULL
    );

    // When found highlight found text
    if (found) {
        gtk_text_buffer_select_range(file->buffer, &pstart, &pend);
        gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_START, &pstart, FALSE);
        gtk_text_buffer_create_mark(file->buffer, SEARCH_MARK_END, &pend, FALSE);
		gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(file->text_view), search_start);
    } else {
        info_message(editor, _("Not found"));
    }
}

void create_search_menu(TextEditor *editor) {
    // Create search menu item
    GtkWidget *search = gtk_menu_item_new_with_mnemonic(_("_Search"));
    gtk_menu_shell_append(GTK_MENU_SHELL(editor->menubar), search);
    
    // Create menu item
    GtkWidget *searchmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(search), searchmenu);

    // Add search items
    GtkWidget *find = create_icon_item(_("_Find"), "edit-find", editor->accel_group, GDK_KEY_f, GDK_CONTROL_MASK);
    GtkWidget *find_next = create_item(_("_FindNext"), editor->accel_group, GDK_KEY_g, GDK_CONTROL_MASK);
    GtkWidget *find_previous = create_item(_("_FindPrevious"), editor->accel_group, GDK_KEY_g, GDK_CONTROL_MASK | GDK_SHIFT_MASK);

    editor->menubar_struct.search_find = find;
    editor->menubar_struct.search_find_next = find_next;
    editor->menubar_struct.search_find_previous = find_previous;

    // Set default 
    gtk_widget_set_sensitive(GTK_WIDGET(find), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(find_next), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(find_previous), FALSE);

    // Append items to menu
    gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), find);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), find_next);
	gtk_menu_shell_append(GTK_MENU_SHELL(searchmenu), find_previous);
    
    // Add signals
	g_signal_connect(G_OBJECT(find), "activate", G_CALLBACK(find_cb), editor);
	g_signal_connect(G_OBJECT(find_next), "activate", G_CALLBACK(find_next_cb), editor);
	g_signal_connect(G_OBJECT(find_previous), "activate", G_CALLBACK(find_previous_cb), editor);
}