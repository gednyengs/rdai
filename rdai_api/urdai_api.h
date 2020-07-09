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

#ifndef RDAI_API_H
#define RDAI_API_H

#include <stdlib.h>
#include "rdai_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Get all platforms registered with the runtime
 *
 * @return NULL-terminated list of RDAI_Platform pointers
 */
RDAI_Platform **RDAI_get_all_platforms( void );

/**
 * Get all platforms of a given type registered with the runtime
 *
 * @param platform_type The type of platforms to retrieve
 * @return NULL-terminated list of RDAI_Platform pointers
 */
RDAI_Platform **RDAI_get_platforms_with_type( const RDAI_PlatformType *platform_type );

/**
 * Get platforms with a certain property
 *
 * @param property The property to use in the search for platforms
 * @return NULL-terminated list of RDAI_Platform pointers
 */
RDAI_Platform **RDAI_get_platforms_with_property( const RDAI_Property *property );

/**
 * Get platforms with certain properties
 *
 * @param property_list NULL-terminated list of RDAI_Property pointers to use in the search for platforms
 * @return NULL-terminated list of RDAI_Platform pointers
 */
RDAI_Platform **RDAI_get_platforms_with_properties( const RDAI_Property **property_list );

/**
 * Get a platform with a given ID
 *
 * @param platform_id The ID of the platform to search for
 * @return The retrieved platform pointer or NULL
 */
RDAI_Platform *RDAI_get_platform_with_id( const RDAI_ID * platform_id );

/**
 * Get all devices in a platform
 *
 * @param platform The platform to search devices in
 * @return NULL-terminated list of RDAI_Device pointers
 */
RDAI_Device **RDAI_get_all_devices( const RDAI_Platform *platform );

/**
 * Get all devices in a platform with the specified VLNV
 *
 * @param platform The platform to search devices in
 * @param device_vlnv The VLNV to use in the device search
 * @return NULL-terminated list of RDAI_Device pointers
 */
RDAI_Device **RDAI_get_devices_with_vlnv( const RDAI_Platform *platform, const RDAI_VLNV *device_vlnv );

/**
 * Get all devices with a specific property
 *
 * @param platform The plaform to search devices in
 * @property The property to use in the search
 * @return NULL-terminated list of RDAI_Device pointers
 */
RDAI_Device **RDAI_get_devices_with_property( const RDAI_Platform *platform, const RDAI_Property *property );

/**
 * Get all devices with the specified properties
 *
 * @param platform The platform to search devices in
 * @param property_list NULL-terminated list of RDAI_Property pointers to use in the search for devices
 * @return NULL-terminated list of RDAI_Device pointers
 */
RDAI_Device **RDAI_get_devices_with_properties( const RDAI_Platform *platform, const RDAI_Property **property_list );

/**
 * Get a device with a specific ID
 *
 * @param platform The platform to search devices in
 * @param device_id The device ID to use in the search for devices
 * @return The retrieved device pointer or NULL
 */
RDAI_Device *RDAI_get_device_with_id( const RDAI_Platform *platform, const RDAI_ID *device_id );

/**
 * Check whether a platform has the specified property
 *
 * @param platform The platform to check a property on
 * @param property The property to search for in the given platform
 * @return 0 if the platform does not have the specified property, non-zero otherwise
 */
int RDAI_platform_has_property( const RDAI_Platform *platform, const RDAI_Property *property );

/**
 * Check whether a platform has the specified properties
 *
 * @param platform The platform to check the properties on
 * @param property_list NULL-terminated list of RDAI_Property pointers to search for in the given platform
 * @return 0 if the platform does not have the specified properties, non-zero otherwise
 */
int RDAI_platform_has_properties( const RDAI_Platform *platform, const RDAI_Property **property_list );

/**
 * Check whether a device has the specified property
 *
 * @param device The device to check the property on
 * @param property The property to search for in the given device
 * @return 0 if the device does not have the specified property, non-zero otherwise
 */
int RDAI_device_has_property( const RDAI_Device *device, const RDAI_Property *property );

/**
 * Check whether a device has the specified properties
 *
 * @param device The device to check the properties on
 * @param property_list NULL-terminated list of RDAI_Property pointers to search for in the given device
 * @return 0 if the device does not have the specified properties, non-zero otherwise
 */
int RDAI_device_has_properties( const RDAI_Device *device, const RDAI_Property *property_list );

/**
 * Register a platform with the runtime
 *
 * @param platform_ops RDAI interface implementation for the platform to register
 * @return The registered platform or NULL
 */
RDAI_Platform *RDAI_register_platform( RDAI_PlatformOps *platform_ops );

/**
 * Unregister a platform from the runtime
 *
 * @param platform The platform to unregister
 * @return status
 */
RDAI_Status RDAI_unregister_platform( RDAI_Platform *platform );

/**
 * Allocate a RDAI_MEM_HOST memory object
 *
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
RDAI_MemObject *RDAI_mem_host_allocate( size_t size );

/**
 * Allocate a RDAI_MEM_DEVICE memory object
 *
 * @param device The device to allocate the memory object on
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
RDAI_MemObject *RDAI_mem_device_allocate( RDAI_Device *device, size_t size );

/**
 * Allocate a RDAI_MEM_SHARED memory object
 *
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
RDAI_MemObject *RDAI_mem_shared_allocate( size_t size );

/**
 * Free a memory object
 *
 * @param mem_object The memory object to free
 * @return status
 */
RDAI_Status RDAI_mem_free( RDAI_MemObject *mem_object );

/**
 * Synchronous copy from a memory object to another
 *
 * If src and dest are both RDAI_MEM_DEVICE memory objects, the copy operation
 * is relayed to the platform associated with src memory object
 *
 * @param src The source memory object
 * @param dest The destination memory object
 * @return status
 */
RDAI_Status RDAI_mem_copy( RDAI_MemObject *src, RDAI_MemObject *dest );

/**
 * Asynchronous copy from a memory object to another
 *
 * If src and dest are both RDAI_MEM_DEVICE memory objects, the copy operation
 * is relayed to the platform associated with src memory object
 *
 * @param src The source memory object
 * @param dest The destination memory object
 * @return status (with async handle)
 */
RDAI_Status RDAI_mem_copy_async( RDAI_MemObject *src, RDAI_MemObject *dest );

/**
 * Create a cropped/sliced view of a memory object
 *
 * It is allowed to create a cropped/sliced view of another cropped/sliced view
 *
 * @param The memory object to crop/slice
 * @param offset The address offset in bytes from the origin of the source memory object where
 *               to start cropping
 * @param crop_size The size in bytes of the resulting cropped memory object
 * @return The cropped memory object or NULL
 */
RDAI_MemObject *RDAI_mem_crop( RDAI_MemObject *src, size_t offset, size_t crop_size );

/**
 * Free a cropped/sliced view
 *
 * @param cropped_mem_object The cropped view to free
 * @return status
 */
RDAI_Status RDAI_mem_free_crop( RDAI_MemObject *cropped_mem_object );

/**
 * Initialize a hardware platform
 *
 * The initialization semantics of a hardware platform are platform-dependent
 *
 * @param platform The platform to initialize (pointer)
 * @param user_data Platform-dependent initialization context/data (opaque pointer)
 * @return status
 */
RDAI_Status RDAI_platform_init( RDAI_Platform *platform, void *user_data );

/**
 * Deinitialize a hardware platform
 *
 * The deinitialization semantics of a hardware platform are platform-dependent
 *
 * @param platform The platform to deinitialize (pointer)
 * @param user_data Platform-dependent context/data (opaque pointer)
 * @return status
 */
RDAI_Status RDAI_platform_deinit( RDAI_Platform *platform, void *user_data );

/**
 * Initialize an accelerator device
 *
 * The initialization semantics for an accelerator device are device-dependent
 *
 * @param device The accelerator device to initialize (pointer)
 * @param user_data Device-dependent context/data (opaque pointer)
 * @return status
 */
RDAI_Status RDAI_device_init( RDAI_Device *device, void *user_data );

/**
 * Deinitialize an accelerator device
 *
 * The deinitialization semantics for an acceletator device are device-dependent
 *
 * @param device The device to deinitialize (pointer)
 * @param user_data Device-dependent context/data (opaque pointer)
 * @return status
 */
RDAI_Status RDAI_device_deinit( RDAI_Device *device, void *user_data );

/**
 * Synchronously run an accelerator device
 *
 * @param device The device to run
 * @param mem_object_list A NULL-terminated list of memory objects pointers.
 *                  The last (non-NULL) element designates the output memory object.
 *                  All other elements designate input memory objects.
 * @return status
 *
 * NOTE: The positional meaning of the input memory objects is device-dependent
 */
RDAI_Status RDAI_device_run( RDAI_Device *device, RDAI_MemObject **mem_object_list );

/**
 * Asynchronously run an accelerator device
 *
 * @param device The device to run
 * @param mem_object_list A NULL-terminated list of memory objects pointers.
 *                  The last element is the output memory object.
 *                  All other elements are input memory objects.
 * @return status (with async handle)
 *
 * NOTE: The positional meaning of the input memory objects is device-dependent
 */
RDAI_Status RDAI_device_run_async( RDAI_Device *device, RDAI_MemObject **mem_object_list );

/**
 * Synchronize execution for an async call
 *
 * @param async_handle The handle to the async call to synchronize
 * @return status
 */
RDAI_Status RDAI_sync( RDAI_AsyncHandle *async_handle );

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // RDAI_API_H
