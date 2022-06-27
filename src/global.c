#include "global.h"

void info_message(TextEditor *editor, const gchar *message) {
	GtkWidget *dialog;

	// Create info message dialog
	dialog = gtk_message_dialog_new(
		GTK_WINDOW(editor->window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"%s", message
	);

	gtk_window_set_title(GTK_WINDOW(dialog), _("Info"));
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void error_message(TextEditor *editor, const gchar *message) {
	GtkWidget *dialog;
	
	/* create an error message dialog and display modally */
	dialog = gtk_message_dialog_new(
		GTK_WINDOW(editor->window), 
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		"%s", message
	);

	gtk_window_set_title(GTK_WINDOW(dialog), _("Error!"));
	gtk_dialog_run(GTK_DIALOG(dialog));      
	gtk_widget_destroy(dialog);
}