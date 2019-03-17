/*
    roxterm - VTE/GTK terminal emulator with tabs
    Copyright (C) 2019 Tony Houghton <h@realh.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "roxterm-application.h"
#include "roxterm-header-bar.h"
#include "roxterm-window.h"

struct _RoxtermWindow {
    GtkApplicationWindow parent_instance;
    RoxtermLaunchParams *lp;
    RoxtermWindowLaunchParams *wp;
};

G_DEFINE_TYPE(RoxtermWindow, roxterm_window, MULTITEXT_TYPE_WINDOW);

static void roxterm_window_dispose(GObject *obj)
{
    RoxtermWindow *self = ROXTERM_WINDOW(obj);
    if (self->wp)
    {
        roxterm_window_launch_params_unref(self->wp);
        self->wp = NULL;
    }
    if (self->lp)
    {
        roxterm_launch_params_unref(self->lp);
        self->lp = NULL;
    }
}

static void roxterm_window_class_init(UNUSED RoxtermWindowClass *klass)
{
}

static void roxterm_window_init(RoxtermWindow *self)
{
    GtkWindow *gwin = GTK_WINDOW(self);
    RoxtermHeaderBar *header = roxterm_header_bar_new();
    gtk_window_set_titlebar(gwin, GTK_WIDGET(header));
}

RoxtermWindow *roxterm_window_new(RoxtermApplication *app)
{
    GObject *obj = g_object_new(ROXTERM_TYPE_WINDOW,
            "application", app,
            NULL);
    RoxtermWindow *self = ROXTERM_WINDOW(obj);
    return self;
}

void roxterm_window_apply_launch_params(RoxtermWindow *self,
        RoxtermLaunchParams *lp, RoxtermWindowLaunchParams *wp)
{
    self->lp = lp ? roxterm_launch_params_ref(lp) : NULL;
    self->wp = wp ? roxterm_window_launch_params_ref(wp) : NULL;
}

static void roxterm_window_spawn_callback(VteTerminal *vte,
        GPid pid, GError *error, gpointer handle)
{
    //RoxtermWindow *self = handle;
    MultitextWindow *mwin = handle;
    if (pid == -1)
    {
        // TODO: GUI dialog
        g_critical("Child command failed to run: %s", error->message);
        // error is implicitly transfer-none according to vte's gir file
        gtk_container_remove(
                GTK_CONTAINER(multitext_window_get_notebook(mwin)),
                GTK_WIDGET(vte));
        // TODO: Verify that this also destroys the tab's label
    }
}

void roxterm_window_diagnose_size(MultitextGeometryProvider *gp)
{
    gtk_widget_show_all(GTK_WIDGET(gp));
    int columns, rows;
    int cell_width, cell_height;
    int target_width, target_height;
    int current_width, current_height;
    multitext_geometry_provider_get_initial_size(gp, &columns, &rows);
    multitext_geometry_provider_get_cell_size(gp,
            &cell_width, &cell_height);
    target_width = cell_width * columns;
    target_height = cell_height * rows;
    g_debug("GP wants %dx%d cells: %dx%d px",
            columns, rows, target_width, target_height);
    multitext_geometry_provider_get_current_size(gp, &columns, &rows);
    current_width = cell_width * columns;
    current_height = cell_height * rows;
    g_debug("GP's current size is %dx%d cells: %dx%d px",
            columns, rows, current_width, current_height);
    GtkWidget *gpw = GTK_WIDGET(gp);
    int min_w, nat_w, min_h, nat_h;
    gtk_widget_get_preferred_width(gpw, &min_w, &nat_w);
    gtk_widget_get_preferred_height(gpw, &min_h, &nat_h);
    g_debug("Min size %dx%d px natural %dx%d px", min_w, min_h, nat_w, nat_h);
}

RoxtermVte *roxterm_window_new_tab(RoxtermWindow *self,
        RoxtermTabLaunchParams *tp, int index)
{
    RoxtermVte *rvt = roxterm_vte_new();
    roxterm_vte_apply_launch_params(rvt, self->lp, self->wp, tp);
    //roxterm_window_diagnose_size(MULTITEXT_GEOMETRY_PROVIDER(rvt));
    GtkScrollable *scrollable = GTK_SCROLLABLE(rvt);
    GtkWidget *vpw = gtk_scrolled_window_new(
            gtk_scrollable_get_hadjustment(scrollable), 
            gtk_scrollable_get_vadjustment(scrollable));
    GtkScrolledWindow *viewport = GTK_SCROLLED_WINDOW(vpw);
    gtk_scrolled_window_set_policy(viewport, GTK_POLICY_NEVER,
            GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_overlay_scrolling(viewport, TRUE);
    gtk_container_add(GTK_CONTAINER(vpw), GTK_WIDGET(rvt));
    MultitextWindow *mwin = MULTITEXT_WINDOW(self);
    GtkNotebook *gnb = GTK_NOTEBOOK(multitext_window_get_notebook(mwin));
    gtk_notebook_insert_page(gnb, vpw, NULL, index);
    multitext_window_set_geometry_provider(MULTITEXT_WINDOW(self),
            MULTITEXT_GEOMETRY_PROVIDER(rvt));
    roxterm_vte_spawn(rvt, roxterm_window_spawn_callback, self);
    return rvt;
}
