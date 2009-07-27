/*
 *  File access, abstract interface for accessing files
 *  Copyright (C) 2008 Andreas Öman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILEACCESS_H
#define FILEACCESS_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <queue.h>

#include "navigator.h"

int fileaccess_init(void);


/**
 *
 */
TAILQ_HEAD(fa_dir_entry_queue, fa_dir_entry);

/**
 *
 */
typedef struct fa_dir_entry {
  TAILQ_ENTRY(fa_dir_entry) fde_link;
  char *fde_filename;
  char *fde_url;
  int   fde_type; /* CONTENT_ .. types from showtime.h */
  void *fde_opaque;
  prop_t *fde_metadata;
} fa_dir_entry_t;

/**
 *
 */
typedef struct fa_dir {
  struct fa_dir_entry_queue fd_entries;
  int fd_count;
} fa_dir_t;

fa_dir_t *fa_dir_alloc(void);

void fa_dir_free(fa_dir_t *nd);

void fa_dir_add(fa_dir_t *nd, const char *path, const char *name, int type,
		 prop_t *metadata);

void fa_dir_sort(fa_dir_t *nd);



/**
 *
 */
LIST_HEAD(fa_protocol_list, fa_protocol);
extern struct fa_protocol_list fileaccess_all_protocols;


/**
 *
 */
typedef struct fa_handle {
  const struct fa_protocol *fh_proto;
} fa_handle_t;



fa_dir_t *fa_scandir(const char *url, char *errbuf, size_t errsize);
void *fa_open(const char *url, char *errbuf, size_t errsize);
void *fa_open_theme(const char *url, const char *themepath);
void fa_close(void *fh);
int fa_read(void *fh, void *buf, size_t size);
int64_t fa_seek(void *fh, int64_t pos, int whence);
int64_t fa_fsize(void *fh);
int fa_stat(const char *url, struct stat *buf, char *errbuf, size_t errsize);
int fa_findfile(const char *path, const char *file, 
		char *fullpath, size_t fullpathlen);

int fa_can_handle(const char *url, char *errbuf, size_t errsize);

void *fa_reference(const char *url);
void fa_unreference(void *fh);

const char *fa_ffmpeg_error_to_txt(int err);

#define FA_SCANNER_DETERMINE_VIEW 0x1

void fa_scanner(const char *url, prop_t *root, int flags);

#endif /* FILEACCESS_H */
