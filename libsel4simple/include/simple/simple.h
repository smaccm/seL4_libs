/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef _INTERFACE_SIMPLE_H_
#define _INTERFACE_SIMPLE_H_

#include <autoconf.h>

#include <assert.h>
#include <sel4/sel4.h>
#include <simple/arch/simple.h>
#include <stdlib.h>
#include <utils/util.h>
#include <stdbool.h>
#include <vka/cspacepath_t.h>


/**
 * Get the cap to the physical frame of memory and put it at specified location
 *
 * @param data cookie for the underlying implementation
 *
 * @param page aligned physical address
 *
 * @param size of the region in bits
 *
 * @param The path to where to put this cap
 */
typedef seL4_Error (*simple_get_frame_cap_fn)(void *data, void *paddr, int size_bits, cspacepath_t *path);

/**
 * Request mapped address to a region of physical memory.
 *
 * Note: This function will only return the mapped virtual address that it knows about. It does not do any mapping its self nor can it guess where mapping functions are going to map.
 *
 * @param data cookie for the underlying implementation
 *
 * @param page aligned physical address
 *
 * @param size of the region in bits
 *
 * Returns the virtual address to which this physical address is mapped or NULL if frame is unmapped
 */

typedef void *(*simple_get_frame_mapping_fn)(void *data, void *paddr, int size_bits);

/**
 * Request data to a region of physical memory.
 *
 * Note: This function will only return the mapped virtual address that it knows about. It does not do any mapping its self nor can it guess where mapping functions are going to map.
 *
 * @param data cookie for the underlying implementation
 *
 * @param page aligned physical address for the frame
 *
 * @param size of the region in bits
 *
 * @param cap to the frame gets set. Will return the untyped cap unless the underlying implementation has access to the frame cap. Check with implementation but it should be a frame cap if and only if a vaddr is returned.
 *
 * @param (potentially) the offset within the untyped cap that was returned
 * Returns the vritual address to which this physical address is mapped or NULL if frame is unmapped
 */
typedef void *(*simple_get_frame_info_fn)(void *data, void *paddr, int size_bits, seL4_CPtr *cap, seL4_Word *ut_offset);

/**
 * Assign the vspace to the current threads ASID pool
 *
 * @param data cookie for the underlying implementation
 *
 * @param vspace to assign
*/
typedef seL4_Error (*simple_ASIDPool_assign_fn)(void *data, seL4_CPtr vspace);

/**
 * Get the total number of caps this library can address
 *
 * @param data cookie for the underlying implementation
*/
typedef int (*simple_get_cap_count_fn)(void *data);

/**
 * Get the nth cap that this library can address
 *
 * @param data cookie for the underlying implementation
 *
 * @param the nth starting at 0
*/
typedef seL4_CPtr (*simple_get_nth_cap_fn)(void *data, int n);


/**
 * Get the cap to init caps with numbering based on bootinfo.h
 *
 * @param data for the underlying implementation
 *
 * @param the value of the enum matching in bootinfo.h
*/

typedef seL4_CPtr (*simple_get_init_cap_fn)(void *data, seL4_CPtr cap);

/**
 * Get the size of the threads cnode in bits
 *
 * @param data for the underlying implementation
*/

typedef uint8_t  (*simple_get_cnode_size_fn)(void *data);

/**
 * Get the amount of untyped caps available
 *
 * @param data for the underlying implementation
 *
*/

typedef int (*simple_get_untyped_count_fn)(void *data);
/**
 * Get the nth untyped cap that this library can address
 *
 * @param data cookie for the underlying implementation
 *
 * @param the nth starting at 0
 *
 * @param the size of the untyped for the returned cap
 *
 * @param the physical address of the returned cap
*/

typedef seL4_CPtr (*simple_get_nth_untyped_fn)(void *data, int n, size_t *size_bits, uintptr_t *paddr, bool *device);

/**
 * Get the amount of user image caps available
 *
 * @param data for the underlying implementation
 *
*/

typedef int (*simple_get_userimage_count_fn)(void *data);

/**
 * Get the nth untyped cap that this library can address
 *
 * @param data cookie for the underlying implementation
 *
 * @param the nth starting at 0
 *
*/

typedef seL4_CPtr (*simple_get_nth_userimage_fn)(void *data, int n);

#ifdef CONFIG_IOMMU
/**
 * Get the IO space capability for the specified PCI device and domain ID
 *
 * @param data cookie for the underlying implementation
 * @param domainID domain ID to request
 * @param deviceID PCI device ID
 * @param path Path to where to put this cap
 *
*/
typedef seL4_Error (*simple_get_iospace_fn)(void *data, uint16_t domainID, uint16_t deviceID, cspacepath_t *path);
#endif

/**
 *
 * Get simple to print all the information it has about its environment
 */
typedef void (*simple_print_fn)(void *data);

typedef struct simple_t {
    void *data;
    simple_get_frame_cap_fn frame_cap;
    simple_get_frame_mapping_fn frame_mapping;
    simple_get_frame_info_fn frame_info;
    simple_ASIDPool_assign_fn ASID_assign;
    simple_get_cap_count_fn cap_count;
    simple_get_nth_cap_fn nth_cap;
    simple_get_init_cap_fn init_cap;
    simple_get_cnode_size_fn cnode_size;
    simple_get_untyped_count_fn untyped_count;
    simple_get_nth_untyped_fn nth_untyped;
    simple_get_userimage_count_fn userimage_count;
    simple_get_nth_userimage_fn nth_userimage;
    simple_print_fn print;
    arch_simple_t arch_simple;
} simple_t;


static inline void *
simple_get_frame_info(simple_t *simple, void *paddr, int size_bits, seL4_CPtr *frame_cap, seL4_Word *ut_offset)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return NULL;
    }
    if (!simple->frame_info) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return NULL;
    }

    return simple->frame_info(simple->data, paddr, size_bits, frame_cap, ut_offset);
}

static inline seL4_Error 
simple_get_frame_cap(simple_t *simple, void *paddr, int size_bits, cspacepath_t *path)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_InvalidArgument;
    }
    if (!simple->frame_cap) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_InvalidArgument;
    }
    return simple->frame_cap(simple->data, paddr, size_bits, path);
}

static inline void *
simple_get_frame_vaddr(simple_t *simple, void *paddr, int size_bits)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return NULL;
    }
    if (simple->frame_mapping) {
        return simple->frame_mapping(simple->data, paddr, size_bits);
    } else {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return NULL;
    }
}

static inline seL4_Error 
simple_get_IRQ_control(simple_t *simple, int irq, cspacepath_t path)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_InvalidArgument;
    }
    if (!simple->arch_simple.irq) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_InvalidArgument;
    }
    return simple->arch_simple.irq(simple->data, irq, path.root, path.capPtr, path.capDepth);
}

static inline seL4_Error 
simple_ASIDPool_assign(simple_t *simple, seL4_CPtr vspace)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_InvalidArgument;
    }
    if (!simple->ASID_assign) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_InvalidArgument;
    }

    return simple->ASID_assign(simple->data, vspace);
}


static inline 
seL4_CPtr simple_get_IOPort_cap(simple_t *simple, uint16_t start_port, uint16_t end_port)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }

    return arch_simple_get_IOPort_cap(&simple->arch_simple, start_port, end_port);
}

static inline int 
simple_get_cap_count(simple_t *simple)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return -1;
    }
    if (!simple->cap_count) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
    }
    return simple->cap_count(simple->data);
}

static inline seL4_CPtr 
simple_get_nth_cap(simple_t *simple, int n)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }

    if (!simple->nth_cap) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_CapNull;
    }

    return simple->nth_cap(simple->data, n);
}

static inline int
simple_get_cnode_size_bits(simple_t *simple)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return -1;
    }
    if (!simple->cnode_size) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return -1;
    }

    return simple->cnode_size(simple->data);
}

static inline seL4_CPtr
simple_init_cap(simple_t *simple, seL4_CPtr cap)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }

    if (!simple->init_cap) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_CapNull;
    }

    return simple->init_cap(simple->data, cap);
}

static inline seL4_CPtr
simple_get_cnode(simple_t *simple)
{
    return simple_init_cap(simple, seL4_CapInitThreadCNode);
}

static inline seL4_CPtr 
simple_get_tcb(simple_t *simple)
{
    return simple_init_cap(simple, seL4_CapInitThreadTCB);
}

static inline seL4_CPtr
simple_get_pd(simple_t *simple)
{
    return simple_init_cap(simple, seL4_CapInitThreadPD);
}

static inline seL4_CPtr 
simple_get_irq_ctrl(simple_t *simple)
{
    return simple_init_cap(simple, seL4_CapIRQControl);
}

static inline seL4_CPtr 
simple_get_init_cap(simple_t *simple, seL4_CPtr cap)
{
    return simple_init_cap(simple, cap);
}

static inline int 
simple_get_untyped_count(simple_t *simple)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return -1;
    }
    if (!simple->untyped_count) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return -1;
    }

    return simple->untyped_count(simple->data);
}

static inline seL4_CPtr
simple_get_nth_untyped(simple_t *simple, int n, size_t *size_bits, uintptr_t *paddr, bool *device)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }
    if (!simple->nth_untyped) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_CapNull;
    }

    return simple->nth_untyped(simple->data, n, size_bits, paddr, device);
}

static inline int 
simple_get_userimage_count(simple_t *simple)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return -1;
    }
    if (!simple->userimage_count) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return -1;
    }

    return simple->userimage_count(simple->data);
}

static inline seL4_CPtr
simple_get_nth_userimage(simple_t *simple, int n)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }
    if (!simple->nth_userimage) {
        ZF_LOGE("%s Not implemented", __FUNCTION__);
        return seL4_CapNull;
    }
    return simple->nth_userimage(simple->data, n);
}

#ifdef CONFIG_IOMMU
static inline seL4_CPtr 
simple_get_iospace(simple_t *simple, uint16_t domainID, uint16_t deviceID, cspacepath_t *path)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }
    if (!simple->arch_simple.iospace) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return seL4_CapNull;
    }

    return simple->arch_simple.iospace(simple->data, domainID, deviceID, path);
}
#endif

#ifdef CONFIG_ARM_SMMU
static inline seL4_Error
simple_get_iospace_cap_count(simple_t *simple, int *count)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_InvalidArgument;
    }
    if (!simple->data) {
        ZF_LOGE("Simple data is NULL");
        return seL4_InvalidArgument;
    }
    if (!count) {
        ZF_LOGE("NULL pointer");
        return seL4_InvalidArgument;
    }
    if (!simple->arch_simple.iospace_cap_count) {
        ZF_LOGE("arch %s not implemented", __FUNCTION__);
        return seL4_IllegalOperation;
    }

    return simple->arch_simple.iospace_cap_count(simple->data, count);
}

static inline seL4_CPtr
simple_get_nth_iospace_cap(simple_t *simple, int n)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return seL4_CapNull;
    }
    if (!simple->data) {
        ZF_LOGE("Simple data is NULL");
        return seL4_CapNull;
    }
    if (!simple->arch_simple.iospace_get_nth_cap) {
        ZF_LOGE("arch %s not implemented", __FUNCTION__);
        return seL4_CapNull;
    }

    return simple->arch_simple.iospace_get_nth_cap(simple->data, n);
}
#endif

static inline void 
simple_print(simple_t *simple)
{
    if (!simple) {
        ZF_LOGE("Simple is NULL");
        return;
    }
    if (!simple->print) {
        ZF_LOGE("%s not implemented", __FUNCTION__);
        return;
    }

    simple->print(simple->data);
}
#endif /* _INTERFACE_SIMPLE_H_ */
