/*
 * gEDA - GNU Electronic Design Automation
 * This file is a part of gerbv.
 *
 *   Copyright (C) 2000-2003 Stefan Petersen (spe@stacken.kth.se)
 *
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.s
 *
 * This program is distributed in the hope that it toowill be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */

/** \file interface.c
    \brief GUI building functions for Gerbv
    \ingroup gerbv
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gimpruler.h"

#include "gerbv.h"
#include "main.h"
#include "callbacks.h"
#include "interface.h"
#include "render.h"

#include "draw.h"

#include "gerbv_icon.h"
#include "icons.h"
extern gerbv_render_info_t screenRenderInfo;

#define WIN_TITLE "Gerber Viewer"
#define _(String) (String)

#define dprintf if(DEBUG) printf

/* ---------------------------------------------- */
void
rename_main_window(char const* filename, GtkWidget *main_win)
{
	GString *win_title=g_string_new(NULL);
	static GtkWidget *win=NULL;

	if( main_win != NULL )
	win = main_win;

	g_assert(win != NULL);

	if (strlen(filename)) {
		g_string_printf (win_title,"%s - %s", filename, WIN_TITLE);
	}
	else {
		g_string_printf (win_title,"%s", WIN_TITLE);
	}
	gtk_window_set_title(GTK_WINDOW(win), win_title->str);
	g_string_free(win_title,TRUE);			 
}

/* ---------------------------------------------- */
void
set_window_icon (GtkWidget * this_window)
{
	GdkPixbuf *icon;

	icon = gdk_pixbuf_new_from_xpm_data((const char **) gerbv_icon_xpm);
	gtk_window_set_icon (GTK_WINDOW(this_window), icon);
}

/* ---------------------------------------------- */
void
interface_load_accels (void)
{	
	gchar *accel_map_filename = g_build_filename (g_get_home_dir(), GERBV_ACCELS_RELPATH, NULL);
	if (accel_map_filename) {
		gtk_accel_map_load (accel_map_filename);
		g_free (accel_map_filename);
	}
}

/* ---------------------------------------------- */
void
interface_save_accels (void)
{
	gchar *accel_map_filename = g_build_filename (g_get_home_dir(), GERBV_ACCELS_RELPATH, NULL);
	if (accel_map_filename) {
		gtk_accel_map_save (accel_map_filename);
		g_free (accel_map_filename);
	}
}

/* ---------------------------------------------- */
void
interface_create_gui (int req_width, int req_height)
{
	GtkStockItem stock;

	GtkWidget *mainWindow;
	GtkWidget *vbox1;
	GtkWidget *menubar1;

	GtkWidget *menuitem_file;
	GtkWidget *menuitem_file_menu;
	GtkWidget *new;
	GtkWidget *open_project;
	GtkWidget *image33;
	GtkWidget *open_layer;
	GtkWidget *revert;
	GtkWidget *save;
	GtkWidget *save_as;
	GtkWidget *save_layer;
	GtkWidget *save_as_layer;
	GtkWidget *separatormenuitem1;
	GtkWidget *menuitem_file_export;
	GtkWidget *menuitem_file_export_menu;
	GtkWidget *png;
	GtkWidget *pdf;
	GtkWidget *svg;
	GtkWidget *postscript;
	GtkWidget *rs274x,*drill,*rs274xm,*drillm;
	
	GtkWidget *separator1;
	GtkWidget *print;
	GtkWidget *separator2;
	GtkWidget *quit;

	GtkWidget *menuitem_edit;
	GtkWidget *menuitem_edit_menu;
	GtkWidget *properties_selected;
	GtkWidget *delete_selected;
	GtkWidget *menuitem_view;
	GtkWidget *menuitem_view_menu;
	GtkWidget *view_fullscreen;
	GtkWidget *show_toolbar;
	GtkWidget *show_sidepane;
	GtkWidget *separator3;
	GtkWidget *toggle_layer_visibility_item1;
	GtkWidget *toggle_layer_visibility_item2;
	GtkWidget *toggle_layer_visibility_item3;
	GtkWidget *toggle_layer_visibility_item4;
	GtkWidget *toggle_layer_visibility_item5;
	GtkWidget *toggle_layer_visibility_item6;
	GtkWidget *toggle_layer_visibility_item7;
	GtkWidget *toggle_layer_visibility_item8;
	GtkWidget *toggle_layer_visibility_item9;
	GtkWidget *toggle_layer_visibility_item10;
	GtkWidget *separator4;
	GtkWidget *zoom_in;
	GtkWidget *zoom_out;
	GtkWidget *fit_to_window;
	GtkWidget *separator5;
	GtkWidget *backgroundColor;
	GtkWidget *menuitem_view_render, *menuitem_view_render_menu;
	GSList *menu_view_render_group;
	GtkWidget *render_normal, *render_hq;
	GtkWidget *menuitem_view_unit, *menuitem_view_unit_menu;
	GSList *menu_view_unit_group;
	GtkWidget *unit_mil, *unit_mm, *unit_in;
	GtkWidget *menuitem_layer;
	GtkWidget *menuitem_layer_menu;
	GtkWidget *layer_visibility;
	GtkWidget *layer_invert;
	GtkWidget *layer_color;
	GtkWidget *layer_reload;
	GtkWidget *layer_orientation;
	GtkWidget *layer_format;
	GtkWidget *layer_up;
	GtkWidget *layer_down;
	GtkWidget *layer_remove;
	GtkWidget *menuitem_analyze;
	GtkWidget *menuitem_analyze_menu;
	GtkWidget *analyze_active_gerbers;
	GtkWidget *analyze_active_drill;
	GtkWidget *analyze_benchmark;
	/*GtkWidget *control_gerber_options;*/
	GtkWidget *menuitem_tools;
	GtkWidget *menuitem_tools_menu;
	GtkWidget *toggletoolbutton_pointer;
	GtkWidget *pointer_tool;
	GdkPixbuf *pointerpixbuf;
	GtkWidget *pointerimage;
	GtkWidget *pan_tool;
	GtkWidget *zoom_tool;
	GtkWidget *measure_tool;
	GtkWidget *menuitem_help;
	GtkWidget *layer_visibility_menu;
	GtkWidget *layer_visibility_main_menu;
	GtkWidget *menuitem_help_menu;
	/*GtkWidget *online_manual;*/
	GtkWidget *about;
	GtkWidget *bugs_menuitem;
	GtkWidget *image34;
	GtkWidget *toolbar_hbox;
	GtkWidget *button_toolbar;
	/*GtkIconSize tmp_toolbar_icon_size;*/
	GtkWidget *toolbutton_new;
	GtkWidget *toolbutton_open;
	GtkWidget *toolbutton_revert;
	GtkWidget *toolbutton_save;
	GtkWidget *separatortoolitem1;
	GtkWidget *toolbutton_print;
	GtkWidget *separatortoolitem2;
	GtkWidget *toolbutton_zoom_in;
	GtkWidget *toolbutton_zoom_out;
	GtkWidget *toolbutton_zoom_fit;
	/* Implement these tool buttons later when we have icons */
/*	GtkWidget *separatortoolitem3; */
/*	GtkWidget *toolbutton_analyze; */
/*	GtkWidget *toolbutton_validate;*/
/*	GtkWidget *toolbutton_control; */
	GtkWidget *separatortoolitem4;
	GtkWidget *toggletoolbutton_pan;
	GtkWidget *toggletoolbutton_zoom;
	GtkWidget *toggletoolbutton_measure;
	GtkWidget *hpaned1;
	GtkWidget *sidepane_vbox;
	GtkWidget *sidepane_notebook;
	GtkWidget *vbox10;
	GtkWidget *hbox4;
	GtkWidget *label1;
	GtkWidget *render_combobox;
	GtkWidget *scrolledwindow1;
	GtkWidget *hbox1;
	GtkWidget *button4;
	GtkWidget *image8;
	GtkWidget *button5;
	GtkWidget *image9;
	GtkWidget *button6;
	GtkWidget *image10;
	GtkWidget *button7;
	GtkWidget *image11;
	GtkWidget *Layer_label;
	GtkWidget *vbox11;
	GtkWidget *messages_scrolledwindow;
	GtkWidget *message_textview;
	GtkWidget *clear_messages_button;
	GtkWidget *Message_label;
	GtkWidget *vbox2;
	GtkWidget *main_view_table;
	GtkWidget *hRuler;
	GtkWidget *vRuler;
	GtkWidget *hbox5;
	GtkWidget *statusbar_label_left;
	GtkWidget *statusUnitComboBox;
	GtkWidget *statusbar_label_right;
	GtkWidget *drawingarea, *hAdjustment, *vAdjustment, *hScrollbar, *vScrollbar;
	
	GtkAccelGroup *accel_group;

	/* Inline icons */
	GdkPixbuf *zoompixbuf;
	GtkWidget *zoomimage;
	GdkPixbuf *measurepixbuf;
	GtkWidget *measureimage;
	GdkPixbuf *movepixbuf;
	GtkWidget *moveimage;

	GtkWidget *tempImage;

	/* Initialize Gtk3 CSS for our GIMP ruler. This should be in a CSS
	 * file, but who cares */
	{
		GtkCssProvider *provider;
		GdkDisplay *display;
		GdkScreen *screen;

		provider = gtk_css_provider_new ();
		display = gdk_display_get_default ();
		screen = gdk_display_get_default_screen (display);
		gtk_style_context_add_provider_for_screen (screen,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_USER);

		gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider),
			"GimpRuler { background-color: @bg_color; }",
			-1, NULL);
		g_object_unref (provider);
	}

	pointerpixbuf = gdk_pixbuf_new_from_inline(-1, pointer, FALSE, NULL);
	movepixbuf = gdk_pixbuf_new_from_inline(-1, move, FALSE, NULL);
	zoompixbuf = gdk_pixbuf_new_from_inline(-1, lzoom, FALSE, NULL);
	measurepixbuf = gdk_pixbuf_new_from_inline(-1, ruler, FALSE, NULL);

	accel_group = gtk_accel_group_new ();

	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (mainWindow), _(WIN_TITLE));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (mainWindow), vbox1);

	menubar1 = gtk_menu_bar_new ();
	gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

	/* --- File menu --- */
	menuitem_file = gtk_menu_item_new_with_mnemonic (_("_File"));
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_file);

	/* File menu items dealing with a gerbv project. */
	menuitem_file_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_file_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_file_menu), ACCEL_FILE);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_file), menuitem_file_menu);

	new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, NULL);
	SET_ACCELS_FROM_STOCK (GTK_STOCK_NEW, ACCEL_FILE_NEW);
	gtk_widget_set_tooltip_text (new, "Close all layers and start a new project");
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), new);

	open_project = gtk_image_menu_item_new_with_mnemonic (_("_Open project..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), open_project);
	gtk_widget_set_tooltip_text (open_project, "Open an existing Gerbv project");
	image33 = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (open_project), image33);

	save = gtk_image_menu_item_new_with_mnemonic (_("Save project"));
	screen.win.curFileMenuItem1 = save;
	gtk_widget_set_tooltip_text (save, "Save the current project");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (save), tempImage);
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), save);

	save_as = gtk_image_menu_item_new_with_mnemonic (_("Save project as..."));
	screen.win.curFileMenuItem2 = save_as;
	gtk_widget_set_tooltip_text (save_as, "Save the current project to a new file");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_SAVE_AS, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (save_as), tempImage);
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), save_as);
	
	revert = gtk_image_menu_item_new_from_stock (GTK_STOCK_REVERT_TO_SAVED, NULL);
	screen.win.curFileMenuItem3 = revert;
	SET_ACCELS_FROM_STOCK (GTK_STOCK_REVERT_TO_SAVED, ACCEL_FILE_REVERT);
	gtk_widget_set_tooltip_text (revert, "Reload all layers");
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), revert);

	/* File menu items dealing individual layers. */
	separator1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), separator1);
	gtk_widget_set_sensitive (separator1, FALSE);

	open_layer = gtk_menu_item_new_with_mnemonic (_("Open _layer(s)..."));
	gtk_accel_map_add_entry (ACCEL_FILE_OPEN_LAYER_PATH, ACCEL_FILE_OPEN_LAYER_KEY, ACCEL_FILE_OPEN_LAYER_MOD);
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), open_layer);
	gtk_widget_set_tooltip_text (open_layer, "Open Gerber, drill, or pick and place file(s)");

	save_layer = gtk_menu_item_new_with_mnemonic (_("_Save active layer"));
	screen.win.curFileMenuItem4 = save_layer;
	gtk_widget_set_tooltip_text (save_layer, "Save the active layer");
	gtk_accel_map_add_entry (ACCEL_FILE_SAVE_LAYER_PATH, ACCEL_FILE_SAVE_LAYER_KEY, ACCEL_FILE_SAVE_LAYER_MOD);
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), save_layer);
	
	save_as_layer = gtk_menu_item_new_with_mnemonic (_("Save active layer _as..."));
	screen.win.curFileMenuItem5 = save_as_layer;
	gtk_widget_set_tooltip_text (save_as_layer, "Save the active layer to a new file");
	gtk_accel_map_add_entry (ACCEL_FILE_SAVE_LAYER_AS_PATH, ACCEL_FILE_SAVE_LAYER_AS_KEY, ACCEL_FILE_SAVE_LAYER_AS_MOD);
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), save_as_layer);

	/* File menu items dealing with exporting different types of files. */
	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), separatormenuitem1);
	gtk_widget_set_sensitive (separatormenuitem1, FALSE);

	menuitem_file_export = gtk_menu_item_new_with_mnemonic (_("_Export"));
	screen.win.curFileMenuItem6 = menuitem_file_export;
	gtk_widget_set_tooltip_text (menuitem_file_export, "Export all visible layers to a new format");
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), menuitem_file_export);

	menuitem_file_export_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_file_export_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_file_export_menu), ACCEL_FILE_EXPORT);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_file_export), menuitem_file_export_menu);

	png = gtk_menu_item_new_with_mnemonic (_("P_NG..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), png);
	gtk_widget_set_tooltip_text (png, _("Export project to a PNG file..."));
	
	pdf = gtk_menu_item_new_with_mnemonic (_("P_DF..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), pdf);
	gtk_widget_set_tooltip_text (pdf, _("Export project to a PDF file..."));

	svg = gtk_menu_item_new_with_mnemonic (_("_SVG..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), svg);
	gtk_widget_set_tooltip_text (svg, _("Export project to a SVG file"));
	
	postscript = gtk_menu_item_new_with_mnemonic (_("_PostScript..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), postscript);
	gtk_widget_set_tooltip_text (postscript, _("Export project to a PostScript file"));

	rs274x = gtk_menu_item_new_with_mnemonic (_("RS-274X (_Gerber)..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), rs274x);
	gtk_widget_set_tooltip_text (rs274x, _("Export layer to a RS-274X (Gerber) file"));
	
	drill = gtk_menu_item_new_with_mnemonic (_("_Excellon drill..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), drill);
	gtk_widget_set_tooltip_text (drill, _("Export layer to an Excellon drill file"));
	
	rs274xm = gtk_menu_item_new_with_mnemonic (_("RS-274X Merge (Gerber)..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), rs274xm);
	gtk_widget_set_tooltip_text (rs274xm, _("Export (merge visible gerber layers) to a RS-274X (Gerber) file"));
	
	drillm = gtk_menu_item_new_with_mnemonic (_("Excellon drill Merge..."));
	gtk_container_add (GTK_CONTAINER (menuitem_file_export_menu), drillm);
	gtk_widget_set_tooltip_text (drillm, _("Export (merge visible drill layers) to an Excellon drill file"));	
	
	/* File menu items dealing with printing and quitting. */
	separator1 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), separator1);
	gtk_widget_set_sensitive (separator1, FALSE);

	if (gtk_stock_lookup(GTK_STOCK_PRINT, &stock)) {
	    gchar new[] = "_Print..."; 
	    stock.label = new;
	    gtk_stock_add(&stock, 1);
	}

	print = gtk_image_menu_item_new_from_stock (GTK_STOCK_PRINT, NULL);
	screen.win.curFileMenuItem7 = print;
	SET_ACCELS_FROM_STOCK (GTK_STOCK_PRINT, ACCEL_FILE_PRINT);
	gtk_widget_set_tooltip_text (print, "Print the visible layers");
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), print);

	separator2 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), separator2);
	gtk_widget_set_sensitive (separator2, FALSE);
	quit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
	SET_ACCELS_FROM_STOCK (GTK_STOCK_QUIT, ACCEL_FILE_QUIT);
	gtk_widget_set_tooltip_text (quit, "Quit Gerbv");
	gtk_container_add (GTK_CONTAINER (menuitem_file_menu), quit);

	/* --- Next menu item (Edit) --- */
	menuitem_edit = gtk_menu_item_new_with_mnemonic (_("_Edit"));
	screen.win.curEditMenuItem = menuitem_edit;
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_edit);

	menuitem_edit_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_edit_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_edit_menu), ACCEL_EDIT);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_edit), menuitem_edit_menu);

	properties_selected = gtk_image_menu_item_new_with_mnemonic (_("Display _properties of selected object(s)"));
	SET_ACCELS_FROM_STOCK (GTK_STOCK_PROPERTIES, ACCEL_EDIT_PROPERTIES);
	gtk_widget_set_tooltip_text (properties_selected, "Examine the properties of the selected object");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (properties_selected), tempImage);
	gtk_container_add (GTK_CONTAINER (menuitem_edit_menu), properties_selected);

	delete_selected = gtk_image_menu_item_new_with_mnemonic (_("_Delete selected object(s)"));
	SET_ACCELS_FROM_STOCK (GTK_STOCK_REMOVE, ACCEL_EDIT_DELETE);
	gtk_widget_set_tooltip_text (delete_selected, "Delete selected objects");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (delete_selected), tempImage);
	gtk_container_add (GTK_CONTAINER (menuitem_edit_menu), delete_selected);

	/*   Include these after they are coded.
	tempMenuItem = gtk_image_menu_item_new_with_label ("Edit object properties");
	gtk_menu_shell_append ((GtkMenuShell *)screen.win.drawWindowPopupMenu, tempMenuItem);
	gtk_widget_set_tooltip_text (tempMenuItem, "Edit the properties of the selected object");
	g_signal_connect ((gpointer) tempMenuItem, "activate",
	                  G_CALLBACK (callbacks_edit_object_properties_clicked), NULL);

	tempMenuItem = gtk_image_menu_item_new_with_label ("Move object(s)");
	gtk_menu_shell_append ((GtkMenuShell *)screen.win.drawWindowPopupMenu, tempMenuItem);
	gtk_widget_set_tooltip_text (tempMenuItem, "Move the selected object(s)");
	g_signal_connect ((gpointer) tempMenuItem, "activate",
	                  G_CALLBACK (callbacks_move_objects_clicked), NULL);

	tempMenuItem = gtk_image_menu_item_new_with_label ("Reduce area");
	gtk_menu_shell_append ((GtkMenuShell *)screen.win.drawWindowPopupMenu, tempMenuItem);
	gtk_widget_set_tooltip_text (tempMenuItem, "Reduce the area of the object (e.g. to prevent component floating)");
	g_signal_connect ((gpointer) tempMenuItem, "activate",
	                  G_CALLBACK (callbacks_reduce_object_area_clicked), NULL);
	*/

	/* Use the "Edit" menu as right click popup menu for the drawing area */
	screen.win.drawWindowPopupMenu = menuitem_edit_menu;

	/* --- Next menu item (View) --- */
	menuitem_view = gtk_menu_item_new_with_mnemonic (_("_View"));
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_view);

	menuitem_view_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_view_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_view_menu), ACCEL_VIEW);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_view), menuitem_view_menu);
	
	view_fullscreen = gtk_check_menu_item_new_with_mnemonic (_("Fullscr_een"));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_fullscreen), FALSE);
	gtk_widget_set_tooltip_text (view_fullscreen, "Toggle between fullscreen and normal view");
	SET_ACCELS (ACCEL_VIEW_FULLSCREEN);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), view_fullscreen);

	show_toolbar = gtk_check_menu_item_new_with_mnemonic (_("Show _Toolbar"));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (show_toolbar), TRUE);
	gtk_widget_set_tooltip_text (show_toolbar, "Toggle visibility of the toolbar");
	SET_ACCELS (ACCEL_VIEW_TOOLBAR);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), show_toolbar);

	show_sidepane = gtk_check_menu_item_new_with_mnemonic (_("Show _Sidepane"));
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (show_sidepane), TRUE);
	gtk_widget_set_tooltip_text (show_sidepane, "Toggle visibility of the sidepane");
	SET_ACCELS (ACCEL_VIEW_SIDEPANE);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), show_sidepane);

	separator3 = gtk_separator_menu_item_new ();
	gtk_widget_set_sensitive (separator3, FALSE);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), separator3);
	
	layer_visibility_main_menu = gtk_menu_item_new_with_mnemonic (_("Toggle layer _visibility"));
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), layer_visibility_main_menu);
	
	layer_visibility_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(layer_visibility_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(layer_visibility_menu), ACCEL_VIEW_VIS);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (layer_visibility_main_menu), layer_visibility_menu);
	
	toggle_layer_visibility_item1 = gtk_menu_item_new_with_label ("Toggle visibility of layer 1");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER1);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item1);

	toggle_layer_visibility_item2 = gtk_menu_item_new_with_label ("Toggle visibility of layer 2");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER2);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item2);

	toggle_layer_visibility_item3 = gtk_menu_item_new_with_label ("Toggle visibility of layer 3");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER3);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item3);

	toggle_layer_visibility_item4 = gtk_menu_item_new_with_label ("Toggle visibility of layer 4");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER4);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item4);

	toggle_layer_visibility_item5 = gtk_menu_item_new_with_label ("Toggle visibility of layer 5");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER5);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item5);

	toggle_layer_visibility_item6 = gtk_menu_item_new_with_label ("Toggle visibility of layer 6");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER6);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item6);

	toggle_layer_visibility_item7 = gtk_menu_item_new_with_label ("Toggle visibility of layer 7");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER7);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item7);

	toggle_layer_visibility_item8 = gtk_menu_item_new_with_label ("Toggle visibility of layer 8");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER8);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item8);

	toggle_layer_visibility_item9 = gtk_menu_item_new_with_label ("Toggle visibility of layer 9");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER9);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item9);

	toggle_layer_visibility_item10 = gtk_menu_item_new_with_label ("Toggle visibility of layer 10");
	SET_ACCELS (ACCEL_VIEW_VIS_LAYER10);
	gtk_container_add (GTK_CONTAINER (layer_visibility_menu), toggle_layer_visibility_item10);

	separator4 = gtk_separator_menu_item_new ();
	gtk_widget_set_sensitive (separator4, FALSE);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), separator4);

	zoom_in = gtk_image_menu_item_new_from_stock (GTK_STOCK_ZOOM_IN, NULL);
	SET_ACCELS_FROM_STOCK (GTK_STOCK_ZOOM_IN, ACCEL_VIEW_ZOOM_IN);
	gtk_widget_set_tooltip_text (zoom_in, "Zoom in");
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), zoom_in);
	                        
	zoom_out = gtk_image_menu_item_new_from_stock (GTK_STOCK_ZOOM_OUT, NULL);
	SET_ACCELS_FROM_STOCK (GTK_STOCK_ZOOM_OUT, ACCEL_VIEW_ZOOM_OUT);
	gtk_widget_set_tooltip_text (zoom_out, "Zoom out");
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), zoom_out);
	                        
	fit_to_window = gtk_image_menu_item_new_from_stock (GTK_STOCK_ZOOM_FIT, NULL);
	gtk_widget_set_tooltip_text (fit_to_window, "Zoom to fit all visible layers in the window");
	SET_ACCELS_FROM_STOCK (GTK_STOCK_ZOOM_FIT, ACCEL_VIEW_ZOOM_FIT);
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), fit_to_window);
	                        
	separator5 = gtk_separator_menu_item_new ();
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), separator5);
	gtk_widget_set_sensitive (separator5, FALSE);
	
	backgroundColor = gtk_menu_item_new_with_mnemonic ("Change background color");
	gtk_widget_set_tooltip_text (backgroundColor, "Change the background color");
	gtk_container_add (GTK_CONTAINER (menuitem_view_menu), backgroundColor);

	{	// rendering submenu
		menuitem_view_render = gtk_menu_item_new_with_mnemonic (_("_Rendering"));
		gtk_container_add (GTK_CONTAINER (menuitem_view_menu), menuitem_view_render);
		
		menuitem_view_render_menu = gtk_menu_new ();
		gtk_menu_set_accel_group (GTK_MENU(menuitem_view_render_menu), accel_group);
		gtk_menu_set_accel_path (GTK_MENU(menuitem_view_render_menu), ACCEL_VIEW_RENDER);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_view_render), menuitem_view_render_menu);

		menu_view_render_group = NULL;

		render_normal = gtk_radio_menu_item_new_with_mnemonic (menu_view_render_group, _("_Normal"));
		menu_view_render_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (render_normal));
		gtk_container_add (GTK_CONTAINER (menuitem_view_render_menu), render_normal);

		render_hq = gtk_radio_menu_item_new_with_mnemonic (menu_view_render_group, _("High _Quality"));
		menu_view_render_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (render_hq));
		gtk_container_add (GTK_CONTAINER (menuitem_view_render_menu), render_hq);

		screen.win.menu_view_render_group = malloc(4 * sizeof(GtkWidget *));
		if(screen.win.menu_view_render_group == NULL)
			GERB_FATAL_ERROR("malloc for rendering type synchronization failed.\n");

		screen.win.menu_view_render_group[GERBV_RENDER_TYPE_GDK] = NULL;
		screen.win.menu_view_render_group[GERBV_RENDER_TYPE_GDK_XOR] = NULL;
		screen.win.menu_view_render_group[GERBV_RENDER_TYPE_CAIRO_NORMAL] = GTK_CHECK_MENU_ITEM(render_normal);
		screen.win.menu_view_render_group[GERBV_RENDER_TYPE_CAIRO_HIGH_QUALITY] = GTK_CHECK_MENU_ITEM(render_hq);

		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (render_normal), TRUE);
	}

	{	// units submenu
		menuitem_view_unit = gtk_menu_item_new_with_mnemonic (_("U_nits"));
		gtk_container_add (GTK_CONTAINER (menuitem_view_menu), menuitem_view_unit);

		menuitem_view_unit_menu = gtk_menu_new ();
		gtk_menu_set_accel_group (GTK_MENU(menuitem_view_unit_menu), accel_group);
		gtk_menu_set_accel_path (GTK_MENU(menuitem_view_unit_menu), ACCEL_VIEW_UNITS);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_view_unit), menuitem_view_unit_menu);

		menu_view_unit_group = NULL;

		unit_mil = gtk_radio_menu_item_new_with_mnemonic (menu_view_unit_group, _("mi_l"));
		menu_view_unit_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (unit_mil));
		gtk_container_add (GTK_CONTAINER (menuitem_view_unit_menu), unit_mil);

		unit_mm = gtk_radio_menu_item_new_with_mnemonic (menu_view_unit_group, _("_mm"));
		menu_view_unit_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (unit_mm));
		gtk_container_add (GTK_CONTAINER (menuitem_view_unit_menu), unit_mm);

		unit_in = gtk_radio_menu_item_new_with_mnemonic (menu_view_unit_group, _("_in"));
		menu_view_unit_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (unit_in));
		gtk_container_add (GTK_CONTAINER (menuitem_view_unit_menu), unit_in);
		
		screen.win.menu_view_unit_group = malloc(3*sizeof(GtkWidget *));
		if(screen.win.menu_view_unit_group == NULL)
			GERB_FATAL_ERROR("malloc for display unit synchronization failed.\n");

		screen.win.menu_view_unit_group[GERBV_MILS] = GTK_CHECK_MENU_ITEM(unit_mil);
		screen.win.menu_view_unit_group[GERBV_MMS] = GTK_CHECK_MENU_ITEM(unit_mm);
		screen.win.menu_view_unit_group[GERBV_INS] = GTK_CHECK_MENU_ITEM(unit_in);
	}

	/* --- Next menu item (Current Layer) --- */
	menuitem_layer = gtk_menu_item_new_with_mnemonic (_("_Layer"));
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_layer);

	menuitem_layer_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU (menuitem_layer_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU (menuitem_layer_menu), ACCEL_LAYER);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_layer), menuitem_layer_menu);

	layer_visibility = gtk_menu_item_new_with_mnemonic ("Toggle _visibility");
	gtk_widget_set_tooltip_text (layer_visibility, "Toggles the visibility of the layer currently selected in the sidepane");
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_visibility);

	layer_invert = gtk_menu_item_new_with_mnemonic ("_Invert color");
	gtk_widget_set_tooltip_text (layer_invert, "Invert the display polarity of the layer currently selected in the sidepane");
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_invert);

	layer_color = gtk_menu_item_new_with_mnemonic (_("_Change color"));
	SET_ACCELS (ACCEL_LAYER_COLOR);
	gtk_widget_set_tooltip_text (layer_color, "Change the display color of the layer currently selected in the sidepane");
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_color);

	layer_reload = gtk_menu_item_new_with_mnemonic (_("_Reload layer"));
	gtk_widget_set_tooltip_text (layer_reload, "Reload the layer from disk");
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_reload);

	layer_orientation = gtk_menu_item_new_with_mnemonic (_("Modify _orientation"));
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_orientation);
	gtk_widget_set_tooltip_text (layer_orientation, "Translate, scale, rotate, or mirror the layer currently selected in the sidepane");

	layer_format = gtk_menu_item_new_with_mnemonic (_("Edit file _format"));
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_format);
	gtk_widget_set_tooltip_text (layer_format, "View and edit the numerical format used to parse this layer currently selected in the sidepane");

	layer_up = gtk_image_menu_item_new_with_mnemonic (_("Move u_p"));
	gtk_widget_set_tooltip_text (layer_up, "Move the layer currently selected in the sidepane one step up");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_GO_UP, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (layer_up), tempImage);
	SET_ACCELS (ACCEL_LAYER_UP);
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_up);

	layer_down = gtk_image_menu_item_new_with_mnemonic (_("Move dow_n"));
	gtk_widget_set_tooltip_text (layer_down, "Move the layer currently selected in the sidepane one step down");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (layer_down), tempImage);
	SET_ACCELS (ACCEL_LAYER_DOWN);
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_down);

	layer_remove = gtk_image_menu_item_new_with_mnemonic (_("_Delete"));
	gtk_container_add (GTK_CONTAINER (menuitem_layer_menu), layer_remove);
	gtk_widget_set_tooltip_text (layer_remove, "Remove the layer currently selected in the sidepane");
	tempImage = gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (layer_remove), tempImage);

	/* The callbacks need this reference to grey the layer menu out, if there are none loaded. */
	screen.win.curLayerMenuItem = menuitem_layer;

	/* Use the "Current Layer" menu as right click popup menu for layer tree */
	screen.win.layerTreePopupMenu = menuitem_layer_menu;

	/* --- Next menu item (Analyze) --- */
	menuitem_analyze = gtk_menu_item_new_with_mnemonic (_("_Analyze"));
	screen.win.curAnalyzeMenuItem = menuitem_analyze;
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_analyze);

	menuitem_analyze_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_analyze_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_analyze_menu), ACCEL_ANAL);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_analyze), menuitem_analyze_menu);

	analyze_active_gerbers = gtk_menu_item_new_with_mnemonic (_("Analyze visible _Gerber layers"));
	gtk_widget_set_tooltip_text (analyze_active_gerbers, 
			      "Examine a detailed anaylsis of the contents of all visible Gerber layers");
	gtk_container_add (GTK_CONTAINER (menuitem_analyze_menu), analyze_active_gerbers);

	analyze_active_drill = gtk_menu_item_new_with_mnemonic (_("Analyze visible _drill layers"));
	gtk_widget_set_tooltip_text (analyze_active_drill, 
			      "Examine a detailed anaylsis of the contents of all visible drill layers");
	gtk_container_add (GTK_CONTAINER (menuitem_analyze_menu), analyze_active_drill);

	analyze_benchmark = gtk_menu_item_new_with_mnemonic (_("_Benchmark (1 min)"));
	gtk_widget_set_tooltip_text (analyze_benchmark, 
			      "Benchmark different rendering methods. Will make the application unresponsive for 1 minute!");
	gtk_container_add (GTK_CONTAINER (menuitem_analyze_menu), analyze_benchmark);


	/* Wait and add in for 2.1??
	control_gerber_options = gtk_menu_item_new_with_mnemonic (_("Control Gerber options..."));
	gtk_widget_set_tooltip_text (control_gerber_options, "Set which Gerber features should be displayed");
	gtk_container_add (GTK_CONTAINER (menuitem_analyze_menu), control_gerber_options);
	*/
	menuitem_tools = gtk_menu_item_new_with_mnemonic (_("_Tools"));
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_tools);

	menuitem_tools_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_tools_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_tools_menu), ACCEL_TOOLS);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_tools), menuitem_tools_menu);
	pointer_tool = gtk_image_menu_item_new_with_mnemonic (_("_Pointer Tool"));
	/*
	GtkWidget *tempImage = gtk_image_new_from_pixbuf (pointerpixbuf);
	gtk_image_menu_item_set_image ((GtkImageMenuItem *)pointer_tool, tempImage);
	*/
	SET_ACCELS (ACCEL_TOOLS_POINTER);
	gtk_container_add (GTK_CONTAINER (menuitem_tools_menu), pointer_tool);
	gtk_widget_set_tooltip_text (pointer_tool, "Select objects on the screen");
	pan_tool = gtk_image_menu_item_new_with_mnemonic ("Pa_n Tool");
	/*
	tempImage = gtk_image_new_from_pixbuf (movepixbuf);
	gtk_image_menu_item_set_image ((GtkImageMenuItem *)pan_tool, tempImage);
	*/
	SET_ACCELS (ACCEL_TOOLS_PAN);
	gtk_container_add (GTK_CONTAINER (menuitem_tools_menu), pan_tool);
	gtk_widget_set_tooltip_text (pan_tool, "Pan by left clicking and dragging");

	zoom_tool = gtk_image_menu_item_new_with_mnemonic (_("_Zoom Tool"));
	/*
	tempImage = gtk_image_new_from_pixbuf (zoompixbuf);
	gtk_image_menu_item_set_image ((GtkImageMenuItem *)zoom_tool, tempImage);
	*/
	SET_ACCELS (ACCEL_TOOLS_ZOOM);
	gtk_container_add (GTK_CONTAINER (menuitem_tools_menu), zoom_tool);
	gtk_widget_set_tooltip_text (zoom_tool, "Zoom by left clicking or dragging");

	measure_tool = gtk_image_menu_item_new_with_mnemonic (_("_Measure Tool"));
	/*
	tempImage = gtk_image_new_from_pixbuf (measurepixbuf);
	gtk_image_menu_item_set_image ((GtkImageMenuItem *)measure_tool, tempImage);
	*/
	SET_ACCELS (ACCEL_TOOLS_MEASURE);
	gtk_container_add (GTK_CONTAINER (menuitem_tools_menu), measure_tool);
	gtk_widget_set_tooltip_text (measure_tool, "Measure distances on the screen");

	menuitem_help = gtk_menu_item_new_with_mnemonic (_("_Help"));
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem_help);

	menuitem_help_menu = gtk_menu_new ();
	gtk_menu_set_accel_group (GTK_MENU(menuitem_help_menu), accel_group);
	gtk_menu_set_accel_path (GTK_MENU(menuitem_help_menu), ACCEL_HELP);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_help), menuitem_help_menu);

	/* Not ready for 2.0
	online_manual = gtk_menu_item_new_with_mnemonic (_("_Online Manual..."));
	gtk_container_add (GTK_CONTAINER (menuitem_help_menu), online_manual);
	gtk_widget_set_tooltip_text (online_manual, "View the online help documentation");
	*/

	about = gtk_image_menu_item_new_with_mnemonic (_("_About Gerbv..."));
	gtk_container_add (GTK_CONTAINER (menuitem_help_menu), about);
	gtk_widget_set_tooltip_text (about, "View information about gerbv");
	image34 = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (about), image34);

	bugs_menuitem = gtk_menu_item_new_with_mnemonic (_("Known _bugs in this version..."));
	gtk_container_add (GTK_CONTAINER (menuitem_help_menu), bugs_menuitem);
	gtk_widget_set_tooltip_text (bugs_menuitem, "View list of known gerbv bugs");	

	/* Create toolbar (button bar) beneath main menu */
	toolbar_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), toolbar_hbox, FALSE, FALSE, 0);

	button_toolbar = gtk_toolbar_new ();
	gtk_style_context_add_class (gtk_widget_get_style_context (button_toolbar),
		GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
	gtk_widget_set_size_request (button_toolbar, 500, -1);
	gtk_box_pack_start (GTK_BOX (toolbar_hbox), button_toolbar, TRUE, TRUE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (button_toolbar), GTK_TOOLBAR_ICONS);
	/*tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (button_toolbar));*/

	toolbutton_new = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
	gtk_widget_set_tooltip_text (toolbutton_new, "Close all layers and start a new project");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_new);

	toolbutton_open = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_OPEN);
	gtk_widget_set_tooltip_text (toolbutton_open, "Open a previously saved gerbv project");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_open);

	toolbutton_revert = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_REVERT_TO_SAVED);
	gtk_widget_set_tooltip_text (toolbutton_revert, "Reload all layers in project");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_revert);

	toolbutton_save = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_SAVE);
	gtk_widget_set_tooltip_text (toolbutton_save, "Save the current project");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_save);

	separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (button_toolbar), separatortoolitem1);

	toolbutton_print = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_PRINT);
	gtk_widget_set_tooltip_text (toolbutton_print, "Print the visible layers");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_print);

	separatortoolitem2 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (button_toolbar), separatortoolitem2);

	toolbutton_zoom_in = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_ZOOM_IN);
	gtk_widget_set_tooltip_text (toolbutton_zoom_in, "Zoom in");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_zoom_in);

	toolbutton_zoom_out = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_ZOOM_OUT);
	gtk_widget_set_tooltip_text (toolbutton_zoom_out, "Zoom out");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_zoom_out);

	toolbutton_zoom_fit = (GtkWidget*) gtk_tool_button_new_from_stock (GTK_STOCK_ZOOM_FIT);
	gtk_widget_set_tooltip_text (toolbutton_zoom_fit, "Zoom to fit all visible layers in the window");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_zoom_fit);

/* Turn these on later when we have icons for these buttons */
/*
	separatortoolitem3 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (button_toolbar), separatortoolitem3);

	toolbutton_analyze = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-apply");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_analyze);

	toolbutton_validate = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-apply");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_validate);

	toolbutton_control = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-apply");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toolbutton_control);
*/

	separatortoolitem4 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_container_add (GTK_CONTAINER (button_toolbar), separatortoolitem4);
	toggletoolbutton_pointer = (GtkWidget*) gtk_toggle_tool_button_new();
	pointerimage = gtk_image_new_from_pixbuf(pointerpixbuf);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(toggletoolbutton_pointer),
					pointerimage);
	gtk_widget_set_tooltip_text (toggletoolbutton_pointer, "Select objects on the screen");	
	gtk_container_add (GTK_CONTAINER (button_toolbar), toggletoolbutton_pointer);
	toggletoolbutton_pan = (GtkWidget*) gtk_toggle_tool_button_new();
	moveimage = gtk_image_new_from_pixbuf(movepixbuf);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(toggletoolbutton_pan),
					moveimage);
	gtk_widget_set_tooltip_text (toggletoolbutton_pan, "Pan by left clicking and dragging");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toggletoolbutton_pan);
	

	toggletoolbutton_zoom = (GtkWidget*) gtk_toggle_tool_button_new();
	zoomimage = gtk_image_new_from_pixbuf(zoompixbuf);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(toggletoolbutton_zoom),
					zoomimage);
	gtk_widget_set_tooltip_text (toggletoolbutton_zoom, "Zoom by left clicking or dragging");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toggletoolbutton_zoom);

	toggletoolbutton_measure = (GtkWidget*) gtk_toggle_tool_button_new();
	measureimage = gtk_image_new_from_pixbuf(measurepixbuf);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(toggletoolbutton_measure),
					measureimage);
	gtk_widget_set_tooltip_text (toggletoolbutton_measure, "Measure distances on the screen");
	gtk_container_add (GTK_CONTAINER (button_toolbar), toggletoolbutton_measure);
	
	hpaned1 = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (vbox1), hpaned1, TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (hpaned1), 225);

	sidepane_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request (sidepane_vbox, 150, -1);
	
	gtk_paned_pack1 (GTK_PANED (hpaned1), sidepane_vbox, TRUE, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (sidepane_vbox), 5);

	sidepane_notebook = gtk_notebook_new ();
	gtk_widget_set_size_request (sidepane_notebook, 100, -1);
	gtk_box_pack_start (GTK_BOX (sidepane_vbox), sidepane_notebook, TRUE, TRUE, 0);

	vbox10 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
	gtk_container_add (GTK_CONTAINER (sidepane_notebook), vbox10);
	gtk_widget_set_size_request (vbox10, 82, -1);
	gtk_container_set_border_width (GTK_CONTAINER (vbox10), 4);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox10), hbox4, FALSE, FALSE, 0);

	label1 = gtk_label_new (_("Rendering: "));
	gtk_box_pack_start (GTK_BOX (hbox4), label1, FALSE, FALSE, 0);

	render_combobox = gtk_combo_box_text_new ();
	gtk_box_pack_start (GTK_BOX (hbox4), render_combobox, TRUE, TRUE, 0);

	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (render_combobox), _("Normal"));
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (render_combobox), _("High quality"));
	if (screenRenderInfo.renderType >= GERBV_RENDER_TYPE_CAIRO_NORMAL && screenRenderInfo.renderType < GERBV_RENDER_TYPE_MAX)
	    gtk_combo_box_set_active (GTK_COMBO_BOX (render_combobox), screenRenderInfo.renderType - GERBV_RENDER_TYPE_CAIRO_NORMAL);

	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (vbox10), scrolledwindow1, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow1), 2);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), 
					GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), 
					     GTK_SHADOW_IN);

	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox10), hbox1, FALSE, FALSE, 0);

	button4 = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hbox1), button4, FALSE, TRUE, 0);

	image8 = gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (button4), image8);

	button5 = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hbox1), button5, FALSE, TRUE, 0);

	image9 = gtk_image_new_from_stock (GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (button5), image9);

	button6 = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hbox1), button6, FALSE, TRUE, 0);

	image10 = gtk_image_new_from_stock (GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (button6), image10);

	button7 = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hbox1), button7, FALSE, TRUE, 0);

	image11 = gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (button7), image11);

	Layer_label = gtk_label_new (_("Layers"));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (sidepane_notebook), 
				    gtk_notebook_get_nth_page (GTK_NOTEBOOK (sidepane_notebook), 0), 
				    Layer_label);

	vbox11 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
	gtk_container_add (GTK_CONTAINER (sidepane_notebook), vbox11);
	gtk_container_set_border_width (GTK_CONTAINER (vbox11), 3);

	messages_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (vbox11), messages_scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (messages_scrolledwindow), 
					GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

	message_textview = gtk_text_view_new ();
	gtk_container_add (GTK_CONTAINER (messages_scrolledwindow), message_textview);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (message_textview), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (message_textview), GTK_WRAP_WORD);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (message_textview), FALSE);

	clear_messages_button = gtk_button_new_from_stock (GTK_STOCK_CLEAR);
	gtk_box_pack_start (GTK_BOX (vbox11), clear_messages_button, FALSE, FALSE, 0);

	Message_label = gtk_label_new (_("Messages"));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (sidepane_notebook), 
				    gtk_notebook_get_nth_page (GTK_NOTEBOOK (sidepane_notebook), 1), 
				    Message_label);

	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
	gtk_paned_pack2 (GTK_PANED (hpaned1), vbox2, TRUE, FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
	
	main_view_table = gtk_grid_new ();
	gtk_box_pack_start (GTK_BOX (vbox2), main_view_table, TRUE, TRUE, 0);

	hRuler = gimp_ruler_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach (GTK_GRID (main_view_table), hRuler, 1, 0, 1, 1);
	gtk_widget_set_hexpand (GTK_WIDGET(hRuler), TRUE);
	gimp_ruler_set_range (GIMP_RULER (hRuler), 0, 100, 1000);
	gimp_ruler_set_position (GIMP_RULER (hRuler), 8.56051);

	vRuler = gimp_ruler_new (GTK_ORIENTATION_VERTICAL);
	gtk_grid_attach (GTK_GRID (main_view_table), vRuler, 0, 1, 1, 1);
	gtk_widget_set_vexpand (GTK_WIDGET(vRuler), TRUE);
	gimp_ruler_set_range (GIMP_RULER (vRuler), 0, 100, 1000);
	gimp_ruler_set_position (GIMP_RULER (vRuler), 8.37341);

	drawingarea = gtk_drawing_area_new();
	gtk_grid_attach (GTK_GRID (main_view_table), drawingarea, 1, 1, 1, 1);
	gtk_widget_set_hexpand (GTK_WIDGET(drawingarea), TRUE);
	gtk_widget_set_vexpand (GTK_WIDGET(drawingarea), TRUE);
	
	hAdjustment = (GtkWidget *) gtk_adjustment_new (0.0, -1000.0, 1000.0, 1000.0, 1000.0, 500.0);
	vAdjustment = (GtkWidget *) gtk_adjustment_new (0.0, -1000.0, 1000.0, 1000.0, 1000.0, 500.0);
	
	hScrollbar = gtk_scrollbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_ADJUSTMENT (hAdjustment));
	gtk_grid_attach (GTK_GRID (main_view_table), hScrollbar, 1, 2, 1, 1);
	gtk_widget_set_hexpand (GTK_WIDGET(hScrollbar), TRUE);
	                  
	vScrollbar = gtk_scrollbar_new (GTK_ORIENTATION_VERTICAL, GTK_ADJUSTMENT (vAdjustment));
	gtk_grid_attach (GTK_GRID (main_view_table), vScrollbar, 2, 1, 1, 1);
	gtk_widget_set_vexpand (GTK_WIDGET(vScrollbar), TRUE);
	
	hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start (GTK_BOX (vbox2), hbox5, FALSE, FALSE, 0);

	statusbar_label_left = gtk_label_new ("( 0.0,  0.0 )");
	gtk_box_pack_start (GTK_BOX (hbox5), statusbar_label_left, FALSE, FALSE, 0);
	gtk_widget_set_size_request (statusbar_label_left, 130, -1);
	gtk_label_set_justify ((GtkLabel *) statusbar_label_left, GTK_JUSTIFY_RIGHT);
	
	statusUnitComboBox = gtk_combo_box_text_new ();
	gtk_box_pack_start (GTK_BOX (hbox5), statusUnitComboBox, FALSE, FALSE, 0);
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (statusUnitComboBox), _("mil"));
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (statusUnitComboBox), _("mm"));
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (statusUnitComboBox), _("in"));

	statusbar_label_right = gtk_label_new (_(""));
	gtk_box_pack_start (GTK_BOX (hbox5), statusbar_label_right, TRUE, TRUE, 0);
	gtk_label_set_ellipsize ((GtkLabel *)statusbar_label_right, PANGO_ELLIPSIZE_END);
	gtk_misc_set_alignment (GTK_MISC (statusbar_label_right), 0, 0.5);



    /*
     *  Connect signals to widgets
     */

	/* --- File menu --- */
	g_signal_connect ((gpointer) new, "activate",
	                  G_CALLBACK (callbacks_new_activate),
	                  NULL);
	g_signal_connect ((gpointer) open_project, "activate",
	                  G_CALLBACK (callbacks_open_project_activate),
	                  NULL);
	g_signal_connect ((gpointer) open_layer, "activate",
	                  G_CALLBACK (callbacks_open_layer_activate),
	                  NULL);
	g_signal_connect ((gpointer) revert, "activate",
	                  G_CALLBACK (callbacks_revert_activate),
	                  NULL);
	g_signal_connect ((gpointer) save_layer, "activate",
	                  G_CALLBACK (callbacks_save_layer_activate),
	                  NULL);
	g_signal_connect ((gpointer) save_as_layer, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_LAYER_AS);
	g_signal_connect ((gpointer) save, "activate",
	                  G_CALLBACK (callbacks_save_project_activate),
	                  NULL);
	g_signal_connect ((gpointer) save_as, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_PROJECT_AS);
	g_signal_connect ((gpointer) png, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                 (gpointer)  CALLBACKS_SAVE_FILE_PNG);

	g_signal_connect ((gpointer) pdf, "activate",
	                 G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_PDF);
	g_signal_connect ((gpointer) svg, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_SVG);
	g_signal_connect ((gpointer) postscript, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_PS);
	g_signal_connect ((gpointer) rs274x, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_RS274X);
	g_signal_connect ((gpointer) drill, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_DRILL);
	g_signal_connect ((gpointer) rs274xm, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_RS274XM);
	g_signal_connect ((gpointer) drillm, "activate",
	                  G_CALLBACK (callbacks_generic_save_activate),
	                  (gpointer) CALLBACKS_SAVE_FILE_DRILLM);

	g_signal_connect ((gpointer) print, "activate",
	                  G_CALLBACK (callbacks_print_activate),
	                  NULL);
	g_signal_connect ((gpointer) quit, "activate",
	                  G_CALLBACK (callbacks_quit_activate),
	                  NULL);

	/* --- Edit menu --- */
	g_signal_connect ((gpointer) delete_selected, "activate",
	                  G_CALLBACK (callbacks_delete_objects_clicked),
	                  NULL);
	g_signal_connect ((gpointer) properties_selected, "activate",
	                  G_CALLBACK (callbacks_display_object_properties_clicked),
	                  NULL);

	/* --- View menu --- */
	g_signal_connect ((gpointer) view_fullscreen, "activate",
	                  G_CALLBACK (callbacks_fullscreen_toggled),
	                  GINT_TO_POINTER(0));
	g_signal_connect ((gpointer) show_toolbar, "toggled",
	                  G_CALLBACK (callbacks_show_toolbar_toggled),
	                  toolbar_hbox);
	g_signal_connect ((gpointer) show_sidepane, "toggled",
	                  G_CALLBACK (callbacks_show_sidepane_toggled),
	                  sidepane_vbox);
	g_signal_connect ((gpointer) toggle_layer_visibility_item1, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(0));
	g_signal_connect ((gpointer) toggle_layer_visibility_item2, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(1));
	g_signal_connect ((gpointer) toggle_layer_visibility_item3, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(2));
	g_signal_connect ((gpointer) toggle_layer_visibility_item4, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(3));
	g_signal_connect ((gpointer) toggle_layer_visibility_item5, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(4));
	g_signal_connect ((gpointer) toggle_layer_visibility_item6, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(5));
	g_signal_connect ((gpointer) toggle_layer_visibility_item7, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(6));
	g_signal_connect ((gpointer) toggle_layer_visibility_item8, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(7));
	g_signal_connect ((gpointer) toggle_layer_visibility_item9, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(8));
	g_signal_connect ((gpointer) toggle_layer_visibility_item10, "activate",
	                  G_CALLBACK (callbacks_toggle_layer_visibility_activate),
	                  GINT_TO_POINTER(9));
	g_signal_connect ((gpointer) zoom_in, "activate",
	                  G_CALLBACK (callbacks_zoom_in_activate),
	                  NULL);
	g_signal_connect ((gpointer) zoom_out, "activate",
	                  G_CALLBACK (callbacks_zoom_out_activate),
	                  NULL);
	g_signal_connect ((gpointer) fit_to_window, "activate",
	                  G_CALLBACK (callbacks_fit_to_window_activate),
	                  NULL);
	g_signal_connect ((gpointer) backgroundColor, "activate",
	                  G_CALLBACK (callbacks_change_background_color_clicked),
	                  NULL);
	g_signal_connect ((gpointer) unit_mil, "activate",
	                  G_CALLBACK (callbacks_viewmenu_units_changed),
	                  GINT_TO_POINTER(GERBV_MILS));
	g_signal_connect ((gpointer) unit_mm, "activate",
	                  G_CALLBACK (callbacks_viewmenu_units_changed),
	                  GINT_TO_POINTER(GERBV_MMS));
	g_signal_connect ((gpointer) unit_in, "activate",
	                  G_CALLBACK (callbacks_viewmenu_units_changed),
	                  GINT_TO_POINTER(GERBV_INS));

	/* --- Current Layer menu --- */
	g_signal_connect ((gpointer) layer_visibility, "activate", G_CALLBACK (callbacks_toggle_layer_visibility_activate), GINT_TO_POINTER(-1));
	g_signal_connect ((gpointer) layer_invert, "activate", G_CALLBACK (callbacks_invert_layer_clicked), NULL);
	g_signal_connect ((gpointer) layer_color, "activate", G_CALLBACK (callbacks_change_layer_color_clicked), NULL);
	g_signal_connect ((gpointer) layer_reload, "activate", G_CALLBACK (callbacks_reload_layer_clicked), NULL);
	g_signal_connect ((gpointer) layer_orientation, "activate", G_CALLBACK (callbacks_change_layer_orientation_clicked), NULL);
	g_signal_connect ((gpointer) layer_format, "activate", G_CALLBACK (callbacks_change_layer_format_clicked), NULL);
	g_signal_connect ((gpointer) layer_remove, "activate", G_CALLBACK (callbacks_remove_layer_button_clicked), NULL);
	g_signal_connect ((gpointer) layer_up, "activate", G_CALLBACK (callbacks_move_layer_up_menu_activate), NULL);
	g_signal_connect ((gpointer) layer_down, "activate", G_CALLBACK (callbacks_move_layer_down_menu_activate), NULL);

	/* --- Analyze menu --- */
	g_signal_connect ((gpointer) analyze_active_gerbers, "activate",
	                  G_CALLBACK (callbacks_analyze_active_gerbers_activate),
	                  NULL);
	g_signal_connect ((gpointer) analyze_active_drill, "activate",
	                  G_CALLBACK (callbacks_analyze_active_drill_activate),
	                  NULL);
	g_signal_connect ((gpointer) analyze_benchmark, "activate",
	                  G_CALLBACK (callbacks_benchmark_clicked),
	                  NULL);


	/* Wait for 2.1
	g_signal_connect ((gpointer) control_gerber_options, "activate",
	                  G_CALLBACK (callbacks_control_gerber_options_activate),
	                  NULL);
	*/

	/* --- Tools menu --- */
	g_signal_connect ((gpointer) pointer_tool, "activate",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 0);
	g_signal_connect ((gpointer) pan_tool, "activate",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 1);
	g_signal_connect ((gpointer) zoom_tool, "activate",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 2);
	g_signal_connect ((gpointer) measure_tool, "activate",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 3);

	/* --- Help menu --- */
	/*
	g_signal_connect ((gpointer) online_manual, "activate",
	                  G_CALLBACK (callbacks_online_manual_activate),
	                  NULL);
	*/
	g_signal_connect ((gpointer) about, "activate",
	                  G_CALLBACK (callbacks_about_activate),
	                  NULL);

	g_signal_connect ((gpointer) bugs_menuitem, "activate",
	                  G_CALLBACK (callbacks_bugs_activate),
	                  NULL);

	/* End of Glade generated code */
	g_signal_connect ((gpointer) toolbutton_new, "clicked",
	                  G_CALLBACK (callbacks_new_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_save, "clicked",
	                  G_CALLBACK (callbacks_save_project_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_open, "clicked",
	                  G_CALLBACK (callbacks_open_project_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_revert, "clicked",
	                  G_CALLBACK (callbacks_revert_activate),
	                  NULL);
	g_signal_connect ((gpointer) clear_messages_button, "clicked",
	                  G_CALLBACK (callbacks_clear_messages_button_clicked),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_print, "clicked",
	                  G_CALLBACK (callbacks_print_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_zoom_in, "clicked",
	                  G_CALLBACK (callbacks_zoom_in_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_zoom_out, "clicked",
	                  G_CALLBACK (callbacks_zoom_out_activate),
	                  NULL);
	g_signal_connect ((gpointer) toolbutton_zoom_fit, "clicked",
	                  G_CALLBACK (callbacks_fit_to_window_activate),
	                  NULL);
	g_signal_connect ((gpointer) toggletoolbutton_pointer, "clicked",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 0);
	g_signal_connect ((gpointer) toggletoolbutton_pan, "clicked",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 1);
	g_signal_connect ((gpointer) toggletoolbutton_zoom, "clicked",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 2);
	g_signal_connect ((gpointer) toggletoolbutton_measure, "clicked",
	                  G_CALLBACK (callbacks_change_tool), (gpointer) 3);

	g_signal_connect ((gpointer) statusUnitComboBox, "changed",
	                  G_CALLBACK (callbacks_statusbar_unit_combo_box_changed),
	                  NULL);
	                  
	g_signal_connect ((gpointer) button4, "clicked",
	                  G_CALLBACK (callbacks_add_layer_button_clicked), NULL);
	g_signal_connect ((gpointer) button7, "clicked",
	                  G_CALLBACK (callbacks_remove_layer_button_clicked), NULL);
	g_signal_connect ((gpointer) button5, "clicked",
	                  G_CALLBACK (callbacks_move_layer_down_button_clicked), NULL);
	g_signal_connect ((gpointer) button6, "clicked",
	                  G_CALLBACK (callbacks_move_layer_up_button_clicked), NULL);

	g_signal_connect ((gpointer) hAdjustment, "value-changed",
	                  G_CALLBACK (callbacks_hadjustment_value_changed), NULL);
	g_signal_connect ((gpointer) vAdjustment, "value-changed",
	                  G_CALLBACK (callbacks_vadjustment_value_changed), NULL);
	g_signal_connect ((gpointer) hScrollbar, "button-press-event",
	                  G_CALLBACK (callbacks_scrollbar_button_pressed), NULL);                 
	g_signal_connect ((gpointer) hScrollbar, "button-release-event",
	                  G_CALLBACK (callbacks_scrollbar_button_released), NULL);
	g_signal_connect ((gpointer) vScrollbar, "button-press-event",
	                  G_CALLBACK (callbacks_scrollbar_button_pressed), NULL);                 
	g_signal_connect ((gpointer) vScrollbar, "button-release-event",
	                  G_CALLBACK (callbacks_scrollbar_button_released), NULL);               
	

	if (screen.unit == GERBV_MILS)
		gtk_combo_box_set_active (GTK_COMBO_BOX (statusUnitComboBox), 0);
	else if (screen.unit == GERBV_MMS)
		gtk_combo_box_set_active (GTK_COMBO_BOX (statusUnitComboBox), 1);
	else
		gtk_combo_box_set_active (GTK_COMBO_BOX (statusUnitComboBox), 2);
	   
	gint width, height;
              
	gtk_window_add_accel_group (GTK_WINDOW (mainWindow), accel_group);

	GtkListStore *list_store;

	list_store = gtk_list_store_new (4,	G_TYPE_BOOLEAN,
		GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
		
	GtkWidget *tree;

	tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (list_store));
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes ("Visible",
	                                                renderer,
	                                                "active", 0,
	                                                NULL);
	gtk_tree_view_column_set_min_width  ((GtkTreeViewColumn *)column,25);
	g_signal_connect(renderer, "toggled",
		       G_CALLBACK(callbacks_layer_tree_visibility_button_toggled), NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	renderer = gtk_cell_renderer_pixbuf_new ();
	column = gtk_tree_view_column_new_with_attributes ("Color",
	                                                renderer,
	                                                "pixbuf", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "foreground", "red", "xalign", 0.5,
			"family", "Times", "size-points", 12.0, NULL);
	column = gtk_tree_view_column_new_with_attributes ("Modified",
	                                                renderer,
	                                                "text", 3,
	                                                NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
	gtk_tree_view_column_set_min_width  ((GtkTreeViewColumn *)column,20);
	
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Name",
	                                                renderer,
	                                                "markup", 2,
	                                                NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	gtk_tree_view_set_headers_visible   ((GtkTreeView *)tree, FALSE);
	g_signal_connect(tree, "key-press-event",
		G_CALLBACK(callbacks_layer_tree_key_press), NULL);
	g_signal_connect(tree, "button-press-event",
		G_CALLBACK(callbacks_layer_tree_button_press), NULL);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), tree);
	
	GtkTreeSelection *selection;
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW (tree), FALSE);
	gtk_tree_view_set_reorderable (GTK_TREE_VIEW (tree), TRUE);

	g_signal_connect (G_OBJECT(list_store), "row-inserted",
			  G_CALLBACK (callbacks_layer_tree_row_inserted), NULL);
	/* steal the focus to the tree to make sure none of the buttons are focused */
	gtk_widget_grab_focus (tree);
	/*
	* Connect all events on drawing area 
	*/    
	g_signal_connect(drawingarea, "draw",
		       G_CALLBACK(callbacks_drawingarea_draw), NULL);
	g_signal_connect(drawingarea,"configure_event",
		       G_CALLBACK(callbacks_drawingarea_configure_event), NULL);
	g_signal_connect(drawingarea, "motion_notify_event",
		       G_CALLBACK(callbacks_drawingarea_motion_notify_event), NULL);
	g_signal_connect(drawingarea, "button_press_event",
		       G_CALLBACK(callbacks_drawingarea_button_press_event), NULL);
	g_signal_connect(drawingarea, "button_release_event",
		       G_CALLBACK(callbacks_drawingarea_button_release_event), NULL);
	g_signal_connect_after(mainWindow, "key_press_event",
		       G_CALLBACK(callbacks_window_key_press_event), NULL);
	g_signal_connect_after(mainWindow, "key_release_event",
		       G_CALLBACK(callbacks_window_key_release_event), NULL);
	g_signal_connect_after(mainWindow, "scroll_event",
		       G_CALLBACK(callbacks_window_scroll_event), NULL);
	g_signal_connect_after(mainWindow, "delete_event",
		       G_CALLBACK(callbacks_quit_activate), NULL);
	
	gtk_widget_set_events(drawingarea, GDK_EXPOSURE_MASK
			  | GDK_LEAVE_NOTIFY_MASK
			  | GDK_ENTER_NOTIFY_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_BUTTON_RELEASE_MASK
			  | GDK_KEY_PRESS_MASK
			  | GDK_KEY_RELEASE_MASK
			  | GDK_POINTER_MOTION_MASK
			  | GDK_POINTER_MOTION_HINT_MASK
			  | GDK_SCROLL_MASK
			  );

	/*
	* Set gtk error log handler
	*/
	g_log_set_handler (NULL, 
		       G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION | G_LOG_LEVEL_MASK, 
		       callbacks_handle_log_messages, NULL); 
  
	/*
	 * Setup some GTK+ defaults.
	 * These should really be somewhere else.
	 */
	GdkColor selection_color = {0, 65000, 65000, 65000};

	gerbv_color_t tool_color = { 1.0, 1.0, 1.0, 0.8 };
	gerbv_color_t tool_outline_color = { 0.0, 0.0, 0.0, 0.5 };
	
	screen.selection_color = selection_color;

	screen.tool_color = tool_color;
	screen.tool_outline_color = tool_outline_color;

	screen.drawing_area = drawingarea;
	screen.win.hAdjustment = hAdjustment;
	screen.win.vAdjustment = vAdjustment;
	screen.win.hRuler = hRuler;
	screen.win.vRuler = vRuler;	
	screen.win.sidepane_notebook = sidepane_notebook;
	screen.win.sidepaneRenderComboBox = GTK_COMBO_BOX(render_combobox);
	screen.win.toolButtonPointer = toggletoolbutton_pointer;
	screen.win.toolButtonPan = toggletoolbutton_pan;
	screen.win.toolButtonZoom = toggletoolbutton_zoom;
	screen.win.toolButtonMeasure = toggletoolbutton_measure;

	/* 
	* Good defaults according to Ales. Gives aspect ratio of 1.3333...
	*/
	if ((req_width != -1) && (req_height != -1)) {
		width = req_width;
		height = req_height;
		} 
	else {
		GdkScreen *screen;
		int nmonitors;

		screen = gdk_screen_get_default();
		nmonitors = gdk_screen_get_n_monitors(screen);

		width = gdk_screen_get_width(screen) * 3/4 / nmonitors;
		height = gdk_screen_get_height(screen) * 3/4 / nmonitors;
	}

	gtk_window_set_default_size((GtkWindow *)mainWindow, width, height);
	
	GtkSettings* gtksettings = gtk_settings_get_default ();
	g_object_set (G_OBJECT(gtksettings), "gtk-can-change-accels", TRUE, NULL);
	interface_load_accels ();
	gtk_widget_show_all (mainWindow);
	screen.win.topLevelWindow = mainWindow;
	screen.win.messageTextView = message_textview;
	screen.win.statusMessageLeft = statusbar_label_left;
	screen.win.statusMessageRight = statusbar_label_right;
	screen.win.statusUnitComboBox = statusUnitComboBox;
	screen.win.layerTree = tree;
	screen.win.treeIsUpdating = FALSE;

	screen.selectionInfo.selectedNodeArray = g_array_new (FALSE,
			FALSE, sizeof(gerbv_selection_item_t));
	callbacks_change_tool (NULL, (gpointer) 0);
	rename_main_window("",mainWindow);

	set_window_icon (mainWindow);
	callbacks_update_layer_tree ();

	/* connect this signals as late as possible to avoid triggering them before
	   the gui is drawn */
	g_signal_connect ((gpointer) render_normal, "activate",
	                  G_CALLBACK (callbacks_viewmenu_rendertype_changed),
	                  GINT_TO_POINTER(GERBV_RENDER_TYPE_CAIRO_NORMAL));
	g_signal_connect ((gpointer) render_hq, "activate",
	                  G_CALLBACK (callbacks_viewmenu_rendertype_changed),
	                  GINT_TO_POINTER(GERBV_RENDER_TYPE_CAIRO_HIGH_QUALITY));
	g_signal_connect ((gpointer) render_combobox, "changed",
	                  G_CALLBACK (callbacks_sidepane_render_type_combo_box_changed),
	                  NULL);
	gtk_main();
	interface_save_accels ();
}

/* ----------------------------------------------------  */
void 
interface_set_render_type (int t)
{
	if (t < GERBV_RENDER_TYPE_CAIRO_NORMAL || t >= GERBV_RENDER_TYPE_MAX)
		return;

	screenRenderInfo.renderType = t;

	/* make sure the interface is already up before calling
	 * gtk_combo_box_set_active()
	 */
	if (!screen.win.sidepaneRenderComboBox)
		return;

	gtk_combo_box_set_active (GTK_COMBO_BOX (screen.win.sidepaneRenderComboBox), t - GERBV_RENDER_TYPE_CAIRO_NORMAL);
	gtk_check_menu_item_set_active (screen.win.menu_view_render_group[t], TRUE);
}

/* ----------------------------------------------------  */
/**
  * This dialog box shows a message and two buttons:
  * "OK" and "Cancel".  It returns gboolean 1 if the
  * user clicked "OK", and gboolean 0 if the user
  * clicked "Cancel".
  *
  */

gboolean
interface_get_alert_dialog_response (gchar *primaryText, gchar *secondaryText, 
				     gboolean show_checkbox, gboolean *ask_to_show_again )
     /* This fcn returns TRUE if the user presses the OK button,
	otherwise it returns FALSE. */
{
  /* Set show_checkbox = TRUE to show "do not show this again" checkbox. */
  /* Point ask_to_show_again to the variable to set to not show the checkbox. */
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *checkbox=NULL;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  gboolean returnVal = FALSE;

  dialog1 = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 6);
  gtk_window_set_resizable (GTK_WINDOW (dialog1), FALSE);
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (dialog1));

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 6);

  image1 = gtk_image_new_from_icon_name (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (image1), 0.5, 0);

  gchar *labelMessage = g_strconcat ("<span weight=\"bold\" size=\"larger\">",primaryText,
  		"</span>\n<span/>\n",secondaryText,NULL);
  label1 = gtk_label_new (labelMessage);
  g_free (labelMessage);
  GtkWidget *vbox9 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (vbox9), label1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox9, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (label1), TRUE);

  // even with no checkbox, this extra hbox gives the recommended 24 px space between the
  //   label and the buttons
  GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  if (show_checkbox) {
    GtkWidget *label3 = gtk_label_new ("    ");

    checkbox =  gtk_check_button_new_with_label("Do not show this dialog again.");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(checkbox), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox2), label3, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox2), checkbox, FALSE, FALSE, 0);
  }
  gtk_box_pack_start (GTK_BOX (vbox9), hbox2, FALSE, FALSE, 12);
  
  dialog_action_area1 = gtk_dialog_get_action_area(GTK_DIALOG (dialog1));
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_CANCEL);
  gtk_widget_set_can_default(cancelbutton1, TRUE);
  gtk_widget_grab_default (cancelbutton1);
  gtk_widget_grab_focus (cancelbutton1);

  okbutton1 = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default(okbutton1, TRUE);

  gtk_widget_show_all (dialog1);

  if (gtk_dialog_run ((GtkDialog*)dialog1) == GTK_RESPONSE_OK) {
    /* check to see if user clicked on "do not show again" box */
    if ((show_checkbox == TRUE) &&
	(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox)) == TRUE) &&
	(ask_to_show_again != NULL) ) {
      /* The user clicked the "do not show again box".  Set corresponding
       * flag to FALSE. */
      *ask_to_show_again = FALSE;
    }
    returnVal = TRUE;
  }
  gtk_widget_destroy (dialog1);
		
  return returnVal;
}



/* ----------------------------------------------------  */
/**
  * This dialog box shows a textmessage and one button:
  * "OK".  It does not return anything.
  *
  */
void
interface_show_alert_dialog (gchar *primaryText, gchar *secondaryText, 
			     gboolean show_checkbox, gboolean *ask_to_show_again )
     /* This fcn tells the user something, and only displays "OK" */
{
  /* Set show_checkbox = TRUE to show "do not show this again" checkbox. */
  /* Point ask_to_show_again to the variable to set to not show the checkbox. */
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *checkbox=NULL;
  GtkWidget *dialog_action_area1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 6);
  gtk_window_set_resizable (GTK_WINDOW (dialog1), FALSE);
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = gtk_dialog_get_content_area (GTK_DIALOG (dialog1));

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 6);

  image1 = gtk_image_new_from_icon_name (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (image1), 0.5, 0);

  gchar *labelMessage = g_strconcat ("<span weight=\"bold\" size=\"larger\">",primaryText,
  		"</span>\n<span/>\n",secondaryText,NULL);
  label1 = gtk_label_new (labelMessage);
  g_free (labelMessage);
  
  GtkWidget *vbox9 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (vbox9), label1, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_line_wrap (GTK_LABEL (label1), TRUE);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox9, FALSE, FALSE, 0);
  
  GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  if (show_checkbox) {
    GtkWidget *label3 = gtk_label_new ("    ");

    checkbox =  gtk_check_button_new_with_label("Do not show this dialog again.");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(checkbox), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox2), label3, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox2), checkbox, FALSE, FALSE, 0);
  }
  gtk_box_pack_start (GTK_BOX (vbox9), hbox2, FALSE, FALSE, 12);
  
  dialog_action_area1 = gtk_dialog_get_action_area (GTK_DIALOG (dialog1));
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  okbutton1 = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  gtk_widget_set_can_default (okbutton1, TRUE);

  gtk_widget_show_all (dialog1);

  /* check to see if user clicked on "do not show again" box */
  if ((show_checkbox == TRUE) &&
      (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbox)) == TRUE) &&
      (ask_to_show_again != NULL) ) {
    /* The user clicked the "do not show again box".  Set corresponding
     * flag to FALSE. */
    *ask_to_show_again = FALSE;
  }

  gtk_dialog_run (GTK_DIALOG(dialog1));
  gtk_widget_destroy (dialog1);
		
  return;
}

void
interface_show_modify_orientation_dialog (gerbv_user_transformation_t *transform,
		gerbv_unit_t screenUnit) {

	GtkBuilder *builder;
	GError *error = NULL;

	GtkWidget *dialog;
	GtkWidget *check1,*check2;
	GtkWidget *spin1Label, *spin2Label;
	GtkWidget *spin1,*spin2,*spin3,*spin4,*spin5;
	gerbv_user_transformation_t startTransform = *transform;
	double translateX, translateY;

	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, UIDIR "/modify-orientation-dialog.ui", &error);
	g_assert_no_error (error);

	dialog = GTK_WIDGET (gtk_builder_get_object (builder, "modify-orientation-dialog"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW (screen.win.topLevelWindow));

	spin1 = GTK_WIDGET (gtk_builder_get_object (builder, "spin1"));
	spin1Label = GTK_WIDGET (gtk_builder_get_object (builder, "spin1-label"));
	spin2 = GTK_WIDGET (gtk_builder_get_object (builder, "spin2"));
	spin2Label = GTK_WIDGET (gtk_builder_get_object (builder, "spin2-label"));
	if (screenUnit == (gerbv_unit_t) GERBV_MILS) {
		gtk_label_set_text(GTK_LABEL(spin1Label), "X (mil):");
		gtk_label_set_text(GTK_LABEL(spin2Label), "Y (mil):");
		translateX = transform->translateX * 1000;
		translateY = transform->translateY * 1000;
	}
	else if (screen.unit == (gerbv_gui_unit_t) GERBV_MMS) {
		gtk_label_set_text(GTK_LABEL(spin1Label), "X (mm):");
		gtk_label_set_text(GTK_LABEL(spin2Label), "Y (mm):");
		translateX = transform->translateX * 25.4;
		translateY = transform->translateY * 25.4;
	}
	else {
		gtk_label_set_text(GTK_LABEL(spin1Label), "X (in):");
		gtk_label_set_text(GTK_LABEL(spin2Label), "Y (in):");
		translateX = transform->translateX;
		translateY = transform->translateY;
	}
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin1), translateX);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin2), translateY);

	spin3 = GTK_WIDGET (gtk_builder_get_object (builder, "spin3"));
	spin4 = GTK_WIDGET (gtk_builder_get_object (builder, "spin4"));

	spin5 = GTK_WIDGET (gtk_builder_get_object (builder, "spin5"));
	gdouble degreeRotation = transform->rotation/M_PI*180;
	if ((degreeRotation < 135)&&(degreeRotation >= 45))
		gtk_combo_box_set_active (GTK_COMBO_BOX(spin5), 1);
	else if ((degreeRotation < 225)&&(degreeRotation >= 135))
		gtk_combo_box_set_active (GTK_COMBO_BOX(spin5), 2);
	else if ((degreeRotation < 315)&&(degreeRotation >= 225))
		gtk_combo_box_set_active (GTK_COMBO_BOX(spin5), 3);
	else
		gtk_combo_box_set_active (GTK_COMBO_BOX(spin5), 0);

	check1 = GTK_WIDGET (gtk_builder_get_object (builder, "check1"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check1), transform->mirrorAroundX);

	check2 = GTK_WIDGET (gtk_builder_get_object (builder, "check2"));
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check2), transform->mirrorAroundY);

	gtk_widget_show_all (dialog);
	gint result = GTK_RESPONSE_APPLY;

	// each time the user selects "apply", update the screen and re-enter the dialog loop
	while (result == GTK_RESPONSE_APPLY) {
		result = gtk_dialog_run (GTK_DIALOG(dialog));
		if (result != GTK_RESPONSE_NONE) {
			/* extract all the parameters */
			if (screenUnit == (gerbv_unit_t) GERBV_MILS) {
				transform->translateX = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin1))/
					1000;
				transform->translateY = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin2))/
					1000;
			}
			else if (screen.unit == (gerbv_gui_unit_t) GERBV_MMS) {
				transform->translateX = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin1))/
					25.4;
				transform->translateY = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin2))/
					25.4;
			}
			else {
				transform->translateX = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin1));
				transform->translateY = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin2));
			}
			transform->scaleX = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin3));
			transform->scaleY = gtk_spin_button_get_value (GTK_SPIN_BUTTON(spin4));
			gint rotationIndex = gtk_combo_box_get_active (GTK_COMBO_BOX(spin5));
			if (rotationIndex == 0)
				transform->rotation = 0;
			else if (rotationIndex == 1)
				transform->rotation = 90.0/180*M_PI;
			else if (rotationIndex == 2)
				transform->rotation = 180.0/180*M_PI;
			else if (rotationIndex == 3)
				transform->rotation = 270.0/180*M_PI;	
			transform->mirrorAroundX = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(check1));
			transform->mirrorAroundY = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(check2));
			
			render_refresh_rendered_image_on_screen ();
			callbacks_update_layer_tree ();
		}
	}
	if (result == GTK_RESPONSE_NONE) {
		// revert back to the start values if the user cancelled
		*transform = startTransform;
	}
	gtk_widget_destroy (dialog);
	g_object_unref(builder);

	return;
}


