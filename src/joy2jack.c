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
#include "joystick_mapping.h"
#include "midi.h"

static jack_port_t *output_midi_port;

#define JACK_CLIENT_NAME_BASE "joy2jack"

#define MAX_MIDI_NOTES 256

jack_midi_data_t note_queue[3*MAX_MIDI_NOTES]; /* Status, Note, Velocity */
static int current_note = -1;


void fatal_error(const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
   exit(1);
}

/* Called by jack when if the server shuts down or disconnects the client */
void jack_shutdown(void *arg)
{
   exit(1);
}


int button_to_note(uint8_t button)
{
   return (button + 36) & 0x7f;
}

void joy_event(struct js_event *e)
{
   static int current_midi_channel = 0;
   static int send_note_off = 0;
   printf("Joy event: %d %d %d\n", e->type, e->number, e->value);
   if (CHECK_EVENT(*e, MIDI_NEXT_CHANNEL))
   {
      current_midi_channel++;
      if (current_midi_channel == 16)
         current_midi_channel = 0;
      printf("Switching to channel %d\n", current_midi_channel);
      return;
   }

   if (CHECK_EVENT(*e, MIDI_PREVIOUS_CHANNEL))
   {
      if (current_midi_channel == 0)
         current_midi_channel = 15;
      else
         current_midi_channel--;

      printf("Switching to channel %d\n", current_midi_channel);
      return;
   }

   if (CHECK_EVENT(*e, MIDI_SEND_NOTE_OFF))
   {
      send_note_off = 1;
      printf("Now sending note off\n");
      return;
   }

   if (CHECK_EVENT(*e, MIDI_DONT_SEND_NOTE_OFF))
   {
      send_note_off = 0;
      printf("Do not send note off anymore\n");
      return;
   }

      
   if (e->type != JS_EVENT_BUTTON) /* Only use buttons yet */
      return;

   if (!e->value && !send_note_off) /* Do not send NOTE_OFF */
      return;
   current_note++;
   jack_midi_data_t *note = note_queue + 3*current_note; /* Status, Note, Velocity */
   /* See http://www.midi.org/techspecs/midimessages.php for MIDI message specification */
   if (e->value)
      note[0] = NOTE_ON | current_midi_channel;
   else
      note[0] = NOTE_OFF | current_midi_channel;
   note[1] = button_to_note(e->number);
   note[2] = 100;//MIDI_MAX_VELOCITY; /* max velocity */
   printf("Sending(%d) %x %d %d\n", e->type, note[0], note[1], note[2]);
}

int process(jack_nframes_t nframes, void *arg)
{


   void *port_buffer = jack_port_get_buffer(output_midi_port, 1);
   jack_midi_clear_buffer(port_buffer);
   if (current_note == -1)
      return 0;
   printf("Sending %d notes to jack\n", current_note+1);
   jack_midi_data_t *midi_data = jack_midi_event_reserve(port_buffer, 0, (current_note+1)*3);
   memcpy(midi_data, note_queue, (current_note+1)*3);
   current_note = -1;
   return 0;
}

int main(void)
{
   jack_client_t *client;
   joystick_t joy;

   client = jack_client_open(JACK_CLIENT_NAME_BASE, JackNullOption, NULL);
   if (!client)
      fatal_error("Failed to connect to jack server\n");

   jack_set_process_callback (client, process, 0);
   
   output_midi_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
                                         JackPortIsOutput | JackPortIsTerminal | JackPortIsPhysical, 0);

   if (!output_midi_port)
      fatal_error("Failed to create midi port\n");

   if (initialize_joystick("/dev/input/js0", &joy) == -1)
      fatal_error("Failed to initialize joystick\n");

   printf("%d\n", jack_activate(client));
   
   printf("Initialized joystick %s, %d axes, %d buttons\n", joy.name, joy.axes_count, joy.buttons_count);

   
   for(;;)
   {
      struct js_event e;
      if (read_joystick_event(&joy, &e) == -1)
         fatal_error("Failed to read joystick event\n");
      joy_event(&e);
   }
   return 0;
}
