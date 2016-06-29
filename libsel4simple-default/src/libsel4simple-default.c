/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <autoconf.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include <sel4/sel4.h>
#include <sel4debug/debug.h>
#include <simple-default/simple-default.h>

#include <vspace/page.h>
#include <vka/kobject_t.h>

void *simple_default_get_frame_info(void *data, void *paddr, int size_bits, seL4_CPtr *frame_cap, seL4_Word *offset) {
    unsigned int i;
    seL4_BootInfo *bi = (seL4_BootInfo *) data;
    assert(bi && paddr);

    for (i = 0; i < bi->untyped.end - bi->untyped.start; i++) {
        if (bi->untypedList[i].paddr <= (seL4_Word)paddr &&
            bi->untypedList[i].paddr + BIT(bi->untypedList[i].sizeBits) >= (seL4_Word)paddr + BIT(size_bits)) {
            *frame_cap = bi->untyped.start + i;
            *offset = (seL4_Word)paddr - bi->untypedList[i].paddr;
            break;
        }
    }
    return NULL;
}
seL4_Error simple_default_get_frame_cap(void *data, void *paddr, int size_bits, cspacepath_t *path) {
    unsigned int i;
    seL4_BootInfo *bi = (seL4_BootInfo *) data;
    assert(bi && paddr);

    for (i = 0; i < bi->untyped.end - bi->untyped.start; i++) {
        if (bi->untypedList[i].paddr == (seL4_Word)paddr &&
            bi->untypedList[i].sizeBits >= size_bits) {
            return seL4_Untyped_Retype(bi->untyped.start + i, kobject_get_type(KOBJECT_FRAME, size_bits),
                                       size_bits, path->root, path->dest, path->destDepth, path->offset, 1);
        }
    }
    return seL4_FailedLookup;
}

void *simple_default_get_frame_mapping(void *data, void *paddr, int size_bits) {
    return NULL;
}

seL4_Error simple_default_set_ASID(void *data, seL4_CPtr vspace) {
    return seL4_ARCH_ASIDPool_Assign(seL4_CapInitThreadASIDPool, vspace);
}

int simple_default_cap_count(void *data) {
    assert(data);

    seL4_BootInfo * bi = data;

    return   (bi->sharedFrames.end - bi->sharedFrames.start)
           + (bi->userImageFrames.end - bi->userImageFrames.start)
           + (bi->userImagePaging.end - bi->userImagePaging.start)
           + (bi->untyped.end - bi->untyped.start)
           + seL4_NumInitialCaps; //Include all the init caps
}

seL4_CPtr simple_default_nth_cap(void *data, int n) {
    assert(data);

    seL4_BootInfo * bi = data;
    size_t shared_frame_range = bi->sharedFrames.end - bi->sharedFrames.start + seL4_NumInitialCaps;
    size_t user_img_frame_range = bi->userImageFrames.end - bi->userImageFrames.start + shared_frame_range;
    size_t user_img_paging_range = bi->userImagePaging.end - bi->userImagePaging.start + user_img_frame_range;
    size_t untyped_range = bi->untyped.end - bi->untyped.start + user_img_paging_range;

    seL4_CPtr true_return = seL4_CapNull;

    if (n < seL4_CapInitThreadASIDPool) {
        true_return = (seL4_CPtr) n+1;
    } else if (n < seL4_NumInitialCaps) {
        true_return = (seL4_CPtr) n+1;
#if defined(CONFIG_ARCH_ARM)
        true_return++;
#endif
#ifndef CONFIG_IOMMU
        if(true_return >= seL4_CapIOSpace) {
            true_return++;
        }
#endif
    } else if (n < shared_frame_range) {
        return bi->sharedFrames.start + (n - seL4_NumInitialCaps);
    } else if (n < user_img_frame_range) {
        return bi->userImageFrames.start + (n - shared_frame_range);
    } else if (n < user_img_paging_range) {
        return bi->userImagePaging.start + (n - user_img_frame_range);
    } else if (n < untyped_range) {
        return bi->untyped.start + (n - user_img_paging_range);
    }

    return true_return;
}

seL4_CPtr simple_default_init_cap(void *data, seL4_CPtr cap_pos) {
    return cap_pos;
}

uint8_t simple_default_cnode_size(void *data) {
    assert(data);

    return ((seL4_BootInfo *)data)->initThreadCNodeSizeBits;
}

int simple_default_untyped_count(void *data) {
    assert(data);

    return ((seL4_BootInfo *)data)->untyped.end - ((seL4_BootInfo *)data)->untyped.start;
}

seL4_CPtr simple_default_nth_untyped(void *data, int n, size_t *size_bits, uintptr_t *paddr, bool *device) {
    assert(data && size_bits && paddr);

    seL4_BootInfo *bi = data;

    if(n < (bi->untyped.end - bi->untyped.start)) {
        if(paddr != NULL) {
            *paddr = bi->untypedList[n].paddr;
        }
        if(size_bits != NULL) {
            *size_bits = bi->untypedList[n].sizeBits;
        }
        if (device != NULL) {
            *device = (bool)bi->untypedList[n].isDevice;
        }
        return bi->untyped.start + (n);
    }

    return seL4_CapNull;
}

int simple_default_userimage_count(void *data) {
    assert(data);

    return ((seL4_BootInfo *)data)->userImageFrames.end - ((seL4_BootInfo *)data)->userImageFrames.start;
}

seL4_CPtr simple_default_nth_userimage(void *data, int n) {
    assert(data);

    seL4_BootInfo *bi = data;

    if(n < (bi->userImageFrames.end - bi->userImageFrames.start)) {
        return bi->userImageFrames.start + (n);
    }

    return seL4_CapNull;
}

void simple_default_print(void *data) {
    if (data == NULL) {
        ZF_LOGE("Data is null!");
    }

    debug_print_bootinfo(data);
}

void simple_default_init_bootinfo(simple_t *simple, seL4_BootInfo *bi) {
    assert(simple);
    assert(bi);

    simple->data = bi;
    simple->frame_info = &simple_default_get_frame_info;
    simple->frame_cap = &simple_default_get_frame_cap;
    simple->frame_mapping = &simple_default_get_frame_mapping;
    simple->ASID_assign = &simple_default_set_ASID;
    simple->cap_count = &simple_default_cap_count;
    simple->nth_cap = &simple_default_nth_cap;
    simple->init_cap = &simple_default_init_cap;
    simple->cnode_size = &simple_default_cnode_size;
    simple->untyped_count = &simple_default_untyped_count;
    simple->nth_untyped = &simple_default_nth_untyped;
    simple->userimage_count = &simple_default_userimage_count;
    simple->nth_userimage = &simple_default_nth_userimage;
    simple->print = &simple_default_print;
    simple_default_init_arch_simple(&simple->arch_simple, NULL);
}

