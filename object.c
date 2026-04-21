#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <openssl/evp.h>

#define OBJECTS_DIR ".pes/objects"

// ensure directory exists
static void ensure_dir(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

// convert binary hash → hex
static void hash_to_hex(const unsigned char *hash, char *hex) {
    for (int i = 0; i < 32; i++) {
        sprintf(hex + (i * 2), "%02x", hash[i]);
    }
    hex[64] = '\0';
}

int object_write(const char *type, const void *data, size_t size, char *hash_out) {
    ensure_dir(".pes");
    ensure_dir(OBJECTS_DIR);

    // header
    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type, size) + 1;

    // combine
    size_t total_size = header_len + size;
    unsigned char *buffer = malloc(total_size);
    if (!buffer) {
        perror("malloc failed");
        return -1;
    }

    memcpy(buffer, header, header_len);
    memcpy(buffer + header_len, data, size);

    // hash
    unsigned char hash[32];
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, buffer, total_size);
    EVP_DigestFinal_ex(ctx, hash, NULL);
    EVP_MD_CTX_free(ctx);

    hash_to_hex(hash, hash_out);

    // directory structure
    char dir[256], path[512];
    snprintf(dir, sizeof(dir), "%s/%.2s", OBJECTS_DIR, hash_out);
    ensure_dir(dir);

    snprintf(path, sizeof(path), "%s/%s", dir, hash_out + 2);

    // write file
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        perror("fopen failed");
        free(buffer);
        return -1;
    }

    fwrite(buffer, 1, total_size, fp);
    fclose(fp);

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
