/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef __ALLOCMAN_SEL4_ARCH_MAPPING__
#define __ALLOCMAN_SEL4_ARCH_MAPPING__

#include <allocman/allocman.h>

static inline seL4_Error allocman_sel4_arch_create_object_at_level(allocman_t *alloc, seL4_Word bits, cspacepath_t *path, void *vaddr, seL4_CPtr vspace_root) {
    struct object {
        seL4_Word bits;
        seL4_Word type;
        long (*map)(seL4_CPtr, seL4_CPtr, seL4_Word, seL4_Word);
    };
    struct object objects[] = {
        [SEL4_MAPPING_LOOKUP_NO_PT] = {seL4_PageTableBits, seL4_X86_PageTableObject, seL4_X86_PageTable_Map},
        [SEL4_MAPPING_LOOKUP_NO_PD] = {seL4_PageDirBits, seL4_X86_PageDirectoryObject, seL4_X86_PageDirectory_Map},
        [SEL4_MAPPING_LOOKUP_NO_PDPT] = {seL4_PDPTBits, seL4_X86_PDPTObject, seL4_X86_PDPT_Map},
    };
    if (bits >= ARRAY_SIZE(objects) || !objects[bits].map) {
        ZF_LOGF("Unknown lookup bits %d", (int)bits);
        return seL4_InvalidArgument;
    }
    struct object *obj = &objects[bits];
    int error;
    seL4_Word cookie;
    cookie = allocman_utspace_alloc(alloc, obj->bits, obj->type, path, false, &error);
    if (error) {
        ZF_LOGV("Failed to allocate object %d of size %d, got error %d", (int)obj->type, (int)obj->bits, error);
        return seL4_NotEnoughMemory;
    }
    error = obj->map(path->capPtr, vspace_root, (seL4_Word)vaddr, seL4_X86_Default_VMAttributes);
    if (error) {
        allocman_utspace_free(alloc, cookie, obj->bits);
    }
    return error;
}

#endif
