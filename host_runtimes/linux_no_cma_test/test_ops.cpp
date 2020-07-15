#include "rdai_api.h"

// ================= DATA
extern RDAI_Platform clockwork_platform;

static RDAI_Device clockwork_device = {
    { 1 },
    {
        { "aha" },
        { "halide_hardware" },
        { "conv_3_3_clockwork" },
        1
    },
    &clockwork_platform,
    NULL,
    1
};

static RDAI_Device *clockwork_platform_devices[2] = { &clockwork_device, NULL };

RDAI_Platform clockwork_platform = {
    RDAI_PlatformType::RDAI_CLOCKWORK_PLATFORM,
    { 0 },
    NULL,
    clockwork_platform_devices
};


// ================= Helpers
static RDAI_Status make_status_error( RDAI_ErrorReason reason = RDAI_REASON_UNIMPLEMENTED )
{
    RDAI_Status status;
    status.status_code = RDAI_STATUS_ERROR;
    status.error_reason = reason;
    return status;
}


static RDAI_Status make_status_ok()
{
    RDAI_Status status;
    status.status_code = RDAI_STATUS_OK;
    return status;
}

// ================= OPs
static RDAI_MemObject * op_mem_allocate( RDAI_MemObjectType mem_object_type,
                                         size_t size,
                                         RDAI_Device *device)
{
    return NULL;
}


static RDAI_Status op_mem_free( RDAI_MemObject *mem_object )
{
    return make_status_error();
}

static RDAI_Status op_mem_copy( RDAI_MemObject *src, RDAI_MemObject *dest )
{
    return make_status_error();
}

static RDAI_Status op_mem_copy_async( RDAI_MemObject *src, RDAI_MemObject *dest )
{
    return make_status_error();
}

static RDAI_MemObject *op_mem_crop( RDAI_MemObject *src, size_t offset, size_t cropped_size )
{
    return NULL;
}

static RDAI_Status op_mem_free_crop( RDAI_MemObject *obj )
{
    return make_status_error();
}

static RDAI_Platform *op_platform_create( void )
{
    return &clockwork_platform;
}

static RDAI_Status op_platform_destroy( RDAI_Platform *platform )
{
    return make_status_ok();
}

static RDAI_Status op_platform_init( RDAI_Platform *platform, void *user_data )
{
    return make_status_error();
}

static RDAI_Status op_platform_deinit( RDAI_Platform *platform, void *user_data )
{
    return make_status_error();
}

static RDAI_Status op_device_init( RDAI_Device *device, void *user_data )
{
    return make_status_error();
}

static RDAI_Status op_device_deinit( RDAI_Device *device, void *user_data )
{
    return make_status_error();
}

static RDAI_Status op_device_run( RDAI_Device *device, RDAI_MemObject **mem_object_list )
{
    if( device && mem_object_list && mem_object_list[0] ) {
        size_t num_els = 1;
        while(mem_object_list[num_els]) num_els++;
        RDAI_MemObject *output = mem_object_list[num_els - 1];
        for(size_t i = 0; i < output->size; i++) {
            output->host_ptr[i] = i & 0xFF;
        }
        return make_status_ok();
    }
    return make_status_error();
}

static RDAI_Status op_device_run_async( RDAI_Device *device, RDAI_MemObject **mem_object_list )
{
    return op_device_run( device, mem_object_list );
}

static RDAI_Status op_sync( RDAI_AsyncHandle *handle )
{
    if(handle && handle->platform) {
        return make_status_ok();
    }
    return make_status_error();
}

// ======================== PlatformOps

RDAI_PlatformOps ops = {
    .mem_allocate       = op_mem_allocate,
    .mem_free           = op_mem_free,
    .mem_copy           = op_mem_copy,
    .mem_copy_async     = op_mem_copy_async,
    .mem_crop           = op_mem_crop,
    .mem_free_crop      = op_mem_free_crop,
    .platform_create    = op_platform_create,
    .platform_destroy   = op_platform_destroy,
    .platform_init      = op_platform_init,
    .platform_deinit    = op_platform_deinit,
    .device_init        = op_device_init,
    .device_deinit      = op_device_deinit,
    .device_run         = op_device_run,
    .device_run_async   = op_device_run_async,
    .sync               = op_sync
};
