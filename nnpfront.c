#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/xmalloc.h>
#include <mini-os/events.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <mini-os/lib.h>
#include <fcntl.h>

#define NNPFRONT_PRINT_DEBUG
#ifdef NNPFRONT_PRINT_DEBUG
#define NNPFRONT_DEBUG(fmt,...) printk("Nnpfront:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#define NNPFRONT_DEBUG_MORE(fmt,...) printk(fmt, ##__VA_ARGS__)
#else
#define NNPFRONT_DEBUG(fmt,...)
#endif
#define NNPFRONT_ERR(fmt,...) printk("Nnpfront:Error " fmt, ##__VA_ARGS__)
#define NNPFRONT_LOG(fmt,...) printk("Nnpfront:Info " fmt, ##__VA_ARGS__)

void init_nnpfront()
{
   const char* nodename;
   char path[512];
   char* value, *err;
   unsigned long long ival;

   printk("============= Init NNP Front ================\n");

   /* Get backend domid */
   if((err = xenbus_read(XBT_NIL, "/local/domain/backend-id", &value))) {
      NNPFRONT_ERR("Unable to read %s during nnpfront initialization! error = %s\n", path, err);
      free(err);
      goto error;
   }
   if(sscanf(value, "%llu", &ival) != 1) {
      NNPFRONT_ERR("%s has non-integer value (%s)\n", path, value);
      free(value);
      goto error;
   }
   free(value);
   domid_t bedomid = ival;

   NNPFRONT_LOG("Initialization Completed successfully\n");
}
