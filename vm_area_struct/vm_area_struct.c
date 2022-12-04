#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/pid.h>
 
SYSCALL_DEFINE1(vm_area_struct, int, pid_val)
{
 struct pid* pid_val;
 struct task_struct* process;
 struct mm_struct* mm;
 struct vm_area_struct * vm_area;
 
 pid_val = find_get_pid(1);
 process = get_pid_task(pid_val, PIDTYPE_PID);
 if (process == NULL) {
	 printk(KERN_INFO "no task with gived pid \n");
	 return 0;
 }
 mm = process->mm;
 if (mm == NULL) {
	 printk(KERN_INFO "no memory mappings \n");
	 return 0;
 }
 vm_area = mm->mmap;
 
 printk(KERN_INFO "VM area struct \n");
 printk(KERN_INFO "vm area start: 0x%lx \n", vm_area->vm_start);
 printk(KERN_INFO "vm area end: 0x%lx \n", vm_area->vm_end);
 printk(KERN_INFO "vm area flags: 0x%lx\n", vm_area->vm_flags);
 printk(KERN_INFO "some basic characteristics: \n");
 if ((vm_area->vm_flags & 1) == 1) {
	 printk(KERN_INFO "+ VM_READ\n");
 } else {
	 printk(KERN_INFO "- VM_READ\n");
 }
 if ((vm_area->vm_flags & 2) == 2) {
	 printk(KERN_INFO "+ VM_WRITE\n");
 }
 else {
	 printk(KERN_INFO "- VM_WRITE\n");
 }
 if ((vm_area->vm_flags & 4) == 4) {
	 printk(KERN_INFO "+ VM_EXEC\n");
 } else {
	 printk(KERN_INFO "- VM_EXEC\n");
 }
 if ((vm_area->vm_flags & 8) == 8) {
	 printk(KERN_INFO "+ VM_SHARED\n");
 } else {
	 printk(KERN_INFO "- VM_SHARED\n");
 }
 printk(KERN_INFO "file-backed: %s \n", (vm_area->vm_file == NULL)? "no":"yes");
 if (vm_area->vm_file != NULL) {
	 printk(KERN_INFO "file name: %s \n", vm_area->vm_file->f_path.dentry->d_name.name);
	 printk(KERN_INFO "file offset (in page sz units): 0x%lx \n", vm_area->vm_pgoff); 
 }
 return 0;
 
}
