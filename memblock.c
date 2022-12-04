#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/memblock.h>
 
SYSCALL_DEFINE0(memblock)
{
 	printk(KERN_INFO "memblock \n");
 	printk(KERN_INFO "bottom up: %s \n", ((int)memblock.bottom_up== 0)?"no":"yes");
 	printk(KERN_INFO "current limit: 0x%llx \n", (u64)memblock.current_limit);
 	printk(KERN_INFO "memblock types: \n");
 	printk(KERN_INFO "1.\n");
 	printk(KERN_INFO "memory name: %s \n", memblock.memory.name);
 	printk(KERN_INFO "total_sz: 0x%llx \n", (u64)memblock.memory.total_size);
 	printk(KERN_INFO "count: %lu\n", memblock.memory.cnt);
 	printk(KERN_INFO "2.\n");
 	printk(KERN_INFO "memory name: %s \n", memblock.reserved.name);
 	printk(KERN_INFO "total_sz: 0x%llx \n", (u64)memblock.reserved.total_size);
 	printk(KERN_INFO "count: %lu \n", memblock.reserved.cnt);
 	return 0;
}
