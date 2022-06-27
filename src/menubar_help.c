#include "menubar_help.h"
#include "menubar_create_item.h"

static void about_cb(GtkMenuItem *menuitem, TextEditor *editor) {
    static const gchar * const authors[] = {
		"Matúš Škuta <xskuta04@stud.fit.vutbr.cz>",
		NULL
	};

    // Show about dialog
    gtk_show_about_dialog (GTK_WINDOW (editor->window),
		"authors", authors,
		"comments", "Simple text editor",
		"copyright", "Copyright \xc2\xa9 2021 Matúš Škuta",
		"version", "1.0",
		"website", "http://www.fit.vutbr.cz",
		"program-name", "Simple Text Editor",
		"logo-icon-name", "document-open",
		NULL
    ); 
}

void create_help_menu(TextEditor *editor) {
    // Create help menu item
    GtkWidget *help = gtk_menu_item_new_with_mnemonic(_("_Help"));
    gtk_menu_shell_append(GTK_MENU_SHELL(editor->menubar), help);
    
    // Create menu item
    GtkWidget *helpmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);

    // Add help items
    GtkWidget *about = create_icon_item(_("_About"), "help-about", NULL, 0, 0);

    // Append items to menu
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);

    // Add onclick signals
	g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(about_cb), editor);
}