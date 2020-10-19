#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "rdai-cma.h"

#define N_BUFFERS   2
#define BUF_SIZE    (256 * 256 * 4)

typedef struct UserData
{
    uint32_t dev_id;
    uint32_t dev_status;
    char dev_vlnv[128];
    uint64_t timeout;
    RDAIDrvMemObj in_obj;
    RDAIDrvMemObj out_obj;

} UserData;    

int main(int argc, char *argv[])
{
    int fd_dma;
    RDAIDrvMemObj mem_objs[N_BUFFERS];

    // open RDAI DMA Driver
    fd_dma = open("/dev/rdai_dma", O_RDWR);
    if(fd_dma < 0) {
        printf("failed to open RDAI DMA driver\n");
	return -1;
    }
    printf("successfully opened RDAI DMA driver\n");

    // initialize buffer sizes
    printf("initializing buffer sizes\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        mem_objs[i].size = BUF_SIZE;
    }
    printf("completed buffer size initializations\n");

    // allocate buffers
    printf("allocating buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        if(rdai_cma_alloc(fd_dma, &mem_objs[i]) == 0) {
            printf("successfully allocated buffer[%d]\n", i);
	    printf("\tphys_addr = %lx\n", mem_objs[i].phys_addr);
	    printf("\tkern_addr = %lx\n",  mem_objs[i].kern_addr);
	    printf("\tsize = %lx\n", mem_objs[i].size);
	    printf("\talloc_size = %lx\n", mem_objs[i].alloc_size);
        } else {
            printf("failed to allocate buffer[%d]\n", i);
            for(int j = i - 1; j >= 0; j--) {
                printf("deallocating buffer [%d]\n", j);
                rdai_cma_free(fd_dma, &mem_objs[j]);
            }
            close(fd_dma);
            return -1;
        }
    }
    printf("completed buffer allocations\n");

    // mmap buffers
    printf("mmapping the buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        void *uaddr = rdai_cma_mmap(fd_dma, &mem_objs[i]);
        if(uaddr == MAP_FAILED) {
            printf("mapping buffer[%d] failed\n", i);
        } else {
            printf("mapped buffer[%d] @ %p\n", i, uaddr);
	}
    }
    printf("completed buffer mmap\n");

    // perform writes on input buffer
    printf("starting input buffer writes\n");
    for(uint32_t i = 0; i < 256; i++) {
	for(uint32_t j = 0; j < 256; j++) {
	    uint32_t offset = 256 * i + j;
	    *((uint32_t *) mem_objs[0].user_addr + offset) = 256 * i + j;
	}
    }
    printf("completed input writes\n");

    printf("zero out output\n");
    for(uint32_t i = 0; i < 256; i++) {
	for(uint32_t j = 0; j < 256; j++) {
	    uint32_t offset = 256 * i + j;
	    *((uint32_t *) mem_objs[1].user_addr + offset) = 0;
	}
    }

    // get accelerator ID
    UserData udata;
    udata.timeout = 3000;
    udata.dev_id = 0;
    udata.in_obj = mem_objs[0];
    udata.out_obj = mem_objs[1];
    strcpy(udata.dev_vlnv, "aha");

    if(ioctl(fd_dma, RDAI_GET_DEV_BY_VLNV, &udata)) {
        printf("failed to get device by VLNV\n");
    }

    printf("starting device run async\n");
    if(ioctl(fd_dma, DEVICE_RUN_ASYNC, &udata)) {
        printf("device run async failed!\n");
    } else {
        int status;
        printf("starting device sync\n");
        if(status = ioctl(fd_dma, DEVICE_SYNC, &udata)) {
            printf("device sync failed with code [%d]!\n", status);
        } else {
            printf("device run_async and sync ran\n");
        }
    }

    // perform read check
    int errors = 0;
    printf("starting output buffer reads\n");
    for(uint32_t i = 0; i < 256; i++) {
	for(uint32_t j = 0; j < 256; j++) {
	    uint32_t offset = 256 * i + j;
	    uint32_t value = *((uint32_t *) mem_objs[1].user_addr + offset);
	    uint32_t exp_value = 256 * i + j + 1;
	    if(value != exp_value) {
		errors++;
		printf("failed at [%d, %d] = %d, expected %d\n", i, j, value, exp_value);
	    }
	    printf("[%d, %d] = %d, expected %d\n", i, j, value, exp_value);
	}
    }

    // unmap the buffers
    printf("unmapping buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        if(rdai_cma_unmap(&mem_objs[i])) {
            printf("failed to unmap buffer[%d]\n", i);
        } else {
            printf("successfully unmapped buffer[%d]\n", i);
	}
    }
    printf("completed buffer unmap\n");

    // free the buffers
    printf("freeing buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        if(rdai_cma_free(fd_dma, &mem_objs[i])) {
            printf("failed to free buffer[%d]\n", i);
        }
    }
    printf("completed freeing buffers\n");

    // close RDAI devices/drivers
    close(fd_dma);

    if(errors) {
        printf("TEST FAILED!\n");
    } else {
        printf("TEST PASSED!\n");
    }

    printf("DONE!\n");

    return 0;
}
