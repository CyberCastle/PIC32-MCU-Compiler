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

#include "linker.h"

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
  char path[FILENAME_MAX];
} reserve_section, *p_reserve_section;

static char *save_gld_to=0;
static reserve_section all_reserve_sections[RESERVE_MAX];
static int which_reserve_section=0;
static int num_reserved_sects = 0;

#if defined(_BUILD_C32_)
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

#else

/*
 * removed the 'contents' because this is no load - 
 *   putting stuff in here makes it have contents and marks the section
 *   as PROGBITS (because it has content)
 *
 * it may be minor, but wanted to clean it up (CAW)
 */

static char reserve_template[] = "SECTIONS\n"
"{\n"
"  /*** Reserved Section ***/\n"
"  %s %#x (NOLOAD) :"
"{\n"
"    . = %#x;\n"
"  } "
"> %s\n"
"}\n\n";
#endif

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*  L O C A L    F U N C T I O N    D E F I N I T I O N S               */
/*----------------------------------------------------------------------*/

char * get_reserve_section_opt(const char * unprocessed) {
  int format_num = 0;
  p_reserve_section parts;
  char * reserve_opt = NULL;

  /* In essence this is what we want, but %s is greedy. So things
   * must be split up
   * format_num = sscanf(unprocessed, RESERVE "=%s@%x:%x",
   *              parts->section, num_begin, num_end);
   */
  if ( (which_reserve_section) >= RESERVE_MAX) {
    fprintf(stderr, "Warning: Only %i reserve sections supported\n"
            "  Following reserve ignored: %s\n", RESERVE_MAX, unprocessed);
    return NULL;
  }
  parts = &all_reserve_sections[which_reserve_section];

  /* set the starting and ending memory ranges */
  {
    char region[FILENAME_MAX];
    char mem_range[FILENAME_MAX];
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
    format_num = sscanf(region, RESERVE "=%s", parts->section);
    if (format_num != 1) {
      fprintf(stderr, "Format of option is not correct\n");
      exit(1);
    }

    /* Extract end and start */
    format_num = sscanf(mem_range, "%x:%x", &parts->start, &parts->end);
    if (format_num != 2) {
      fprintf(stderr, "Format of option is not correct\n");
      exit(1);
    }

    /* Command line gives an ending point,
     * but linker understands length,
     * so math is involved */

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
    char full_path[FILENAME_MAX];
    FILE * pfile;
    char *tmp_exists;
    char default_tmp[] = "/tmp";
    struct stat sb;

#if defined (_BUILD_C32_)
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

#else
    template_len = strlen(reserve_template) +
                   sizeof(parts->start) +
                   sizeof(parts->length) +
                   strlen(parts->section) +
                   strlen(parts->reserve_name);

    /* Create template to put into temp file */
    template_string = malloc( sizeof(char) * template_len );
    char_printed = sprintf(template_string, reserve_template,
       parts->reserve_name, parts->start, parts->length, parts->section);
#if DEBUG
    fprintf(stderr, "%s\n",  template_string);
#endif
#endif

    /*** Find a suitable directory to create a temp file ***/
    /* Setting string to temp variable */
    tmp_exists = getenv("TMP");
    if (!tmp_exists) {
      tmp_exists = getenv("TEMP");
    }
    /* Use the temp environment variable if it exists */
    if (tmp_exists) {
#ifndef MINGW
      strcpy(full_path, tmp_exists);
#else
      /* Windows TEMP/TMP looks like c:\foo\bar note that the slashes
         aren't escaped... lets not just copy the TMP directory with strcpy */
      char *src,*dst;

      for (dst = full_path, src = tmp_exists; *src; dst++, src++) {
        if (src[0] == SLASHTYPE[0]) {
          if (src[1] == SLASHTYPE[1]) {
            src++;
          }
          *dst++ = SLASHTYPE[0];
          *dst = SLASHTYPE[1];
        } else *dst = *src;
      }
#endif
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

    /* We have the file name, now save for later deletion */
    strcpy(parts->path, full_path);

#if DEBUG
    fprintf(stderr, "%s\n", full_path);
#endif
    /* open file, Write data, and close file */
    pfile = fopen(full_path, "w");

    if (template_string != NULL)
    {
      fwrite(template_string, sizeof(char), char_printed, pfile);
      free(template_string);
    }

#if defined(_BUILD_C32_)
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
#endif

    fclose(pfile);

    /* Save path to option */
    reserve_opt = malloc(sizeof(char)*FILENAME_MAX);
    sprintf(reserve_opt, "-T\"%s\"", full_path);
    which_reserve_section++;

    return reserve_opt;
  }
}

void check_and_delete_reserved_sects() {
  if (save_gld_to) return;

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
  /* save_gld_to allows us to predefine a directory to save files to */
  if (save_gld_to) {
    sprintf(tmpname,"%s.%02d", save_gld_to, i);
  } else if (n && *n) {
    for (j = strlen(n); j>=0; j--) {
      if ((n[j] == '/') || (n[j] == '\\')) break;
    }
    sprintf(tmpname,"%s.%02d", &n[j+1], i);
  } else {
    sprintf(tmpname,"tmp%8.8x.%02d", getpid(), i);
  }
  i++;
  return tmpname;
}

void set_gld_save_folder(char * folder_name) {
  save_gld_to = folder_name;
}

