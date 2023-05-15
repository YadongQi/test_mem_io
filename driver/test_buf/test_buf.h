#ifndef KDRV_RMEM_H
#define KDRV_RMEM_H

#include <linux/ioctl.h>

#define TBUF_START    _IOWR('r','r',uint32_t*)
#define TBUF_SETLOOPS _IOW('r','s',uint32_t*)
#define TBUF_SETMEMRO _IOW('r','t',uint32_t*)


#endif
