static void add_widget_to_warning_area(GtkWidget *widget)
{
    g_warning_issued = true;
    gtk_box_pack_start(g_box_warning_labels, widget, false, false, 0);
    gtk_widget_show_all(widget);
}
