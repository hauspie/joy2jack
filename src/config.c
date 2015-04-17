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
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "vector.h"

#define MAX_TOKEN_LENGTH 128

typedef struct {
   char symbol[MAX_TOKEN_LENGTH];
   int  value;
} symbolic_value_t;


static vector_t symbols; /* global symbol list. Is reset every time parse_config_file is called */

#define ACTION_TYPE_IS_NOTE(t) ((t) >= MIDI_NOTE_START && (t) <= MIDI_NOTE_END)


/* Strips comments and ending space charaters and returns a pointer on
 * the first non space charater.
 * DOES modify the string!
 */
static char *chomp(char *buffer)
{
   char *ptr = buffer;

   /* Strip first spaces */
   while (*ptr && isspace(*ptr))
      ++ptr;
   if (*ptr == '\0')
      return ptr;
   char *start = ptr;
   /* Find comments */
   while (*ptr && *ptr != '#')
      ++ptr;
   /* Even if '#' was not found, it does not harm and just overwrites
    * \0 */
   *ptr = '\0';
   if (ptr == buffer)
      return ptr;
   /* strip last spaces */
   --ptr;
   while (ptr != start && isspace(*ptr))
      --ptr;
   if (ptr != start)
      ptr[1] = '\0';
   return start;
}

static char *subcpy(char *dst, const char *str, int so, int eo)
{
   strncpy(dst, str+so, eo-so);
   dst[eo-so] = '\0';
   return dst;
}

int create_symbol_list(vector_t *list, int starting_size)
{
   return vector_create(list, starting_size, sizeof(symbolic_value_t));
}

int add_symbol(vector_t *list, const char *name, int value)
{
   symbolic_value_t s;
   strncpy(s.symbol, name, MAX_TOKEN_LENGTH-1);
   s.symbol[MAX_TOKEN_LENGTH-1] = '\0';
   s.value = value;

   return vector_add_value(list, &s);
}

/* Returns the value of the symbol. Warning, returns -1 if not found
   but as it is, symbols can not have negative values (because of the
   regexp used to parse)
*/
int get_symbol_value(vector_t *list, const char *name)
{
   int i;
   symbolic_value_t *symbols = (symbolic_value_t*) list->element_array;
   for (i = 0 ; i < list->element_count ; ++i)
   {
      symbolic_value_t *s = &symbols[i];
      if (strncmp(s->symbol, name, MAX_TOKEN_LENGTH) == 0)
         return s->value;
   }
   if (strncmp(name, "DEFAULT_VELOCITY", MAX_TOKEN_LENGTH) == 0)
      return 100;
   if (strncmp(name, "DEFAULT_MIDI_CHANNEL", MAX_TOKEN_LENGTH) == 0)
      return 1;
   return -1;
}

void display_symbol_list(vector_t *list)
{
   int i;
   symbolic_value_t *symbols = (symbolic_value_t*) list->element_array;
   for (i = 0 ; i < list->element_count ; ++i)
   {
      symbolic_value_t *s = &symbols[i];
      printf("%s = %d\n", s->symbol, s->value);
   }
}

void display_mapping_list(vector_t *list)
{
   int i;
   mapping_t *mappings = (mapping_t*) list->element_array;
   for (i = 0 ; i < list->element_count ; ++i)
   {
      mapping_t *m = &mappings[i];
      printf("%d(%d, %d) -> %d(", m->event.type, m->event.number, m->event.value, m->action.type);
      if (ACTION_TYPE_IS_NOTE(m->action.type))
         printf("%d,%d,", m->action.parameter.note.note, m->action.parameter.note.velocity);
      printf("%d)\n", m->action.channel);
   }
}

#define RETURN_IF_MATCH(token, value, retvalue) do {            \
      if (strncmp((token), (value), MAX_TOKEN_LENGTH) == 0)     \
         return (retvalue);                                     \
   } while (0)

int action_token_to_enum(const char *token)
{
   RETURN_IF_MATCH(token, "noteon", NOTEON);
   RETURN_IF_MATCH(token, "noteoff", NOTEOFF);
   RETURN_IF_MATCH(token, "note", NOTE);
   return UNKNOW_ACTION;
}

int event_token_to_enum(const char *token)
{
   RETURN_IF_MATCH(token, "pressed", PRESSED);
   RETURN_IF_MATCH(token, "released", RELEASED);
   RETURN_IF_MATCH(token, "pushed", PUSHED);
   RETURN_IF_MATCH(token, "axis", AXIS);
   return UNKNOW_EVENT;
}

/* parse a list a integer value (with possible symbolic value), fills
 * the array with maximum max_values and returns the actual number of
 * values
 */

int build_val_array(int *array, int max_values, char *param_string)
{
   int i;
   char *ptr = strtok(param_string, ",");
   for (i = 0 ; ptr && i < max_values; ++i)
   {
      ptr = chomp(ptr);
      array[i] = get_symbol_value(&symbols, ptr);
      if (array[i] == -1)
         array[i] = atoi(ptr);
      ptr = strtok(NULL, ",");
   }
   return i;
}

/* build event: params are: number, value */
int build_event(event_t *event, char *param_string)
{
   int vals[2];
   int n = build_val_array(vals, 2, param_string);
   if (n <= 0)
      return -1;
   event->number = vals[0];
   if (n == 2)
      event->value = vals[1];
   else
      event->value = -1;
   return 0;
}

/* Action param is
   - for note, noteon, noteoff: note [, velocity, channel]
*/
int build_action(action_t *action, char *param_string)
{
   if (action->type >= MIDI_NOTE_START && action->type <= MIDI_NOTE_END)
   {
      int vals[3];
      int n = build_val_array(vals, 2, param_string);
      if (n <= 0)
         return -1;
      action->parameter.note.note = vals[0];
      if (n >= 2 && vals[1] != -1)
         action->parameter.note.velocity = vals[1];
      else
         action->parameter.note.velocity = get_symbol_value(&symbols, "DEFAULT_VELOCITY");
      if (n >= 3 && vals[2] != -1)
         action->channel = vals[2];
      else
         action->channel = get_symbol_value(&symbols, "DEFAULT_MIDI_CHANNEL");
      return 0;
   }
   return -1;
}



int add_mapping(vector_t *mapping_list, const char *event, char *event_params,
                const char *action, char *action_params)
{
   mapping_t mapping;
   mapping.event.type = event_token_to_enum(event);
   mapping.action.type = action_token_to_enum(action);
   if (mapping.event.type == UNKNOW_EVENT || mapping.action.type == UNKNOW_ACTION)
      return -1;
   build_event(&mapping.event, event_params);
   build_action(&mapping.action, action_params);
   vector_add_value(mapping_list, &mapping);
   return 0;
}



int parse_config_file(const char *path, vector_t *mapping_list)
{
   FILE *f;

   f = fopen(path, "r");
   if (!f)
   {
      perror(path);
      return -1;
   }

   if (create_symbol_list(&symbols, 10) == -1)
   {
      fclose(f);
      return -1;
   }

   if (vector_create(mapping_list, 10, sizeof(mapping_t)) == -1)
   {
      fclose(f);
      vector_free(&symbols);
      return -1;
   }
   
   /* Build regex for parsing lines */
   regex_t assignments;
   regex_t mappings;

   if (regcomp(&assignments, "([[:alnum:]_]+)[[:space:]]*=[[:space:]]*([[:digit:]]*)", REG_EXTENDED) != 0)
   {
      perror("regcomp(assignments)");
      vector_free(&symbols);
      vector_free(mapping_list);
      fclose(f);
      return -1;
   }

   if (regcomp(&mappings, "([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)[[:space:]]*->[[:space:]]*([[:alnum:]_]+)\\(([[:alnum:][:space:],_]+)\\)", REG_EXTENDED) != 0)
   {
      perror("regcomp(mappings)");
      vector_free(&symbols);
      vector_free(mapping_list);
      fclose(f);
      return -1;
   }

   
   char buffer[1024]; /* 1024 bytes lines should be large enough :p */

   int line = 0;
   while (fgets(buffer, sizeof(buffer), f))
   {
      char *ptr = chomp(buffer);
      ++line;
      if (*ptr == '\0')
         continue;
      regmatch_t pmatch[5];
      if (regexec(&assignments, ptr, 3, pmatch, 0) == 0)
      {
         char symbol[256], value[256];
         subcpy(symbol, ptr, pmatch[1].rm_so, pmatch[1].rm_eo);
         subcpy(value, ptr, pmatch[2].rm_so, pmatch[2].rm_eo);
         add_symbol(&symbols, symbol, atoi(value));
         continue;
      }

      if (regexec(&mappings, ptr, 5, pmatch, 0) == 0)
      {
         char event[256], event_param[256], action[256], action_param[256];
         subcpy(event, ptr, pmatch[1].rm_so, pmatch[1].rm_eo);
         subcpy(event_param, ptr, pmatch[2].rm_so, pmatch[2].rm_eo);
         subcpy(action, ptr, pmatch[3].rm_so, pmatch[3].rm_eo);
         subcpy(action_param, ptr, pmatch[4].rm_so, pmatch[4].rm_eo);
         printf("New mapping: %s(%s) -> %s(%s)\n", event, event_param, action, action_param);
         add_mapping(mapping_list, event, event_param, action, action_param);
         continue;
      }
      fprintf(stderr, "Warning: possible bogus line: %s:%d\n", path, line);
   }
   display_symbol_list(&symbols);
   display_mapping_list(mapping_list);
   regfree(&assignments);
   regfree(&mappings);
   vector_free(&symbols);
   vector_free(mapping_list);
   fclose(f);
   return 0;
}
