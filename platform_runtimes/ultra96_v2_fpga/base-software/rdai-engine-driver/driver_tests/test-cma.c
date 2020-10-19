#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "rdai-cma.h"

#define N_BUFFERS   10
#define BUF_SIZE    (1024 * 768 * 3)

int main(int argc, char *argv[])
{
    int fd;
    RDAIDrvMemObj mem_objs[N_BUFFERS];

    // open RDAI driver
    fd = open("/dev/rdai_dma", O_RDWR);
    if(fd < 0) {
        printf("failed to open RDAI driver\n");
        return -1;
    }
    printf("successfully opened RDAI driver\n");

    // initialize buffer sizes
    printf("initializing buffer sizes\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        mem_objs[i].size = BUF_SIZE;
    }
    printf("completed buffer size initializations\n");

    // allocate buffers
    printf("allocating buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        if(rdai_cma_alloc(fd, &mem_objs[i]) == 0) {
            printf("successfully allocated buffer[%d]\n", i);
	    printf("\tphys_addr = %lx\n",  mem_objs[i].phys_addr);
	    printf("\tkern_addr = %lx\n",  mem_objs[i].kern_addr);
	    printf("\tsize = %lx\n", mem_objs[i].size);
	    printf("\talloc_size = %lx\n",  mem_objs[i].alloc_size);
        } else {
            printf("failed to allocate buffer[%d]\n", i);
            for(int j = i - 1; j >= 0; j--) {
                printf("deallocating buffer [%d]\n", j);
                rdai_cma_free(fd, &mem_objs[j]);
            }
            close(fd);
            return -1;
        }
    }
    printf("completed buffer allocations\n");

    // mmap buffers
    printf("mmapping the buffers\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        void *user_addr = rdai_cma_mmap(fd, &mem_objs[i]);
        if(user_addr == MAP_FAILED) {
            printf("mapping buffer[%d] failed\n", i);
        } else {
            printf("mapped buffer[%d] @ %lx\n", i, mem_objs[i].user_addr);
	}
    }
    printf("completed buffer mmap\n");

    // perform write/read tests on buffers
    printf("starting write/read test\n");
    for(int i = 0; i < N_BUFFERS; i++) {
        for(int j = 0; j < BUF_SIZE; j++) {
            if((void *)mem_objs[i].user_addr != MAP_FAILED) {
                *((char *)mem_objs[i].user_addr + j) = (j + i * 25) % 256;
            }
        }
    }
    int errors = 0;
    for(int i = 0; i < N_BUFFERS; i++) {
        for(int j = 0; j < BUF_SIZE; j++) {
            if((void *)mem_objs[i].user_addr != MAP_FAILED) {
                char value = *((char *)mem_objs[i].user_addr + j);
                if(value != (j + i * 25) % 256) {
		    errors++;
                    break;
                }
            }
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
        if(rdai_cma_free(fd, &mem_objs[i])) {
            printf("failed to free buffer[%d]\n", i);
        }
    }
    printf("completed freeing buffers\n");

    // close driver
    close(fd);

    if(errors) printf("TEST FAILED!\n");
    else printf("TEST PASSED!\n");

    printf("DONE!\n");

    return 0;
}
