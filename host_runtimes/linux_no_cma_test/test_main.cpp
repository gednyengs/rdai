#include <iostream>

#include "rdai_api.h"

extern RDAI_PlatformOps ops;

int main(int argc, char *argv[])
{
    RDAI_VLNV dev_vlnv = {
        { "aha" },
        { "halide_hardware" },
        { "conv_3_3_clockwork" },
        1
    };

    RDAI_Platform *platform = RDAI_register_platform( &ops );
    if( platform ) {
        std::cout << "we got a platform" << "\n";
        RDAI_Device **device_list = RDAI_get_devices_with_vlnv( platform, &dev_vlnv );
        if(device_list) {
            if( device_list[0] ) {
                RDAI_Device *device = device_list[0];

                // print device info
                device->vlnv.vendor.value[RDAI_STRING_ID_LENGTH-1] = '\0';
                device->vlnv.library.value[RDAI_STRING_ID_LENGTH-1] = '\0';
                device->vlnv.name.value[RDAI_STRING_ID_LENGTH-1] = '\0';
                std::cout << "we found a device\n";
                std::cout << "   device vendor: " << device->vlnv.vendor.value << "\n";
                std::cout << "   device library: " << device->vlnv.library.value << "\n";
                std::cout << "   device name: " << device->vlnv.name.value << "\n";
                std::cout << "   device version: " << device->vlnv.version << "\n";

                // get a buffer
                RDAI_MemObject *output = RDAI_mem_shared_allocate( 256 );
                if( output ) {
                    std::cout << "allocated output buffer of size " << output->size << "\n";
                    for(int i = 0; i < 258; i++) {
                        output->host_ptr[i] = 0;
                    }
                    std::cout << "cleared output buffer \n";
                    RDAI_MemObject *mem_obj_list[2] = { output, NULL };
                    RDAI_Status status = RDAI_device_run( device, mem_obj_list );
                    if( status.status_code == RDAI_STATUS_OK ) {
                        std::cout << "device run returned success status code\n";
                        bool passed = true;
                        for(size_t i = 0; i <= 255; i++) {
                            if( output->host_ptr[i] != i ) passed = false;
                        }
                        if( passed ) {
                            std::cout << "TEST PASSED!\n";
                        } else {
                            std::cout << "TEST FAILED\n";
                        }
                    } else {
                        std::cout << "device run return error status code\n";
                    }
                    RDAI_mem_free( output );
                } else {
                    std::cout << "failed to allocate output buffer\n";
                }

            } else {
                std::cout << "no devices found\n";
            }
            RDAI_free_device_list( device_list );
        } else {
            std::cout << "no device list found\n";
        }
        RDAI_unregister_platform( platform );
    } else {
        std::cout << "no platforms found\n";
    }

    return 0;
}
