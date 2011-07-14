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

#include "oss_functions.h"

extern int
build_chan_mask (int device_subs, int left_subs, int right_subs)
{
/* Here we build a binary wich will hold the subscripts 
* to find the needed device,right and left in the arrays
*
* 0000 0000 0000 0000 0000 0000
* --------- --------- ---------
*   |          |       |
*   |          |       ---> right chanel subscript
*   |          |----------> left chanel subscript
*   |---------------------> device subscript
*
*/
  return ((device_subs << 16) | (left_subs << 8) | (right_subs));
}

int
mixer_change_vols (int dev, int new_vol)
{
  extern int mixerfd;
  if (ioctl (mixerfd, MIXER_WRITE (dev), &new_vol) == -1) {
    return (0);
  } else {
    return (1);
  }
}

int
mixer_check_vol (int dev)
{
  extern int mixerfd;
  int k;
  if (ioctl (mixerfd, MIXER_READ (dev), &k) == -1) {
    return (-1);
  } else {
    return (k);
  }
}

int
mixer_check_available_chans (void)
{
  extern int mixerfd;
  int k;
  if (ioctl (mixerfd, MIXER_READ (SOUND_MIXER_DEVMASK), &k) == -1) {
    return (-1);
  } else {
    return (k);
  }
}

int
mixer_check_stereo_chans (void)
{
  extern int mixerfd;
  int k;
  if (ioctl (mixerfd, MIXER_READ (SOUND_MIXER_STEREODEVS), &k) == -1) {
    return (-1);
  } else {
    return (k);
  }
}

int
mixer_open (void)
{
  extern int mixerfd;
  mixerfd = open (MIXER, O_RDWR);
  if (mixerfd < 0) {
    return (-1);
  } else {
    return (0);
  }
}

void
mixer_close (void)
{
  extern int mixerfd;
  if (close (mixerfd) == -1) {
  }
}
