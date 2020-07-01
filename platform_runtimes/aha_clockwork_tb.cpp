#include <stdlib.h>
#include "urdai_types.h"

/**
 * Allocate memory on a device
 *
 * @param mem_object_type Type of memory object to allocate
 * @param size The size of the allocation in bytes
 * @param device The device on which to make the allocation (pointer)
 * @return the allocated memory object or NULL (must free the MemObject)
 * */
static URDAI_MemObject *clockwork_mem_allocate( URDAI_MemObjectType mem_object_type, 
												size_t size, URDAI_Device *device ) {
	if (mem_object_type != URDAI_MEM_UNKNOWN) return NULL;

	URDAI_MemObject *memObject = malloc(sizeof(URDAI_MemObject));

	uint8_t *data = malloc(size)

	memObject->mem_type 	= mem_object_type;
	memObject->view_type 	= URDAI_VIEW_FULL;
	memObject->device 		= device;
	memObject->parent 		= NULL;
	memObject->host_ptr		= (mem_object_type == URDAI_MEM_HOST || 
							   mem_object_type == URDAI_MEM_SHARED)? data : NULL;
	memObject->device_ptr 	= (mem_object_type == URDAI_MEM_DEVICE || 
							   mem_object_type == URDAI_MEM_SHARED)? data : NULL;
	memObject->size 		= size;
	memObject->flags 		= NULL;
	memObject->user_tag 	= NULL;

	return memObject;
}
/**
 * Free allocated memory on a device
 *
 * @param mem_object The allocated memory object (pointer)
 * @return status
 */
static URDAI_Status clockwork_mem_free( URDAI_MemObject *mem_object ) {
	free(mem_object->host_ptr);
	free(mem_object);

	URDAI_Status status;

	status.status_code = URDAI_UNINITIALIZED;
	return status;
}

static URDAI_Status clockwork_mem_copy( URDAI_MemObject *src, URDAI_MemObject *dest ){
	memcpy(dest, src, sizeof(URDAI_MemObject));

	URDAI_Status status;

	status->status_code = URDAI_INITIALIZED;
	return status;
}

static URDAI_Status clockwork_mem_copy_async( URDAI_MemObject *src, URDAI_MemObject *dest ){
	memcpy(dest, src, sizeof(URDAI_MemObject));

	URDAI_Status status;

	status->status_code = URDAI_UNINITIALIZED;
	status->async_handle.id = 1;
	status->async_handle.platform = ??;

	return status;
}

URDAI_MemObject* clockwork_mem_crop( URDAI_MemObject *src, 
									 size_t offset, size_t cropped_size ){
	URDAI_MemObject *croppedObject = malloc(sizeof(URDAI_MemObject));

	memObject->mem_type 	= src->mem_type;
	memObject->view_type 	= URDAI_VIEW_CROP;
	memObject->device 		= src->device;
	memObject->parent 		= src;
	memObject->host_ptr		= (mem_object_type == URDAI_MEM_HOST || 
							   mem_object_type == URDAI_MEM_SHARED)? (src->data)+offset : NULL;
	memObject->device_ptr 	= (mem_object_type == URDAI_MEM_DEVICE || 
							   mem_object_type == URDAI_MEM_SHARED)? (src->data)+offset  : NULL;
	memObject->size 		= cropped_size;
	memObject->flags 		= NULL;
	memObject->user_tag 	= NULL;

	return croppedObject;
}

URDAI_Status clockwork_mem_free_crop( URDAI_MemObject *cropped_mem_object ){
	free(cropped_mem_object);

	URDAI_Status status;

	status->status_code = URDAI_UNINITIALIZED;
	status->async_handle.id = NULL;
	status->async_handle.platform = NULL;

	return status;
}

URDAI_Platform* clockwork_platform_create( void ){
	return NULL;
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct URDAI_PlatformOps ClockWork_URDAI_Ops = {
		.mem_allocate 		= clockwork_mem_allocate,
		.mem_free 			= clockwork_mem_free,
		.mem_copy 			= clockwork_mem_copy,
		.mem_copy_async 	= clockwork_mem_copy_async,
		.mem_crop 			= clockwork_mem_crop,
		.mem_free_crop 		= clockwork_mem_free_crop,
		.platform_create 	= clockwork_platform_create,


};

#ifdef __cplusplus
}
#endif