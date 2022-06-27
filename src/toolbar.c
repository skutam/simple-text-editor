#include "toolbar.h"

/**
 * Generate toolbar item
 * */
static GtkToolItem *create_new_tool_item(
    TextEditor *editor,
    const gchar *icon_name,
    const gchar *btn_text,
    GtkWidget **btn,
    GtkWidget **toolbar_struct_item,
    gboolean is_sensitive
) {
    GtkToolItem *tool_item = gtk_tool_item_new();

    (*btn) = gtk_button_new_from_icon_name(icon_name, GTK_ICON_SIZE_BUTTON);
    if (btn_text != NULL)
    {
        gtk_button_set_label(GTK_BUTTON((*btn)), btn_text);
    }
    
    if (toolbar_struct_item != NULL)
    {
        (*toolbar_struct_item) = (*btn);
    }

    gtk_widget_set_sensitive(GTK_WIDGET(*btn), is_sensitive);
    
    gtk_container_add(GTK_CONTAINER(tool_item), (*btn));
    return tool_item;
}

void create_toolbar(TextEditor *editor) {
    // Create toolbar
    editor->toolbar = gtk_toolbar_new();

    // Set style to icon
    gtk_toolbar_set_style(GTK_TOOLBAR(editor->toolbar), GTK_TOOLBAR_ICONS);

    GtkWidget *btn_new;
    GtkWidget *btn_open;
    GtkWidget *btn_close;
    GtkWidget *btn_save;
    GtkWidget *btn_print;
    GtkWidget *btn_cut;
    GtkWidget *btn_copy;
    GtkWidget *btn_paste;
    GtkWidget *btn_search;

    // Create new toolitems
    GtkToolItem *new = create_new_tool_item(
        editor,
        "document-new",
        NULL,
        &btn_new,
        NULL,
        TRUE
    );
    GtkToolItem *open = create_new_tool_item(
        editor,
        "document-open",
        _("Open"),
        &btn_open,
        NULL,
        TRUE
    );
    GtkToolItem *close = create_new_tool_item(
        editor,
        "window-close",
        _("Close"),
        &btn_close,
        &(editor->toolbar_struct.close),
        FALSE
    );
    GtkToolItem *save = create_new_tool_item(
        editor,
        "document-save",
        NULL,
        &btn_save,
        &(editor->toolbar_struct.save),
        FALSE
    );
    GtkToolItem *print = create_new_tool_item(
        editor,
        "document-print",
        NULL,
        &btn_print,
        &(editor->toolbar_struct.print),
        FALSE
    );
    GtkToolItem *cut = create_new_tool_item(
        editor,
        "edit-cut",
        NULL,
        &btn_cut,
        &(editor->toolbar_struct.cut),
        FALSE
    );
    GtkToolItem *copy = create_new_tool_item(
        editor,
        "edit-copy",
        NULL,
        &btn_copy,
        &(editor->toolbar_struct.copy),
        FALSE
    );
    GtkToolItem *paste = create_new_tool_item(
        editor,
        "edit-paste",
        NULL,
        &btn_paste,
        &(editor->toolbar_struct.paste),
        FALSE
    );
    GtkToolItem *search = create_new_tool_item(
        editor,
        "edit-find",
        NULL,
        &btn_search,
        &(editor->toolbar_struct.search),
        FALSE
    );

    // Add signals
    g_signal_connect(G_OBJECT(btn_new), "clicked", G_CALLBACK(new_item_cb), editor);
    g_signal_connect(G_OBJECT(btn_open), "clicked", G_CALLBACK(open_item_cb), editor);
    g_signal_connect(G_OBJECT(btn_close), "clicked", G_CALLBACK(close_item_cb), editor);
    g_signal_connect(G_OBJECT(btn_save), "clicked", G_CALLBACK(save_item_cb), editor);
    g_signal_connect(G_OBJECT(btn_print), "clicked", G_CALLBACK(print_item_cb), editor);
    g_signal_connect(G_OBJECT(btn_cut), "clicked", G_CALLBACK(cut_cb), editor);
    g_signal_connect(G_OBJECT(btn_copy), "clicked", G_CALLBACK(copy_cb), editor);
    g_signal_connect(G_OBJECT(btn_paste), "clicked", G_CALLBACK(paste_cb), editor);
    g_signal_connect(G_OBJECT(btn_search), "clicked", G_CALLBACK(find_cb), editor);

    // Add tool items
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), new, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), open, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), close, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), save, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), gtk_separator_tool_item_new(), -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), print, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), gtk_separator_tool_item_new(), -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), cut, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), copy, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), paste, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), gtk_separator_tool_item_new(), -1);
    gtk_toolbar_insert(GTK_TOOLBAR(editor->toolbar), search, -1);

    // Add to VBOX
    gtk_box_pack_start(GTK_BOX(editor->vbox), editor->toolbar, FALSE, FALSE, 0);
}