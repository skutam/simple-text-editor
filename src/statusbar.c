#include "statusbar.h"

static void set_col_line_text(TextEditor *editor, guint lines, guint row, guint col) {
    // Generate text
    gchar *col_line_text = g_strdup_printf("Ln %d, Col %d, Lines %d", row, col, lines);

    // Set label text    
    gtk_label_set_text(GTK_LABEL(editor->col_line_label), col_line_text);

    // Free generated text
    g_free(col_line_text);
}

void set_col_line_text_from_file(TextEditor *editor, TextEditor_File *file) {
    gint lines = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(file->buffer));
    GtkTextIter iter;
    
    // Get itteration
    gtk_text_buffer_get_iter_at_mark(file->buffer,
      &iter,
      gtk_text_buffer_get_insert(file->buffer)
    );

    // Get row and col
    gint row = gtk_text_iter_get_line(&iter);
    gint col = gtk_text_iter_get_line_offset(&iter);

    // Set col_line_text
    set_col_line_text(editor, lines, row, col);
}

void set_statustext(TextEditor *editor, STATUSBAR_TYPES type, const gchar *text) {
    gchar *status = NULL;
    
    // Generate status
    switch (type)
    {
        case STATUSBAR_FILE_LOADING:
            status = g_strdup_printf(_("Loading: %s..."), text);
            break;
        
        case STATUSBAR_FILE_OPENING:
            status = g_strdup_printf(_("Loading file: \'%s\'..."), text);
            break;

        case STATUSBAR_FILE_SAVING:
            status = g_strdup_printf(_("Saving file: %s..."), text);
            break;
        
        case STATUSBAR_FILE_OPENED:
            status = g_strdup_printf(_("File: %s"), text);
            break;

        case STATUSBAR_EMPTY:
        default:
            break;
    }

    // Push status, when defined
    if (status != NULL) {
        gtk_statusbar_push(
            GTK_STATUSBAR(editor->statusbar),
            editor->statusbar_context_id,
            status
        );
    }

    // Clear allocated status
    if (status != NULL)
    {
        g_free(status);
    }
}

void reset_default_status(TextEditor *editor) {
    // Clear all statusbar 
    gtk_statusbar_remove_all(GTK_STATUSBAR(editor->statusbar), editor->statusbar_context_id);

    // When file is open, show File:
    const gchar *open_filepath = gtk_stack_get_visible_child_name(GTK_STACK(editor->stack));

    if (open_filepath != NULL)
    {
        set_statustext(editor, STATUSBAR_FILE_OPENED, open_filepath);
        return;
    }

    // Otherwise leave empty
    set_statustext(editor, STATUSBAR_EMPTY, "");
}

void create_statusbar(TextEditor *editor) {
    // Create new box for holding statusbar
    editor->hbox_statusbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Create new statusbar, and get context_id of statusbar
    editor->statusbar = gtk_statusbar_new();
    editor->statusbar_context_id = gtk_statusbar_get_context_id(
        GTK_STATUSBAR(editor->statusbar),
        _(PROGRAM_NAME)
    );

    // Create label for 
    editor->col_line_label = gtk_label_new("");

    // Add to container
    gtk_box_pack_start(GTK_BOX(editor->hbox_statusbar), editor->statusbar, TRUE, TRUE, 6);
    gtk_box_pack_end(GTK_BOX(editor->hbox_statusbar), editor->col_line_label, FALSE, FALSE, 6);

    // Add to main VBOX
    gtk_box_pack_start(GTK_BOX(editor->vbox), editor->hbox_statusbar, FALSE, FALSE, 0);

    // Set default label value
    set_col_line_text(editor, 0, 0, 0);
}