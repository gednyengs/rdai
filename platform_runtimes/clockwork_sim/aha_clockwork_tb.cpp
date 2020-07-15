#include <iostream>
#include <future>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "rdai_types.h"

#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "clockwork_testscript.h"


using namespace Halide::Tools;
using namespace Halide::Runtime;
using namespace std;

static uint32_t hardware_id = 1;
static RDAI_ID urdai_id = {hardware_id};
static RDAI_VLNV urdai_vlnv;

static vector<future<RDAI_Status> > asyncStatuses;
static uint32_t async_id = 1;

/**
 * Allocate memory on a device
 *
 * @param mem_object_type Type of memory object to allocate
 * @param size The size of the allocation in bytes
 * @param device The device on which to make the allocation (pointer)
 * @return the allocated memory object or NULL (must free the MemObject)
 * */
static RDAI_MemObject *clockwork_mem_allocate( RDAI_MemObjectType mem_object_type, 
												size_t size, RDAI_Device *device )
{
	if (mem_object_type == RDAI_MEM_UNKNOWN) return NULL;

	RDAI_MemObject *memObject = (RDAI_MemObject *) malloc(sizeof(RDAI_MemObject));

	uint8_t *data = (uint8_t *) malloc(size);

	memObject->mem_type 	= mem_object_type;
	memObject->view_type 	= RDAI_VIEW_FULL;
	memObject->device 		= device;
	memObject->parent 		= NULL;
	memObject->host_ptr		= (mem_object_type == RDAI_MEM_HOST || 
							   mem_object_type == RDAI_MEM_SHARED)? data : NULL;
	memObject->device_ptr 	= NULL;
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
static RDAI_Status clockwork_mem_free( RDAI_MemObject *mem_object )
{
	free(mem_object->host_ptr);
	free(mem_object);

	RDAI_Status status;
	status.status_code = RDAI_OK;

	return status;
}

static RDAI_Status clockwork_mem_copy( RDAI_MemObject *src, RDAI_MemObject *dest )
{
	dest->mem_type 		= src->mem_type;
	dest->view_type 	= src->view_type;
	dest->device 		= src->device;
	dest->parent		= src->parent;
	memcpy(dest->host_ptr, src->host_ptr, src->size);
	dest->device_ptr	= src->device_ptr;
	dest->size 			= src->size;

	RDAI_Status status;
	status.status_code = RDAI_OK;

	return status;
}

RDAI_Status memcpy_helper( RDAI_MemObject *src, RDAI_MemObject *dest )
{
	memcpy(dest, src, sizeof(RDAI_MemObject));

	RDAI_Status status;
	status.status_code = RDAI_SYNCHRONIZED;
	return status;
}

static RDAI_Status clockwork_mem_copy_async( RDAI_MemObject *src, RDAI_MemObject *dest )
{
	asyncStatuses.push_back( async( memcpy_helper, dest, src ) );

	RDAI_Status status;
	status.status_code = RDAI_UNINITIALIZED;
	status.async_handle.id.value = async_id;
	status.async_handle.platform = src->device->platform;
	async_id++;

	return status;
}

static RDAI_MemObject* clockwork_mem_crop( RDAI_MemObject *src, size_t offset, 
											size_t cropped_size )
{
	RDAI_MemObject *croppedObject = (RDAI_MemObject *) malloc(sizeof(RDAI_MemObject));

	croppedObject->mem_type 	= src->mem_type;
	croppedObject->view_type 	= RDAI_VIEW_CROP;
	croppedObject->device 		= src->device;
	croppedObject->parent 		= src;
	croppedObject->host_ptr		= (src->host_ptr)+offset;
	croppedObject->device_ptr 	= (src->device_ptr == NULL)? NULL : (src->device_ptr)+offset;
	croppedObject->size 		= cropped_size;
	// croppedObject->flags 		= 0;
	// croppedObject->user_tag 	= 0;

	return croppedObject;
}

static RDAI_Status clockwork_mem_free_crop( RDAI_MemObject *cropped_mem_object )
{
	free(cropped_mem_object);

	RDAI_Status status;
	status.status_code = RDAI_OK;

	return status;
}


// NEED TO MODIFY
static RDAI_Platform* clockwork_platform_create( void )
{
	RDAI_Platform *platform = (RDAI_Platform *) malloc(sizeof(RDAI_Platform));

	RDAI_Device device = {urdai_id, urdai_vlnv, platform, NULL, 1};
	RDAI_Device *device_ptr = &device;
	RDAI_Device **device_list = (RDAI_Device **) malloc(sizeof(RDAI_Device));
	memcpy(device_list, &device_ptr, sizeof(RDAI_Device*));

	RDAI_Platform temp_platform = {RDAI_CLOCKWORK_PLATFORM, urdai_id, NULL, device_list};
			
	hardware_id++;

	memcpy(platform, &temp_platform, sizeof(RDAI_Platform));

	return platform;
}

static RDAI_Status clockwork_platform_destroy( RDAI_Platform *platform )
{
	free(platform->device_list);
	free(platform);
	RDAI_Status status;
	status.status_code = RDAI_UNINITIALIZED;
	return status;
}

static RDAI_Status clockwork_platform_init( RDAI_Platform *platform, void *user_data )
{

}

static RDAI_Status clockwork_platform_deinit( RDAI_Platform *platform, void *user_data )
{

}

static RDAI_Status clockwork_device_init( RDAI_Device *device, void *user_data )
{

}

static RDAI_Status clockwork_device_deinit( RDAI_Device *device, void *user_data )
{

}

static RDAI_Status run_clockwork_device( RDAI_Device *device, 
										  RDAI_MemObject **mem_object_list )
{
	// Buffer<uint8_t> input(64, 64);
	// memcpy(input.begin(), (*mem_object_list)->host_ptr, (*mem_object_list)->size);
	// Buffer<uint8_t> output(62, 62);

	// run_clockwork_program(input, output);
	// memcpy(mem_object_list[1]->host_ptr, output.begin(), mem_object_list[1]->size);

	run_clockwork_program(mem_object_list);

	Buffer<uint8_t> output(62, 62);
	memcpy(output.begin(), mem_object_list[1]->host_ptr, mem_object_list[1]->size);

	string output_filename = "output_conv_3_3.png";
	convert_and_save_image(output, output_filename);
	cout << "First pixel of output..." << endl;
	cout << (int) output(0, 0) << endl;	

	cout << "Ran " << "conv_3_3" << " on " << "clockwork" << "\n";

	RDAI_Status status;
	status.status_code = RDAI_OK;
	return status;
}

RDAI_Status run_clockwork_device_async_helper( RDAI_Device *device,
												RDAI_MemObject **mem_object_list )
{
	// Buffer<uint8_t> input(64, 64);
	// memcpy(input.begin(), (*mem_object_list)->host_ptr, (*mem_object_list)->size);
	// Buffer<uint8_t> output(62, 62);

	// run_clockwork_program(input, output);
	// memcpy(mem_object_list[1]->host_ptr, output.begin(), mem_object_list[1]->size);

	run_clockwork_program(mem_object_list);

	Buffer<uint8_t> output(62, 62);
	memcpy(output.begin(), mem_object_list[1]->host_ptr, mem_object_list[1]->size);

	string output_filename = "output_conv_3_3_async.png";
	convert_and_save_image(output, output_filename);
	cout << "First pixel of output..." << endl;
	cout << (int) output(0, 0) << endl;	

	cout << "Ran " << "conv_3_3 " << "async" << " on " << "clockwork" << "\n";

	RDAI_Status status;
	status.status_code = RDAI_SYNCHRONIZED;
	return status;
}

static RDAI_Status run_clockwork_device_async( RDAI_Device *device, 
												RDAI_MemObject **mem_object_list )
{
	asyncStatuses.push_back( async( run_clockwork_device_async_helper, device, mem_object_list ));

	RDAI_Status status;
	status.status_code = RDAI_OK;
	status.async_handle.id.value = async_id;
	status.async_handle.platform = device->platform;
	async_id++;	
	return status;
}

static RDAI_Status clockwork_sync( RDAI_AsyncHandle *async_handle )
{
	return asyncStatuses[async_handle->id.value - 1].get();
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct RDAI_PlatformOps Clockwork_RDAI_Ops = {
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

RDAI_MemObject* load_halide_buffer_to_mem_object(RDAI_Platform* platform, 
												  Buffer<uint8_t> buffer, size_t size)
{
	RDAI_MemObject* memObject = Clockwork_RDAI_Ops.mem_allocate(RDAI_MEM_SHARED, 
																  size, *(platform->device_list));

	memcpy(memObject->host_ptr, buffer.begin(), size);

	return memObject;
}

RDAI_MemObject** initialize_mem_object_list(RDAI_MemObject* input, RDAI_MemObject* output)
{
	RDAI_MemObject** mem_obj_list = (RDAI_MemObject **) malloc(3 * sizeof(RDAI_MemObject*));
	memcpy(&mem_obj_list[0], &input, sizeof(RDAI_MemObject*));
	memcpy(&mem_obj_list[1], &output, sizeof(RDAI_MemObject*));
	// last element must be null
	memset(&mem_obj_list[2], 0, sizeof(RDAI_MemObject*));

	return mem_obj_list;
}

void RDAI_clockwork_run_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	RDAI_Status curr_status;
	// Launch platform
	RDAI_Platform* clockwork_platform = Clockwork_RDAI_Ops.platform_create();
	// convert input and output to MemObjects
	RDAI_MemObject* RDAI_input = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	RDAI_MemObject* RDAI_output = load_halide_buffer_to_mem_object(clockwork_platform, 
																	 output, output.size_in_bytes());
	RDAI_MemObject** mem_obj_list = initialize_mem_object_list(RDAI_input, RDAI_output);

	curr_status = run_clockwork_device(*(clockwork_platform->device_list), mem_obj_list);
	// Free memory
	free(mem_obj_list);
	Clockwork_RDAI_Ops.mem_free(RDAI_input);
	Clockwork_RDAI_Ops.mem_free(RDAI_output);
	
	Clockwork_RDAI_Ops.platform_destroy(clockwork_platform);
} 

void RDAI_clockwork_run_async_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	RDAI_Status curr_status;
	RDAI_Status async_status;
	// Launch platform
	RDAI_Platform* clockwork_platform = Clockwork_RDAI_Ops.platform_create();
	// convert input and output to MemObjects
	RDAI_MemObject* RDAI_input = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	RDAI_MemObject* RDAI_output = load_halide_buffer_to_mem_object(clockwork_platform, 
																	 output, output.size_in_bytes());
	RDAI_MemObject** mem_obj_list = initialize_mem_object_list(RDAI_input, RDAI_output);

	async_status = run_clockwork_device_async(*(clockwork_platform->device_list), mem_obj_list);
	RDAI_AsyncHandle* handle = &async_status.async_handle;

	curr_status = run_clockwork_device(*(clockwork_platform->device_list), mem_obj_list);

	cout << "--- Sync Call Here ---" << endl;
	curr_status = clockwork_sync(handle);
	cout << "Status Code: " << curr_status.status_code << endl;

	// Free memory
	free(mem_obj_list);
	Clockwork_RDAI_Ops.mem_free(RDAI_input);
	Clockwork_RDAI_Ops.mem_free(RDAI_output);
	
	Clockwork_RDAI_Ops.platform_destroy(clockwork_platform);
} 

void RDAI_clockwork_copy_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	RDAI_Status curr_status;
	// Launch platform
	RDAI_Platform* clockwork_platform = Clockwork_RDAI_Ops.platform_create();
	// convert input and output to MemObjects
	RDAI_MemObject* original = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	RDAI_MemObject* copy = Clockwork_RDAI_Ops.mem_allocate(RDAI_MEM_SHARED, input.size_in_bytes(),
														   *(clockwork_platform->device_list));

	curr_status = Clockwork_RDAI_Ops.mem_copy(original, copy);

	if (copy->mem_type == original->mem_type)
		cout << "mem_type is the same!" << endl;
	else 
		cout << "mem_type not the same." <<endl;

	if (copy->view_type == original->view_type)
		cout << "view_type is the same!" << endl;
	else 
		cout << "view_type not the same." <<endl;
	
	if (copy->device == original->device)
		cout << "device is the same!" << endl;
	else 
		cout << "device not the same." <<endl;

	if (copy->parent == original->parent)
		cout << "parent is the same!" << endl;
	else 
		cout << "parent not the same." <<endl;

	if (copy->host_ptr[0] == original->host_ptr[0])
		cout << "data in host_ptr is the same!" << endl;
	else 
		cout << "data in host_ptr not the same." <<endl;
	
	if (copy->device_ptr == original->device_ptr)
		cout << "data in device_ptr is the same!" << endl;
	else 
		cout << "data in device_ptr not the same." <<endl;

	if (copy->size == original->size)
		cout << "size is the same!" << endl;
	else
		cout << "size not the same." <<endl;

	// for visual confirmation
	Buffer<uint8_t> copy_out(64,64);
	memcpy(copy_out.begin(), copy->host_ptr, copy->size);
	string output_filename = "output_mem_copy_test.png";
	convert_and_save_image(copy_out, output_filename);

	// Free memory
	Clockwork_RDAI_Ops.mem_free(original);
	Clockwork_RDAI_Ops.mem_free(copy);
	Clockwork_RDAI_Ops.platform_destroy(clockwork_platform);
}

void RDAI_clockwork_copy_async_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	RDAI_Status curr_status;
	RDAI_Status async_status;
	// Launch platform
	RDAI_Platform* clockwork_platform = Clockwork_RDAI_Ops.platform_create();
	// convert input and output to MemObjects
	RDAI_MemObject* original = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	RDAI_MemObject* copy = Clockwork_RDAI_Ops.mem_allocate(RDAI_MEM_SHARED, input.size_in_bytes(),
														   *(clockwork_platform->device_list));

	async_status = Clockwork_RDAI_Ops.mem_copy_async(original, copy);
	RDAI_AsyncHandle* handle = &async_status.async_handle;

	RDAI_clockwork_run_test(input, output);

	cout << "--- Sync Call Here ---" << endl;
	curr_status = clockwork_sync(handle);
	cout << "Status Code: " << curr_status.status_code << endl;

	if (copy->mem_type == original->mem_type)
		cout << "mem_type is the same!" << endl;
	else 
		cout << "mem_type not the same." <<endl;

	if (copy->view_type == original->view_type)
		cout << "view_type is the same!" << endl;
	else 
		cout << "view_type not the same." <<endl;
	
	if (copy->device == original->device)
		cout << "device is the same!" << endl;
	else 
		cout << "device not the same." <<endl;

	if (copy->parent == original->parent)
		cout << "parent is the same!" << endl;
	else 
		cout << "parent not the same." <<endl;

	if (copy->host_ptr == original->host_ptr)
		cout << "data in host_ptr is the same!" << endl;
	else 
		cout << "data in host_ptr not the same." <<endl;
	
	if (copy->device_ptr == original->device_ptr)
		cout << "data in device_ptr is the same!" << endl;
	else 
		cout << "data in device_ptr not the same." <<endl;

	if (copy->size == original->size)
		cout << "size is the same!" << endl;
	else
		cout << "size not the same." <<endl;

	// for visual confirmation
	Buffer<uint8_t> copy_out(64,64);
	memcpy(copy_out.begin(), copy->host_ptr, copy->size);
	string output_filename = "output_mem_copy_async_test.png";
	convert_and_save_image(copy_out, output_filename);

	// Free memory
	Clockwork_RDAI_Ops.mem_free(original);
	Clockwork_RDAI_Ops.mem_free(copy);
	Clockwork_RDAI_Ops.platform_destroy(clockwork_platform);
}

void RDAI_clockwork_crop_test(Buffer<uint8_t> input, Buffer<uint8_t> output)
{
	RDAI_Status curr_status;
	// Launch platform
	RDAI_Platform* clockwork_platform = Clockwork_RDAI_Ops.platform_create();
	// convert input and output to MemObjects
	RDAI_MemObject* original = load_halide_buffer_to_mem_object(clockwork_platform, 
																	input, input.size_in_bytes());
	cout << "Original Size in Bytes: " << input.size_in_bytes() << endl;
	
	RDAI_MemObject* crop = Clockwork_RDAI_Ops.mem_crop(original, 2048, 2048);

	if (crop->mem_type == original->mem_type)
		cout << "mem_type is the same!" << endl;
	else 
		cout << "mem_type not the same." <<endl;

	if (crop->view_type == RDAI_VIEW_CROP)
		cout << "view_type is RDAI_VIEW_CROP!" << endl;
	else 
		cout << "view_type not RDAI_VIEW_CROP." <<endl;
	
	if (crop->device == original->device)
		cout << "device is the same!" << endl;
	else 
		cout << "device not the same." <<endl;

	if (crop->parent == original)
		cout << "parent of crop is the original!" << endl;
	else 
		cout << "parent of crop not the original." <<endl;

	if (crop->host_ptr[0] == original->host_ptr[2048])
		cout << "data in host_ptr is cropped!" << endl;
	else 
		cout << "data in host_ptr not sucessfully cropped." <<endl;
	
	if (crop->device_ptr == original->device_ptr)
		cout << "data in device_ptr is cropped!" << endl;
	else 
		cout << "data in device_ptr not sucessfully cropped." <<endl;

	if (crop->size == (original->size)/2)
		cout << "size is croppped!" << endl;
	else
		cout << "size not successfully cropped." <<endl;

	// for visual confirmation
	Buffer<uint8_t> crop_out(64,32);
	memcpy(crop_out.begin(), crop->host_ptr, crop->size);
	string output_filename = "output_mem_crop_test.png";
	convert_and_save_image(crop_out, output_filename);

	// Free memory
	Clockwork_RDAI_Ops.mem_free(original);
	Clockwork_RDAI_Ops.mem_free_crop(crop);
	Clockwork_RDAI_Ops.platform_destroy(clockwork_platform);
}

int main( int argc, char *argv[] )
{
	Buffer<uint8_t> input = load_and_convert_image((string) argv[1]);
	Buffer<uint8_t> output(62,62);

	RDAI_clockwork_run_test(input, output);

	//RDAI_clockwork_run_async_test(input, output);

	//RDAI_clockwork_copy_test(input, output);

	//RDAI_clockwork_copy_async_test(input, output);

	//RDAI_clockwork_crop_test(input, output);
}
