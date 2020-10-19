#ifndef RDAI_CMA_H
#define RDAI_CMA_H

#include <sys/mman.h>
#include <sys/ioctl.h>

#include <stdint.h>

#include "rdai-ioctl.h"

typedef struct RDAIDrvMemObj
{
    uint64_t phys_addr;  //device_ptr
    uint64_t kern_addr;
    uint64_t size;		 //size
    uint64_t alloc_size;
    uint64_t user_addr;  //host_ptr
} RDAIDrvMemObj;

int rdai_cma_alloc(int fd, RDAIDrvMemObj *mem_obj);
int rdai_cma_free(int fd, RDAIDrvMemObj *mem_obj);

void *rdai_cma_mmap(int fd, RDAIDrvMemObj *mem_obj);
int rdai_cma_unmap(RDAIDrvMemObj *mem_obj);


#endif /* RDAI_CMA_H */
