/
// Every piece of data (file contents, directory listings, commits) is stored
// as an "object" named by its SHA-256 hash. Objects are stored under
// .pes/objects/XX/YYYYYY... where XX is the first two hex characters of the
// hash (directory sharding).
//
// PROVIDED functions: compute_hash, object_path, object_exists, h
// ─── PROVIDED ────────────────────────────────────────────────────────────────

void hash_to_hex(const ObjectID *id, char *hex_out) {
    for (int i = 0; i < HASH_SIZE; i++) {
        sprintf(hex_out + i * 2, "%02x", id->hash[i]);
    }
    hex_out[HASH_HEX_SIZE] = '\0';
}

int hex_to_hash(const char *hex, ObjectID *id_out) {
    if (strlen(hex) < HASH_HEX_SIZE) return -1;
    for (int i = 0; i < HASH_SIZE; i++) {
        unsigned int byte;
        if (sscanf(hex + i * 2, "%2x", &byte) != 1) return -1;
        id_out->hash[i] = (uint8_t)byte;   }
    return 0;
}

void compute_hash(const void *data, size_t len, ObjectID *id_out) {
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, id_out->hash, &hash_len);
    EVP_MD_CTX_free(ctx);
}

// Get the filesystem path where an object should be stored.
// Format: .pes/objects
// The first 2 hex chars form the shard directory; the rest is the filename.
void object_path(const ObjectID *id, char *path_out, size_t path_size) {
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id, hex);
    snprintf(path_out, path_size, "%s/%.2s/%s", OBJECTS_DIR, hex, hex + 2);
}

int object_exists(const ObjectID *id) {
    char path[512];
    object_path(id, path, sizeof(path));
    return access(path, F_OK) == 0;
}

// ─── TODO: Implement these ──────────────────────────────────────────────────

#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define OBJECTS_DIR ".pes/objects"

// helper to ensure directory exists
static void ensure_dir(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

int object_write(const char *type, const void *data, size_t size, char *hash_out) {
    (void)hash_out; // not used yet

    // ensure .pes and objects dir exist
    ensure_dir(".pes");
    ensure_dir(OBJECTS_DIR);

    // create header: "type size\0"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type, size) + 1;

    // allocate buffer for full object
    size_t total_size = header_len + size;
    char *buffer = malloc(total_size);
    if (!buffer) {
        perror("malloc failed");
        return -1;
    }

    // copy header + data
    memcpy(buffer, header, header_len);
    memcpy(buffer + header_len, data, size);

    // for now: just free and return success
    free(buffer);

    return 0;
}

int object_read(const char *hash, char *type_out, void **data_out, size_t *size_out) {
    (void)hash;
    (void)type_out;
    (void)data_out;
    (void)size_out;

    // not implemented yet
    return -1;
}
