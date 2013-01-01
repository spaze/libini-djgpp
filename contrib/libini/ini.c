/*************************************************************************\
* LibINI 1.1                                                              *
* ~~~~~~~~~~                                                              *
* Copyright 1999 STEALTH Technologies                                     *
* [stealth.tech@seznam.cz, http://stealthtech.tsx.org]                    *
* See 'libini.txt' and 'copying.lib' for details                          *
*                                                                         *
* Library for work with INI files                                         *
\*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ini.h>

int ini_new(INI_NODE **ARG_start)
/*************************************************************************
 * Creates new INI list in memory starting @ 'ARG_start'                 *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 *************************************************************************/
{
  if ((*ARG_start = (INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
    return(1);

  (*ARG_start)->node_id = 1;
  (*ARG_start)->prev_node = (*ARG_start)->next_node = NULL;
  if (((*ARG_start)->comment = (char *)malloc(1)) == NULL)
    return(1);
  strcpy((*ARG_start)->comment, "\0");

  return(0);
}        /* ini_new */

int ini_load(INI_NODE **ARG_start, const char *ARG_file)
/*************************************************************************
 * Loads INI 'ARG_file' into memory starting @ 'ARG_start'               *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 2 - fopen failed                                                      *
 * 6 - file is empty                                                     *
 *************************************************************************/
{
  char *text;
  FILE *file;
  INI_NODE *node, *temp_node;

  if ((text = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  if ((*ARG_start = node = temp_node = (INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
    return(1);

  node->prev_node = node->next_node = NULL;

  if ((file = fopen(ARG_file, "rt")) == NULL)
    return(2);

  fseek(file, 0, SEEK_END);
  if (ftell(file) == 0)
    return(6);

  fseek(file, 0, SEEK_SET);
  while (fgets(text, MAX_LINE, file) != NULL) {
    strcat(text, "\0");
    if (text[0] == ';' || text[0] == '#') {
      node->node_id = 1;
      if ((node->comment = (char *)malloc(strlen(text))) == NULL)
        return(1);
      strcpy(node->comment, text);
      temp_node = node;
    }
    else
      if (strchr(text, '[') != NULL && strchr(text, ']') != NULL && strchr(text, '=') == NULL && (text[0] != ';' || text[0] != '#')) {
        node->node_id = 2;
        text[strcspn(text, "]")] = '\0';
        if ((node->section = (char *)malloc(strlen(strchr(text, '[') + 1) + 1)) == NULL)
          return(1);
        strcpy(node->section, strchr(text, '[') + 1);
        temp_node = node;
      }
      else
        if (strchr(text, '=') && text[0] != ';' && text[0] != '#') {
          node->node_id = 3;
          if ((node->value = (char *)malloc(strchr(text, '\0') - strchr(text, '=') + 1)) == NULL)
            return(1);
          strcpy(node->value, strchr(text, '=') + 1);
          if ((node->profile = (char *)malloc(strchr(text, '=') - text + 1)) == NULL)
            return(1);
          strcpy(node->profile, strtok(text, "="));
          strcat(node->profile, "\0");
          temp_node = node;
        }
        else
        {
          node->node_id = 1;
          if ((node->comment = (char *)malloc(strlen(text) + 1)) == NULL)
            return(1);
          strcpy(node->comment, text);
          temp_node = node;
        }

    if ((node = (INI_NODE *)node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
      return(1);
    node->prev_node = (struct INI_NODE *)temp_node;
  }

  fclose(file);

  free(text);

  free(node);
  temp_node->next_node = NULL;

  return(0);
}        /* ini_load */

int ini_flush(INI_NODE *ARG_start, const char *ARG_file)
/*************************************************************************
 * Writes INI 'ARG_file' from memory starting @ 'ARG_start'              *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 2 - fopen failed                                                      *
 * 3 - fprintf failed                                                    *
 *************************************************************************/
{
  FILE *file;
  INI_NODE *node;

  if ((file = fopen(ARG_file, "wt")) == NULL)
    return(2);

  node = ARG_start;
  do {
    switch (node->node_id) {
      case 1:
        if ((unsigned)fprintf(file, "%s", node->comment) != strlen(node->comment))
          return(3);
        break;

      case 2:
        if ((unsigned)fprintf(file, "[%s]\n", node->section) != strlen(node->section) + 3)
          return(3);
        break;
        
      case 3:
        if ((unsigned)fprintf(file, "%s=%s", node->profile, node->value) != strlen(node->profile) + strlen(node->value) + 1)
          return(3);
        break;
    }
  } while ((node = (INI_NODE *)node->next_node) != NULL);

  fclose(file);

  return(0);
}        /* ini_flush */

void ini_free(INI_NODE *ARG_start)
/*************************************************************************
 * Frees memory allocated for INI file starting @ 'ARG_start'            *
 *************************************************************************/
{
  INI_NODE *node, *temp_node;

  node = ARG_start;

  do {
    temp_node = (INI_NODE *)node->next_node;
    switch (node->node_id) {
      case 1:
        free(node->comment);
        break;

      case 2:
        free(node->section);
        break;

      case 3:
        free(node->value);
        free(node->profile);
        break;
    }
    free(node);
    node = temp_node;
  } while (node != NULL);
}        /* ini_free */

int ini_get_string(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, char *ARG_value)
/*************************************************************************
 * Gets string from INI file  starting @ 'ARG_start', from 'ARG_section' *
 * and 'ARG_profile' to 'ARG_value'                                      *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 4 - section not found                                                 *
 * 5 - profile not found                                                 *
 *************************************************************************/
{
  int found = 0;
  char *temp_char_ptr, *temp_value;
  INI_NODE *node, *temp_node;

  node = ARG_start;
  do {
    if (node->node_id == 2)
      if (stricmp(node->section, ARG_section) == 0)
        found = 1;
  } while ((node = (INI_NODE *)node->next_node) != NULL && found == 0);

  if (found == 0)
    return(4);

  found = 0;

  do {
    temp_node = node;
    if (node->node_id == 3) {
      char temp_profile[strlen(temp_node->profile)];
      char *temp_profile_ptr;

      strcpy(temp_profile, temp_node->profile);
      temp_profile_ptr = temp_node->profile;
      while (temp_node->profile[0] == ' ' || temp_node->profile[0] == '\t')
        temp_node->profile++;
      while (temp_node->profile[strlen(temp_node->profile) - 1] == ' ' || temp_node->profile[strlen(temp_node->profile) - 1] == '\t')
        temp_node->profile[strlen(temp_node->profile) - 1] = '\0';
      if (stricmp(temp_node->profile, ARG_profile) == 0)
        found = 1;
      temp_node->profile = temp_profile_ptr;
      strcpy(temp_node->profile, temp_profile);
    }
  } while ((node = (INI_NODE *)node->next_node) != NULL && temp_node->node_id != 2 && found == 0);

  if (found == 0)
    return(5);

  if ((temp_value = (char *)malloc(strlen(temp_node->value) + 1)) == NULL)
    return(1);

  strcpy(temp_value, temp_node->value);
  temp_char_ptr = temp_node->value;
  if (temp_node->value[strlen(temp_node->value) - 1] == '\n')
    temp_node->value[strlen(temp_node->value) - 1] = '\0';
  while (temp_node->value[0] == ' ' || temp_node->value[0] == '\t')
    temp_node->value++;
  if (strrchr(temp_node->value, ';') > strrchr(temp_node->value, '\"'))
    *strrchr(temp_node->value, ';') = '\0';
  if (strrchr(temp_node->value, '#') > strrchr(temp_node->value, '\"'))
    *strrchr(temp_node->value, '#') = '\0';
  while (temp_node->value[strlen(temp_node->value) - 1] == ' ' || temp_node->value[strlen(temp_node->value) - 1] == '\t')
    temp_node->value[strlen(temp_node->value) - 1] = '\0';
  if (temp_node->value[0] == '\"')
    temp_node->value++;
  if (temp_node->value[strlen(temp_node->value) - 1] == '\"')
    temp_node->value[strlen(temp_node->value) - 1] = '\0';
  strcpy(ARG_value, temp_node->value);
  temp_node->value = temp_char_ptr;
  strcpy(temp_node->value, temp_value);

  return(0);
}        /* ini_get_string */

int ini_get_int(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, int *ARG_value)
/*************************************************************************
 * Gets integer from INI file starting @ 'ARG_start', from 'ARG_section' *
 * and 'ARG_profile'  to 'ARG_value', if it isn't integer it stores 0 to *
 * 'ARG_value'                                                           *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 4 - section not found                                                 *
 * 5 - profile not found                                                 *
 *************************************************************************/
{
  int ini_result;
  char *temp_char_ptr;

  if ((temp_char_ptr = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  if ((ini_result = ini_get_string(ARG_start, ARG_section, ARG_profile, temp_char_ptr)) == INI_CALL_SUCCESSFUL)
    *ARG_value = strtol(temp_char_ptr, NULL, 0);

  free(temp_char_ptr);

  return(ini_result);
}        /* ini_get_int */

int ini_get_long(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, long *ARG_value)
/*************************************************************************
 * Gets long  from INI file  starting @ 'ARG_start', from  'ARG_section' *
 * and 'ARG_profile'  to 'ARG_value', if  it  isn't long it stores 0  to *
 * 'ARG_value'                                                           *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 4 - section not found                                                 *
 * 5 - profile not found                                                 *
 *************************************************************************/
{
  int ini_result;
  char *temp_char_ptr;

  if ((temp_char_ptr = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  if ((ini_result = ini_get_string(ARG_start, ARG_section, ARG_profile, temp_char_ptr)) == INI_CALL_SUCCESSFUL)
    *ARG_value = strtol(temp_char_ptr, NULL, 0);

  free(temp_char_ptr);

  return(ini_result);
}        /* ini_get_long */

int ini_get_double(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, double *ARG_value)
/*************************************************************************
 * Gets double from INI file  starting @ 'ARG_start', from 'ARG_section' *
 * and 'ARG_profile'  to 'ARG_value', if it isn't double  it stores 0 to *
 * 'ARG_value'                                                           *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 4 - section not found                                                 *
 * 5 - profile not found                                                 *
 *************************************************************************/
{
  int ini_result;
  char *temp_char_ptr;

  if ((temp_char_ptr = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  if ((ini_result = ini_get_string(ARG_start, ARG_section, ARG_profile, temp_char_ptr)) == INI_CALL_SUCCESSFUL)
    *ARG_value = atof(temp_char_ptr);

  free(temp_char_ptr);

  return(ini_result);
}        /* ini_get_double */

int ini_get_truth(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, int *ARG_value)
/*************************************************************************
 * Gets 0 or 1 from INI file  starting @ 'ARG_start', from 'ARG_section' *
 * and 'ARG_profile' to 'ARG_value'                                      *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 * 4 - section not found                                                 *
 * 5 - profile not found                                                 *
 *************************************************************************/
{
  int ini_result;
  char *temp_char_ptr;

  if ((temp_char_ptr = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  if ((ini_result = ini_get_string(ARG_start, ARG_section, ARG_profile, temp_char_ptr)) == INI_CALL_SUCCESSFUL) {
    if (strcmp(temp_char_ptr, "1") == 0 || stricmp(temp_char_ptr, "YES") == 0 || stricmp(temp_char_ptr, "ON") == 0 || stricmp(temp_char_ptr, "TRUE") == 0)
      *ARG_value = 1;
    else
      if (strcmp(temp_char_ptr, "0") == 0 || stricmp(temp_char_ptr, "NO") == 0 || stricmp(temp_char_ptr, "OFF") == 0 || stricmp(temp_char_ptr, "FALSE") == 0)
        *ARG_value = 0;
  }

  free(temp_char_ptr);

  return(ini_result);
}        /* ini_get_truth */

int ini_put_entry(INI_NODE *ARG_start, const char *ARG_section_comment, const char *ARG_section, const char *ARG_entry_comment, const char *ARG_profile, const char *ARG_value, ...)
/*************************************************************************
 * Puts  'ARG_value'   from  'ARG_profile'  +   'ARG_entry_comment'   of *
 * 'ARG_section' + 'ARG_section_comment'  to  INI starting @ 'ARG_start' *
 *                                                                       *
 * Return codes:                                                         *
 * 0 - call successful                                                   *
 * 1 - malloc failed                                                     *
 *************************************************************************/
{
  char *real_string;
  int found;
  INI_NODE *node, *temp_node, *temp_prev_node;
  va_list arguments;

  if ((real_string = (char *)malloc(MAX_LINE + 1)) == NULL)
    return(1);

  va_start(arguments, ARG_value);
  vsprintf(real_string, ARG_value, arguments);
  va_end(arguments);

  found = 0;
  node = ARG_start;
  do {
    if (node->node_id == 2)
      if (stricmp(node->section, ARG_section) == 0)
        found = 1;
    temp_node = node;
  } while ((node = (INI_NODE *)node->next_node) != NULL && found == 0);

  if (found == 0) {
    char *temp_char_ptr;

    if ((node = (INI_NODE *)temp_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
      return(1);

    node->prev_node = (struct INI_NODE *)temp_node;
    node->node_id = 1;
    switch (temp_node->node_id) {
      case 1:
        temp_char_ptr = temp_node->comment;
        break;
      case 2:
        temp_char_ptr = temp_node->section;
        break;
      case 3:
        temp_char_ptr = temp_node->value;
        break;
      default:
        temp_char_ptr = NULL;
        break;
    }
    found = temp_node->node_id == 2 || temp_char_ptr[strlen(temp_char_ptr) - 1] == '\n' ? 1 : 0;
    if ((node->comment = (char *)malloc(found == 1 ? 2 : 3)) == NULL)
      return(1);

    strcpy(node->comment, found == 1 ? "\n\0" : "\n\n\0");
    temp_node = node;

    if (ARG_section_comment != NULL) {
      if ((node = (INI_NODE *)temp_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
        return(1);

      node->prev_node = (struct INI_NODE *)temp_node;
      node->node_id = 1;
      if ((node->comment = (char *)malloc(strlen(ARG_section_comment) + 1)) == NULL)
        return(1);

      strcat(strcpy(node->comment, ARG_section_comment), "\n");
      temp_node = node;
    }
  
    if ((node = (INI_NODE *)temp_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
      return(1);

    node->prev_node = (struct INI_NODE *)temp_node;
    node->node_id = 2;
    if ((node->section = (char *)malloc(strlen(ARG_section) + 1)) == NULL)
      return(1);

    strcpy(node->section, ARG_section);
    temp_node = node;

    if (ARG_entry_comment != NULL) {
      if ((node = (INI_NODE *)temp_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
        return(1);

      node->prev_node = (struct INI_NODE *)temp_node;
      node->node_id = 1;
      if ((node->comment = (char *)malloc(strlen(ARG_entry_comment) + 2)) == NULL)
        return(1);

      strcat(strcpy(node->comment, ARG_entry_comment), "\n");
      temp_node = node;
    }

    if ((node = (INI_NODE *)temp_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
      return(1);

    node->prev_node = (struct INI_NODE *)temp_node;
    node->node_id = 3;
    if ((node->profile = (char *)malloc(strlen(ARG_profile) + 1)) == NULL)
      return(1);

    strcpy(node->profile, ARG_profile);
    if ((node->value = (char *)malloc(strlen(real_string) + 2)) == NULL)
      return(1);

    strcat(strcpy(node->value, real_string), "\n");
    node->next_node = NULL;
  }
  else {
    found = 0;
    do {
      temp_prev_node = temp_node;
      temp_node = node;
      if (node->node_id == 3) {
        char temp_profile[strlen(temp_node->profile)];

        strcpy(temp_profile, temp_node->profile);
        while (temp_node->profile[0] == ' ' || temp_node->profile[0] == '\t')
          temp_node->profile++;
        while (temp_node->profile[strlen(temp_node->profile) - 1] == ' ' || temp_node->profile[strlen(temp_node->profile) - 1] == '\t')
          temp_node->profile[strlen(temp_node->profile) - 1] = '\0';
        if (stricmp(temp_node->profile, ARG_profile) == 0)
          found = 1;
        strcpy(temp_node->profile, temp_profile);
      }
    } while ((node = (INI_NODE *)node->next_node) != NULL && temp_node->node_id != 2 && found == 0);

    if (found == 0) {
      if (temp_node->node_id == 1 && temp_node->comment[strlen(temp_node->comment) - 1] != '\n') {
        char temp_string[MAX_LINE + 1];

        strcpy(temp_string, temp_node->comment);
        temp_node->comment = (char *)realloc(temp_node->comment, strlen(temp_node->comment) + 2);
        strcat(strcat(strcpy(temp_node->comment, temp_string), "\n"), "\0");
      }
      if (temp_node->node_id != 2) {
        temp_prev_node = temp_node;
        temp_node = NULL;
      }
      else {
        if (temp_prev_node->node_id == 1 && strspn(temp_prev_node->comment, " \t\n") == strlen(temp_prev_node->comment)) {
          temp_node = temp_prev_node;
          temp_prev_node = (INI_NODE *)temp_prev_node->prev_node;
        }
      }
      if (ARG_entry_comment != NULL) {
        if ((node = (INI_NODE *)temp_prev_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
          return(1);

        node->prev_node = (struct INI_NODE *)temp_prev_node;
        node->node_id = 1;
        if ((node->comment = (char *)malloc(strlen(ARG_entry_comment) + 2)) == NULL)
          return(1);

        strcat(strcpy(node->comment, ARG_entry_comment), "\n");
        temp_prev_node = node;
      }
  
      if ((node = (INI_NODE *)temp_prev_node->next_node = (struct INI_NODE *)malloc(sizeof(INI_NODE))) == NULL)
        return(1);

      node->prev_node = (struct INI_NODE *)temp_prev_node;
      node->node_id = 3;
      if ((node->profile = (char *)malloc(strlen(ARG_profile) + 1)) == NULL)
        return(1);

      strcpy(node->profile, ARG_profile);
      if ((node->value = (char *)malloc(strlen(real_string) + 2)) == NULL)
        return(1);

      strcat(strcpy(node->value, real_string), "\n");
      node->next_node = (struct INI_NODE *)temp_node;
    }
    else {
      char *temp_value, *temp_char_ptr, *temp_comment_ptr;
      int temp_len;

      temp_char_ptr = temp_node->value;
      if ((temp_value = malloc(strlen(temp_node->value) + 1)) == NULL)
        return(1);

      strcpy(temp_value, temp_node->value);
      if (temp_node->value[strlen(temp_node->value) - 1] == '\n')
        temp_node->value[strlen(temp_node->value) - 1] = '\0';
      if (strrchr(temp_node->value, ';') > strrchr(temp_node->value, '\"'))
        *strrchr(temp_node->value, ';') = '\0';
      if (strrchr(temp_node->value, '#') > strrchr(temp_node->value, '\"'))
        *strrchr(temp_node->value, '#') = '\0';
      while (temp_node->value[strlen(temp_node->value) - 1] == ' ' || temp_node->value[strlen(temp_node->value) - 1] == '\t')
        temp_node->value[strlen(temp_node->value) - 1] = '\0';
      temp_len = strlen(temp_node->value);
      strcpy(temp_node->value = temp_char_ptr, temp_value);
      temp_comment_ptr = temp_node->value + temp_len;
      free(temp_value);

      temp_char_ptr = real_string;
      if (real_string[strlen(real_string) - 1] == '\n')
        real_string[strlen(real_string) - 1] = '\0';
      while (real_string[0] == ' ' || real_string[0] == '\t')
        real_string++;
      if (strrchr(real_string, ';') > strrchr(real_string, '\"'))
        *strrchr(real_string, ';') = '\0';
      if (strrchr(real_string, '#') > strrchr(real_string, '\"'))
        *strrchr(real_string, '#') = '\0';
      while (real_string[strlen(real_string) - 1] == ' ' || real_string[strlen(real_string) - 1] == '\t')
        real_string[strlen(real_string) - 1] = '\0';
      if ((temp_value = malloc(strlen(real_string) + strlen(temp_comment_ptr) + 1)) == NULL)
        return(1);

      strcat(strcpy(temp_value, real_string), temp_comment_ptr);
      free(temp_node->value);
      if ((temp_node->value = malloc(strlen(temp_value) + 1)) == NULL)
        return(1);

      strcpy(temp_node->value, temp_value);
      free(temp_value);
      real_string = temp_char_ptr;
    }
  }

  free(real_string);

  return(0);
}        /* ini_put_entry */
