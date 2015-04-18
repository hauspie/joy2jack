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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "vector.h"

typedef struct
{
   enum {
      PRESSED,  /* pressed() */
      RELEASED, /* released() */
      PUSHED,   /* pushed() */
      AXIS,     /* axis */
      UNKNOW_EVENT,
   } type;
   int number; /* Button or axis number */
   int value;  /* for axes, the value can be more complicated than just pressed and released */
   
} event_t;

#define BUTTON_EVENT(t) ((t) >= PRESSED && (t) <= PUSHED)
#define AXIS_EVENT(t) ((t) == AXIS)


typedef struct
{
   enum {
      NOTEON,    /* noteon */
      NOTEOFF, /* noteoff */
      NOTE,    /* note */
      UNKNOW_ACTION,
   } type;
   union
   {
      struct {
         int note, velocity;
      } note;
   } parameter;
   int channel;
} action_t;

#define NOTE_ACTION(t) ((t) >= NOTEON && (t) <= NOTE)

typedef struct
{
   event_t event;
   action_t action;
} mapping_t;


/** Fills a mapping_list_t array of action mapped on events */
int parse_config_file(const char *path, vector_t *mapping);



#endif
