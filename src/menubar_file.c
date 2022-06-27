#include "menubar_file.h"

gboolean write_file(TextEditor *editor, TextEditor_File *file) {
    // File is not saved, get filename
    if (!file->file_saved)
    {
        GtkWidget *chooser;
        gchar *filename=NULL;

        // Open dialog for choosing where to save file
        chooser = gtk_file_chooser_dialog_new(
            _("Choose file name."),
            GTK_WINDOW(editor->window),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            _("_Cancel"), GTK_RESPONSE_CANCEL,
            _("Save _As"), GTK_RESPONSE_OK,
            NULL
        );

        // Set default name
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(chooser), file->filename);

        // Run dialog
        gboolean res = gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_OK ? TRUE : FALSE;

        // Open dialog, when OK return filename
        if (res) {
		    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
            // Get filename and path
            if (file->filepath != NULL)
            {
                g_free(file->filepath);
                file->filepath = g_strdup_printf("%s", filename);
            }

            if (file->filename != NULL)
            {
                g_free(file->filename);
                file->filename = g_strdup_printf("%s", g_path_get_basename(file->filepath));
            
            }
            
            // Set file_saved because we know where to save file
            file->file_saved = TRUE;
        }

        gtk_widget_destroy(chooser);
    }

    GError *err=NULL;
	gchar  *text;
	GtkTextIter start, end;

    // Set status saving
    set_statustext(editor, STATUSBAR_FILE_SAVING, file->filepath);
    
    while (gtk_events_pending()) gtk_main_iteration();

    // Disable text view modifications and get contents of buffer
    gtk_widget_set_sensitive(file->text_view, FALSE);
	gtk_text_buffer_get_start_iter(file->buffer, &start);
	gtk_text_buffer_get_end_iter(file->buffer, &end);
	text = gtk_text_buffer_get_text(file->buffer, &start, &end, FALSE);
	gtk_text_buffer_set_modified(file->buffer, FALSE);
	gtk_widget_set_sensitive(file->text_view, TRUE);

    /* save text to file with error checking */
	if (g_file_set_contents(file->filepath, text, -1, &err) == FALSE) {
		/* error saving file, show message to user */
		error_message(editor, err->message);
		g_error_free(err);
	}

    // Free memory
    g_free(text);

    // Reset status
    reset_default_status(editor);

    // Set focus back to file
    gtk_widget_grab_focus(GTK_WIDGET(file->text_view));
    return TRUE;
}

/**
 * 
 * @return gboolean True when file was saved, False otherwise 
 * */
gboolean check_for_save(TextEditor *editor) {
    // Get selected child
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));

    // Get file
	TextEditor_File * file = get_file(editor, selected_child);
    GtkWidget *dialog = NULL;
	int ret = 0;

    // No file selected
    if (file == NULL)
    {
        return TRUE;
    }

    // When file was modified, and has atleast 1 character
    if (gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(file->buffer))) {
        // Show popup for savig file
        dialog = gtk_message_dialog_new(
            GTK_WINDOW(editor->window),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            _("Save changes before closing file ?")    
        );

        // Set tilte of popup
		gtk_window_set_title(GTK_WINDOW(dialog), _("Save?"));

        // Run dialog and get return value
		ret = gtk_dialog_run(GTK_DIALOG(dialog));

        // Destroy dialog after we are done using it
		gtk_widget_destroy(dialog);      

        // Save file and then close file
        if (ret != GTK_RESPONSE_NO )
        {
            return write_file(editor, file);
        }

        // User did not want to save, close file
        return TRUE;
    }
    
    return TRUE;
}

void new_item_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    if (editor->new_file_index >= MAX_OPEN_TABS) {
        info_message(editor, "Maximum of opened files reached, you can only have 50 files open!");
        return;
    }
    
    new_file_in_tab(editor);
}

/**
 * Allow user to select multiple files to be opened
 * */
static GSList *get_filenames(TextEditor *editor, gchar *title, gint action, gchar *ok)
{
	GtkWidget *chooser;
    GSList *filenames = NULL;

    // Create file chooser
	chooser = gtk_file_chooser_dialog_new(
        title,
        GTK_WINDOW(editor->window),
        action,
        _("_Cancel"),
        GTK_RESPONSE_CANCEL,
        ok,
        GTK_RESPONSE_OK,
        NULL
    );

    // Allow multiple files select
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(chooser), TRUE);
    
    // Open dialog, when files were choosen, get them
    if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_OK) {
		filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(chooser));
	}

    // Destroy widget
	gtk_widget_destroy(chooser);
	//gtk_widget_grab_focus(editor->scrollw);
	
    // Return filenames
    return filenames;
}

void open_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{
    if (editor->new_file_index >= MAX_OPEN_TABS)
    {
        info_message(editor, "Maximum of opened files reached, you can only have 50 files open!");
        return;
    }
    // Get all files user wants to open
    GSList *filenames = get_filenames(editor, _("Open File..."), GTK_FILE_CHOOSER_ACTION_OPEN, _("_Open"));

    // Loop through them and open each one
    GSList *next = filenames;
    while (next != NULL)
    {
        // Open file
        open_file_in_tab(editor, g_path_get_basename((gchar *)next->data), (gchar *)next->data);
        
        // Move to next file
        next = next->next;
    }
}

void save_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{
    // Get selected child
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File *file = get_file(editor, selected_child);

    // No file to be saved
    if (file == NULL)
    {
        return;
    }

    // When buffer has not changed ingore
    if (!gtk_text_buffer_get_modified(file->buffer))
    {
        return;
    }
    
    // Write file
    write_file(editor, file);
}

static void save_as_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{
    // Get selected child
    GtkWidget *selected_child = gtk_stack_get_visible_child(GTK_STACK(editor->stack));
    TextEditor_File *file = get_file(editor, selected_child);

    if (file == NULL)
    {
        return;
    }

    // Set to false, so write_file, will request new filepath    
    file->file_saved = FALSE;

    // Write file
    write_file(editor, file);
}

void print_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{
    static GtkPrintSettings *settings = NULL;

    GtkPrintOperation *print;
    GtkPrintOperationResult res;

    print = gtk_print_operation_new();

    if (settings != NULL) {
        gtk_print_operation_set_print_settings(print, settings);
    }

    res = gtk_print_operation_run(
        print,
        GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
        GTK_WINDOW(editor->window),
        NULL
    );

	if (res == GTK_PRINT_OPERATION_RESULT_APPLY) {
		if (settings != NULL) {
            g_object_unref(settings);
        }

		settings = g_object_ref(gtk_print_operation_get_print_settings(print));
	}

	g_object_unref(print);
}

void close_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{

    // Error while saving, or user declined to save file
    if (!check_for_save(editor))
    {
        return;
    }

    // Close file after it was saved
    close_open_file_in_tab(editor);
}

static void quit_item_cb(GtkMenuItem *menuitem, TextEditor *editor)
{
    // Loop through every file, and check if it needs saving
    for(int i = 0; i < MAX_OPEN_TABS; i++) {
        // All files are saved, save to quit
        if (editor->files[i] == NULL)
        {
            break;
        }

        // Buffer has been modified
        if (gtk_text_buffer_get_modified(editor->files[i]->buffer))
        {   
            // Change to modified file
            gtk_stack_set_visible_child(GTK_STACK(editor->stack), editor->files[i]->vbox);
        
            // Check for save, failed, stop QUIT PROGRAM
            if (check_for_save(editor) == FALSE)
            {
                return;
            }
        }
    }

    gtk_main_quit();
}

void create_file_menu(TextEditor *editor) {
    // Create new menu item
    GtkWidget *file = gtk_menu_item_new_with_mnemonic(_("_File"));
    gtk_menu_shell_append(GTK_MENU_SHELL(editor->menubar), file);
    
    // Create menu item
    GtkWidget *filemenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);

    // Add file items
    GtkWidget *new = create_icon_item(_("_New"), "document-new", editor->accel_group, GDK_KEY_n, GDK_CONTROL_MASK);
    GtkWidget *open = create_icon_item(_("_Open"), "document-open", editor->accel_group, GDK_KEY_o, GDK_CONTROL_MASK);
    GtkWidget *save = create_icon_item(_("_Save"), "document-save", editor->accel_group, GDK_KEY_s, GDK_CONTROL_MASK);
    GtkWidget *save_as = create_icon_item(_("_SaveAs"), "document-save-as", editor->accel_group, GDK_KEY_s, GDK_CONTROL_MASK | GDK_SHIFT_MASK);
    GtkWidget *print = create_icon_item(_("_Print"), "document-print", editor->accel_group, GDK_KEY_p, GDK_CONTROL_MASK);
    GtkWidget *close = create_icon_item(_("_Close"), "window-close", editor->accel_group, GDK_KEY_w, GDK_CONTROL_MASK);
    GtkWidget *quit = create_icon_item(_("_Quit"), "application-exit", editor->accel_group, GDK_KEY_q, GDK_CONTROL_MASK);

    editor->menubar_struct.file_save = save;
    editor->menubar_struct.file_save_as = save_as;
    editor->menubar_struct.file_print = print;

    // Disable save, save_as and print
    gtk_widget_set_sensitive(GTK_WIDGET(save), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(save_as), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(print), FALSE);

    // Append items to menu
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save_as);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), print);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), close);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);

    // Add signals
	g_signal_connect(G_OBJECT(new), "activate", G_CALLBACK(new_item_cb), editor);
	g_signal_connect(G_OBJECT(open), "activate", G_CALLBACK(open_item_cb), editor);
	g_signal_connect(G_OBJECT(save), "activate", G_CALLBACK(save_item_cb), editor);
	g_signal_connect(G_OBJECT(save_as), "activate", G_CALLBACK(save_as_item_cb), editor);
	g_signal_connect(G_OBJECT(print), "activate", G_CALLBACK(print_item_cb), editor);
	g_signal_connect(G_OBJECT(close), "activate", G_CALLBACK(close_item_cb), editor);
	g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(quit_item_cb), editor);
}