#ifndef KDRV_RMEM_H
#define KDRV_RMEM_H

#include <linux/ioctl.h>

#define TBUF_START    _IOWR('r','r',uint32_t*)
#define TBUF_STOP    _IOWR('r','s',uint32_t*)
#define TBUF_SETMEMRO _IOW('r','t',uint32_t*)
#define TBUF_SET_KSETMRO _IOW('r','u',uint64_t*)
#define TBUF_SET_KSETMRW _IOW('r','v',uint64_t*)


#endif
