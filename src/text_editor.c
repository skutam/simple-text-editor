#include "text_editor.h"

static GtkTargetEntry targetentries[] =
{
    { "text/uri-list", 0, 0}
};

static void destroy_signal(GtkWidget *widget, gpointer data, TextEditor *editor)
{
    // Quit application
	gtk_main_quit();
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, TextEditor *editor)
{
    // Check if each file is saved before ending
    close_all_cb(NULL, editor);
	return FALSE;   	/* send destroy signal */
}

void on_drag_data_received (GtkWidget *wgt, GdkDragContext *context, gint x, gint y, GtkSelectionData *seldata, guint info, guint time, TextEditor *editor) {
    gchar **filenames = NULL;

    filenames = g_uri_list_extract_uris((const gchar *) gtk_selection_data_get_data (seldata));
    
    if (filenames == NULL) // If unable to retrieve filenames:
    {
        gtk_drag_finish(context, FALSE, FALSE, time); // Drag and drop was a failure.
        return;
    }

    int iter = 0;
    while(filenames[iter] != NULL) // The last URI list element is NULL.
    {
        char *filepath = g_filename_from_uri (filenames[iter++], NULL, NULL); 

        // Invalid filepath
        if (filepath == NULL) {
            continue;
        }

        // Invalid file
        if (!g_file_test(filepath, G_FILE_TEST_IS_REGULAR | G_FILE_TEST_EXISTS))
        {
            continue;
        }
        
        // File is directory, ignore
        if (g_file_test(filepath, G_FILE_TEST_IS_DIR))
        {
            gchar *msg = g_strdup_printf(_("Cannot open directory %s."), filepath);
            info_message(editor, msg);
            g_free(msg);
            continue;
        }

        // Open files
        open_file_in_tab(editor, g_path_get_basename(filepath), filepath);
    }
    gtk_drag_finish(context, TRUE, FALSE, time); // Drag and drop was successful!
}

static void create_main_window(TextEditor *editor) {
    GdkRectangle workarea = {0};

    // Get primary monitor size   
    gdk_monitor_get_workarea(
        gdk_display_get_primary_monitor(gdk_display_get_default()),
        &workarea);

    // Create window
    editor->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Set name of application
    gtk_window_set_title(GTK_WINDOW(editor->window), _(PROGRAM_NAME));
    
    /**
     * Set size as 60% of width and 60% of height
     * */
    gtk_window_set_default_size(
        GTK_WINDOW(editor->window),
        (int)(workarea.width * INIT_WINDOW_SIZE),
        (int)(workarea.height * INIT_WINDOW_SIZE)
    );
    
    // Center application
    gtk_window_set_position(GTK_WINDOW(editor->window), GTK_WIN_POS_CENTER);

    // Set minimal width and height of application
    // Based on VSCodium
    gtk_widget_set_size_request(GTK_WIDGET(editor->window), MINIMAL_WIDTH, MINIMAL_HEIGHT);

    // Add signals
    g_signal_connect(
        G_OBJECT(editor->window),
        "destroy",
        G_CALLBACK(destroy_signal),
        NULL
    );
    g_signal_connect(
        G_OBJECT(editor->window),
        "delete_event",
        G_CALLBACK(delete_event),
        editor
    );

    gtk_drag_dest_set(
        GTK_WIDGET(editor->window),
        GTK_DEST_DEFAULT_ALL,
        targetentries,
        1,
        GDK_ACTION_COPY
    );

    g_signal_connect(
        G_OBJECT(editor->window),
        "drag-data-received",
        G_CALLBACK (on_drag_data_received),
        editor
    );
}

void init_app(TextEditor *editor) {
    // Set default value
    editor->new_file_index = 0;

    // Set all file pointers to NULL
    for (int i = 0; i < MAX_OPEN_TABS; i++) {
        editor->files[i] = NULL;
    }

    // Create main window
    create_main_window(editor);

    // Create main container
    #if GTK_MAJOR_VERSION >= 3
        editor->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    #else
        editor->vbox = gtk_vbox_new(FALSE, 0);
    #endif

    // Add created vbox to window
    gtk_container_add(GTK_CONTAINER(editor->window), editor->vbox);

    // Create accel group and add to window
    editor->accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(editor->window), editor->accel_group);

    // Create and add menubar
    create_menubar(editor);

    // Create and add toolbar
    create_toolbar(editor);

    // Create filetabs
    create_filetabs(editor);

    // Create statusbar
    create_statusbar(editor);
}