/*
 * Copyright (C) 2002, Reynaldo H. Verdejo Pinochet 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "logo.h"
#include "oss_functions.h"
#define DEBUG 0



GtkWidget *scale[MAX_ELEMENTS * 2];
GtkWidget *toggle[MAX_ELEMENTS * 2];


const char *channel_labels[] = SOUND_DEVICE_LABELS;
const char *channel_names[] = SOUND_DEVICE_NAMES;

char labels_text[MAX_ELEMENTS * 2][15];
int mixerfd;

/* function prototypes */

gint close_all (GtkWidget * widget, gpointer data);
gint lock_levels (GtkWidget * widget, gpointer data);
gint open_about (GtkWidget * widget, gpointer data);
int get_table_cols_needed (void);
int main (int argc, char *argv[]);


gint
close_all (GtkWidget * widget, gpointer data)
{
  // g_print ("Normal Exit done\n");
  mixer_close ();
  gtk_main_quit ();
  return (FALSE);
}

gint
lock_levels (GtkWidget * widget, gpointer data)
{
  int average;

  gint left_value = gtk_adjustment_get_value
    (gtk_range_get_adjustment (GTK_RANGE (scale[(int) (data - 1)])));

  gint right_value = gtk_adjustment_get_value
    (gtk_range_get_adjustment (GTK_RANGE (scale[(int) data])));

  average = floor ((left_value + right_value) / 2);

  gtk_adjustment_set_value
    (gtk_range_get_adjustment (GTK_RANGE (scale[(int) (data - 1)])), average);

  gtk_adjustment_set_value
    (gtk_range_get_adjustment (GTK_RANGE (scale[(int) data])), average);
  return (TRUE);
}


gint
app_change_vols (GtkWidget * widget, gpointer data)
{
  int right = ((int) data & 0x0000FF);
  int left = (((int) data & 0x00FF00) >> 8);
  int dev = (((int) data & 0xFF0000) >> 16);

  int new_vol_right;
  int new_vol_left;
  int new_vol;

  if (toggle[right] != NULL && GTK_TOGGLE_BUTTON (toggle[right])->active)
  {

      lock_levels (toggle[right], (gpointer) right);


  }

  new_vol_right =
    100 - (int) gtk_adjustment_get_value (gtk_range_get_adjustment
			      (GTK_RANGE (scale[right])));

  new_vol_left =
    100 - (int) gtk_adjustment_get_value (gtk_range_get_adjustment
			      (GTK_RANGE (scale[left])));

  new_vol = ((new_vol_right << 8) | (new_vol_left));


  if (DEBUG)
  {
      g_print ("app_change_vols: %10s to %6d %6d ", channel_names[dev],
	       new_vol_right, new_vol_left);
  }

  if (!mixer_change_vols (dev, new_vol))
  {
      printf ("Fatal, Cant change vols!\n");
  }
  return(TRUE);
}

gint
open_about (GtkWidget * widget, gpointer data)
{
  GtkWidget *about_window;
  GtkWidget *close_button;
  GtkWidget *about_label1;
  GtkWidget *packing_table;
  GtkWidget *about_image;
  GdkPixbuf *about_pixbuf;

  about_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (about_window), "ADGMix - About");

  gtk_container_set_border_width (GTK_CONTAINER (about_window), 10);
  close_button = gtk_button_new_with_label ("Close");
  
  packing_table = gtk_table_new(3,1,FALSE);
  
  about_label1 = gtk_label_new("Another Damn Gtk Mixer\n"\
  "http://sv.nongnu.org/projects/adgmix\n"\
  "Coded by Reynaldo H. Verdejo Pinochet\n"\
  "rverdejo@opendot.cl");
  
  about_pixbuf = gdk_pixbuf_new_from_xpm_data(logo_xpm);
  about_image = gtk_image_new_from_pixbuf(about_pixbuf);
  
  gtk_table_attach_defaults(GTK_TABLE(packing_table),about_image,0,1,0,1);
  gtk_table_attach_defaults(GTK_TABLE(packing_table),about_label1,0,1,1,2);
  gtk_table_attach_defaults(GTK_TABLE(packing_table),close_button,0,1,2,3);

  gtk_label_set_pattern (GTK_LABEL (about_label1),"______________________");
  gtk_label_set_line_wrap (GTK_LABEL (about_label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (about_label1), GTK_JUSTIFY_CENTER);

  g_signal_connect_swapped (G_OBJECT (close_button), "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    G_OBJECT (about_window));
  gtk_container_add (GTK_CONTAINER (about_window), packing_table);
  
  gtk_widget_show (close_button);
  gtk_widget_show (about_label1);
  gtk_widget_show (about_image);
  gtk_widget_show (packing_table);
  gtk_widget_show (about_window);

  return(TRUE);
}

int
get_table_cols_needed (void)
{
  int stereo_devs, supported_mask;
  int i, k, l;
  l = 0;




  if (mixer_open () == -1)
  {
      g_print ("Unable to open %s, exit forced\n", MIXER);
      /* FATAL cant open device */
      gtk_main_quit ();		/* quit */

  }

  supported_mask = mixer_check_available_chans ();
  stereo_devs = mixer_check_stereo_chans ();

  if (supported_mask == -1)	/* chans supported mask */
  {
      g_print ("Unable to check mixer devs, exit forced\n");
      gtk_main_quit ();
  }

  if (stereo_devs == -1)	/* who is stereo? */
  {
      g_print ("Unable to check stereodevs, exit forced\n");
      gtk_main_quit ();
  }

/* main loop
 * here we check for available channels and
 * wich of they are stereo o mono, then
 * we build the scales for each one 
 */
  for (i = 0; i < MAX_ELEMENTS; i++)
  {
    if (!(supported_mask & (1 << i)))
    {
      if (DEBUG)
	      fprintf (stderr, "%s Not present\n", channel_labels[i]);
	}else
	{
	  k = mixer_check_vol (i);
	  if (k == -1)
	      fprintf (stderr, "Error trying to read from %s \n", channel_labels[i]);

/* take notice, k mask (chan vol) - <sys/soundcard.h>
 * 0000 0000 0000 0000 0000 0000 0000 0000
 * <-Undefined bytes-> --------- ---------
 *                         |        |
 *                         |        |--> Left channel vol 
 *                         |
 *                         |-----------> Right channel vol
 *
 * left channel = overall for mono  
 */

	  if (stereo_devs & (1 << i))
	  {
	      /* stereo chans here */
	      if (DEBUG)
		    {
		      g_print ("%6s Present (%d,%d) - Stereo\n",
			    channel_labels[i], (k & 0xFF), ((k & 0x0000FF00) >> 8));
		    }
	      /* left */
	      scale[l] = gtk_vscale_new_with_range (0, 100, 1);
	      gtk_adjustment_set_value (gtk_range_get_adjustment
					(GTK_RANGE (scale[l])),
					100 - (k & 0xFF));

	      sprintf (labels_text[l], "%s", channel_labels[i]);

	      l++;
	      /* right */
	      scale[l] = gtk_vscale_new_with_range (0, 100, 1);
	      gtk_adjustment_set_value (gtk_range_get_adjustment
					(GTK_RANGE (scale[l])),
					100 - ((k & 0x0000FF00) >> 8));

	      toggle[l] = gtk_toggle_button_new_with_label ("Lock");

/* here we connect the bars changes with the app_change_vols function
 * stereo chans here
 */

	      gtk_signal_connect (GTK_OBJECT (toggle[l]), "toggled",
				  GTK_SIGNAL_FUNC (lock_levels),
				  (gpointer) l);

	      gtk_signal_connect (GTK_OBJECT
				  (gtk_range_get_adjustment
				   (GTK_RANGE (scale[l - 1]))),
				  "value_changed",
				  GTK_SIGNAL_FUNC (app_change_vols),
				  (gpointer) build_chan_mask (i, (l - 1), l));

	      gtk_signal_connect (GTK_OBJECT
				  (gtk_range_get_adjustment
				   (GTK_RANGE (scale[l]))), "value_changed",
				  GTK_SIGNAL_FUNC (app_change_vols),
				  (gpointer) build_chan_mask (i, (l - 1), l));
	    }else
	    {
	      /* mono chans here */
	      if (DEBUG)
		      g_print ("%6s Present (%d)  - Mono\n", channel_labels[i], (k & 0xFF));

	      /* left - overall */
	      scale[l] = gtk_vscale_new_with_range (0, 100, 1);

	      gtk_adjustment_set_value (gtk_range_get_adjustment
					(GTK_RANGE (scale[l])),
					100 - (k & 0xFF));

	      sprintf (labels_text[l], "%s", channel_labels[i]);

	      gtk_signal_connect (GTK_OBJECT
				  (gtk_range_get_adjustment
				   (GTK_RANGE (scale[l]))), "value_changed",
				  GTK_SIGNAL_FUNC (app_change_vols),
				  (gpointer) build_chan_mask (i, l, l));
	    }

	  l++;

	  }
  }

  return l;


}


int
main (int argc, char *argv[])
{
  /* what do we need here */

  GtkWidget *window;		/* main window */
  GtkWidget *menubar;
  GtkWidget *file_menu;
  GtkWidget *help_menu;
  GtkWidget *file_item;
  GtkWidget *help_item;
  GtkWidget *exit_menu_item;
  GtkWidget *about_menu_item;
  GtkWidget *table;		/* fotmat table */
  GtkWidget *label_text;
  
  int cols_needed, i, j = 1;
  /* needed counters */

  gtk_init (&argc, &argv);	/* all sistems clear */
  /* create the main window */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "ADGMix");
  /* gtk_container_set_border_width(GTK_CONTAINER(window),5); */


  /* standar windowmngr want to close us calls */

  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (gtk_exit), NULL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC (gtk_exit), NULL);

  /* how many channels do we have? */



  cols_needed = get_table_cols_needed ();

  /* create the main table */

  table = gtk_table_new (5, cols_needed, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), (gint) 4);
  gtk_table_set_row_spacings (GTK_TABLE (table), (gint) 4);
  gtk_container_add (GTK_CONTAINER (window), table);

  /* menu bar */

  menubar = gtk_menu_bar_new ();

  file_menu = gtk_menu_new ();
  help_menu = gtk_menu_new ();

  exit_menu_item = gtk_menu_item_new_with_label ("Quit");
  about_menu_item = gtk_menu_item_new_with_label ("About");
  gtk_menu_append (GTK_MENU (file_menu), exit_menu_item);
  gtk_menu_append (GTK_MENU (help_menu), about_menu_item);

  g_signal_connect_swapped (G_OBJECT (exit_menu_item),
			    "activate", G_CALLBACK (close_all),
			    NULL);
  g_signal_connect (G_OBJECT (about_menu_item), "activate",
		    GTK_SIGNAL_FUNC (open_about), NULL);

  file_item = gtk_menu_item_new_with_label ("File");
  help_item = gtk_menu_item_new_with_label ("Help");


  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_item), file_menu);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (help_item), help_menu);

  gtk_menu_bar_append (GTK_MENU_BAR (menubar), file_item);
  gtk_menu_bar_append (GTK_MENU_BAR (menubar), help_item);

  gtk_table_attach_defaults (GTK_TABLE (table), menubar, 0,
			     cols_needed, 0, 1);

  gtk_widget_show (exit_menu_item);
  gtk_widget_show (about_menu_item);
  gtk_widget_show (file_item);
  gtk_widget_show (help_item);
  gtk_widget_show (menubar);


  /* attach chanel scales */
  for (i = 0; i < cols_needed; i++)
  {
    if (scale[i] != NULL)
    {
	    label_text = gtk_label_new (labels_text[i]);
	    gtk_table_attach_defaults (GTK_TABLE (table), label_text, (j - 1),
			                    	     j, 1, 2);
	    gtk_widget_set_size_request (scale[i], -1, 80);
	    gtk_table_attach_defaults (GTK_TABLE (table), scale[i], (j - 1), j,
			                    	     2, 3);

	  if (toggle[i] != NULL)
	  {
	    gtk_table_attach_defaults(GTK_TABLE (table), toggle[i], (j - 2), j, 3, 4);
	    gtk_widget_show (toggle[i]);
	  }
	  /* all systems clear :-) */
	  if (!DEBUG)
      gtk_scale_set_draw_value (GTK_SCALE (scale[i]), FALSE);

	  gtk_widget_show (scale[i]);
	  gtk_widget_show (label_text);
	  j++;

	  }
  }

  gtk_widget_show (table);
  gtk_widget_show (window);
  gtk_main ();

  return (0);
}
