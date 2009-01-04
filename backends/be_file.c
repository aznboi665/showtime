/*
 *  Backend using file I/O
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


#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "showtime.h"
#include "navigator.h"
#include "fileaccess/fileaccess.h"
#include "fileaccess/fa_probe.h"
#include "playqueue.h"

TAILQ_HEAD(be_file_entry_queue,  be_file_entry);

nav_backend_t be_file;

typedef struct be_file_page {
  nav_page_t h;

  hts_thread_t bfp_scanner;

  prop_t *bfp_nodes;

  struct be_file_entry_queue bfp_entries;

} be_file_page_t;


/**
 *
 */
typedef struct be_file_entry {
  TAILQ_ENTRY(be_file_entry) bfe_link;
  char *bfe_uri;
  prop_t *bfe_prop;
  int bfe_type;

} be_file_entry_t;



/**
 *
 */
static int
be_file_canhandle(const char *uri)
{
  fa_protocol_t *fap;
  return fa_resolve_proto(uri, &fap) != NULL;
}


/**
 *
 */
static void
scandir_callback(void *arg, const char *uri, const char *filename, int type)
{
  prop_t *p;
  be_file_page_t *bfp = arg;
  prop_t *urip, *media;
  be_file_entry_t *bfe;

  p = prop_create(NULL, "node");

  prop_set_string(prop_create(p, "filename"), filename);

  urip = prop_create(p, "url");
  prop_set_string(urip, uri);

  media = prop_create(p, "media");
  prop_set_string(prop_create(media, "title"), filename);

  prop_set_string(prop_create(media, "type"), 
		  type == FA_DIR ? "directory" : "file");
  
  bfe = malloc(sizeof(be_file_entry_t));
  bfe->bfe_type = type;
  bfe->bfe_uri = strdup(uri);
  bfe->bfe_prop = p;
  prop_ref_inc(p);
  TAILQ_INSERT_TAIL(&bfp->bfp_entries, bfe, bfe_link);

  prop_set_parent(p, bfp->bfp_nodes);
}


/**
 *
 */
static void *
scanner(void *aux)
{
  be_file_page_t *bfp = aux;
  be_file_entry_t *bfe;
  prop_t *media, *p;
  int r;

  TAILQ_INIT(&bfp->bfp_entries);
  fileaccess_scandir(bfp->h.np_uri, scandir_callback, bfp);

  while((bfe = TAILQ_FIRST(&bfp->bfp_entries)) != NULL) {
    TAILQ_REMOVE(&bfp->bfp_entries, bfe, bfe_link);

    p = bfe->bfe_prop;
    media = prop_create(p, "media");

    if(bfe->bfe_type == FA_DIR) {
      r = fa_probe_dir(media, bfe->bfe_uri);
    } else {
      r = fa_probe(media, bfe->bfe_uri, NULL, 0);
    }

    free(bfe->bfe_uri);
    free(bfe);

    if(r == FA_UNKNOWN)
      prop_destroy(p);

    prop_ref_dec(p);
  }

  return NULL;
}

/**
 *
 */
static int
file_open_dir(const char *uri0, nav_page_t **npp, char *errbuf, size_t errlen)
{
  be_file_page_t *bfp;
  prop_t *p;

  /* TODO: Check if it is a DVD */

  bfp = nav_page_create(&be_file, uri0, sizeof(be_file_page_t));
  p = bfp->h.np_prop_root;

  prop_set_string(prop_create(p, "type"), "filedirectory");
  
  bfp->bfp_nodes = prop_create(p, "nodes");
  
  hts_thread_create_detached(&bfp->bfp_scanner, scanner, bfp);
  *npp = &bfp->h;
  return 0;
}

/**
 *
 */
static int
file_open_file(const char *uri0, nav_page_t **npp, char *errbuf, size_t errlen)
{
  char redir[512];
  int r;
  prop_t *media;

  media = prop_create(NULL, "media");

  r = fa_probe(media, uri0, redir, sizeof(redir));
  
  switch(r) {
  case FA_ARCHIVE:
    prop_destroy(media);
    return file_open_dir(redir, npp, errbuf, errlen);
  case FA_AUDIO:
    playqueue_play(uri0, NULL, media, 0);
    *npp = NULL;
    return 0;

  default:
    snprintf(errbuf, errlen, "Can not handle file contents");
    return -1;
  }
}

/**
 *
 */
static int
be_file_open(const char *uri0, nav_page_t **npp, char *errbuf, size_t errlen)
{
  fa_protocol_t *fap;
  const char *uri;
  struct stat buf;

  if((uri = fa_resolve_proto(uri0, &fap)) == NULL) {
    snprintf(errbuf, errlen, "Protocol not handled");
    return -1;
  }

  if(fap->fap_stat(uri, &buf)) {
    snprintf(errbuf, errlen, "Unable to stat uri");
    return -1;
  }

  return S_ISDIR(buf.st_mode) ? file_open_dir(uri0, npp, errbuf, errlen) :
    file_open_file(uri0, npp, errbuf, errlen);
}


/**
 *
 */
nav_backend_t be_file = {
  .nb_canhandle = be_file_canhandle,
  .nb_open = be_file_open,
};


