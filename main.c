#include <gtk/gtk.h>
#include <stdlib.h>
#define GB_STRING_IMPLEMENTATION
#include "gb_string.h"

gchar** getEmulatorAVDS();

struct MainWndObjects {
    GtkWidget       *window;
    GtkTreeView     *listbox;
    GtkWidget       *statusbar;
    GtkListStore* listboxStore;
};

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GError *error = NULL;
    struct MainWndObjects* wndObj = g_malloc(sizeof(struct MainWndObjects));
    GtkBuilder* builder = gtk_builder_new();
    if (!gtk_builder_add_from_file (builder, "ui.glade", &error))
    {
        g_warning( "%s\n", error->message );
        g_free( error );
        return ( 1 );
    }

    wndObj->window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWnd"));
    wndObj->listbox = GTK_TREE_VIEW(gtk_builder_get_object(builder, "listbox"));
    wndObj->statusbar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));
    wndObj->listboxStore = GTK_LIST_STORE(gtk_builder_get_object(builder, "listboxstore"));

    gchar** avds = getEmulatorAVDS();
    if(avds) {
        GtkTreeIter list_iterator;
        for(int i=0; avds[i] && avds[i][0]; ++i) {
            //GtkWidget *avdsLabel = gtk_label_new(avds[i]);
            //GtkListBoxRow *avdsLabel = gtk_list_box_row_new();
            //gtk_list_box_row_se
            //gtk_container_add(GTK_CONTAINER(listbox), avdsLabel);
            //gtk_list_box_insert(GTK_LIST_BOX(listbox), avdsLabel, -1);

            gtk_list_store_append(GTK_LIST_STORE(wndObj->listboxStore), &list_iterator);
            gtk_list_store_set(GTK_LIST_STORE(wndObj->listboxStore), &list_iterator,
                               0, avds[i],
                               -1);

        }

        //gtk_tree_view_set_model(GTK_TREE_VIEW(listbox), listboxStore);
        g_strfreev(avds);

        gtk_statusbar_push(GTK_STATUSBAR(wndObj->statusbar), 0, "Ready");
    }

    gtk_builder_connect_signals(GTK_BUILDER(builder), wndObj);
    g_object_unref(G_OBJECT(builder));

    gtk_widget_show_all(GTK_WIDGET(wndObj->window));
    gtk_main();

    g_free(wndObj);

    return 0;
}

// called when window is closed
void on_mainWnd_destroy()
{
    gtk_main_quit();
}

void on_mainWnd_show() {

}

gchar** getEmulatorAVDS() {
    const gchar* cmdLine = "emulator -list-avds";
    gchar* eOut;
    gboolean r = g_spawn_command_line_sync(cmdLine, &eOut, NULL, NULL, NULL);
    if (r) {
        gchar** lines = g_strsplit(eOut, "\n", -1);
        g_free(eOut);
        return lines;
        //if(lines) {
            //g_print("line:%s", lines[0]);
            //g_strfreev(lines);
        //}
    }
}

void on_runBtn_clicked(GtkWidget *button, gpointer data) {
    struct MainWndObjects* wndObj = (struct MainWndObjects*)data;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char* homeDir;

    selection = gtk_tree_view_get_selection ( wndObj->listbox );
    model = gtk_tree_view_get_model ( GTK_TREE_VIEW(wndObj->listbox) );
    gboolean s = gtk_tree_selection_get_selected (selection, &model, &iter);

    if (s) {
        gchar* name;
        gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &name, -1);
        if (name) {
            switch( fork() )
            {
                case -1 : // Error
                    g_print("Error while fork");
                    break;

                case 0 :
                    homeDir = getenv("HOME");
                    gbString workDir = gb_make_string(homeDir ? homeDir : "");
                    workDir = gb_append_cstring(workDir, "/Android/Sdk/tools");
                    g_print("chdir to %s\n", workDir);
                    chdir(workDir);
                    gb_free_string(workDir);
                    signal (SIGHUP, SIG_IGN);
                    execlp("emulator", "emulator", "-avd", name, NULL);
                    exit(0); // May never be returned
                default :

                    // Do what you want
                    break;
            }
        }
    }
}