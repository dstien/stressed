/*
 * gpcunpack - Grand Prix Circuit resource unpacker
 * Based on pcs_decompress.py algorithm by Distinctive Software Inc.
 * 
 * Compression detection:
 *   - byte[0] == 0x82: Stunts format (use stunpack.c)
 *   - byte[0] != 0x82: GPC format (use gpcunpack.c)
 *
 * GPC compressed file format:
 *   Offset 0: uint32 decomp_size (little-endian)
 *   Offset 4: compressed data (RLE encoded)
 *
 * RLE algorithm (two-pass):
 *   Pass 1: Sequence pass - expands byte sequences delimited by sentinel
 *   Pass 2: Single pass - expands using escape code lookup table
 */

#ifndef GPCUNPACK_H
#define GPCUNPACK_H

#include <stdint.h>

#define GPC_ERR_BUF_SIZE 256

typedef struct {
    uint8_t *data;
    uint32_t offset;
    uint32_t len;
} gpc_Buffer;

#ifdef __cplusplus
extern "C"
#endif
int gpc_decomp(gpc_Buffer *src, gpc_Buffer *dst, char *err);

#endif /* GPCUNPACK_H */
