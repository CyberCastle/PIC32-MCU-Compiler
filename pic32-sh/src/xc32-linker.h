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

#ifndef __LINKER_H__
#define __LINKER_H__

#define RESERVE     "--mreserve"
#define RESERVE_LEN 10

struct reserve_section_opt {
  char *region;
  const char *gld_file;
  unsigned int start;
  unsigned int end;
  struct reserve_section_opt *next;
};

extern struct reserve_section_opt *rs_opt_list;

struct reserve_section_opt *insert(const char* region, const char* start, const char* end, const char* gld_file, struct reserve_section_opt *list);
void free_reserve_section_list(struct reserve_section_opt *list);
void print_reserve_section_list(struct reserve_section_opt *list);

char* collect_reserve_section_opts(const char * unprocessed);
void write_reserve_sections(struct reserve_section_opt *list, char** new_argv, int *shi);
//char * get_reserve_section_opt(const char * unprocessed);

void check_and_delete_reserved_sects();
char *tmpgldname(char *n);
void set_gld_save_folder(char * folder_name);
void unbackslashify (char *s);

#endif
