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

#ifndef RDAI_TYPES_H
#define RDAI_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Constants */

#ifndef RDAI_STRING_ID_LENGTH
    #define RDAI_STRING_ID_LENGTH       32      // Length of a String ID
#endif // RDAI_STRING_ID_LENGTH

/* Forward Declarations */
struct RDAI_Platform;
struct RDAI_Device;

/**
 * RDAI Hardware ID
 *
 * @value: an unsigned 32-bit value representing a hardware ID.
 *         Valid ID value must be > 0
 */
typedef struct RDAI_ID
{
    uint32_t value;

} RDAI_ID;

/**
 * RDAI Hardware Platform Type
 *
 * Platform types can be as generic or as specific as needed.
 * This enum provides a mechanism to add additional platform types
 *
 * @RDAI_UNKNOWN_PLATFORM: specifies an unknown platform
 * @RDAI_FPGA_PLATFORM: specifies a generic FPGA platform
 * @RDAI_CGRA_PLATFORM: specifies a generic CGRA platform
 *
 */
typedef enum RDAI_PlatformType
{
    RDAI_UNKNOWN_PLATFORM              = 0,
    RDAI_FPGA_PLATFORM                 = 1,
    RDAI_CGRA_PLATFORM                 = 2,
    RDAI_CLOCKWORK_PLATFORM            = 3,

} RDAI_PlatformType;

/**
 * RDAI Property
 *
 * This enum is used to specify both platform and device properties
 *
 * @RDAI_UNKNOWN_PROPERTY: specifies an unknown property
 * @RDAI_DEVICE_MEM_PRESENT: specifies that the device supports
 *                            RDAI_MEM_DEVICE memory objects
 */
typedef enum RDAI_Property
{
    RDAI_UNKNOWN_PROPERTY              = 0,    
    RDAI_DEVICE_MEM_PRESENT            = 1,

} RDAI_Property;

/**
 * RDAI Status Code
 *
 * This enum captures all the status codes supported.
 *
 * @RDAI_STATUS_OK : Generic Success Code
 * @RDAI_STATUS_ERROR : Generic Error Code
 */
typedef enum RDAI_StatusCode
{
    RDAI_STATUS_OK                     = 0,
    RDAI_STATUS_ERROR                  = 1,

} RDAI_StatusCode;

/**
 * RDAI Status Error Reason
 *
 * This enum captures all the codes associated with status error reasons.
 *
 */
typedef enum RDAI_ErrorReason
{
    RDAI_REASON_NO_PLATFORM             = 0,
    RDAI_REASON_NO_PLATFORM_OPS         = 1,
    RDAI_REASON_UNIMPLEMENTED           = 2,
    RDAI_REASON_INVALID_OBJECT          = 3,
    RDAI_REASON_INVALID_BUFFER_COUNT    = 4,

} RDAI_ErrorReason;

/**
 * RDAI Handle for Asynchronous Calls
 *
 * Asynchronous API calls can be later synchronized using information
 * returned in this struct
 *
 * @id: the handle ID. Valid ID must be > than 0
 * @platform: platform which issued this handle
 */
typedef struct RDAI_AsyncHandle
{
    RDAI_ID id;
    RDAI_Platform *platform;
    void *user_data;
} RDAI_AsyncHandle;

/**
 * RDAI Status
 *
 * A struct to represent a status that is usable across both synchronous
 * and asynchronous API calls
 *
 * @status_code: the returned status code from an API call
 * @error_reason: the returned error reason from an API call
 * @async_handle: If an async API call is made, async_handle will contain
 *                synchronization info
 */
typedef struct RDAI_Status
{
    RDAI_StatusCode status_code;
    RDAI_ErrorReason error_reason;
    RDAI_AsyncHandle async_handle;

} RDAI_Status;

/**
 * RDAI Platform
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
typedef struct RDAI_Platform
{
    RDAI_PlatformType type;
    RDAI_ID id;
    RDAI_Property **property_list;
    RDAI_Device **device_list;

} RDAI_Platform;

/* RDAI String ID
 *
 * This struct defines a generic String ID
 *
 * @value: a null-terminated and size-constrained array of characters representing a String ID
 * Note: value[RDAI_STRING_ID_LENGTH - 1] is assumed to be '\0'
 */
typedef struct RDAI_StringID
{
    char value[RDAI_STRING_ID_LENGTH];

} RDAI_StringID;

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
typedef struct RDAI_VLNV
{
    RDAI_StringID vendor;
    RDAI_StringID library;
    RDAI_StringID name;
    uint32_t version;

} RDAI_VLNV;

/**
 * RDAI Accelerator Device
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
typedef struct RDAI_Device
{
    RDAI_ID id;
    RDAI_VLNV vlnv;
    RDAI_Platform *platform;
    RDAI_Property **property_list;
    uint32_t num_inputs;

} RDAI_Device;

/**
 * RDAI Memory Object Type
 *
 * This enum specifies the supported types of memory objects
 *
 * @RDAI_MEM_UNKNOWN: specifies an unknown memory object type
 * @RDAI_MEM_HOST: specifies a memory object with host view only
 * @RDAI_MEM_DEVICE: specifies a memory object with device view only
 * @RDAI_MEM_SHARED: specifies a memory object with both host and device views
 */
typedef enum RDAI_MemObjectType
{
    RDAI_MEM_UNKNOWN                   = 0,
    RDAI_MEM_HOST                      = 1,
    RDAI_MEM_DEVICE                    = 2,
    RDAI_MEM_SHARED                    = 3,

} RDAI_MemObjectType;

/**
 * RDAI Memory Object View Type
 *
 * This enum specifies the supported types of views for a memory object
 *
 * @RDAI_VIEW_UNKNOWN: specifies an unkown view type
 * @RDAI_VIEW_FULL: specifies that the associated memory object is the original object
 * @RDAI_VIEW_CROP: specifies that the associated memory object is a crop/slice of another memory object
 */
typedef enum RDAI_MemViewType
{
    RDAI_VIEW_UNKNOWN                  = 0,
    RDAI_VIEW_FULL                     = 1,
    RDAI_VIEW_CROP                     = 2,

} RDAI_MemViewType;

/**
 * RDAI Memory Object
 *
 * This struct defines a Memory Object. A memory object is a representation of
 * a data buffer in a specific address space
 *
 * @mem_type: the type of the memory object
 * @view_type: the view type of the memory object
 * @device: in the case of a RDAI_MEM_DEVICE memory object, this specifies the device
 *          holding the memory object. For non-device memory objects, this field
 *          should be NULL
 * @parent: for a crop, this represents the parent memory object from which
 *          the crop is created. For RDAI_VIEW_FULL memory objects, this field should be NULL
 * @host_ptr: a host address view of the memory object (for RDAI_MEM_HOST and RDAI_MEM_SHARED memory objects).
 *            This field should be NULL for RDAI_MEM_DEVICE memory objects
 * @device_ptr: a device address view of the memory object (for RDAI_MEM_DEVICE memory objects).
 *              This field should be NULL for non RDAI_MEM_DEVICE memory object types
 * @size: size in bytes of the memory object
 * @flags: memory object flags
 * @user_tag: a user-defined tag for the memory object
 */
typedef struct RDAI_MemObject RDAI_MemObject;
struct RDAI_MemObject
{
    RDAI_MemObjectType mem_type;
    RDAI_MemViewType view_type;
    RDAI_Device *device;
    RDAI_MemObject *parent;
    uint8_t *host_ptr;
    uint8_t *device_ptr;
    size_t size;
    uint64_t flags;
    void *user_tag;
};

/**
 * RDAI Platform Operations
 *
 * This struct provides an interface that must be implemented by hardware platforms.
 * Any platform that implements this interface can seamlessly work with RDAI-enabled environments
 *
 */
typedef struct RDAI_PlatformOps
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
    RDAI_MemObject *   (* mem_allocate )       ( RDAI_MemObjectType mem_object_type,
                                                  size_t size,
                                                  RDAI_Device *device
                                                );
    /**
     * Free allocated memory on a device
     *
     * @param mem_object The allocated memory object (pointer)
     * @return status
     */
    RDAI_Status        (* mem_free )           ( RDAI_MemObject *mem_object );

    /**
     * Synchronous copy from a memory object to another
     *
     * One of the memory objects must be a RDAI_MEM_DEVICE memory object
     *
     * @param src The source memory object (pointer)
     * @param dest The destination memory object (pointer)
     * @return status
     */
    RDAI_Status        (* mem_copy )           ( RDAI_MemObject *src, RDAI_MemObject *dest );

    /**
     * Asynchronous copy from a memory object to another
     *
     * One of the memory objects must be a RDAI_MEM_DEVICE memory object
     *
     * @param src The source memory object (pointer)
     * @param dest The destination memory object (pointer)
     * @return status (with async handle)
     */
    RDAI_Status        (* mem_copy_async )     ( RDAI_MemObject *src, RDAI_MemObject *dest );

    /**
     * Crop/Slice a memory object
     *
     * @param src The source memory object (can be a cropped memory object)
     * @param offset The address offset in bytes from the origin of the source memory object where to start cropping
     * @param cropped_size The size in bytes of the resulting cropped memory object
     * @return The cropped memory object or NULL
     */
    RDAI_MemObject*    (* mem_crop )           ( RDAI_MemObject *src, size_t offset, size_t cropped_size );

    /**
     * Free a crop
     *
     * @param cropped_mem_object The crop to free (pointer)
     * @return status
     */
    RDAI_Status        (* mem_free_crop )      ( RDAI_MemObject *cropped_mem_object );

    // ================================
    // Device and Platform Management 
    // ================================
    
    /**
     * Create a hardware platform instance
     *
     * @return A hardware platform or NULL
     */
    RDAI_Platform*     (* platform_create )    ( void );

    /**
     * Destroy a hardware platform
     *
     * @param platform The platform to destroy (pointer)
     * @return status
     */
    RDAI_Status        (* platform_destroy )   ( RDAI_Platform *platform );

    /**
     * Initialize a hardware platform
     *
     * The initialization semantics of a hardware platform are platform-dependent
     *
     * @param platform The platform to initialize (pointer)
     * @param user_data Platform-dependent initialization context/data (opaque pointer)
     * @return status
     */
    RDAI_Status        (* platform_init )      ( RDAI_Platform *platform, void *user_data );

    /**
     * Deinitialize a hardware platform
     *
     * The deinitialization semantics of a hardware platform are platform-dependent
     *
     * @param platform The platform to deinitialize (pointer)
     * @param user_data Platform-dependent context/data (opaque pointer)
     * @return status
     */
    RDAI_Status        (* platform_deinit )    ( RDAI_Platform *platform, void *user_data );

    /**
     * Initialize an accelerator device
     *
     * The initialization semantics for an accelerator device are device-dependent
     *
     * @param device The accelerator device to initialize (pointer)
     * @param user_data Device-dependent context/data (opaque pointer)
     * @return status
     */
    RDAI_Status        (* device_init )        ( RDAI_Device *device, void *user_data );

    /**
     * Deinitialize an accelerator device
     *
     * The deinitialization semantics for an acceletator device are device-dependent
     *
     * @param device The device to deinitialize (pointer)
     * @param user_data Device-dependent context/data (opaque pointer)
     * @return status
     */
    RDAI_Status        (* device_deinit )      ( RDAI_Device *device, void *user_data );

    // ================================
    // Execution
    // ================================

    /**
     * Synchronously run an accelerator device
     *
     * @param device The device to run
     * @param mem_object_list A NULL-terminated list of memory object pointers.
     *                  The last (non-NULL) element designates the output memory object.
     *                  All other elements designate input memory objects.
     * @return status
     *
     * NOTE: The positional meaning of the input memory objects is device-dependent
     */
    RDAI_Status        (* device_run )         ( RDAI_Device *device, RDAI_MemObject **mem_object_list );

    /**
     * Asynchronously run an accelerator device
     *
     * @param device The device to run
     * @param mem_object_list A NULL-terminated list of memory object pointers.
     *                  The last element is the output memory object.
     *                  All other elements are input memory objects.
     * @return status (with async handle)
     *
     * NOTE: The positional meaning of the input memory objects is device-dependent
     */
    RDAI_Status        (* device_run_async )   ( RDAI_Device *device, RDAI_MemObject **mem_object_list );

    /**
     * Synchronize execution for an async call
     *
     * @param async_handle The handle to the async call to synchronize
     * @return status
     */
    RDAI_Status        ( *sync )               ( RDAI_AsyncHandle *async_handle );

} RDAI_PlatformOps;


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // RDAI_TYPES_H
