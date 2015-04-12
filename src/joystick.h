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
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include <stdint.h>
#include <linux/joystick.h>

#define MAX_CONTROLER_NAME_SIZE 128

typedef struct
{
   int fd;
   char name[MAX_CONTROLER_NAME_SIZE];
   uint8_t axes_count;
   uint8_t buttons_count;
} joystick_t;

/** Opens a joystick file and sets the needed ioctl to use it
 * correctly 
 *
 * @param [out] joystick gets filled with joystick description
 * @return -1 on error, positive value on success
 */
int initialize_joystick(const char *joyfile_path, joystick_t *joystick);

/** Wait until next joystick event then returns a js_event structure
 * describing the event (see linux/joystick.h for structure description)
 *
 * @return -1 on error, positive value on success
 */
int read_joystick_event(joystick_t *joystick, struct js_event *event);


#endif
