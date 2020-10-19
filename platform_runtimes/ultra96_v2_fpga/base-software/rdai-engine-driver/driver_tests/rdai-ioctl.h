#ifndef RDAI_IOCTL_H
#define RDAI_IOCTL_H

#include <linux/ioctl.h>

#define RDAI_MAGIC  'Z'

/* Buffer Related IOCTLs */
#define ALLOC_BUFFER            _IOWR(RDAI_MAGIC, 0x20, void *)
#define FREE_BUFFER             _IOWR(RDAI_MAGIC, 0x21, void *)

/* Execution Related IOCTLs */
#define DEVICE_RUN_ASYNC        _IOWR(RDAI_MAGIC, 0x30, void *)
#define DEVICE_SYNC             _IOWR(RDAI_MAGIC, 0x31, void *)

/* Management Related IOCTLs */
#define RDAI_GET_DEV_BY_VLNV    _IOWR(RDAI_MAGIC, 0x40, void *)
#define RDAI_GET_DEV_BY_ID      _IOWR(RDAI_MAGIC, 0x41, void *)
#define RDAI_GET_DEV_STATUS     _IOWR(RDAI_MAGIC, 0x42, void *)


#endif /* RDAI_IOCTL_H */
