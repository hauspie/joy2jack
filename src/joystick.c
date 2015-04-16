/* This file is part of joy2jack. Copyright  2015- Michael Hauspie
 *
 * joy2jack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * joy2jack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with joy2jack.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <linux/joystick.h>


#include "joystick.h"

int initialize_joystick(const char *joyfile_path, joystick_t *description)
{
   int fd = open(joyfile_path, O_RDONLY);

   if (fd == -1)
   {
      perror(joyfile_path);
      return -1;
   }

   if (description)
   {
      description->fd = fd;
      ioctl(fd, JSIOCGAXES, &description->axes_count);
      ioctl(fd, JSIOCGBUTTONS, &description->buttons_count);
      memset(description->name, 0, MAX_CONTROLER_NAME_SIZE);
      ioctl(fd, JSIOCGNAME(MAX_CONTROLER_NAME_SIZE), description->name);
   }
   return fd;
}


int read_joystick_event(joystick_t *joystick, struct js_event *event)
{
   if (read(joystick->fd, event, sizeof(struct js_event)) <= 0)
      return -1;
   return 1;
}
