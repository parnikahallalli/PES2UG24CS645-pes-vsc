#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <openssl/evp.h>

#define OBJECTS_DIR ".pes/objects"

// helper to ensure directory exists
static void ensure_dir(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

// convert binary hash → hex string
static void hash_to_hex(const unsigned char *hash, char *hex) {
    for (int i = 0; i < 32; i++) {
        sprintf(hex + (i * 2), "%02x", hash[i]);
    }
    hex[64] = '\0';
}

int object_write(const char *type, const void *data, size_t size, char *hash_out) {
    // ensure directories
    ensure_dir(".pes");
    ensure_dir(OBJECTS_DIR);

    // create header: "type size\0"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type, size) + 1;

    // combine header + data
    size_t total_size = header_len + size;
    unsigned char *buffer = malloc(total_size);
    if (!buffer) {
        perror("malloc failed");
        return -1;
    }

    memcpy(buffer, header, header_len);
    memcpy(buffer + header_len, data, size);

    // compute SHA-256
    unsigned char hash[32];
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        free(buffer);
        return -1;
    }

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, buffer, total_size);
    EVP_DigestFinal_ex(ctx, hash, NULL);

    EVP_MD_CTX_free(ctx);

    // convert to hex
    hash_to_hex(hash, hash_out);

    // free buffer
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
