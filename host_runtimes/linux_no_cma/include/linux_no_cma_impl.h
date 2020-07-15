/* Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef RDAI_LINUX_NO_CMAi_IMPL_H
#define RDAI_LINUX_NO_CMA_IMPL_H

#include <map>

#include "rdai_api.h"

class RDAI_Platform_Impl
{
public:

    RDAI_Platform **get_all_platforms( void );
    RDAI_Platform **get_platforms_with_type( const RDAI_PlatformType *platform_type );
    RDAI_Platform **get_platforms_with_property( const RDAI_Property *property );
    RDAI_Platform **get_platforms_with_properties( const RDAI_Property **property_list );
    RDAI_Platform *get_platform_with_id( const RDAI_ID *platform_id );
    RDAI_Status free_platform_list( RDAI_Platform **platform_list );
    
    RDAI_Device **get_all_devices( const RDAI_Platform *platform );
    RDAI_Device **get_devices_with_vlnv( const RDAI_Platform *platform, const RDAI_VLNV *device_vlnv );
    RDAI_Device **get_devices_with_property( const RDAI_Platform *platform, const RDAI_Property *property );
    RDAI_Device **get_devices_with_properties( const RDAI_Platform *platform, const RDAI_Property **property_list );
    RDAI_Device *get_device_with_id( const RDAI_Platform *platform, const RDAI_ID *device_id );
    RDAI_Status free_device_list( RDAI_Device **device_list );

    int platform_has_property( const RDAI_Platform *platform, const RDAI_Property *property );
    int platform_has_properties( const RDAI_Platform *platform, const RDAI_Property **property_list );

    int device_has_property( const RDAI_Device *device, const RDAI_Property *property );
    int device_has_properties( const RDAI_Device *device, const RDAI_Property **property_list );

    RDAI_Platform *register_platform( RDAI_PlatformOps *platform_ops );
    RDAI_Status unregister_platform( RDAI_Platform *platform );

    RDAI_MemObject *mem_host_allocate( size_t size );
    RDAI_MemObject *mem_device_allocate( RDAI_Device *device, size_t size );
    RDAI_MemObject *mem_shared_allocate( size_t size );
    RDAI_Status mem_free( RDAI_MemObject *mem_object );
    RDAI_Status mem_copy( RDAI_MemObject *src, RDAI_MemObject *dest );
    RDAI_Status mem_copy_async( RDAI_MemObject *src, RDAI_MemObject *dest );
    RDAI_MemObject *mem_crop( RDAI_MemObject *src, size_t offset, size_t crop_size );
    RDAI_Status mem_free_crop( RDAI_MemObject *mem_object );

    RDAI_Status platform_init( RDAI_Platform *platform, void *user_data );
    RDAI_Status platform_deinit( RDAI_Platform *platform, void *user_data );

    RDAI_Status device_init( RDAI_Device *device, void *user_data );
    RDAI_Status device_deinit( RDAI_Device *device, void *user_data );

    RDAI_Status device_run( RDAI_Device *device, RDAI_MemObject **mem_object_list );
    RDAI_Status device_run_async( RDAI_Device *device, RDAI_MemObject **mem_object_list );

    RDAI_Status sync( RDAI_AsyncHandle *async_handle );

private:
    std::map<RDAI_Platform *, RDAI_PlatformOps *> platform_to_ops;
    std::map<RDAI_PlatformOps *, RDAI_Platform* > ops_to_platform;
};

#endif // RDAI_LINUX_NO_CMA_IMPL_H
