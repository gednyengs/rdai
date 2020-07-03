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

#ifndef URDAI_TYPES_H
#define URDAI_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Constants */

#ifndef URDAI_STRING_ID_LENGTH
    #define URDAI_STRING_ID_LENGTH          32      // Length of a String ID
#endif // URDAI_STRING_ID_LENGTH

/* Forward Declarations */
struct URDAI_Platform;
struct URDAI_Device;

/**
 * URDAI Hardware ID
 *
 * @value: an unsigned 32-bit value representing a hardware ID.
 *         Valid ID value must be > 0
 */
typedef struct URDAI_ID
{
    uint32_t value;

} URDAI_ID;

/**
 * URDAI Hardware Platform Type
 *
 * Platform types can be as generic or as specific as needed.
 * This enum provides a mechanism to add additional platform types
 *
 * @URDAI_UNKNOWN_PLATFORM: specifies an unknown platform
 * @URDAI_FPGA_PLATFORM: specifies a generic FPGA platform
 * @URDAI_CGRA_PLATFORM: specifies a generic CGRA platform
 *
 */
typedef enum URDAI_PlatformType
{
    URDAI_UNKNOWN_PLATFORM              = 0,
    URDAI_FPGA_PLATFORM                 = 1,
    URDAI_CGRA_PLATFORM                 = 2,
    URDAI_CLOCKWORK_PLATFORM            = 3,

} URDAI_PlatformType;

/**
 * URDAI Property
 *
 * This enum is used to specify both platform and device properties
 *
 * @URDAI_UNKNOWN_PROPERTY: specifies an unknown property
 * @URDAI_DEVICE_MEM_PRESENT: specifies that the device supports
 *                            URDAI_MEM_DEVICE memory objects
 */
typedef enum URDAI_Property
{
    URDAI_UNKNOWN_PROPERTY              = 0,    
    URDAI_DEVICE_MEM_PRESENT            = 1,

} URDAI_Property;

/**
 * URDAI Status Code
 *
 * This enum captures all the status codes supported.
 *
 * @URDAI_UNKNOWN_STATUS_CODE: specifies an unknown status code 
 * @URDAI_INITIALIZED: specifies that an entity is initialized and ready to be used
 * @URDAI_UNINITIALIZED: specifies that an entity is not initialized and
 *                       therefore isn't ready for use
 */
typedef enum URDAI_StatusCode
{
    URDAI_UNKNOWN_STATUS_CODE           = 0,
    URDAI_INITIALIZED                   = 1,
    URDAI_UNINITIALIZED                 = 2,
    URDAI_OK                            = 3,

} URDAI_StatusCode;

/**
 * URDAI Handle for Asynchronous Calls
 *
 * Asynchronous API calls can be later synchronized using information
 * returned in this struct
 *
 * @id: the handle ID. Valid ID must be > than 0
 * @platform: platform which issued this handle
 */
typedef struct URDAI_AsyncHandle
{
    URDAI_ID id;
    URDAI_Platform *platform;

} URDAI_AsyncHandle;

/**
 * URDAI Status
 *
 * A struct to represent a status that is usable across both synchronous
 * and asynchronous API calls
 *
 * @status_code: the returned status code from an API call
 * @async_handle: If an async API call is made, async_handle will contain
 *                synchronization info
 */
typedef struct URDAI_Status
{
    URDAI_StatusCode status_code;
    URDAI_AsyncHandle async_handle;

} URDAI_Status;

/**
 * URDAI Platform
 *
 * A Platform is a reconfigurable device on which accelerators
 * can be implemented.
 *
 * @type: the type of the hardware platform
 * @id: the assigned ID for a platform
 * @property_list: a NULL-terminated list of platform properties
 *                 (when NULL, the list is assumed empty)
 * @device_list: a NULL-terminated list of devices provided by the platform
 *               (when NULL, the list is assumed empty)
 */
typedef struct URDAI_Platform
{
    URDAI_PlatformType type;
    URDAI_ID id;
    URDAI_Property *const property_list;
    URDAI_Device *const device_list;

} URDAI_Platform;

/* URDAI String ID
 *
 * This struct defines a generic String ID
 *
 * @value: a null-terminated and size-constrained array of characters representing a String ID
 * Note: value[URDAI_STRING_ID_LENGTH - 1] is assumed to be '\0'
 */
typedef struct URDAI_StringID
{
    char value[URDAI_STRING_ID_LENGTH];

} URDAI_StringID;

/**
 * VLNV Descriptor
 *
 * This struct defines a tuple used to identity accelerator devices
 *
 * @vendor: the name of the vendor of an accelerator device
 * @library (or class): the library/class name of an accelerator device
 * @name: the name of an accelerator device
 * @version: the version of an accelerator device
 */
typedef struct URDAI_VLNV
{
    URDAI_StringID vendor;
    URDAI_StringID library;
    URDAI_StringID name;
    uint32_t version;

} URDAI_VLNV;

/**
 * URDAI Accelerator Device
 *
 * An accelerator device is a device dedicated to the computation
 * of a hardware kernel within a given hardware platform
 *
 * @id: the assigned ID of the accelerator device
 * @vlnv: vendor-library-name-version info for the accelerator device
 * @platform: the platform containing the accelerator device
 * @property_list: a NULL-terminated list of device properties
 *                 (when NULL, the list is assumed empty)
 * @num_inputs: the number of inputs on the accelerator device.
 * 
 * NOTE: accelerator device are assumed to have only 1 output.
 *       However, the output can be a tuple
 */
typedef struct URDAI_Device
{
    URDAI_ID id;
    URDAI_VLNV vlnv;
    URDAI_Platform *const platform;
    URDAI_Property *const property_list;
    uint32_t num_inputs;

} URDAI_Device;

/**
 * URDAI Memory Object Type
 *
 * This enum specifies the supported types of memory objects
 *
 * @URDAI_MEM_UNKNOWN: specifies an unknown memory object type
 * @URDAI_MEM_HOST: specifies a memory object with host view only
 * @URDAI_MEM_DEVICE: specifies a memory object with device view only
 * @URDAI_MEM_SHARED: specifies a memory object with both host and device views
 */
typedef enum URDAI_MemObjectType
{
    URDAI_MEM_UNKNOWN                   = 0,
    URDAI_MEM_HOST                      = 1,
    URDAI_MEM_DEVICE                    = 2,
    URDAI_MEM_SHARED                    = 3,

} URDAI_MemObjectType;

/**
 * URDAI Memory Object View Type
 *
 * This enum specifies the supported types of views for a memory object
 *
 * @URDAI_VIEW_UNKNOWN: specifies an unkown view type
 * @URDAI_VIEW_FULL: specifies that the associated memory object is the original object
 * @URDAI_VIEW_CROP: specifies that the associated memory object is a crop/slice of another memory object
 */
typedef enum URDAI_MemViewType
{
    URDAI_VIEW_UNKNOWN                  = 0,
    URDAI_VIEW_FULL                     = 1,
    URDAI_VIEW_CROP                     = 2,

} URDAI_MemViewType;

/**
 * URDAI Memory Object
 *
 * This struct defines a Memory Object. A memory object is a representation of
 * a data buffer in a specific address space
 *
 * @mem_type: the type of the memory object
 * @view_type: the view type of the memory object
 * @device: in the case of a URDAI_MEM_DEVICE memory object, this specifies the device
 *          holding the memory object. For non-device memory objects, this field
 *          should be NULL
 * @parent: for a crop, this represents the parent memory object from which
 *          the crop is created. For URDAI_VIEW_FULL memory objects, this field should be NULL
 * @host_ptr: a host address view of the memory object (for URDAI_MEM_HOST and URDAI_MEM_SHARED memory objects).
 *            This field should be NULL for URDAI_MEM_DEVICE memory objects
 * @device_ptr: a device address view of the memory object (for URDAI_MEM_DEVICE memory objects).
 *              This field should be NULL for non URDAI_MEM_DEVICE memory object types
 * @size: size in bytes of the memory object
 * @flags: memory object flags
 * @user_tag: a user-defined tag for the memory object
 */
typedef struct URDAI_MemObject URDAI_MemObject;
struct URDAI_MemObject
{
    URDAI_MemObjectType mem_type;
    URDAI_MemViewType view_type;
    URDAI_Device *device;
    URDAI_MemObject *parent;
    uint8_t *host_ptr;
    uint8_t *device_ptr;
    size_t size;
    uint64_t flags;
    void *user_tag;
};

/**
 * URDAI Platform Operations
 *
 * This struct provides an interface that must be implemented by hardware platforms.
 * Any platform that implements this interface can seamlessly work with URDAI-enabled environments
 *
 */
typedef struct URDAI_PlatformOps
{
    // ================================
    // Memory Management
    // ================================

    /**
     * Allocate memory on a device
     *
     * @param mem_object_type Type of memory object to allocate
     * @param size The size of the allocation in bytes
     * @param device The device on which to make the allocation (pointer)
     * @return the allocated memory object or NULL
     * */
    URDAI_MemObject *   (* mem_allocate )       ( URDAI_MemObjectType mem_object_type,
                                                  size_t size,
                                                  URDAI_Device *device
                                                );
    /**
     * Free allocated memory on a device
     *
     * @param mem_object The allocated memory object (pointer)
     * @return status
     */
    URDAI_Status        (* mem_free )           ( URDAI_MemObject *mem_object );

    /**
     * Synchronous copy from a memory object to another
     *
     * One of the memory objects must be a URDAI_MEM_DEVICE memory object
     *
     * @param src The source memory object (pointer)
     * @param dest The destination memory object (pointer)
     * @return status
     */
    URDAI_Status        (* mem_copy )           ( URDAI_MemObject *src, URDAI_MemObject *dest );

    /**
     * Asynchronous copy from a memory object to another
     *
     * One of the memory objects must be a URDAI_MEM_DEVICE memory object
     *
     * @param src The source memory object (pointer)
     * @param dest The destination memory object (pointer)
     * @return status (with async handle)
     */
    URDAI_Status        (* mem_copy_async )     ( URDAI_MemObject *src, URDAI_MemObject *dest );

    /**
     * Crop/Slice a memory object
     *
     * @param src The source memory object (can be a cropped memory object)
     * @param offset The address offset in bytes from the origin of the source memory object where to start cropping
     * @param cropped_size The size in bytes of the resulting cropped memory object
     * @return The cropped memory object or NULL
     */
    URDAI_MemObject*    (* mem_crop )           ( URDAI_MemObject *src, size_t offset, size_t cropped_size );

    /**
     * Free a crop
     *
     * @param cropped_mem_object The crop to free (pointer)
     * @return status
     */
    URDAI_Status        (* mem_free_crop )      ( URDAI_MemObject *cropped_mem_object );

    // ================================
    // Device and Platform Management 
    // ================================
    
    /**
     * Create a hardware platform instance
     *
     * @return A hardware platform or NULL
     */
    URDAI_Platform*     (* platform_create )    ( void );

    /**
     * Destroy a hardware platform
     *
     * @param platform The platform to destroy (pointer)
     * @return status
     */
    URDAI_Status        (* platform_destroy )   ( URDAI_Platform *platform );

    /**
     * Initialize a hardware platform
     *
     * The initialization semantics of a hardware platform are platform-dependent
     *
     * @param platform The platform to initialize (pointer)
     * @param user_data Platform-dependent initialization context/data (opaque pointer)
     * @return status
     */
    URDAI_Status        (* platform_init )      ( URDAI_Platform *platform, void *user_data );

    /**
     * Deinitialize a hardware platform
     *
     * The deinitialization semantics of a hardware platform are platform-dependent
     *
     * @param platform The platform to deinitialize (pointer)
     * @param user_data Platform-dependent context/data (opaque pointer)
     * @return status
     */
    URDAI_Status        (* platform_deinit )    ( URDAI_Platform *platform, void *user_data );

    /**
     * Initialize an accelerator device
     *
     * The initialization semantics for an accelerator device are device-dependent
     *
     * @param device The accelerator device to initialize (pointer)
     * @param user_data Device-dependent context/data (opaque pointer)
     * @return status
     */
    URDAI_Status        (* device_init )        ( URDAI_Device *device, void *user_data );

    /**
     * Deinitialize an accelerator device
     *
     * The deinitialization semantics for an acceletator device are device-dependent
     *
     * @param device The device to deinitialize (pointer)
     * @param user_data Device-dependent context/data (opaque pointer)
     * @return status
     */
    URDAI_Status        (* device_deinit )      ( URDAI_Device *device, void *user_data );

    // ================================
    // Execution
    // ================================

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
    URDAI_Status        (* device_run )         ( URDAI_Device *device, URDAI_MemObject *mem_object_list );

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
    URDAI_Status        (* device_run_async )   ( URDAI_Device *device, URDAI_MemObject *mem_object_list );

    /**
     * Synchronize execution for an async call
     *
     * @param async_handle The handle to the async call to synchronize
     * @return status
     */
    URDAI_Status        ( *sync )               ( URDAI_AsyncHandle *async_handle );

} URDAI_PlatformOps;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // URDAI_TYPES_H
