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

#include <sys/soundcard.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define MIXER "/dev/mixer"
#define MAX_ELEMENTS SOUND_MIXER_NRDEVICES

int build_chan_mask (int device_subs, int left_subs, int right_subs);
int mixer_change_vols (int dev, int new_vol);
int mixer_check_vol (int dev);
int mixer_check_available_chans (void);
int mixer_check_stereo_chans (void);
int mixer_open (void);
void mixer_close (void);
