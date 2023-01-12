#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SZ 4096

enum memblock_flags {
	MEMBLOCK_NONE		= 0x0,	/* No special request */
	MEMBLOCK_HOTPLUG	= 0x1,	/* hotpluggable region */
	MEMBLOCK_MIRROR		= 0x2,	/* mirrored region */
	MEMBLOCK_NOMAP		= 0x4,	/* don't add to kernel direct mapping */
	MEMBLOCK_DRIVER_MANAGED = 0x8,	/* always detected via a driver */
};

struct memblock_region_view {
 uint64_t base;
 uint64_t size;
 enum memblock_flags flags;
};

struct memblock_type_view {
 unsigned long cnt;
 unsigned long max;
 uint64_t total_size;
 struct memblock_region_view * regions;
 char* name;
};

struct memblock_view {
 int bottom_up;
 uint64_t current_limit;
 struct memblock_type_view memory;
 struct memblock_type_view reserved;

};


struct vm_area_struct_view {
	unsigned long vm_start;
	unsigned long vm_end;
	unsigned long vm_flags;
	unsigned long vm_pgoff;
	char* filename;
 };


void error_msg() {
	printf("Invalid prog usage. Possible variants:\n");	
	printf("./usr_side -m\n");	
	printf("./usr_side -v <pid>\n");	
}

void print_vm_area(struct vm_area_struct_view* vm_area) {
	printf("VM AREA STRUCT \n");
	printf("vm area start: 0x%lx \n", vm_area->vm_start);
	printf("vm area end: 0x%lx \n", vm_area->vm_end);
	printf("vm area flags: 0x%lx\n", vm_area->vm_flags);
	printf("some basic characteristics: \n");
	if ((vm_area->vm_flags & 1) == 1) {
		printf("+ VM_READ\n");
	} else {
		printf("- VM_READ\n");
	}
	if ((vm_area->vm_flags & 2) == 2) {
		printf("+ VM_WRITE\n");
	 }
	 else {
		 printf("- VM_WRITE\n");
	 }
	 if ((vm_area->vm_flags & 4) == 4) {
		 printf("+ VM_EXEC\n");
	 } else {
		 printf("- VM_EXEC\n");
	 }
	 if ((vm_area->vm_flags & 8) == 8) {
		 printf("+ VM_SHARED\n");
	 } else {
		 printf("- VM_SHARED\n");
	 }
	 printf("file-backed: %s \n", (vm_area->filename == NULL)? "no":"yes");
	 if (vm_area->filename != NULL) {
		 printf("file name: %s \n", vm_area->filename);
		 printf("file offset (in page sz units): 0x%lx \n", vm_area->vm_pgoff); 
	 }
	
}

void print_vm_area1(struct vm_area_struct_view* vm_area) {
	char* rd = ((vm_area->vm_flags & 1) == 1)?"r":"-";
	char* wr = ((vm_area->vm_flags & 2) == 2)?"w":"-";
	char* ex = ((vm_area->vm_flags & 4) == 4)?"e":"-";
	char* sh = ((vm_area->vm_flags & 8) == 8)?"s":"-";
	char* fn = (vm_area->filename != NULL)? vm_area->filename : "-";
	unsigned long offs = (vm_area->filename != NULL)?vm_area->vm_pgoff:0;
	
	printf("0x%010lx  0x%010lx  0x%08lx  %s%s%s%s  0x%04lx  %s\n", 
	vm_area->vm_start, vm_area->vm_end, vm_area->vm_flags,
	rd,wr,ex,sh, offs, fn);

	
}

void print_memblock_region_arr(struct memblock_region_view* arr, uint64_t count) {
	
	if (arr == NULL) {
		printf("no regions specified\n");
	} else {
		for (uint64_t i = 0; i < count; i++) {
			printf("MEMBLOCK REGION %lu\n", i+1);
			printf("base 0x%lx\n", arr[i].base);
			printf("size %lu\n", arr[i].size);
			printf("flags 0x%x\n", arr[i].flags);
			
		}
	}
	
	
}

void print_memblock_type(struct memblock_type_view mt) {
	printf("memblock_type name: %s \n", mt.name);
	printf("total_sz: %lu \n", mt.total_size);
	printf("count: %lu\n", mt.cnt);
	printf("max: %lu\n", mt.max);
	print_memblock_region_arr(mt.regions, mt.cnt);
	
}

void print_memblock(struct memblock_view* memblock) {
	printf("MEMBLOCK STRUCT \n");
	printf("bottom up: %s \n", ((int)memblock->bottom_up== 0)?"no":"yes");
	printf("current limit: 0x%lx \n", (uint64_t)memblock->current_limit);
	printf("memblock types: \n");
	printf("1.\n");
	print_memblock_type(memblock->memory);
	printf("2.\n");
	print_memblock_type(memblock->reserved);
	
}

void print_vm_area_structs(void* dest_arr, int num, int initial) {
		char* pos = (char*)dest_arr;
		if (initial == 1) {
			printf("%14s  %14s  %10s  %4s  %6s  %s\n", 
			"start", "end", "flags", "rwes", "offs", "file");
		}
		for (int i=0; i<num; i++) {
			print_vm_area1((struct vm_area_struct_view*) pos);
			pos = pos + sizeof(struct vm_area_struct_view);
		}
}

int main(int argc, char *argv[])
{
	
	
	if (argc < 2) {
		error_msg();
	} else {
		
		
		int res = 0;
		int memblock = -1;
		int vm_area = -1;
		int pid = -1;
		int buff_sz = MAX_BUFFER_SZ;
		
		while ((res = getopt(argc, argv, "mv:b:")) != -1) {
			
			if (res == 'm') {
				memblock = 1;
			}
			if (res == 'v') {
				vm_area = 1;
				pid = atoi(optarg);
			}
			if (res == '?') {
				printf("Invalid param usage\n");
			}
		
		}
		
		if ((memblock == -1) && (vm_area == -1)) {
			error_msg();
			return -1;
		}
		
		int ans = -1;
		
		if (memblock == 1) {
			
			void* dest_arr = malloc(buff_sz);
			void* buff_arr = malloc(buff_sz);
			int ans = syscall(436, dest_arr, buff_sz, buff_arr, buff_sz);
			
			if (ans == -1) {
				printf("syscall failed; arrs sz is not enough\n");
			} else {
				print_memblock((struct memblock_view*)dest_arr);
			}
			
			free(dest_arr);
			free(buff_arr);
			
		} 
		
		if (vm_area == 1) {
			
			int main_arr_sz = buff_sz;
			int buff_arr_sz = buff_sz;
			int res = syscall(438, pid, &main_arr_sz, &buff_arr_sz);
			printf("%d - arr sz, %d - buff sz\n", main_arr_sz, buff_arr_sz);
			
			void* dest_arr = malloc(main_arr_sz + 100);
			void* buff_arr = malloc(buff_arr_sz + 100);
			
			int ans = syscall(437, pid, dest_arr, main_arr_sz, buff_arr, buff_arr_sz);
			if (ans == -1) {
				printf("No such pid or other error occured\n");
			}
			
			if (ans == 0) {
				printf("No memory mappings\n");
			}
			int struct_num = ans;
			if (ans > 0) {
				
				print_vm_area_structs(dest_arr, ans, 1);
			}
			
			printf("total areas count %d\n", struct_num);
			free(dest_arr);
			free(buff_arr);
			
		} 
		
		
	}
	return 0;
}

