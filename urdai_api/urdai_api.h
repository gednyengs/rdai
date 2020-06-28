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

#ifndef URDAI_API_H
#define URDAI_API_H

#include <stdlib.h>
#include "urdai_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Get all platforms registered with the runtime
 *
 * @return NULL-terminated list of platforms
 */
URDAI_Platform *URDAI_get_all_platforms( void );

/**
 * Get all platforms of a given type registered with the runtime
 *
 * @param platform_type The type of platforms to retrieve
 * @return NULL-terminated list of platforms
 */
URDAI_Platform *URDAI_get_platforms_with_type( const URDAI_PlatformType *platform_type );

/**
 * Get platforms with a certain property
 *
 * @param property The property to use in the search for platforms
 * @return NULL-terminated list of platforms
 */
URDAI_Platform *URDAI_get_platforms_with_property( const URDAI_Property *property );

/**
 * Get platforms with certain properties
 *
 * @param property_list The list of properties to use in the search for platforms
 * @return NULL-terminated list of platforms
 */
URDAI_Platform *URDAI_get_platforms_with_properties( const URDAI_Property *property_list );

/**
 * Get a platform with a given ID
 *
 * @param platform_id The ID of the platform to search for
 * @return The retrieved platform pointer or NULL
 */
URDAI_Platform *URDAI_get_platform_with_id( const URDAI_ID * platform_id );

/**
 * Get all devices in a platform
 *
 * @param platform The platform to search devices in
 * @return NULL-terminated list of devices
 */
URDAI_Device *URDAI_get_all_devices( const URDAI_Platform *platform );

/**
 * Get all devices in a platform with the specified VLNV
 *
 * @param platform The platform to search devices in
 * @param device_vlnv The VLNV to use in the device search
 * @return NULL-terminated list of devices
 */
URDAI_Device *URDAI_get_devices_with_vlnv( const URDAI_Platform *platform, const URDAI_VLNV *device_vlnv );

/**
 * Get all devices with a specific property
 *
 * @param platform The plaform to search devices in
 * @property The property to use in the search
 * @return NULL-terminated list of devices
 */
URDAI_Device *URDAI_get_devices_with_property( const URDAI_Platform *platform, const URDAI_Property *property );

/**
 * Get all devices with the specified properties
 *
 * @param platform The platform to search devices in
 * @param property_list NULL-terminated list of properties to use in the search for devices
 * @return NULL-terminated list of devices
 */
URDAI_Device *URDAI_get_devices_with_properties( const URDAI_Platform *platform, const URDAI_Property *property_list );

/**
 * Get a device with a specific ID
 *
 * @param platform The platform to search devices in
 * @param device_id The device ID to use in the search for devices
 * @return The retrieved device pointer or NULL
 */
URDAI_Device *URDAI_get_device_with_id( const URDAI_Platform *platform, const URDAI_ID *device_id );

/**
 * Check whether a platform has the specified property
 *
 * @param platform The platform to check a property on
 * @param property The property to search for in the given platform
 * @return 0 if the platform does not have the specified property, non-zero otherwise
 */
int URDAI_platform_has_property( const URDAI_Platform *platform, const URDAI_Property *property );

/**
 * Check whether a platform has the specified properties
 *
 * @param platform The platform to check the properties on
 * @param property_list NULL-terminated list of properties to search for in the given platform
 * @return 0 if the platform does not have the specified properties, non-zero otherwise
 */
int URDAI_platform_has_properties( const URDAI_Platform *platform, const URDAI_Property *property_list );

/**
 * Check whether a device has the specified property
 *
 * @param device The device to check the property on
 * @param property The property to search for in the given device
 * @return 0 if the device does not have the specified property, non-zero otherwise
 */
int URDAI_device_has_property( const URDAI_Device *device, const URDAI_Property *property );

/**
 * Check whether a device has the specified properties
 *
 * @param device The device to check the properties on
 * @param property_list NULL-terminated list of properties to search for in the given device
 * @return 0 if the device does not have the specified properties, non-zero otherwise
 */
int URDAI_device_has_properties( const URDAI_Device *device, const URDAI_Property *property_list );

/**
 * Register a platform with the runtime
 *
 * @param platform_ops URDAI interface implementation for the platform to register
 * @return The registered platform or NULL
 */
URDAI_Platform *URDAI_register_platform( URDAI_PlatformOps *platform_ops );

/**
 * Unregister a platform from the runtime
 *
 * @param platform The platform to unregister
 * @return status
 */
URDAI_Status URDAI_unregister_platform( URDAI_Platform *platform );

/**
 * Allocate a URDAI_MEM_HOST memory object
 *
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
URDAI_MemObject *URDAI_mem_host_allocate( size_t size );

/**
 * Allocate a URDAI_MEM_DEVICE memory object
 *
 * @param device The device to allocate the memory object on
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
URDAI_MemObject *URDAI_mem_device_allocate( URDAI_Device *device, size_t size );

/**
 * Allocate a URDAI_MEM_SHARED memory object
 *
 * @param size The allocation size in bytes
 * @return The allocated memory object or NULL
 */
URDAI_MemObject *URDAI_mem_shared_allocate( size_t size );

/**
 * Free a memory object
 *
 * @param mem_object The memory object to free
 * @return status
 */
URDAI_Status URDAI_mem_free( URDAI_MemObject *mem_object );

/**
 * Synchronous copy from a memory object to another
 *
 * If src and dest are both URDAI_MEM_DEVICE memory objects, the copy operation
 * is relayed to the platform associated with src memory object
 *
 * @param src The source memory object
 * @param dest The destination memory object
 * @return status
 */
URDAI_Status URDAI_mem_copy( URDAI_MemObject *src, URDAI_MemObject *dest );

/**
 * Asynchronous copy from a memory object to another
 *
 * If src and dest are both URDAI_MEM_DEVICE memory objects, the copy operation
 * is relayed to the platform associated with src memory object
 *
 * @param src The source memory object
 * @param dest The destination memory object
 * @return status (with async handle)
 */
URDAI_Status URDAI_mem_copy_async( URDAI_MemObject *src, URDAI_MemObject *dest );

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
URDAI_MemObject *URDAI_mem_crop( URDAI_MemObject *src, size_t offset, size_t crop_size );

/**
 * Free a cropped/sliced view
 *
 * @param cropped_mem_object The cropped view to free
 * @return status
 */
URDAI_Status URDAI_mem_free_crop( URDAI_MemObject *cropped_mem_object );

/**
 * Initialize a hardware platform
 *
 * The initialization semantics of a hardware platform are platform-dependent
 *
 * @param platform The platform to initialize (pointer)
 * @param user_data Platform-dependent initialization context/data (opaque pointer)
 * @return status
 */
URDAI_Status URDAI_platform_init( URDAI_Platform *platform, void *user_data );

/**
 * Deinitialize a hardware platform
 *
 * The deinitialization semantics of a hardware platform are platform-dependent
 *
 * @param platform The platform to deinitialize (pointer)
 * @param user_data Platform-dependent context/data (opaque pointer)
 * @return status
 */
URDAI_Status URDAI_platform_deinit( URDAI_Platform *platform, void *user_data );

/**
 * Initialize an accelerator device
 *
 * The initialization semantics for an accelerator device are device-dependent
 *
 * @param device The accelerator device to initialize (pointer)
 * @param user_data Device-dependent context/data (opaque pointer)
 * @return status
 */
URDAI_Status URDAI_device_init( URDAI_Device *device, void *user_data );

/**
 * Deinitialize an accelerator device
 *
 * The deinitialization semantics for an acceletator device are device-dependent
 *
 * @param device The device to deinitialize (pointer)
 * @param user_data Device-dependent context/data (opaque pointer)
 * @return status
 */
URDAI_Status URDAI_device_deinit( URDAI_Device *device, void *user_data );

/**
 * Synchronously run an accelerator device
 *
 * @param device The device to run
 * @param mem_object_list A NULL-terminated list of memory objects.
 *                  The last (non-NULL) element designates the output memory object.
 *                  All other elements designate input memory objects.
 * @return status
 *
 * NOTE: The positional meaning of the input memory objects is device-dependent
 */
URDAI_Status URDAI_device_run( URDAI_Device *device, URDAI_MemObject *mem_object_list );

/**
 * Asynchronously run an accelerator device
 *
 * @param device The device to run
 * @param mem_object_list A NULL-terminated list of memory objects.
 *                  The last element is the output memory object.
 *                  All other elements are input memory objects.
 * @return status (with async handle)
 *
 * NOTE: The positional meaning of the input memory objects is device-dependent
 */
URDAI_Status URDAI_device_run_async( URDAI_Device *device, URDAI_MemObject *mem_object_list );

/**
 * Synchronize execution for an async call
 *
 * @param async_handle The handle to the async call to synchronize
 * @return status
 */
URDAI_Status URDAI_sync( URDAI_AsyncHandle *async_handle );

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // URDAI_API_H
