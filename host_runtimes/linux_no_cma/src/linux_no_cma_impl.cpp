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

#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

#include "linux_no_cma_impl.h"

template <typename T>
static T** convert_to_c_list( const std::vector<T *> &src )
{
    size_t n = src.size();
    if( n < 1 ) return NULL;
    T** c_list = (T** ) malloc( sizeof(T*) * (n + 1) );
    if( c_list ) {
        for( size_t i = 0; i < n; i++ ) {
            c_list[i] = src[i];
        }
        c_list[n] = NULL;
    }
    return c_list;
}

template <typename T, typename Callable>
static void traverse_c_list( T** c_list, Callable&& c )
{
    T* item;
    int i = 0;
    while( c_list && (item = c_list[i]) ) {
        c( item );
        i++;
    }
}

template <typename T>
static size_t get_size_of_c_list( T** c_list )
{
    size_t i = 0;
    T* item;
    while( c_list && (item = c_list[i]) ) i++;
    return i;
}

static RDAI_Status make_status_error( RDAI_ErrorReason reason )
{
    RDAI_Status status;
    status.status_code  = RDAI_StatusCode::RDAI_STATUS_ERROR;
    status.error_reason = reason;
    return status;
}

static RDAI_Status make_status_ok()
{
    RDAI_Status status;
    status.status_code  = RDAI_StatusCode::RDAI_STATUS_OK;
    return status;
}

RDAI_Platform** RDAI_Platform_Impl::get_all_platforms( void )
{
    std::vector<RDAI_Platform *> ptfm_vector;
    std::for_each( platform_to_ops.cbegin(), platform_to_ops.cend(), [&ptfm_vector]( const auto& p ) {
                ptfm_vector.push_back( p.first );
            });
    return ::convert_to_c_list<RDAI_Platform>( ptfm_vector );
}

RDAI_Platform** RDAI_Platform_Impl::get_platforms_with_type( const RDAI_PlatformType *platform_type )
{
    if( platform_type ) {
        std::vector<RDAI_Platform *> ptfm_vector;
        std::for_each( platform_to_ops.cbegin(), platform_to_ops.cend(), [&]( const auto& p ) {
                    if( p.first->type == *platform_type ) ptfm_vector.push_back( p.first );
                });
        return ::convert_to_c_list<RDAI_Platform>( ptfm_vector );
    }
    return NULL;
}

RDAI_Platform** RDAI_Platform_Impl::get_platforms_with_property( const RDAI_Property *property )
{
    return NULL;
}

RDAI_Platform** RDAI_Platform_Impl::get_platforms_with_properties( const RDAI_Property **property_list )
{
    return NULL;
}

RDAI_Platform* RDAI_Platform_Impl::get_platform_with_id( const RDAI_ID *id )
{
    return NULL;
}

RDAI_Status RDAI_Platform_Impl::free_platform_list( RDAI_Platform **platform_list )
{
    if( platform_list ) {
        free( platform_list );
        return ::make_status_ok();
    }
    return ::make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}

RDAI_Device** RDAI_Platform_Impl::get_all_devices( const RDAI_Platform *platform )
{
    if( platform && platform->device_list ) {
        std::vector<RDAI_Device *> dev_vector;
        ::traverse_c_list<RDAI_Device>( platform->device_list, [&dev_vector](auto *p) {
                    dev_vector.push_back( p );
                });
        return ::convert_to_c_list<RDAI_Device>( dev_vector );
    }
    return NULL;
}

RDAI_Device** RDAI_Platform_Impl::get_devices_with_vlnv( const RDAI_Platform *platform,
                                                         const RDAI_VLNV *vlnv )
{
    auto vlnv_comp = []( const auto &lhs, const auto &rhs ) -> bool {
        bool verdict = lhs.version == rhs.version;
        verdict = verdict && (strncmp( lhs.vendor.value, rhs.vendor.value, RDAI_STRING_ID_LENGTH ) == 0);
        verdict = verdict && (strncmp( lhs.library.value, rhs.library.value, RDAI_STRING_ID_LENGTH ) == 0);
        verdict = verdict && (strncmp( lhs.name.value, rhs.name.value, RDAI_STRING_ID_LENGTH ) == 0);
        return verdict;
    };

    if( platform && platform->device_list && vlnv ) {
        std::vector<RDAI_Device *> dev_vector;
        ::traverse_c_list<RDAI_Device>( platform->device_list, [&](auto *dev) {
                    if( vlnv_comp(dev->vlnv, *vlnv) ) dev_vector.push_back( dev );
                });
        return ::convert_to_c_list<RDAI_Device>( dev_vector );
    }
    return NULL;
}

RDAI_Device** RDAI_Platform_Impl::get_devices_with_property( const RDAI_Platform *platform,
                                                             const RDAI_Property *property )
{
    return NULL;
}

RDAI_Device** RDAI_Platform_Impl::get_devices_with_properties( const RDAI_Platform *platform,
                                                               const RDAI_Property **property_list )
{
    return NULL;
}

RDAI_Device* RDAI_Platform_Impl::get_device_with_id( const RDAI_Platform *platform, const RDAI_ID *device_id )
{
    return NULL;
}

RDAI_Status RDAI_Platform_Impl::free_device_list( RDAI_Device **device_list )
{
    if( device_list ) {
        free( device_list );
    }
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}

int RDAI_Platform_Impl::platform_has_property( const RDAI_Platform *platform, const RDAI_Property *property )
{
    return 0;
}

int RDAI_Platform_Impl::platform_has_properties( const RDAI_Platform *platform, const RDAI_Property **property_list  )
{
    return 0;
}

int RDAI_Platform_Impl::device_has_property( const RDAI_Device *device, const RDAI_Property *property )
{
    return 0;
}

int RDAI_Platform_Impl::device_has_properties( const RDAI_Device *device, const RDAI_Property **property_list  )
{
    return 0;
}

RDAI_Platform* RDAI_Platform_Impl::register_platform( RDAI_PlatformOps *platform_ops )
{
    if( platform_ops ) {
        RDAI_Platform *platform = ops_to_platform[platform_ops];
        if( platform ) return platform;
        platform = platform_ops->platform_create();
        if( platform ) {
            platform_to_ops[platform] = platform_ops;
            ops_to_platform[platform_ops] = platform;
        }
        return platform;
    }
    return NULL;
}

RDAI_Status RDAI_Platform_Impl::unregister_platform( RDAI_Platform *platform )
{
    if( platform ) {
        RDAI_PlatformOps *platform_ops = platform_to_ops[platform];
        if( platform_ops ) {
            platform_to_ops.erase( platform );
            ops_to_platform.erase( platform_ops );
            return platform_ops->platform_destroy( platform );
        }
        else return ::make_status_error( RDAI_ErrorReason::RDAI_REASON_NO_PLATFORM_OPS );
    }
    return ::make_status_error( RDAI_ErrorReason::RDAI_REASON_NO_PLATFORM );
}

RDAI_MemObject* RDAI_Platform_Impl::mem_host_allocate( size_t size )
{
    if( size ) {
        RDAI_MemObject *mem_obj = (RDAI_MemObject *) malloc( sizeof( RDAI_MemObject ) );
        if( mem_obj ) {
            memset( mem_obj, 0, sizeof( RDAI_MemObject ) );
            uint8_t *host_ptr = (uint8_t *) malloc( size );
            if( host_ptr ) {
                mem_obj->mem_type   = RDAI_MemObjectType::RDAI_MEM_HOST;
                mem_obj->view_type  = RDAI_MemViewType::RDAI_VIEW_FULL;
                mem_obj->device     = NULL;
                mem_obj->parent     = NULL;
                mem_obj->host_ptr   = host_ptr;
                mem_obj->device_ptr = NULL;
                mem_obj->size       = size;
                mem_obj->flags      = 0;
                mem_obj->user_tag   = NULL;
            } else {
                free( mem_obj );
                mem_obj = NULL;
            }
        }
        return mem_obj;
    }
    return NULL;
}

RDAI_MemObject* RDAI_Platform_Impl::mem_device_allocate( RDAI_Device *device, size_t size )
{
    return NULL;
}

RDAI_MemObject* RDAI_Platform_Impl::mem_shared_allocate( size_t size )
{
    RDAI_MemObject *mem_obj = mem_host_allocate( size );
    if( mem_obj ) {
        mem_obj->mem_type = RDAI_MemObjectType::RDAI_MEM_SHARED;
    }
    return mem_obj;
}

RDAI_Status RDAI_Platform_Impl::mem_free( RDAI_MemObject *mem_object )
{
    if( mem_object ) {
        if( (mem_object->mem_type == RDAI_MemObjectType::RDAI_MEM_HOST) ||
            (mem_object->mem_type == RDAI_MemObjectType::RDAI_MEM_SHARED) ) {
            free( mem_object->host_ptr );
            free( mem_object );
            return make_status_ok();
        }
    }
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}

RDAI_Status RDAI_Platform_Impl::mem_copy( RDAI_MemObject *src, RDAI_MemObject *dest )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::mem_copy_async( RDAI_MemObject *src, RDAI_MemObject *dest )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_MemObject* RDAI_Platform_Impl::mem_crop( RDAI_MemObject *src, size_t offset, size_t crop_size )
{
    return NULL;
}

RDAI_Status RDAI_Platform_Impl::mem_free_crop( RDAI_MemObject *mem_object )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::platform_init( RDAI_Platform *platform, void *user_data )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::platform_deinit( RDAI_Platform *platform, void *user_data )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::device_init( RDAI_Device *device, void *user_data )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::device_deinit( RDAI_Device *device, void *user_data )
{
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_UNIMPLEMENTED );
}

RDAI_Status RDAI_Platform_Impl::device_run( RDAI_Device *device, RDAI_MemObject **mem_object_list )
{
    if( device && device->platform && mem_object_list ) {
        size_t num_els = ::get_size_of_c_list<RDAI_MemObject>( mem_object_list );
        if( num_els < 1 ) return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_BUFFER_COUNT );
        RDAI_PlatformOps *ops = platform_to_ops[device->platform];
        if( ops ) {
            return ops->device_run( device, mem_object_list );
        }
        return make_status_error( RDAI_ErrorReason::RDAI_REASON_NO_PLATFORM_OPS );
    }
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}

RDAI_Status RDAI_Platform_Impl::device_run_async( RDAI_Device *device, RDAI_MemObject **mem_object_list )
{
    if( device && device->platform && mem_object_list ) {
        size_t num_els = ::get_size_of_c_list<RDAI_MemObject>( mem_object_list );
        if( num_els < 1 ) return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_BUFFER_COUNT );
        RDAI_PlatformOps *ops = platform_to_ops[device->platform];
        if( ops ) {
            return ops->device_run_async( device, mem_object_list );
        }
        return make_status_error( RDAI_ErrorReason::RDAI_REASON_NO_PLATFORM_OPS );
    }
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}

RDAI_Status RDAI_Platform_Impl::sync( RDAI_AsyncHandle *async_handle )
{
    if( async_handle && async_handle->platform ) {
        RDAI_PlatformOps *ops = platform_to_ops[async_handle->platform];
        if( ops ) {
            return ops->sync( async_handle );
        }
        return make_status_error( RDAI_ErrorReason::RDAI_REASON_NO_PLATFORM_OPS );
    }
    return make_status_error( RDAI_ErrorReason::RDAI_REASON_INVALID_OBJECT );
}
