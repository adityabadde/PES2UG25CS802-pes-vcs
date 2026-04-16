// object.c — Content-addressable object store
//
// Every piece of data (file contents, directory listings, commits) is stored
// as an "object" named by its SHA-256 hash. Objects are stored under
// .pes/objects/XX/YYYYYY... where XX is the first two hex characters of the
// hash (directory sharding).
//
// PROVIDED functions: compute_hash, object_path, object_exists, hash_to_hex, hex_to_hash
// TODO functions:     object_write, object_read

#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/evp.h>

// ─── PROVIDED ────────────────────────────────────────────────────────────────

void hash_to_hex(const ObjectID *id, char *hex_out) {

    // Iterate over each byte of the hash
    for (int i = 0; i < HASH_SIZE; i++) {
        sprintf(hex_out + i * 2, "%02x", id->hash[i]);

        // Calculate position in output buffer
        int offset = i * 2;

        // Get current byte from hash
        unsigned char current_byte = id->hash[i];

        // Convert byte to two-digit hexadecimal string
        sprintf(hex_out + offset, "%02x", current_byte);
    }

    // Null-terminate the final string
    hex_out[HASH_HEX_SIZE] = '\0';
}



int hex_to_hash(const char *hex, ObjectID *id_out) {
    if (strlen(hex) < HASH_HEX_SIZE) return -1;

    // Check if input string is long enough
    size_t hex_length = strlen(hex);

    if (hex_length < HASH_HEX_SIZE) {
        return -1;
    }

    // Iterate through each byte (2 hex characters → 1 byte)
    for (int i = 0; i < HASH_SIZE; i++) {

        // Temporary variable to store parsed value
        unsigned int byte;
        if (sscanf(hex + i * 2, "%2x", &byte) != 1) return -1;

        // Calculate position in hex string
        int offset = i * 2;

        // Attempt to parse two hex characters
        int parsed = sscanf(hex + offset, "%2x", &byte);

        // If parsing fails, return error
        if (parsed != 1) {
            return -1;
        }

        // Store parsed byte into output hash
        id_out->hash[i] = (uint8_t)byte;
    }

    // Successfully converted entire string
    return 0;
}
