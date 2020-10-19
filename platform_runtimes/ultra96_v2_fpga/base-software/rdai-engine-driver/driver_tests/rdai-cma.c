#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "rdai-cma.h"

typedef struct user_data
{
    uint32_t dev_id;
    uint32_t dev_status;
    char dev_vlnv[128];
    uint64_t timeout;
    RDAIDrvMemObj in_obj;
    RDAIDrvMemObj out_obj;

} user_data;

int rdai_cma_alloc(int fd, RDAIDrvMemObj *mem_obj)
{

    user_data ud;
    int status;

    ud.in_obj = *mem_obj;    
    status = ioctl(fd, ALLOC_BUFFER, &ud);
    *mem_obj = ud.in_obj;

    return status;
}

int rdai_cma_free(int fd, RDAIDrvMemObj *mem_obj)
{
    user_data ud;
    int status;

    ud.in_obj = *mem_obj;    
    status = ioctl(fd, FREE_BUFFER, &ud);
    *mem_obj = ud.in_obj;

    return status;
}

void *rdai_cma_mmap(int fd, RDAIDrvMemObj *mem_obj)
{
    if(!mem_obj || !mem_obj->phys_addr) return NULL;
    mem_obj->user_addr = (uint64_t) mmap(0, mem_obj->alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, ((uint64_t)mem_obj->phys_addr));
    return (void *) mem_obj->user_addr;
}

int rdai_cma_unmap(RDAIDrvMemObj *mem_obj)
{
    return munmap((void *) mem_obj->user_addr, mem_obj->alloc_size);
}
