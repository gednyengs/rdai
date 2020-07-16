#include <iostream>
#include <future>
#include <vector>
#include <stdlib.h>
#include <string.h>


#include "HalideBuffer.h"
#include "halide_image_io.h"
#include "clockwork_testscript.h"


using namespace Halide::Tools;
using namespace Halide::Runtime;
using namespace std;

#include "rdai_api.h"
#include "clockwork_sim_platform.h"

RDAI_MemObject* load_halide_buffer_to_mem_object(RDAI_Platform* platform, 
												  Buffer<uint8_t> buffer, size_t size)
{
	RDAI_MemObject* memObject = mem_allocate(RDAI_MEM_SHARED, 
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
	if (argv[1] == NULL) cout << "Must provide an input image as argument!" << endl;
	Buffer<uint8_t> input = load_and_convert_image((string) argv[1]);
	Buffer<uint8_t> output(60,60);

	RDAI_clockwork_run_test(input, output);

	//RDAI_clockwork_run_async_test(input, output);

	//RDAI_clockwork_copy_test(input, output);

	//RDAI_clockwork_copy_async_test(input, output);

	//RDAI_clockwork_crop_test(input, output);
}
