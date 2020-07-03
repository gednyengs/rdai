#include <iostream>
#include <future>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "../urdai_api/urdai_types.h"

#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace Halide::Tools;
using namespace Halide::Runtime;

static uint32_t hardware_id = 1;

static std::vector< std::future<void*> > asyncFutures;
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
	status.async_handle.id.value = 0;
	return status;
}

static URDAI_Status clockwork_mem_copy( URDAI_MemObject *src, URDAI_MemObject *dest )
{
	memcpy(dest, src, sizeof(URDAI_MemObject));

	URDAI_Status status;

	status.status_code = URDAI_OK;
	status.async_handle.id.value = 0;
	return status;
}

static URDAI_Status clockwork_mem_copy_async( URDAI_MemObject *src, URDAI_MemObject *dest )
{
	asyncFutures.push_back(std::async(memcpy, dest, src, sizeof(URDAI_MemObject)));

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
	status.async_handle.id.value = 0;

	return status;
}

static URDAI_Platform* clockwork_platform_create( void )
{
	URDAI_Platform *platform = (URDAI_Platform *) malloc(sizeof(URDAI_Platform));

	platform->type 			= URDAI_CLOCKWORK_PLATFORM;
	platform->id.value 		= hardware_id;
	hardware_id++;
	//platform->property_list = NULL;

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
    // input image and run on design
    HWStream<hw_uint<16> > input_stream;
    HWStream<hw_uint<16> > output_stream;
    for (int y = 0; y < input.height(); y++) {
    for (int x = 0; x < input.width(); x++) {
    for (int c = 0; c < input.channels(); c++) {
      hw_uint<16> in_val;
      set_at<0*16, 16, 16>(in_val, input(x, y, c));
      input_stream.write(in_val);
    } } }

    // run function
    unoptimized_conv_3_3(input_stream, output_stream);

    // copy to output
    for (int y = 0; y < output.height(); y++) {
    for (int x = 0; x < output.width(); x++) {
    for (int c = 0; c < output.channels(); c++) {
      hw_uint<16> actual = output_stream.read();
      auto actual_lane_0 = actual.extract<0*16, 15>();
      output(x, y, c) = actual_lane_0;
    } } }
}

static URDAI_Status run_clockwork_device_async( URDAI_Device *device, 
												URDAI_MemObject *mem_object_list )
{

}

static URDAI_Status clockwork_sync( URDAI_AsyncHandle *async_handle )
{
	asyncFutures[async_handle->id.value - 1].get();

	URDAI_Status status;
	status.status_code = URDAI_OK;

	return status;
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

int main( int argc, char *argv[] )
{
	URDAI_Platform* clockwork_platform = Clockwork_URDAI_Ops.platform_create();

	Buffer<float> input(64,64);
	//std::cout << argv[1] << "\n";
	input = load_and_convert_image((std::string) argv[1]);
	
	size_t size = input.size_in_bytes();
	std::cout << "Size in Bytes: " << size << "\n";

	URDAI_MemObject* input_object = Clockwork_URDAI_Ops.mem_allocate(URDAI_MEM_SHARED, size, clockwork_platform->device_list);

	memcpy(input_object->host_ptr, input.begin(), size);



}