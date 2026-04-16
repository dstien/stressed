/*
 * gpcunpack.c - Grand Prix Circuit resource unpacker
 * Based on pcs_decompress.py algorithm by Distinctive Software Inc.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gpcunpack.h"

/*
 * Pass 1: Sequence pass
 * Expands byte sequences delimited by sentinel.
 * When sentinel is encountered:
 *   - Copy all bytes until next sentinel (the sequence)
 *   - Skip closing sentinel
 *   - Read repeat count
 *   - Repeat the sequence (count - 1) more times
 */
static uint32_t gpc_seq_pass(const uint8_t *src, uint32_t src_len,
                              uint8_t *dst, uint32_t dst_cap,
                              uint8_t sentinel)
{
    uint32_t dst_len = 0;
    uint32_t i = 0;
    
    while (i < src_len) {
        uint8_t cur = src[i];
        i++;
        
        if (cur == sentinel) {
            /* Start of sequence - copy until next sentinel */
            uint32_t seq_start = dst_len;
            
            while (i < src_len && src[i] != sentinel) {
                if (dst_len < dst_cap) {
                    dst[dst_len] = src[i];
                }
                dst_len++;
                i++;
            }
            
            if (i >= src_len) {
                break;
            }
            
            /* Skip closing sentinel */
            i++;
            
            if (i >= src_len) {
                break;
            }
            
            /* Read repeat count */
            uint8_t rep = src[i];
            i++;
            
            /* Repeat the sequence (rep - 1) times */
            uint32_t seq_len = dst_len - seq_start;
            for (uint8_t r = 1; r < rep && dst_len < dst_cap; r++) {
                for (uint32_t j = 0; j < seq_len && dst_len < dst_cap; j++) {
                    dst[dst_len] = dst[seq_start + j];
                    dst_len++;
                }
            }
        } else {
            /* Literal byte */
            if (dst_len < dst_cap) {
                dst[dst_len] = cur;
            }
            dst_len++;
        }
    }
    
    return dst_len;
}

/*
 * Pass 2: Single byte pass
 * Expands using escape code lookup table.
 *   - lookup[code] == 0: literal byte (copy as-is)
 *   - lookup[code] == 1: read count (1 byte) + value (1 byte), repeat value count times
 *   - lookup[code] == 3: read count (2 bytes LE) + value (1 byte), repeat value count times
 *   - lookup[code] == N (N>1): read value (1 byte), repeat value (N-1) times
 */
static uint32_t gpc_single_pass(const uint8_t *src, uint32_t src_len,
                                uint8_t *dst, uint32_t dst_len,
                                const uint8_t *esc_lookup)
{
    uint32_t dst_pos = 0;
    uint32_t i = 0;
    
    while (dst_pos < dst_len && i < src_len) {
        uint8_t cur = src[i];
        i++;
        uint8_t ec = esc_lookup[cur];
        
        if (ec == 0) {
            /* Literal byte */
            if (dst_pos < dst_len) {
                dst[dst_pos] = cur;
            }
            dst_pos++;
        } else if (ec == 1) {
            /* Single byte repeat: count (1 byte) + value (1 byte) */
            if (i + 1 >= src_len) {
                break;
            }
            uint8_t rep = src[i];
            i++;
            uint8_t value = src[i];
            i++;
            
            for (uint8_t r = 0; r < rep && dst_pos < dst_len; r++) {
                dst[dst_pos] = value;
                dst_pos++;
            }
        } else if (ec == 3) {
            /* Word repeat: count (2 bytes LE) + value (1 byte) */
            if (i + 2 >= src_len) {
                break;
            }
            uint16_t rep = src[i] | (src[i + 1] << 8);
            i += 2;
            uint8_t value = src[i];
            i++;
            
            for (uint16_t r = 0; r < rep && dst_pos < dst_len; r++) {
                dst[dst_pos] = value;
                dst_pos++;
            }
        } else {
            /* Repeat value (ec - 1) times */
            if (i >= src_len) {
                break;
            }
            uint8_t value = src[i];
            i++;
            
            for (uint8_t r = 0; r < ec - 1 && dst_pos < dst_len; r++) {
                dst[dst_pos] = value;
                dst_pos++;
            }
        }
    }
    
    return dst_pos;
}

/*
 * Main decompression function
 * 
 * GPC compressed format:
 *   Offset 0: uint32 decomp_size (little-endian)
 *   Offset 4: uint32 comp_size (little-endian)
 *   Offset 8: 1 byte: escape code count (bits 0-6) + mode flag (bit 7)
 *             bit 7 = 0: RLE mode (sequence pass + single pass)
 *             bit 7 = 1: TABLE mode (single pass only)
 *   Offset 9: N bytes: escape codes (N = byte[8] & 0x7f)
 *   Offset 9+N: M bytes: compressed data (M = word[4])
 */
int gpc_decomp(gpc_Buffer *src, gpc_Buffer *dst, char *err)
{
    if (!src || !src->data || !dst) {
        if (err) {
            snprintf(err, GPC_ERR_BUF_SIZE, "Invalid buffer pointer");
        }
        return -1;
    }
    
    if (src->len < 9) {
        if (err) {
            snprintf(err, GPC_ERR_BUF_SIZE, "Source buffer too small for header");
        }
        return -1;
    }
    
    /* Read decompressed size (uint32 little-endian) */
    uint32_t decomp_size = src->data[0] | (src->data[1] << 8) |
                           (src->data[2] << 16) | (src->data[3] << 24);
    
    /* Read compressed size (uint32 little-endian) */
    uint32_t comp_size = src->data[4] | (src->data[5] << 8) |
                         (src->data[6] << 16) | (src->data[7] << 24);
    
    if (decomp_size == 0 || decomp_size > 0xFFFFFF) {
        if (err) {
            snprintf(err, GPC_ERR_BUF_SIZE, "Invalid decompressed size: 0x%08x", decomp_size);
        }
        return -1;
    }
    
    /* Read escape code count and mode */
    uint8_t esc_count = src->data[8] & 0x7F;
    int mode_table = (src->data[8] & 0x80) != 0;
    
    /* Header size: 9 (sizes + esc count) + esc_count (escape codes) = 9 + esc_count */
    uint32_t header_size = 9 + esc_count;
    
    if (src->len < header_size) {
        if (err) {
            snprintf(err, GPC_ERR_BUF_SIZE, "Source buffer too small for escape codes");
        }
        return -1;
    }
    
    /* Build escape code lookup table */
    uint8_t esc_lookup[256];
    memset(esc_lookup, 0, sizeof(esc_lookup));
    
    for (uint8_t idx = 0; idx < esc_count; idx++) {
        esc_lookup[src->data[9 + idx]] = idx + 1;
    }
    
    /* Sentinel is escape_codes[1] */
    uint8_t sentinel = (esc_count > 1) ? src->data[10] : 0;
    
    /* Compressed data starts after header, limited by comp_size */
    uint32_t comp_offset = header_size;
    uint32_t comp_len = comp_size;
    
    if (mode_table) {
        /* TABLE mode: single pass only */
        if (!dst->data) {
            dst->data = (uint8_t*)malloc(decomp_size);
            if (!dst->data) {
                if (err) {
                    snprintf(err, GPC_ERR_BUF_SIZE, "Memory allocation failed");
                }
                return -1;
            }
        }
        
        uint32_t result = gpc_single_pass(src->data + comp_offset, comp_len,
                                          dst->data, decomp_size, esc_lookup);
        dst->len = result;
        dst->offset = 0;
        
        return (result == decomp_size) ? 0 : 1;
    } else {
        /* RLE mode: sequence pass then single pass */
        
        /* Allocate temporary buffer for sequence pass output */
        uint32_t seq_cap = decomp_size * 4; /* Estimate compression ratio, max 4x */
        if (seq_cap < 4096) {
            seq_cap = 4096;
        }
        
        uint8_t *seq_buf = (uint8_t*)malloc(seq_cap);
        if (!seq_buf) {
            if (err) {
                snprintf(err, GPC_ERR_BUF_SIZE, "Memory allocation failed for sequence buffer");
            }
            return -1;
        }
        
        /* Pass 1: Sequence pass */
        uint32_t seq_len = gpc_seq_pass(src->data + comp_offset, comp_len,
                                        seq_buf, seq_cap, sentinel);
        
        /* Allocate output buffer */
        if (!dst->data) {
            dst->data = (uint8_t*)malloc(decomp_size);
            if (!dst->data) {
                free(seq_buf);
                if (err) {
                    snprintf(err, GPC_ERR_BUF_SIZE, "Memory allocation failed");
                }
                return -1;
            }
        }
        
        /* Pass 2: Single pass */
        uint32_t result = gpc_single_pass(seq_buf, seq_len,
                                          dst->data, decomp_size, esc_lookup);
        
        free(seq_buf);
        
        dst->len = result;
        dst->offset = 0;
        
        return (result == decomp_size) ? 0 : 1;
    }
}
