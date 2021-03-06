/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef _VKA_ARCH_OBJECT_H__
#define _VKA_ARCH_OBJECT_H__

#include <vka/vka.h>
#include <vka/kobject_t.h>
#include <utils/util.h>
#include <vka/sel4_arch/object.h>

/*resource allocation interfaces for virtual machine extensions on x86*/
#ifdef CONFIG_VTX
static inline int vka_alloc_vcpu (vka_t *vka, vka_object_t *result)
{
    return vka_alloc_object(vka, seL4_X86_VCPUObject, seL4_X86_VCPUBits, result);
}

static inline int vka_alloc_ept_page_directory_pointer_table (vka_t *vka, vka_object_t *result)
{
    return vka_alloc_object(vka, seL4_X86_EPTPageDirectoryPointerTableObject, seL4_X86_EPTPageDirectoryPointerTableBits, result);
}

static inline int vka_alloc_ept_page_directory (vka_t *vka, vka_object_t *result)
{
    return vka_alloc_object(vka, seL4_X86_EPTPageDirectoryObject, seL4_X86_EPTPageDirectoryBits, result);
}

static inline int vka_alloc_ept_page_table (vka_t *vka, vka_object_t *result)
{
    return vka_alloc_object(vka, seL4_X86_EPTPageTableObject, seL4_X86_EPTPageTableBits, result);
}
#endif /* CONFIG_VTX */

static inline int vka_alloc_io_page_table(vka_t *vka, vka_object_t *result)
{
    return vka_alloc_object(vka, seL4_X86_IOPageTableObject, seL4_IOPageTableBits, result);
}

LEAKY(io_page_table)

#ifdef CONFIG_VTX
LEAKY(vcpu)
LEAKY(ept_page_directory_pointer_table)
LEAKY(ept_page_directory)
LEAKY(ept_page_table)
#endif /* CONFIG_VTX */

static inline unsigned long
vka_arch_get_object_size(seL4_Word objectType)
{
    switch (objectType) {
    case seL4_X86_4K:
        return seL4_PageBits;
    case seL4_X86_LargePageObject:
        return seL4_LargePageBits;
    case seL4_X86_PageTableObject:
        return seL4_PageTableBits;
    case seL4_X86_PageDirectoryObject:
        return seL4_PageDirBits;

#ifdef CONFIG_VTX
    case seL4_X86_VCPUObject:
        return seL4_X86_VCPUBits;
    case seL4_X86_EPTPageDirectoryPointerTableObject:
        return seL4_X86_EPTPageDirectoryPointerTableBits;
    case seL4_X86_EPTPageDirectoryObject:
        return seL4_X86_EPTPageDirectoryBits;
    case seL4_X86_EPTPageTableObject:
        return seL4_X86_EPTPageTableBits;
#endif

    case seL4_X86_IOPageTableObject:
        return seL4_IOPageTableBits;

    default:
        return vka_x86_mode_get_object_size(objectType);
    }
}

#endif /* _VKA_ARCH_OBJECT_H__ */
