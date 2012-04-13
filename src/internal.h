/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ifndef LIBCOUCHSTORE_INTERNAL_H
#define LIBCOUCHSTORE_INTERNAL_H 1

/*
 * This file contains datastructures and prototypes for functions only to
 * be used by the internal workings of libcoucstore. If you for some reason
 * need access to them from outside the library, you should write a
 * function to give you what you need.
 */

#include <libcouchstore/couch_db.h>

#define COUCH_BLOCK_SIZE 4096
#define COUCH_DISK_VERSION 10
#define COUCH_SNAPPY_THRESHOLD 64

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _nodepointer {
        sized_buf key;
        uint64_t pointer;
        sized_buf reduce_value;
        uint64_t subtreesize;
    } node_pointer;

    typedef struct _db_header {
        uint64_t disk_version;
        uint64_t update_seq;
        node_pointer *by_id_root;
        node_pointer *by_seq_root;
        node_pointer *local_docs_root;
        uint64_t purge_seq;
        uint64_t purge_ptr;
        uint64_t position;
    } db_header;

    struct _db {
        uint64_t file_pos;
        couch_file_ops *file_ops;
        void *file_ops_cookie;
        const char* filename;
        db_header header;
        void *userdata;
    };

    couch_file_ops *couch_get_default_file_ops(void);

    /** Reads a chunk from the file at a given position.
        @param db The database to read from
        @param pos The byte position to read from
        @param ret_ptr On success, will be set to a malloced buffer containing the chunk data,
                or to NULL if the length is zero. Caller is responsible for freeing this buffer!
                On failure, value pointed to is unaltered.
        @return The length of the chunk (zero is a valid length!), or a negative error code */
    int pread_bin(Db *db, off_t pos, char **ret_ptr);

    /** Reads a compressed chunk from the file at a given position.
        Parameters and return value are the same as for pread_bin. */
    int pread_compressed(Db *db, off_t pos, char **ret_ptr);

    /** Reads a file header from the file at a given position.
        Parameters and return value are the same as for pread_bin. */
    int pread_header(Db *db, off_t pos, char **ret_ptr);

    /** Maps data lengths to physical lengths in the file, taking into account the header-detection
        byte at every page boundary. That is, given a number o bytes of data to read from a starting
        offset, it returns the number of bytes of the file that must be read to get that much data.
        @param blockoffset The offset from a page boundary at which the read starts
        @param finallen The number of data bytes that need to be read
        @return The number of physical bytes of the file that need to be read */
    ssize_t total_read_len(off_t blockoffset, ssize_t finallen);

    couchstore_error_t db_write_header(Db *db, sized_buf *buf, off_t *pos);
    int db_write_buf(Db *db, const sized_buf *buf, off_t *pos, size_t *disk_size);
    int db_write_buf_compressed(Db *db, const sized_buf *buf, off_t *pos, size_t *disk_size);

    node_pointer *read_root(char *buf, int size);
    void encode_root(char *buf, node_pointer *node);

#ifdef __cplusplus
}
#endif

#endif