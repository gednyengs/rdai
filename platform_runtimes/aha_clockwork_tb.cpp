#include <iostream>
#include <future>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "../urdai_api/urdai_types.h"

#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "clockwork_testscript.cpp"
#include "unoptimized_conv_3_3.cpp"

using namespace Halide::Tools;
using namespace Halide::Runtime;

static uint32_t hardware_id = 1;
static URDAI_ID urdai_id = {hardware_id};
static URDAI_VLNV urdai_vlnv;

static std::vector< std::future<URDAI_Status> > asyncStatuses;
static uint32_t async_id = 1;

/**
 * Allocate memory on a device
 *
 * @param mem_object_type Type of memory object to allocate
 * @param size The size of the allocation in bytes
 * @param device The device on which to make the allocation (pointer)
 * @return the allocated memory object or NULL (must free the MemObject)
 * */
static URDAI_MemObject *clockwork_mem_allocate( URDAI_MemObjectType mem_object_type, 
												size_t size, URDAI_Device *device )
{
	if (mem_object_type == URDAI_MEM_UNKNOWN) return NULL;

	URDAI_MemObject *memObject = (URDAI_MemObject *) malloc(sizeof(URDAI_MemObject));

	uint8_t *data = (uint8_t *) malloc(size);

	memObject->mem_type 	= mem_object_type;
	memObject->view_type 	= URDAI_VIEW_FULL;
	memObject->device 		= device;
	memObject->parent 		= NULL;
	memObject->host_ptr		= (mem_object_type == URDAI_MEM_HOST || 
							   mem_object_type == URDAI_MEM_SHARED)? data : NULL;
	memObject->device_ptr 	= (mem_object_type == URDAI_MEM_DEVICE || 
							   mem_object_type == URDAI_MEM_SHARED)? data : NULL;
	memObject->size 		= size;

	// memObject->flags 		= 0;
	// memObject->user_tag 	= 0;

	return memObject;
}
/**
 * Free allocated memory on a device
 *
 * @param mem_object The allocated memory object (pointer)
 * @return status
 */
static URDAI_Status clockwork_mem_free( URDAI_MemObject *mem_object )
{
	free(mem_object->host_ptr);
	free(mem_object);

	URDAI_Status status;
	status.status_code = URDAI_OK;

	return status;
}

static URDAI_Status clockwork_mem_copy( URDAI_MemObject *src, URDAI_MemObject *dest )
{

	// need to allocate same host_ptr space as original if host_ptr is unassigned.
	memcpy(dest, src, sizeof(URDAI_MemObject));

	URDAI_Status status;
	status.status_code = URDAI_OK;

	return status;
}

URDAI_Status memcpy_helper( URDAI_MemObject *src, URDAI_MemObject *dest )
{
	memcpy(dest, src, sizeof(URDAI_MemObject));

	URDAI_Status status;
	status.status_code = URDAI_SYNCHRONIZED;

	return status;
}

static URDAI_Status clockwork_mem_copy_async( URDAI_MemObject *src, URDAI_MemObject *dest )
{
	asyncStatuses.push_back( std::async( memcpy_helper, dest, src ) );

	URDAI_Status status;
	status.status_code = URDAI_UNINITIALIZED;
	status.async_handle.id.value = async_id;
	status.async_handle.platform = src->device->platform;
	async_id++;

	return status;
}

static URDAI_MemObject* clockwork_mem_crop( URDAI_MemObject *src, size_t offset, 
											size_t cropped_size )
{
	URDAI_MemObject *croppedObject = (URDAI_MemObject *) malloc(sizeof(URDAI_MemObject));

	croppedObject->mem_type 	= src->mem_type;
	croppedObject->view_type 	= URDAI_VIEW_CROP;
	croppedObject->device 		= src->device;
	croppedObject->parent 		= src;
	croppedObject->host_ptr		= (src->host_ptr)+offset;
	croppedObject->device_ptr 	= (src->device_ptr)+offset;
	croppedObject->size 		= cropped_size;
	// croppedObject->flags 		= 0;
	// croppedObject->user_tag 	= 0;

	return croppedObject;
}

static URDAI_Status clockwork_mem_free_crop( URDAI_MemObject *cropped_mem_object )
{
	free(cropped_mem_object);

	URDAI_Status status;
	status.status_code = URDAI_OK;

	return status;
}

static URDAI_Platform* clockwork_platform_create( void )
{
	URDAI_Platform *platform = (URDAI_Platform *) malloc(sizeof(URDAI_Platform));

	struct URDAI_Device device = {urdai_id, urdai_vlnv, platform, NULL, 1};
	struct URDAI_Platform temp_platform = {URDAI_CLOCKWORK_PLATFORM, urdai_id, NULL, &device};
			
	hardware_id++;

	memcpy(platform, &temp_platform, sizeof(URDAI_Platform));

	return platform;
}

static URDAI_Status clockwork_platform_destroy( URDAI_Platform *platform )
{
	free(platform);
	URDAI_Status status;
	status.status_code = URDAI_UNINITIALIZED;
	return status;
}

static URDAI_Status clockwork_platform_init( URDAI_Platform *platform, void *user_data )
{

}

static URDAI_Status clockwork_platform_deinit( URDAI_Platform *platform, void *user_data )
{

}

static URDAI_Status clockwork_device_init( URDAI_Device *device, void *user_data )
{

}

static URDAI_Status clockwork_device_deinit( URDAI_Device *device, void *user_data )
{

}

/**
TO_DO: adjust device_run to accomodate URDAI_Devices and URDAI_MemObjects
**/
static URDAI_Status run_clockwork_device( URDAI_Device *device, 
										  URDAI_MemObject *mem_object_list )
{
	Buffer<uint8_t> input(64, 64);
	memcpy(input.begin(), mem_object_list->host_ptr, mem_object_list->size);
	Buffer<uint8_t> output(62, 62);

	run_clockwork_program(input, output);
	memcpy(mem_object_list[1].host_ptr, output.begin(), mem_object_list[1].size);

	std::string output_filename = "output_conv_3_3.png";
	convert_and_save_image(output, output_filename);
	cout << "First pixel of output..." << endl;
	cout << (int) output(0, 0) << endl;	

	std::cout << "Ran " << "conv_3_3" << " on " << "clockwork" << "\n";

	URDAI_Status status;
	status.status_code = URDAI_OK;
	return status;
}

URDAI_Status run_clockwork_device_async_helper( URDAI_Device *device,
												URDAI_MemObject *mem_object_list )
{
	Buffer<uint8_t> input(64, 64);
	memcpy(input.begin(), mem_object_list->host_ptr, mem_object_list->size);
	Buffer<uint8_t> output(62, 62);

	run_clockwork_program(input, output);
	memcpy(mem_object_list[1].host_ptr, output.begin(), mem_object_list[1].size);

	std::string output_filename = "output_conv_3_3_async.png";
	convert_and_save_image(output, output_filename);
	cout << "First pixel of output..." << endl;
	cout << (int) output(0, 0) << endl;	

	std::cout << "Ran " << "conv_3_3 " << "async" << " on " << "clockwork" << "\n";

	URDAI_Status status;
	status.status_code = URDAI_SYNCHRONIZED;
	return status;
}

static URDAI_Status run_clockwork_device_async( URDAI_Device *device, 
												URDAI_MemObject *mem_object_list )
{
	asyncStatuses.push_back( std::async( run_clockwork_device_async_helper, device, mem_object_list ));

	URDAI_Status status;
	status.status_code = URDAI_OK;
	status.async_handle.id.value = async_id;
	status.async_handle.platform = device->platform;
	async_id++;	return status;
}

static URDAI_Status clockwork_sync( URDAI_AsyncHandle *async_handle )
{
	return asyncStatuses[async_handle->id.value - 1].get();
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct URDAI_PlatformOps Clockwork_URDAI_Ops = {
		.mem_allocate 		= clockwork_mem_allocate,
		.mem_free 			= clockwork_mem_free,
		.mem_copy 			= clockwork_mem_copy,
		.mem_copy_async 	= clockwork_mem_copy_async,
		.mem_crop 			= clockwork_mem_crop,
		.mem_free_crop 		= clockwork_mem_free_crop,
		.platform_create 	= clockwork_platform_create,
		.platform_destroy  	= clockwork_platform_destroy,
		.platform_init		= clockwork_platform_init,
		.platform_deinit	= clockwork_platform_deinit,
		.device_init 		= clockwork_device_init,
		.device_deinit 		= clockwork_device_deinit,
		.device_run			= run_clockwork_device,
		.device_run_async	= run_clockwork_device_async,
		.sync				= clockwork_sync
};

#ifdef __cplusplus
}
#endif

URDAI_MemObject* load_halide_buffer_to_mem_object(URDAI_Platform* platform, 
												  Buffer<uint8_t> buffer, size_t size)
{
	URDAI_MemObject* memObject = Clockwork_URDAI_Ops.mem_allocate(URDAI_MEM_SHARED, 
																  size, platform->device_list);

	memcpy(memObject->host_ptr, buffer.begin(), size);

	return memObject;
}

URDAI_MemObject* initialize_mem_object_list(URDAI_MemObject* input, URDAI_MemObject* output)
{
	URDAI_MemObject* mem_obj_list = (URDAI_MemObject *) malloc(3 * sizeof(URDAI_MemObject));
	memcpy(&mem_obj_list[0], input, sizeof(URDAI_MemObject));
	memcpy(&mem_obj_list[1], output, sizeof(URDAI_MemObject));
	// last element must be null
	memset(&mem_obj_list[2], 0, sizeof(URDAI_MemObject));

	return mem_obj_list;
}

void URDAI_clockwork_run_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	URDAI_Status curr_status;
	// Launch platform
	URDAI_Platform* clockwork_platform = Clockwork_URDAI_Ops.platform_create();
	// convert input and output to MemObjects
	URDAI_MemObject* URDAI_input = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	URDAI_MemObject* URDAI_output = load_halide_buffer_to_mem_object(clockwork_platform, 
																	 output, output.size_in_bytes());
	URDAI_MemObject* mem_obj_list = initialize_mem_object_list(URDAI_input, URDAI_output);

	curr_status = run_clockwork_device(clockwork_platform->device_list, mem_obj_list);
	// Free memory
	free(mem_obj_list);
	Clockwork_URDAI_Ops.mem_free(URDAI_input);
	Clockwork_URDAI_Ops.mem_free(URDAI_output);
	
	Clockwork_URDAI_Ops.platform_destroy(clockwork_platform);
} 

void URDAI_clockwork_run_async_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	URDAI_Status curr_status;
	URDAI_Status async_status;
	// Launch platform
	URDAI_Platform* clockwork_platform = Clockwork_URDAI_Ops.platform_create();
	// convert input and output to MemObjects
	URDAI_MemObject* URDAI_input = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	URDAI_MemObject* URDAI_output = load_halide_buffer_to_mem_object(clockwork_platform, 
																	 output, output.size_in_bytes());
	URDAI_MemObject* mem_obj_list = initialize_mem_object_list(URDAI_input, URDAI_output);

	async_status = run_clockwork_device_async(clockwork_platform->device_list, mem_obj_list);
	URDAI_AsyncHandle* handle = &async_status.async_handle;

	curr_status = run_clockwork_device(clockwork_platform->device_list, mem_obj_list);

	curr_status = clockwork_sync(handle);
	cout << "Status Code: " << curr_status.status_code << endl;

	// Free memory
	free(mem_obj_list);
	Clockwork_URDAI_Ops.mem_free(URDAI_input);
	Clockwork_URDAI_Ops.mem_free(URDAI_output);
	
	Clockwork_URDAI_Ops.platform_destroy(clockwork_platform);
} 

void URDAI_clockwork_copy_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	URDAI_Status curr_status;
	// Launch platform
	URDAI_Platform* clockwork_platform = Clockwork_URDAI_Ops.platform_create();
	// convert input and output to MemObjects
	URDAI_MemObject* original = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	URDAI_MemObject* copy;

	curr_status = Clockwork_URDAI_Ops.mem_copy(original, copy);

}

int main( int argc, char *argv[] )
{
	Buffer<uint8_t> input = load_and_convert_image((std::string) argv[1]);
	Buffer<uint8_t> output(62,62);

	//URDAI_clockwork_run_test(input, output);

	//URDAI_clockwork_run_async_test(input, output);

	URDAI_clockwork_copy_test(input, output);
}
