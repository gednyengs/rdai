#include "urdai_types.h"

static URDAI_MemObject *clockwork_mem_allocate(URDAI_MemObjectType mem_object_type, size_t size, URDAI_Device *device) {

}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct URDAI_PlatformOps ClockWork_URDAI_Ops = {
		.mem_allocate = clockwork_mem_allocate,
};

#ifdef __cplusplus
}
#endif