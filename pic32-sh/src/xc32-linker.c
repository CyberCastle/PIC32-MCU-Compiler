/*
 * Copyright (c) 2015, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 * 
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above 
 *         copyright notice, this list of conditions and the following 
 *         disclaimer in the documentation and/or other materials provided 
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written 
 *         permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/************************************************************************/
/*  linker.c XC16 and XC32 shared linker functions                      */
/************************************************************************/

/*----------------------------------------------------------------------*/
/*  I N C L U D E    F I L E S                                          */
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xc32-linker.h"

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  C O N S T A N T S    A N D    L I M I T S                           */
/*----------------------------------------------------------------------*/
#if defined(MINGW)
#define DOESHAVEEXE .exe
#define SLASHTYPE "\\"
#else
#define DOESHAVEEXE
#define SLASHTYPE "/"
#endif

#define RESERVE_MAX 100

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    V A R I A B L E    D E F I N I T I O N S               */
/*----------------------------------------------------------------------*/
typedef struct tag_reserve_section {
  unsigned int start;
  unsigned int length;
  unsigned int end;

  char section[FILENAME_MAX];
  char reserve_name[FILENAME_MAX];
  char* path;

} reserve_section, *p_reserve_section;

static char *save_gld_to=NULL;
static reserve_section all_reserve_sections[RESERVE_MAX];
static int which_reserve_section=0;
static int num_reserved_sects = 0;

struct reserve_section_opt *rs_opt_list = NULL;


static const char * get_temp_gld(void);

static char default_memory_region_template[] =
"MEMORY\n"
"{\n"
"  %s%s%s           : ORIGIN = %#x, LENGTH = %#x\n"
"}\n\n";

static char reserve_template[] = 
"SECTIONS\n"
"{\n"
"  /*** Reserved Section ***/\n"
"  %s%s %#x (NOLOAD) :"
"{\n"
"  SHORT(0);\n"
"    . = %#x;\n"
"    . = ALIGN(4);\n"
"  } "
" > %s%s_mem \n"
"}\n\n";


/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    F U N C T I O N    D E F I N I T I O N S               */
/*----------------------------------------------------------------------*/

struct reserve_section_opt *insert(const char* region, const char* startstr, const char* endstr, 
                                   const char* gld_file, struct reserve_section_opt* list)
{
  struct reserve_section_opt *p;
  struct reserve_section_opt *q;
  
  /* create a new node */
  p = (struct reserve_section_opt*)malloc(sizeof(struct reserve_section_opt));
  
  if (p==NULL)
    {
      fprintf(stderr, "Out of memory in linker utility\n");
      exit(1);
    }
  p->next = NULL;
  /* save data into new node */
  p->region = strdup(region);
  p->gld_file = gld_file;
  p->start = strtol(startstr,NULL,16);
  p->end = strtol(endstr,NULL,16);


  /* Sort list by end address of the option */
  if (list == NULL || (p->end < list->end)) {
    /* this option should be the first element */
    p->next = list;
    return p;
    }
  else {
    /* search the linked list for the right location */
    q = list;

    /* This region is identical to the first region */
    if ((p->end==q->end) && (p->start==q->start))
      return list;

    while(q->next != NULL && (p->end > q->next->end)) {
        q = q->next;
    }
    if ((q->next !=NULL) && (p->end == q->next->end))
      {
      /* The new range is a subset of an existing range */
      if (p->start >= q->next->start)
        {
        /* Dont insert it */
  #if (DEBUG==1)
        fprintf(stderr, "skipping %s %x %x\n", region, p->start, p->end);
  #endif
        return list;
        }
      /* new range is a superset */
      else if (q->next != NULL)
        {
        struct reserve_section_opt *replaced;
        replaced = q->next;
      
  #if (DEBUG==1)
        fprintf(stderr, "replacing %s %x %x with %x %x\n", region, q->next->start, q->next->end, p->start, p->end);
  #endif

        p->next = q->next->next;
        q->next = p;
        free(replaced);
      
        return list;
        }
      }
#if (DEBUG==1)
            fprintf(stderr, "checking p->end:%x q->end:%x, p->start:%x q->start:%x\n", p->end, q->end, p->start, q->start);
#endif
    

#if (DEBUG==1)
   fprintf(stderr, "inserting %s %x %x\n", region, p->start, p->end);
#endif

    p->next = q->next;
    q->next = p;
    return list;
  }
}

void free_reserve_section_list(struct reserve_section_opt *list) {
  struct reserve_section_opt *p;
  
  while(list != NULL) {
    p = list->next;
    free(list);
    list = p;
  }
}

void print_reserve_section_list(struct reserve_section_opt *list) {
  struct reserve_section_opt *p;
  
  if (list == NULL)
    return;

#if (DEBUG==1)
   fprintf(stderr, "print_reserve_section_list\n");
#endif

  for (p = list; p != NULL; p = p->next)
    fprintf(stderr, "%s@%x:%x\n", p->region, p->start, p->end);

}


char* collect_reserve_section_opts(const char * unprocessed) {
  int format_num = 0;
  char * reserve_opt = NULL;
  const char * gld_file = NULL;

  /* In essence this is what we want, but %s is greedy. So things
   * must be split up
   * format_num = sscanf(unprocessed, RESERVE "=%s@%x:%x",
   *              parts->section, num_begin, num_end);
   */
  
  /* set the starting and ending memory ranges */
    char region[FILENAME_MAX];
    char mem_range[FILENAME_MAX];
    char section[FILENAME_MAX];
    unsigned int start=0;
    unsigned int end=0;
    char startstr[FILENAME_MAX];
    char endstr[FILENAME_MAX];
    char * at_symbol;

    /* Find the @ symbol */
    at_symbol = strchr(unprocessed, '@');
    if (at_symbol == NULL) {
      fprintf(stderr, "Reserve option is not in correct format\n"
                      "  Expected dash to indicate end memory range\n");
      exit(1);
    }

    /* The length we copy is the difference between the @ and
     * the beginning of the string */
    strncpy(region, unprocessed, at_symbol - unprocessed);
    region[at_symbol - unprocessed] = '\0';

    /* We copy from one past the @ to end of string */
    strcpy(mem_range, at_symbol + 1);

    /* Extract region */
    format_num = sscanf(region, RESERVE "=%s", section);
    if (format_num != 1) {
      fprintf(stderr, "Format of option is not correct (section)\n");
      exit(1);
    }

    /* Extract end and start */
    format_num = sscanf(mem_range, "%x:%x", &start, &end);
    if (format_num != 2) {
      fprintf(stderr, "Format of option is not correct (address range) %s:%s\n", startstr, endstr);
      exit(1);
    }
    format_num = snprintf(startstr, FILENAME_MAX, "0x%x", start);
    format_num = snprintf(endstr, FILENAME_MAX, "0x%x", end);

    /* Command line gives an ending point,
     * but linker understands length,
     * so math is involved */
    
    gld_file = get_temp_gld();
    rs_opt_list = insert(section, startstr, endstr, gld_file, rs_opt_list);

    return reserve_opt;
}

const char * get_temp_gld(void) {
    char *full_path;
//    FILE * pfile;
    char *tmp_exists;
    char default_tmp[] = "/tmp";
     struct stat sb;
    
    full_path = (char*)malloc(sizeof(char)*FILENAME_MAX);
    
    /*** Find a suitable directory to create a temp file ***/
    /* Setting string to temp variable */
    tmp_exists = getenv("TMP");
    if (!tmp_exists) {
      tmp_exists = getenv("TEMP");
    }
    /* Use the temp environment variable if it exists */
    if (tmp_exists) {
      strcpy(full_path, tmp_exists);
      strcat(full_path, SLASHTYPE);
    }
    /* Last option is to see if /tmp folder exists */
    else if ((stat(default_tmp, &sb) == 0) && (S_ISDIR(sb.st_mode))) {
      /* tmp folder exists */
      strcpy(full_path, default_tmp);
      strcat(full_path, SLASHTYPE);
    }
    /* Failure, could not find a directory to create temporary file */
    else {
      fprintf(stderr,
        "Unable to find a valid directory to create temporary files\n");
      exit(1);
    }

    /* Now add the name of file */
    strcat(full_path, tmpgldname(""));

    /* Convert forward slash to backslash. So, no 
       special processing is required.
    */
    unbackslashify (full_path);

    return full_path;
}

static char* write_reserve_section(const char* region, const unsigned int start, const unsigned int end, const char* full_path) 
{
//  int format_num = 0;
  p_reserve_section parts;
  char * reserve_opt = NULL;

#if (DEBUG==1)
   fprintf(stderr, "write_reserve_section %s %x %x\n", region, start, end);
#endif

  /* In essence this is what we want, but %s is greedy. So things
   * must be split up
   * format_num = sscanf(unprocessed, RESERVE "=%s@%x:%x",
   *              parts->section, num_begin, num_end);
   */
  if ( (which_reserve_section) >= RESERVE_MAX) {
    fprintf(stderr, "Warning: Only %i reserve sections supported\n"
            "  Following reserve ignored: %s@%x:%x\n", RESERVE_MAX, region, start, end);
    return NULL;
  }
  parts = &all_reserve_sections[which_reserve_section];
  which_reserve_section++;
  
  /* set the starting and ending memory ranges */
  {

//    char mem_range[FILENAME_MAX];
//    char * at_symbol;

    strncpy(parts->section, region, FILENAME_MAX);
    parts->start = start;
    parts->end = end;

    /* Value checking
     * We'll fail if beginning value is greater than end value */
    if (parts->start >= parts->end) {
      fprintf(stderr,
              "Memory ranges should go from smaller to greater\n");
      exit(1);
    }

    /* Now that we know the values valid, we can record the length */
    parts->length =  parts->end - parts->start + 1;

    /* Where we create the name of the actual section */
    num_reserved_sects += 1;
    sprintf(parts->reserve_name, "reserve_%s_%#x",
            parts->section, parts->start);
  }
  
#if DEBUG
  fprintf(stderr, "Debug: %x %x\n",
    parts->start, parts->length);
#endif

  {
    char * template_string = NULL;
    int template_len=0, char_printed=0;
    FILE * pfile;

#define KVA_TO_PA(v) 	((v)  & 0x1fffffffu)
#define PA_TO_KVA0(pa)	((pa) | 0x80000000u)
#define PA_TO_KVA1(pa)	((pa) | 0xa0000000u)

    char * template_string_kseg0 = NULL;
    char * template_string_kseg1 = NULL;
    char * template_string_region_kseg0 = NULL;
    char * template_string_region_kseg1 = NULL;
    int char_printed_kseg0 = 0;
    int char_printed_kseg1 = 0;
    int char_printed_region_kseg0 = 0;
    int char_printed_region_kseg1 = 0;

    template_len = strlen(reserve_template) +
                   (strlen("0xNNNNNNNN ") * 3) +
                   strlen(parts->section) +
                   strlen(parts->reserve_name) +
                   (strlen("_ksegN ") *2);

    /* Create template to put into temp file */
    template_string_kseg0 = malloc( sizeof(char) * template_len + 1);
    template_string_kseg1 = malloc( sizeof(char) * template_len + 1);
    
    template_len = strlen(default_memory_region_template) +
                   (strlen("0xNNNNNNNNu ") * 2) +
                   strlen(parts->section) + strlen("ksegN_X_mem");
    
    template_string_region_kseg0 = malloc( sizeof(char) * template_len + 1);
    template_string_region_kseg1 = malloc( sizeof(char) * template_len + 1);
    
    if (template_string_kseg0)
    {
      char_printed_region_kseg0 = sprintf(template_string_region_kseg0, default_memory_region_template, 
                                          "kseg0_", parts->section, "_mem",
                                           PA_TO_KVA0(parts->start), (parts->length+3)-(parts->length+3)%4);
      char_printed_kseg0 = sprintf(template_string_kseg0, reserve_template,
         parts->reserve_name, "_kseg0", PA_TO_KVA0(parts->start), 
          parts->length, "kseg0_", parts->section);
    }
    if (template_string_kseg1)
    {
      char_printed_region_kseg1 = sprintf(template_string_region_kseg1, default_memory_region_template, 
                                          "kseg1_", parts->section, "_mem",
                                           PA_TO_KVA1(parts->start), (parts->length+3)-(parts->length+3)%4);
      char_printed_kseg1 = sprintf(template_string_kseg1, reserve_template,
         parts->reserve_name, "_kseg1", PA_TO_KVA1(parts->start), 
         parts->length, "kseg1_", parts->section);
    }

#if DEBUG
    fprintf(stderr, "%s\n",  template_string_region_kseg0);
    fprintf(stderr, "%s\n",  template_string_kseg0);
    fprintf(stderr, "%s\n",  template_string_region_kseg1); 
    fprintf(stderr, "%s\n",  template_string_kseg1);    
#endif

#undef KVA_TO_PA
#undef PA_TO_KVA0
#undef PA_TO_KVA1

    parts->path= strdup(full_path);

#if DEBUG
    fprintf(stderr, "parts->path %s\n", parts->path);
#endif
    /* open file, Write data, and close file */
    pfile = fopen(full_path, "w");


    if (template_string != NULL)
    {
      fwrite(template_string, sizeof(char), char_printed, pfile);
      free(template_string);
    }

    if (template_string_kseg0 != NULL)
    {
      fwrite(template_string_region_kseg0, sizeof(char), char_printed_region_kseg0, pfile);
      fwrite(template_string_kseg0, sizeof(char), char_printed_kseg0, pfile);
      /* Free up stuff */
      free(template_string_region_kseg0);
      free(template_string_kseg0);
    }
    if (template_string_kseg1 != NULL)
    {
      fwrite(template_string_region_kseg1, sizeof(char), char_printed_region_kseg1, pfile);
      fwrite(template_string_kseg1, sizeof(char), char_printed_kseg1, pfile);
      free(template_string_region_kseg1);
      free(template_string_kseg1);
    }

    fclose(pfile);
    
    /* Save path to option */
    reserve_opt = (char*)malloc(strlen(full_path) + strlen ("-T\"\"") + 1);
    sprintf(reserve_opt, "-T\"%s\"", full_path);
    return reserve_opt;
  }
}

/* Write all of the reserve sections to a temporary linker script and 
   generate the -T option to pass the linker script to the linker      
 */
static char* write_reserve_section(const char* region, const unsigned int start, const unsigned int end, const char* gld_file);
void write_reserve_sections(struct reserve_section_opt *list, char** new_argv, int *shi) {
  struct reserve_section_opt *p;
  
  if (list == NULL)
    return;

  for (p = list; p != NULL; p = p->next)
  {
    new_argv[(*shi)+1] = write_reserve_section(p->region, p->start, p->end, p->gld_file);
    (*shi)++;
  }
  
  free_reserve_section_list(list);
}

void check_and_delete_reserved_sects() {
  /* Now we should delete the temp files */
  if (which_reserve_section > 0) {
    int counter = 0;
    while (counter < which_reserve_section) {
      int val = -1;

      /* We'll use the unlink function and only warn if the function fails */
      val = unlink(all_reserve_sections[counter].path);
      if (val == -1) {
        fprintf(stderr,
                "Warning: Unable to delete the created temporary file:\n"
                "\t%s\n", all_reserve_sections[counter].path);
      }
      counter++;
    }
  }
}

/* Creates a temporary gld file
 * Input:
 *   If no input exists the pointer is to the just a random file name
 *   Input can also be a template so template will create template.00
 */
char *tmpgldname(char *n) {
  char *tmpname = 0;
  static int i=0;
  int j;

  if (tmpname == 0) {
    tmpname = malloc(FILENAME_MAX+1);
  }

  if (tmpname==NULL)
    {
      fprintf(stderr, "Out of memory in linker utility\n");
      exit(1);
    }

  /* save_gld_to allows us to predefine a directory to save files to */
  if (save_gld_to) {
    snprintf(tmpname, FILENAME_MAX, "%s.%02d", save_gld_to, i);
  } else if (n && *n) {
    for (j = strlen(n); j>=0; j--) {
      if ((n[j] == '/') || (n[j] == '\\')) break;
    }
    snprintf(tmpname, FILENAME_MAX, "%s.%02d", &n[j+1], i);
  } else {
    snprintf(tmpname, FILENAME_MAX, "tmp%8.8x.%02d", getpid(), i);
  }
  i++;
  return tmpname;
}

void set_gld_save_folder(char * folder_name) {
  save_gld_to = folder_name;
}

