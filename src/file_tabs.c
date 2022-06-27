#include "file_tabs.h"

static gint untitled_index = 1;

static void switch_to_open_file(TextEditor *editor, const gchar *filepath) {
    for (int i = 0; i < MAX_OPEN_TABS; i++) {
        if (editor->files[i] == NULL)
        {
            fprintf(stderr, "Filepath for open file not found: %s\n", (char *)filepath);
            return;
        }

        if (strcmp(editor->files[i]->filepath, filepath) == 0)
        {
            // Show child that is already open
            gtk_widget_show(GTK_WIDGET(editor->files[i]->vbox));
            gtk_stack_set_visible_child(GTK_STACK(editor->stack) ,editor->files[i]->vbox);
            return;
        }
    }
}

static TextEditor_File* create_file(TextEditor *editor, const gchar *filename, const gchar *filepath) {
    // Max 50 files, cant open any more
    if (editor->new_file_index >= MAX_OPEN_TABS)
    {
        return NULL;
    }
    
    TextEditor_File *file = g_slice_new(TextEditor_File);

    // Copy filename
    file->filename = g_strdup_printf("%s", filename);
    file->filepath = g_strdup_printf("%s", filepath);

    // Set default values
    file->buffer = NULL;
    file->vbox = NULL;
    file->text_view = NULL;
    file->file_saved = FALSE;
    file->search_text = NULL;

    // Insert into files
    editor->files[editor->new_file_index++] = file;

    // Return file
    return file;
}

void close_file(TextEditor *editor, GtkWidget *vbox) {
    TextEditor_File *tmp = NULL;
    int index = 0;

    // Find file we are looking for based on widget pointer
    for (; index < MAX_OPEN_TABS; index++) {
        // No more files, file not found
        if (editor->files[index] == NULL)
        {
            return;
        }

        // Find file
        if (editor->files[index]->vbox == vbox) {
            tmp = editor->files[index];
            break;
        }
    }

    // Move pointer one by one
    for (int i = index; i < (MAX_OPEN_TABS - 1); i++) {
        editor->files[i] = editor->files[i+1];
    }

    // Clear last pointer
    editor->files[MAX_OPEN_TABS - 1] = NULL;
    
    // Free copyied filename
    g_free(tmp->filename);
    g_free(tmp->filepath);

    // Free allocated structure
    g_slice_free(TextEditor_File, tmp);
    tmp = NULL;

    // Decrement pointer
    editor->new_file_index--;
}

static int load_file(TextEditor *editor, TextEditor_File *file) {
    // Set status loading file ...
    set_statustext(editor, STATUSBAR_FILE_LOADING, file->filepath);

    while (gtk_events_pending()) gtk_main_iteration();

	GError *err=NULL;
    gchar *text;
    GtkTextIter iter;

    // Read file to string
    gboolean result = g_file_get_contents(file->filepath, &text, NULL, &err);

    // Failed to load file, return error
    if (result == FALSE) {
		// Error loading file
		error_message(editor, err->message);
		g_error_free(err);
		close_file(editor, file->vbox);
        return FALSE;
	}

    /* disable the text mofications while loading buffer */    
	gtk_widget_set_sensitive(file->text_view, FALSE);
	gtk_text_buffer_set_text(file->buffer, text, -1);
	gtk_text_buffer_set_modified(file->buffer, FALSE);

    /* move cursor to the beginning */
	gtk_text_buffer_get_start_iter(file->buffer, &iter);
	gtk_text_buffer_place_cursor(file->buffer, &iter);
	gtk_text_view_place_cursor_onscreen(GTK_TEXT_VIEW(file->text_view));
	gtk_widget_set_sensitive(file->text_view, TRUE);
	g_free(text); 

    // After file has been loaded, clear statusbar
    reset_default_status(editor);

    return TRUE;
}

TextEditor_File * get_file(TextEditor *editor, GtkWidget *vbox) {
    // No vbox specified
    if (vbox == NULL)
    {
        return NULL;
    }
    

    // Try to find file based on given widget
    for (int i = 0; i < MAX_OPEN_TABS; i++) {
        // No file found
        if (editor->files[i] == NULL)
        {
            return NULL;
        }

        // When file was found return it
        if (editor->files[i]->vbox == vbox)
        {
            return editor->files[i];
        }
    }

    // No file found
    return NULL;
}

static GtkWidget *construct_stack_content(const gchar *filepath, GtkWidget **text_view) {
    // Create VBOX
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Create scroll window
    GtkWidget *scroll_w = gtk_scrolled_window_new(NULL, NULL);

    // Create text view
    (*text_view) = gtk_text_view_new();

    // Ignore drag n drop
    gtk_drag_dest_unset(GTK_WIDGET((*text_view)));

    gtk_text_view_set_monospace(GTK_TEXT_VIEW((*text_view)), TRUE);     

    // Add text view to scroll window
	gtk_container_add(GTK_CONTAINER(scroll_w), (*text_view));

    // Add scroll window to VBOX
    gtk_box_pack_start(GTK_BOX(vbox), scroll_w, TRUE, TRUE, 6);

    return vbox;
}

static void cursor_position_cb(GObject *obj, GParamSpec *param, TextEditor *editor) {
    // Get selected child
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // Get file
	TextEditor_File *file = get_file(editor, selected_child);

    // Set line, col and lines
    set_col_line_text_from_file(editor, file);
}

static void modified_changed_cb(GtkTextBuffer *buffer, TextEditor *editor) {
    // Get selected child
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // Get file
	TextEditor_File *file = get_file(editor, selected_child);

    // Skip
    if (file == NULL)
    {
        return;
    }

    // When buffer was modified    
    if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(file->buffer)))
    {
        // Generate filename
        gchar *changed_filename = g_strdup_printf("*%s", file->filename);

        // Change filename to *name
        gtk_container_child_set(
            GTK_CONTAINER(editor->stack),
            file->vbox,
            "title",
            changed_filename,
            NULL
        );

        // Free memory
        g_free(changed_filename);
    // Buffer has unchanged, file was saved ...
    } else {
        // Change filename to name
        gtk_container_child_set(
            GTK_CONTAINER(editor->stack),
            file->vbox,
            "title",
            file->filename,
            NULL
        );
    }
    
}

void new_file_in_tab(TextEditor *editor) {
    gchar *filename = g_strdup_printf("Untitled-%d", untitled_index);
    gchar *filepath = g_strdup_printf("Untitled-%d", untitled_index++);

    TextEditor_File *file = create_file(editor, filename, filepath);

    // Create VBOX
    GtkWidget* vbox = construct_stack_content(filepath, &(file->text_view));

    file->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file->text_view));
    file->vbox = vbox;

    // Connect cursor change
    g_signal_connect(
        G_OBJECT(file->buffer),
        "notify::cursor-position",
        G_CALLBACK(cursor_position_cb),
        editor
    );

    // Connect buffer modified change
    g_signal_connect(
        G_OBJECT(file->buffer),
        "modified-changed",
        G_CALLBACK(modified_changed_cb),
        editor
    );

    // Add to stack
    gtk_stack_add_titled(GTK_STACK(editor->stack), vbox, filepath, filename);

    // Make visible child we want to show
    gtk_widget_show_all(GTK_WIDGET(vbox));
    
    // Set visible newly opened file
    gtk_stack_set_visible_child(GTK_STACK(editor->stack), vbox);

    // Grab focus
    gtk_widget_grab_focus(GTK_WIDGET(file->text_view));

    // Free names
    g_free(filename);
    g_free(filepath);
}

void open_file_in_tab(TextEditor *editor, const gchar *filename, const gchar *filepath) {
    // When file is already open
    if (gtk_stack_get_child_by_name(GTK_STACK(editor->stack), filepath) != NULL)
    {
        // Switch to already open file
        switch_to_open_file(editor, filepath);
        return;
    }

    // OTHERWISE LOAD FILE
    TextEditor_File *file = create_file(editor, filename, filepath);

    // Create VBOX
    GtkWidget* vbox = construct_stack_content(filepath, &(file->text_view));

    file->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file->text_view));
    file->vbox = vbox;
    file->file_saved = TRUE;

    // Connect cursor change
    g_signal_connect(
        G_OBJECT(file->buffer),
        "notify::cursor-position",
        G_CALLBACK(cursor_position_cb),
        editor
    );

    // Connect buffer modified change
    g_signal_connect(
        G_OBJECT(file->buffer),
        "modified-changed",
        G_CALLBACK(modified_changed_cb),
        editor
    );

    // Add to stack
    gtk_stack_add_titled(GTK_STACK(editor->stack), vbox, filepath, filename);

    // Make visible child we want to show
    gtk_widget_show_all(GTK_WIDGET(vbox));
    
    // Set visible newly opened file
    gtk_stack_set_visible_child(GTK_STACK(editor->stack), vbox);

    // Load file
    if(load_file(editor, file) == FALSE) {
        error_message(editor, "Failed to load file!");
        //gtk_container_remove(GTK_CONTAINER(editor->stack), vbox);
    }

    // Grab focus
    gtk_widget_grab_focus(GTK_WIDGET(file->text_view));
}

void close_open_file_in_tab(TextEditor *editor) {
    // Get currently open file
    GtkWidget *curr_open_file = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // No file is currently open, skip
    if (curr_open_file == NULL)
    {
        return;
    }
    
    // Close file
    gtk_container_remove(GTK_CONTAINER(editor->stack), curr_open_file);

    // Delete file
    close_file(editor, curr_open_file);

    // Reset status
    reset_default_status(editor);
}

static void set_disabled_buttons_state(TextEditor *editor, gboolean value) {
    // Set file
    gtk_widget_set_sensitive(editor->menubar_struct.file_save, value);
    gtk_widget_set_sensitive(editor->menubar_struct.file_save_as, value);
    gtk_widget_set_sensitive(editor->menubar_struct.file_print, value);

    // Set edit
    gtk_widget_set_sensitive(editor->menubar_struct.edit_cut, value);
    gtk_widget_set_sensitive(editor->menubar_struct.edit_copy, value);
    gtk_widget_set_sensitive(editor->menubar_struct.edit_paste, value);
    gtk_widget_set_sensitive(editor->menubar_struct.edit_delete, value);
    gtk_widget_set_sensitive(editor->menubar_struct.edit_select_all, value);

    // Set search
    gtk_widget_set_sensitive(editor->menubar_struct.search_find, value);
    gtk_widget_set_sensitive(editor->menubar_struct.search_find_next, value);
    gtk_widget_set_sensitive(editor->menubar_struct.search_find_previous, value);

    // Set documents
    gtk_widget_set_sensitive(editor->menubar_struct.documents_save_all, value);
    gtk_widget_set_sensitive(editor->menubar_struct.documents_close_all, value);
    gtk_widget_set_sensitive(editor->menubar_struct.documents_prev_document, value);
    gtk_widget_set_sensitive(editor->menubar_struct.documents_next_document, value);

    // Set toolbar
    gtk_widget_set_sensitive(editor->toolbar_struct.close, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.save, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.print, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.cut, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.copy, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.paste, value);
    gtk_widget_set_sensitive(editor->toolbar_struct.search, value);
}

static void stack_notify_cb(GtkStack *stack, guint a, TextEditor *editor) {
    // Get selected file
    GtkWidget *selected_file = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File * file = get_file(editor, selected_file);

    // Show file statustext
    if (file != NULL)
    {
        // Reset default status 
        reset_default_status(editor);

        // Enable buttons
        set_disabled_buttons_state(editor, TRUE);

        // Set new cols and rows counter
        set_col_line_text_from_file(editor, file);
        return;
    }

    set_disabled_buttons_state(editor, FALSE);
}

void create_filetabs(TextEditor *editor) {
    
    // Create stack and stack switcher
    editor->stack = gtk_stack_new();
    editor->stack_switcher = gtk_stack_switcher_new();

    // Set stack transition
    gtk_stack_set_transition_type(GTK_STACK(editor->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(editor->stack), 200);

    // Set stack
    gtk_stack_switcher_set_stack (GTK_STACK_SWITCHER(editor->stack_switcher), GTK_STACK(editor->stack));

    // Set alignment and margin
    gtk_widget_set_halign (GTK_WIDGET(editor->stack_switcher), GTK_ALIGN_START);
    gtk_widget_set_margin_start(GTK_WIDGET(editor->stack_switcher), 6);

    // Create VBOX to hold stack and stack switcher
    editor->vbox_stack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Create frame to hold VBOX
    editor->stack_frame = gtk_frame_new(NULL);

    // Scroll for stack switcher
    editor->scroll_w_stack_switcher = gtk_scrolled_window_new(NULL, NULL);

    gtk_widget_set_margin_end(GTK_WIDGET(editor->scroll_w_stack_switcher), 6);

    // Add stack switcher to scroll window
	gtk_container_add(GTK_CONTAINER(editor->scroll_w_stack_switcher), editor->stack_switcher);

    // Add SCROLL WINDOW(STACK SWITCHER) and STACK to VBOX
    gtk_box_pack_start(GTK_BOX(editor->vbox_stack), editor->scroll_w_stack_switcher, FALSE, FALSE, 6);
    gtk_box_pack_start(GTK_BOX(editor->vbox_stack), editor->stack, TRUE, TRUE, 6);

    // Put VBOX into FRAME
    gtk_container_add(GTK_CONTAINER(editor->stack_frame), editor->vbox_stack);

    // Put FRAME into main WINDOW VBOX
    gtk_box_pack_start(GTK_BOX(editor->vbox), editor->stack_frame, TRUE, TRUE, 0);

    // Add signal connect to stack
	g_signal_connect(
        G_OBJECT(editor->stack),
        "notify::visible-child",
        G_CALLBACK(stack_notify_cb),
        editor
    );
}