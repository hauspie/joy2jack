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
#include <jack/jack.h>

static jack_port_t *output_midi_port;

#define JACK_CLIENT_NAME_BASE "joy2jack"

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

int main(int argc, char **argv)
{
   jack_client_t *client;

   client = jack_client_open(JACK_CLIENT_NAME_BASE, JackNullOption, NULL);
   if (!client)
      fatal_error("Failed to connect to jack server\n");


   output_midi_port = jack_port_register(client, "midi_out", JACK_DEFAULT_MIDI_TYPE,
                                         JackPortIsOutput | JackPortIsTerminal | JackPortIsPhysical, 0);

   if (!output_midi_port)
      fatal_error("Failed to create midi port\n");
   
   for(;;);
   return 0;
}
