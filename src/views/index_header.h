/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/**
 * @copyright 2013 Couchbase, Inc.
 *
 * @author Filipe Manana  <filipe@couchbase.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 **/

#ifndef _INDEX_HEADER_H
#define _INDEX_HEADER_H

#include "couchstore_config.h"
#include <stdint.h>
#include <libcouchstore/visibility.h>
#include <libcouchstore/couch_db.h>
#include "bitmap.h"
#include "sorted_list.h"
#include "../node_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LATEST_INDEX_HEADER_VERSION 2

typedef struct {
    uint16_t part_id;
    uint64_t seq;
} part_seq_t;


typedef struct {
    /* sorted_list instance, values of type uint16_t */
    void   *active;
    /* sorted_list instance, values of type uint16_t */
    void   *passive;
    /* sorted_list instance, values of type uint16_t */
    void   *unindexable;
} index_state_transition_t;


typedef struct {
    unsigned char uuid[8];
    uint64_t      seq;
} failover_log_t;


typedef struct {
    uint16_t       part_id;
    uint16_t       num_failover_log;
    failover_log_t *failover_log;
} part_version_t;


typedef struct {
    uint8_t                    version;
    /* MD5 hash */
    unsigned char              signature[16];
    uint8_t                    num_views;
    uint16_t                   num_partitions;
    bitmap_t                   active_bitmask;
    bitmap_t                   passive_bitmask;
    bitmap_t                   cleanup_bitmask;
    /* sorted_list instance, values of type part_seq_t */
    void                       *seqs;
    node_pointer               *id_btree_state;
    /* array of num_views elements */
    node_pointer               **view_states;
    int                        has_replica;
    /* sorted_list instance, values of type uint16_t */
    void                       *replicas_on_transfer;
    index_state_transition_t   pending_transition;
    /* sorted_list instance, values of type part_seq_t */
    void                       *unindexable_seqs;
    /* sorted_list instance, values of type part_ver_t */
    void                       *part_versions;
} index_header_t;


couchstore_error_t decode_index_header(const char *bytes,
                                       size_t len,
                                       index_header_t **header);

couchstore_error_t encode_index_header(const index_header_t *header,
                                       char **buffer,
                                       size_t *buffer_size);

void free_index_header(index_header_t *header);

#ifdef __cplusplus
}
#endif

#endif
