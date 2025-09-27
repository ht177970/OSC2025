/* dtb.c - minimal FDT parser (walks struct block tokens) */
#include "dtb.h"
#include <stddef.h>
#include <stdint.h>

/* FDT token values */
#define FDT_BEGIN_NODE  0x1
#define FDT_END_NODE    0x2
#define FDT_PROP        0x3
#define FDT_NOP         0x4
#define FDT_END         0x9

/* fdt header offsets (big-endian 32-bit fields) */
static inline uint32_t be32(const void *p) {
    const uint8_t *b = (const uint8_t *)p;
    return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
}

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

/* validate header magic */
int fdt_check_header(const void *fdt) {
    if (!fdt) return 0;
    uint32_t magic = be32((const uint8_t *)fdt + 0);
    return magic == 0xd00dfeed;
}

/* helper: safe pointers */
static const uint8_t *struct_base(const void *fdt, const struct fdt_header *h) {
    return (const uint8_t *)fdt + be32(&h->off_dt_struct);
}
static const uint8_t *strings_base(const void *fdt, const struct fdt_header *h) {
    return (const uint8_t *)fdt + be32(&h->off_dt_strings);
}

const char *fdt_get_model(const void *fdt) {
    if (!fdt) return NULL;
    const struct fdt_header *h = (const struct fdt_header *)fdt;
    if (!fdt_check_header(fdt)) return NULL;

    const uint8_t *sbase = strings_base(fdt, h);
    const uint8_t *struc = struct_base(fdt, h);
    uint32_t struct_size = be32(&h->size_dt_struct);
    const uint8_t *p = struc;
    const uint8_t *end = struc + struct_size;

    int depth = 0;
    /* Walk tokens */
    while (p < end) {
        uint32_t token = be32(p);
        p += 4;
        if (token == FDT_END) break;
        else if (token == FDT_NOP) {
            continue;
        } else if (token == FDT_BEGIN_NODE) {
            /* node name (null-terminated) */
            //const char *name = (const char *)p;
            /* root node is first BEGIN_NODE, model is located in root */
            depth++;
            /* skip name (aligned to 4) */
            size_t namelen = 0;
            while (p + namelen < end && p[namelen] != 0) namelen++;
            p += namelen + 1;
            /* align to 4 */
            while (((uintptr_t)p) & 3) p++;
        } else if (token == FDT_PROP) {
            if (p + 8 > end) break; /* malformed */
            uint32_t prop_len = be32(p);
            uint32_t nameoff = be32(p + 4);
            p += 8;
            const uint8_t *val = p;
            const char *pname = (const char *)(sbase + nameoff);

            /* if we're at root node (depth==1) and prop name is "model", return it */
            if (depth == 1) {
                if (pname && pname[0] && (pname[0] == 'm') && (pname[1] == 'o') && (pname[2] == 'd')) {
                    /* simple strcmp for "model" */
                    const char *target = "model";
                    int ok = 1;
                    for (int i = 0; target[i]; ++i) {
                        if (pname[i] != target[i]) { ok = 0; break; }
                    }
                    if (ok) {
                        /* value is a null-terminated string typically */
                        return (const char *)val;
                    }
                }
            }

            /* skip prop payload (prop_len) with 4-byte alignment */
            size_t skip = ((prop_len + 3) / 4) * 4;
            p += skip;
        } else if (token == FDT_END_NODE) {
            if (depth > 0) depth--;
        } else {
            /* unknown token: break */
            break;
        }
    }
    return NULL;
}

/* look for uart node with compatible matching snps,dw-apb-uart or ns16550a
   returns base address (first reg entry combined from two 32-bit words) or 0 if none */
uint64_t fdt_get_uart_base(const void *fdt) {
    if (!fdt) return 0;
    const struct fdt_header *h = (const struct fdt_header *)fdt;
    if (!fdt_check_header(fdt)) return 0;

    const uint8_t *sbase = strings_base(fdt, h);
    const uint8_t *struc = struct_base(fdt, h);
    uint32_t struct_size = be32(&h->size_dt_struct);
    const uint8_t *p = struc;
    const uint8_t *end = struc + struct_size;

    int depth = 0;
    int node_has_match = 0;
    uint64_t node_reg_base = 0;
    int node_reg_found = 0;

    while (p < end) {
        uint32_t token = be32(p);
        p += 4;
        if (token == FDT_END) break;
        else if (token == FDT_NOP) continue;
        else if (token == FDT_BEGIN_NODE) {
            depth++;
            node_has_match = 0;
            node_reg_found = 0;
            node_reg_base = 0;
            /* skip name */
            size_t namelen = 0;
            while (p + namelen < end && p[namelen] != 0) namelen++;
            p += namelen + 1;
            while (((uintptr_t)p) & 3) p++;
        } else if (token == FDT_PROP) {
            if (p + 8 > end) break;
            uint32_t prop_len = be32(p);
            uint32_t nameoff = be32(p + 4);
            p += 8;
            const uint8_t *val = p;
            const char *pname = (const char *)(sbase + nameoff);

            /* check compatible property */
            if (pname && pname[0]) {
                /* compare strings */
                if (pname[0] == 'c' && pname[1] == 'o') { /* likely "compatible" */
                    const char *t = "compatible";
                    int ok = 1;
                    for (int i = 0; t[i]; ++i) if (pname[i] != t[i]) { ok = 0; break; }
                    if (ok) {
                        /* compatible may contain multiple null-terminated strings inside val */
                        const uint8_t *s = val;
                        uint32_t rem = prop_len;
                        while (rem > 0) {
                            const char *cstr = (const char *)s;
                            /* check matching substrings */
                            if ( (cstr[0] && ( (cstr[0]=='k' && cstr[1]=='y' && cstr[2]==',' && cstr[3]=='p') || /* ky,p... */
                                                (cstr[0]=='n' && cstr[1]=='s' && cstr[2]=='1' && cstr[3]=='6') /* ns16550a */ ) ) ) {
                                /* safer full strstr-like checks */
                                /* check for "snps,dw-apb-uart" */
                                const char *k1 = "ky,pxa-uart";
                                const char *k2 = "ns16550a";
                                int matched = 0;
                                /* simple strcmp */
                                int i;
                                for (i=0; k1[i]; ++i) if (cstr[i] != k1[i]) break;
                                if (k1[i] == 0 && cstr[i] == 0) matched = 1;
                                for (i=0; !matched && k2[i]; ++i) if (cstr[i] != k2[i]) break;
                                if (!matched && k2[i] == 0 && cstr[i] == 0) matched = 1;
                                if (matched) node_has_match = 1;
                            } else {
                                /* fallback: do exact compares */
                                const char *k1 = "snps,dw-apb-uart";
                                const char *k2 = "ns16550a";
                                int matched = 0;
                                int i;
                                for (i=0; k1[i] && (i < (int)rem); ++i) if (cstr[i] != k1[i]) break;
                                if (k1[i] == 0 && cstr[i] == 0) matched = 1;
                                for (i=0; !matched && k2[i] && (i < (int)rem); ++i) if (cstr[i] != k2[i]) break;
                                if (!matched && k2[i] == 0 && cstr[i] == 0) matched = 1;
                                if (matched) node_has_match = 1;
                            }

                            /* advance to next null term */
                            uint32_t len = 0;
                            while (len < rem && s[len] != 0) len++;
                            /* skip the terminating NUL */
                            if (len < rem) { len++; } else { /* malformed */ break; }
                            s += len;
                            rem -= len;
                        }
                    }
                }
                /* check reg property name quickly */
                if (pname[0] == 'r' && pname[1] == 'e' && pname[2] == 'g') {
                    /* parse first two big-endian 32-bit words to make 64-bit base if length >= 8 */
                    if (prop_len >= 8) {
                        uint32_t hi = be32(val);
                        uint32_t lo = be32(val + 4);
                        node_reg_base = ((uint64_t)hi << 32) | (uint64_t)lo;
                        node_reg_found = 1;
                    }
                }
            }

            /* skip payload with alignment */
            size_t skip = ((prop_len + 3) / 4) * 4;
            p += skip;
        } else if (token == FDT_END_NODE) {
            /* if node ended and matched & have reg, return it */
            if (node_has_match && node_reg_found) {
                return node_reg_base;
            }
            if (depth > 0) depth--;
            node_has_match = 0;
            node_reg_found = 0;
            node_reg_base = 0;
        } else {
            break;
        }
    }

    return 0;
}

