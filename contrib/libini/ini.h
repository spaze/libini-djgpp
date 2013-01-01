/*************************************************************************\
* LibINI 1.1 header file                                                  *
* ~~~~~~~~~~~~~~~~~~~~~~                                                  *
* Copyright 1999 STEALTH Technologies                                     *
* [stealth.tech@seznam.cz, http://stealthtech.tsx.org]                    *
* See 'libini.txt' and 'copying.lib' for details                          *
*                                                                         *
* Library for work with INI files                                         *
\*************************************************************************/

#ifndef __st_include_ini_h_
#define __st_include_ini_h_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LINE                        256

typedef struct INI_NODE
{
  // Node ID: 1 - Comment, 2 - Section, 3 - Entry
  int node_id;
  struct INI_NODE *prev_node, *next_node;
  // Comments
  char *comment;
  // Sections
  char *section;
  // Entries
  char *profile;
  char *value;
} INI_NODE;

#define INI_CALL_SUCCESSFUL             0
#define INI_MALLOC_FAILED               1
#define INI_FOPEN_FAILED                2
#define INI_FPRINTF_FAILED              3
#define INI_SECTION_NOT_FOUND           4
#define INI_PROFILE_NOT_FOUND           5
#define INI_FILE_IS_EMPTY               6

/* Creates new INI */
int ini_new(INI_NODE **ARG_start);

/* Loads INI */
int ini_load(INI_NODE **ARG_start, const char *ARG_file);

/* Writes INI */
int ini_flush(INI_NODE *ARG_start, const char *ARG_file);

/* Frees memory */
void ini_free(INI_NODE *ARG_start);

/* Gets string */
int ini_get_string(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, char *ARG_value);

/* Gets integer */
int ini_get_int(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, int *ARG_value);

/* Gets long */
int ini_get_long(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, long *ARG_value);

/* Gets double */
int ini_get_double(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, double *ARG_value);

/* Gets 0 or 1 (truth value) */
int ini_get_truth(INI_NODE *ARG_start, const char *ARG_section, const char *ARG_profile, int *ARG_value);

/* Puts entry */
int ini_put_entry(INI_NODE *ARG_start, const char *ARG_section_comment, const char *ARG_section, const char *ARG_entry_comment, const char *ARG_profile, const char *ARG_value, ...);

#ifdef __cplusplus
}
#endif

#endif   /* !__st_include_ini_h_ */
