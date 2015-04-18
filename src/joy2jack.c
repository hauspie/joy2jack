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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <jack/jack.h>
#include <jack/midiport.h>

#include "joystick.h"
#include "midi.h"
#include "config.h"

static jack_port_t *output_midi_port;

#define JACK_CLIENT_NAME_BASE "joy2jack"

#define MAX_MIDI_NOTES 256

jack_midi_data_t note_queue[3*MAX_MIDI_NOTES]; /* Status, Note, Velocity */
static int notes_count = 0;

static vector_t mappings;


void fatal_error(const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
   exit(1);
}

/* Called by jack when if the server shuts down or disconnects the client */
void jack_shutdown()
{
   exit(1);
}


int event_match(struct js_event *joy_event, event_t *mapping_event)
{
   if (joy_event->number != mapping_event->number)
      return 0;
   if (joy_event->type == JS_EVENT_BUTTON)
   {
      if (!BUTTON_EVENT(mapping_event->type))
         return 0;
      switch (mapping_event->type)
      {
         case PRESSED:   return joy_event->value == 1;
         case RELEASED:  return joy_event->value == 0;
         case PUSHED:    return 1; /* PUSHED event always matches */
         default: break;
      }
      return 0;
   }
   if (joy_event->type == JS_EVENT_AXIS)
   {
      if (!AXIS_EVENT(mapping_event->type))
         return 0;
      if (joy_event->value == mapping_event->value)
         return 1;
      return 0;
   }
   return 0;
}

mapping_t *get_matching_mapping(struct js_event *e)
{
   mapping_t *mappings_array = (mapping_t *) mappings.element_array;
   int i;
   for (i = 0 ; i < mappings.element_count ; ++i)
   {
      mapping_t *m = &mappings_array[i];
      if (event_match(e, &m->event))
         return m;
   }
   return NULL;
}

void joy_event(struct js_event *e)
{
   mapping_t *m = get_matching_mapping(e);
   if (!m)
      return;

   
   if (NOTE_ACTION(m->action.type))
   {
      jack_midi_data_t *note = note_queue + 3*notes_count; /* Status, Note, Velocity */

      note[1] = m->action.parameter.note.note;
      note[2] = m->action.parameter.note.velocity;
      int onoff = 0;
      switch (m->action.type)
      {
         case NOTEON: onoff = NOTE_ON; break;
         case NOTEOFF: onoff = NOTE_OFF; break;
         case NOTE: onoff = e->value ? NOTE_ON : NOTE_OFF; break;
         default: return;
      }
      note[0] = onoff | (m->action.channel-1);
      notes_count++;
   }
}

int process()
{
   void *port_buffer = jack_port_get_buffer(output_midi_port, 1);
   jack_midi_clear_buffer(port_buffer);
   if (notes_count == 0)
      return 0;
   int i;
   for (i = 0 ; i < notes_count; ++i)
      jack_midi_event_write(port_buffer, 0, note_queue+i*3, 3);

   notes_count = 0;
   return 0;
}

int main(int argc, char **argv)
{

   if (argc >= 2)
      parse_config_file(argv[1], &mappings);
   else
   {
      fprintf(stderr, "usage: %s config_file [joystick_device_file]\n", argv[0]);
      return 1;
   }
/*   TODO: generate a default mapping if no config file is present
     generate_default_mapping(&mapping);
 */
     
   
   
   jack_client_t *client;
   joystick_t joy;

   client = jack_client_open(JACK_CLIENT_NAME_BASE, JackNullOption | JackNoStartServer, NULL);
   if (!client)
      fatal_error("Failed to connect to jack server\n");

   jack_set_process_callback (client, process, 0);
   
   output_midi_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
                                         JackPortIsOutput | JackPortIsTerminal | JackPortIsPhysical, 0);

   if (!output_midi_port)
      fatal_error("Failed to create midi port\n");

   const char *joyfile = "/dev/input/js0";
   if (argc >= 3)
      joyfile = argv[2];
   if (initialize_joystick(joyfile, &joy) == -1)
      fatal_error("Failed to initialize joystick\n");

   printf("Initialized joystick %s, %d axes, %d buttons\n", joy.name, joy.axes_count, joy.buttons_count);

   jack_activate(client);
   
   for(;;)
   {
      struct js_event e;
      if (read_joystick_event(&joy, &e) == -1)
         fatal_error("Failed to read joystick event\n");
      joy_event(&e);
   }
   return 0;
}
