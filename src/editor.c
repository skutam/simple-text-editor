#include <locale.h>
#include "global.h"
#include "text_editor.h"

int main(int argc, char *argv[]) {
    TextEditor *editor;
    
    setlocale(LC_ALL, "");
    bindtextdomain("editor", ".");
	textdomain("editor");

    // Allocate memory for TextEditor struct
    editor = g_slice_new(TextEditor);

    // Initialize GTK+ libraries
    gtk_init(&argc, &argv);

    // Create widgets
    init_app(editor);

    // Show the window
    gtk_widget_show_all(editor->window);

    // Enter main loop
    gtk_main();

    // Free memory
    g_slice_free(TextEditor, editor);
    return 0;
}
