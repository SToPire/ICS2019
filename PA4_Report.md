# <center>PA4实验报告</center>

**<center>2019年12月19日</center>**

## 1. 实验进度

​	完成了所有必做内容。

## 2. 必答题

### 2.1 理解分页机制

分页机制的出现是为了把程序看到的内存和它被真正加载到的内存解耦。通过硬件MMU实现虚拟内存到物理内存的转换，从而解决了绝对代码的加载问题。朴素的分段机制粒度太大，不能满足按需分配的内存使用要求。因此诞生了分页机制，把连续的存储空间分成小片段来组织管理。



页表就是描述虚拟页和物理页之间映射关系的数据结构，特别地，x86中使用了二级页表结构，页目录基地址储存在cr3寄存器中。`_vme_init()`准备内核页表，启动分页机制。

加载用户程序时，`loader()`函数以页为基本单位。在每个循环中（15行），`loader()`申请一页新的物理页，调用`_map()`函数注册虚拟与物理页的映射，并在该物理页的地址写入数据。24-29行把filesz到memsz中间的部分清零，同样要使用分页机制，在物理页的地址写入0.最后要设置当前进程的`max_brk`值，在简化的机制下不存在堆区的回收，因此只要记录堆区的最大值。只有使用到超出此最大值的虚拟内存时才需要继续建立虚拟-物理页映射。在`loader()`中只需要把写入虚拟地址的最大值记作`max_brk`即可。

```c
static uintptr_t loader(PCB* pcb, const char* filename)
{
    int fd = fs_open(filename, 'r', 0);
    Elf_Ehdr E_hdr;
    Elf_Phdr P_hdr;
    fs_read(fd, &E_hdr, sizeof(Elf_Ehdr));

    for (int i = 0; i < E_hdr.e_phnum; i++) {
        fs_lseek(fd, E_hdr.e_phoff + i * E_hdr.e_phentsize, SEEK_SET);
        fs_read(fd, &P_hdr, E_hdr.e_phentsize);
        if (P_hdr.p_type == PT_LOAD) {
            fs_lseek(fd, P_hdr.p_offset, SEEK_SET);
            void* vaddr = (void*)P_hdr.p_vaddr;
            uint32_t i = 0;
            for (; i < P_hdr.p_filesz; i += PGSIZE) {
                void* paddr = new_page(1);
                uint32_t sz = (P_hdr.p_filesz - i >= PGSIZE) ? PGSIZE : (P_hdr.p_filesz - i);
                _map(&pcb->as, vaddr + i, paddr, 0);
                fs_read(fd, paddr, sz);
            }
            vaddr = vaddr + i;
            i = 0;
            uint32_t zero_size = P_hdr.p_memsz - P_hdr.p_filesz;
            for (; i < zero_size;i += PGSIZE){
                void* paddr = new_page(1);
                uint32_t sz = (zero_size - i >= PGSIZE) ? PGSIZE : (zero_size - i);
                _map(&pcb->as, vaddr + i, paddr, 0);
                memset(paddr, 0, sz);
            }
            pcb->max_brk = PGROUNDUP((uint32_t)vaddr + i);
        }
    }
    fs_close(fd);
    return E_hdr.e_entry;
}
```

在`_map()`中，我们需要建立虚拟地址与物理地址之间的映射关系。第3行根据虚拟地址的DIR段获知其页目录的地址，4和5行负责初始化初次使用的页目录表项，第6行用页目录表项中的页表基址和虚拟地址的PAGE段获知其二级页表地址，并在此处建立虚拟-物理地址映射。

```c
int _map(_AddressSpace* as, void* va, void* pa, int prot)
{
    PDE t_pde = ((PDE*)as->ptr)[PDX(va)];
    if (!(t_pde & PTE_P))
        t_pde = ((PDE*)as->ptr)[PDX(va)] = (uint32_t)pgalloc_usr(1) | PTE_P;
    ((PTE*)PTE_ADDR(t_pde))[PTX(va)] = PTE_ADDR(pa) | PTE_P;
    return 0;
}
```

在分配堆区时，对于超出`max_brk`的申请要求，我们应该对超出的部分建立`_map()`映射，因为只有已经与物理地址建立映射的虚拟地址才能被使用。

```c
int mm_brk(uintptr_t brk, intptr_t increment)
{
    if(brk>current->max_brk){
        int len = brk - current->max_brk;
        while(len >0){
            void* paddr = new_page(1);
            _map(&current->as, (void*)current->max_brk, paddr, 0);
            current->max_brk += PGSIZE;
            len -= PGSIZE;
        }
    }
    return 0;
}
```

为了在AM中加入分页机制，我们需要在`_ucontext()`创建的用户进程上下文中加入地址空间指针，并在`__am_irq_handle()`中使用`__am_get_cur_as()`将当前的地址空间描述符指针保存到上下文中，使用`__am_switch()`来切换地址空间, 将调度目标进程的地址空间落实到MMU中。

最后我们需要在硬件中加入分页机制，修改NEMU中的访存函数。`page_translate()`按照文档要求把虚拟地址转化为物理地址，注意处理一条指令跨越两页的特殊情况（5.2介绍）。

```c
uint32_t isa_vaddr_read(vaddr_t addr, int len)
{
    if (cpu.cr0.paging) {
        if (get_OFFSET(addr) + len > PAGE_SIZE) {
            int s1 = PAGE_SIZE - get_OFFSET(addr);
            int s2 = len - s1;
            uint32_t t1 = paddr_read(page_translate(addr), s1);
            uint32_t t2 = paddr_read(page_translate(addr + s1), s2);

            return (t2 << (8 * s1)) | t1;
        } else {
            paddr_t paddr = page_translate(addr);
            return paddr_read(paddr, len);
        }
    } else {
        return paddr_read(addr, len);
    }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len)
{
    if (cpu.cr0.paging) {
        if (get_OFFSET(addr) + len > PAGE_SIZE) {
            int s1 = PAGE_SIZE - get_OFFSET(addr);
            int s2 = len - s1;
            paddr_write(page_translate(addr), data << (8 * s2) >> (8 * s2), s1);
            paddr_write(page_translate(addr + s1), data >> (8 * s1), s2);
        } else {
            paddr_t paddr = page_translate(addr);
            paddr_write(paddr, data, len);
        }
    } else {
        paddr_write(addr, data, len);
    }
}

paddr_t page_translate(vaddr_t addr)
{
    PDE pde;
    PTE pte;
    pde.val = paddr_read((cpu.cr3.page_directory_base << 12) + get_DIR(addr) * sizeof(PDE), sizeof(PDE));
    assert(pde.present == 1);
    pte.val = paddr_read((pde.page_frame << 12) + get_PAGE(addr) * sizeof(PTE), sizeof(PTE));
    assert(pte.present == 1);
    return (pte.page_frame << 12) + get_OFFSET(addr);
}
```

### 2.2 理解中断机制

硬件中断允许了抢占多任务的实现，硬件发出的中断信号让操作系统重新获得CPU的使用权，区别于分时多任务之下，一个程序会一直占用CPU直到它主动让出为止。

NEMU中，每次执行语句都会查看是否有硬件中断到来。

```c
bool isa_query_intr(void)
{
    if (cpu.INTR && cpu.eflags.IF) {
        cpu.INTR = false;
        raise_intr(IRQ_TIMER, cpu.pc);
        return true;
    }
    return false;
}
```

一旦cpu处于开中断状态且收到中断信号，就产生一次时钟中断。

```
//nexus-am: __am_irq_handle()
case 0x20: ev.event = _EVENT_IRQ_TIMER; break;
```

nanos-lite接收到时钟中断后,输出一句话并调用自陷指令：

```
   //nanos-lite: do_evnet()
   case _EVENT_IRQ_TIMER:
            Log("Timer interrupt");
            _yield();
```

NEMU保证(`raise_intr()`设置IF为0)在软件部分处理时钟中断时，cpu处于关中断状态，避免新的中断信号出现。同时软件保证(`_ucontext()`设置IF为1)处理完时钟中断，返回用户进程后，cpu处于开中断状态，继续等待新的中断信号。

### 2.3 解释

分页机制保证了我们可以加载虚拟地址有重合的不同程序到内存中而不发生冲突。

仙剑奇侠传和hello被加载到两个不同的进程。因为输入输出的速度远远慢于CPU的指令执行速度，CPU可以切换到另一个进程实现“同时运行”的效果。每个时钟中断最终会调用`_yield()`，而`_yield()`最终调用调度函数`schedule()`在两个进程之间切换。

## 3. 思考题

### 3.1 为什么需要使用不同的栈空间?

每个进程运行过程中栈空间是不断生长的（如，过程调用），因此使用的栈空间是没有明确范围的。如果让不同进程共用栈空间可能导致它们使用同一片内存作为自己的栈，导致数据被破坏。（也就是一个控制转移到某进程的时候，它原来存在这的内容已经在控制转到另一个进程时被破坏了）

### 3.2 一山不能容二虎？

这样没法正常运行。原因就是后面说的，没有实现进程独立的地址空间，两个程序加载到同一个物理内存位置导致其内容互相覆盖。

### 3.3 虚存管理中PIC的好处

假如说动态链接库不是PIC的，它就没有资格被称为库了，因为调用它的程序不能保证其每次都被加载到固定内存位置。所以说PIC的好处就是加强了代码的模块化和可移植性。

### 3.4 理解分页细节

基地址作为高20位，与虚拟地址中的DIR部分合在一起对页目录寻址，因此基地址是20位。

页表本身就是虚拟和物理地址之间的映射，如果基地址都用了虚拟地址，那就没有办法把这个虚拟地址转换成物理地址了。

不使用一级页表的目的是避免把所有页表一直保存在内存中，通过多级页表来为真正有用（即包含有效信息）的页提供索引。

### 3.5 空指针真的是"空"的吗?

我认为试图解引用空指针时其实是取虚拟地址0上的值，MMU捕捉到了把虚拟地址0转换为物理地址这个不合法的要求从而报错。

### 3.6 内核映射的作用

去掉这段代码会在`page_translate()`中出错。这是因为这部分是内核代码的虚拟/物理地址映射，去掉这部分后内核的地址转换就无法进行了。

### 3.7 支持开机菜单程序的运行 (选做)

其实我这里没有遇到任何问题，直接加载`/bin/init`就能运行了。一位同学原来在`SYS_execve`中使用空指针调用`naive_uload()`，改为使用current指针调用后他也可以运行开机菜单了。

### 3.8 优先级调度（选做）

这个实现比较简单，我的实现是仙剑奇侠传每调度100次，`hello`调度1次，这样做带来的性能提升是非常明显的。

```c
int cnt = 0;
_Context* schedule(_Context* prev)
{
    current->cp = prev;
    if (current == &pcb[0]) current = fg_pcb;
    else if(cnt!=100){
        ++cnt;
        current = fg_pcb;
    } else {
        cnt = 0;
        current = &pcb[0];
    }
    return current->cp;
}
```

## 4. 遇到的问题

### 4.1 关于退出时返回开机菜单

我一开始认为`sys_execve`时也要使用`context_uload()`，思考后发现这样会破坏已经储存的上下文导致错误，因此`execve`仍使用`native_uload()`。

### 4.2 页表相关的数据结构好复杂啊

试图造轮子之前我进行了`grep`，果然发现NEMU中已经有`mmu.h`定义了相关数据结构，只要把把`mmu.h`添加到`common.h`中即可使用。

### 4.3 实现地址空间后，试图运行dummy时的问题

我发现`_yield()`运行后，cr3的寄存器值为0导致地址转换出错。追踪调用过程发现是因为我错误修改`schedule`函数，导致自陷指令运行之后的上下文为空。

### 4.4 试图实现时钟中断时出现问题

在`isa_query_intr`中，`raise_intr`的返回地址应该是`cpu.pc`而非`seq_pc`,同时必须CPU处于开中断(即，IF为1)且`INTR`为高电平时才处理时钟中断。

## 5. 实验心得

### 5.1 关于上下文切换的实现

首先关于寄存器的初始状态，通过观察我发现NEMU运行时寄存器的初值是随机值，所以不用管它们。只要把`eip`设置为跳转地址即可。

其次关于切换栈顶指针的时机，`__am_asm_trap`如下所示：

```asm
__am_asm_trap:
  pushal
  pushl $0

  pushl %esp
  call __am_irq_handle
  addl $4, %esp
  movl %eax, %esp
  addl $4, %esp
  popal
  addl $4, %esp

  iret
```

`movl`应该插在第二条`add`语句之前而非第一条，因为第一条的作用是把传参导致的`esp`变化恢复。

### 5.2 关于`page_translate()`的实现

`cr3`中base这个名字起的很有迷惑性，使人感觉这是一个基址，查页目录时往上加偏移量。但事实上base应该作为地址的高20位。

关于x86的跨页指令处理，要注意小端序的问题。比如前一页3字节，后一页1字节，此时前一页处理的应该是data的低24位而非高24位。

### 5.3 关于`mm_brk()`的实现

不要忘了在`loader()`里设置`max_brk`，值应该是最后一页写入的内存的下一页。

### 5.4 关于`mov_cr2r()`函数的实现

其实我在这里有点迷惑，不知道怎么获得control register的序号，是框架代码中这两个函数里的print语句给了我提示，应该用id_dest指针的reg成员获取cr寄存器标号。

### 5.5 关于实验讲义

在“超越容量的界限”部分中有这样的内容：“同样的, `nanos-lite/src/loader.c`中的`DEFAULT_ENTRY`也需要作相应的修改。”然而我们的PA中`loader()`是根据ELF头寻找跳转地址的，这部分应该是从往年讲义修改的时候遗忘了。

同样，在这部分中还有：“之前我们让`mm_brk()`函数直接返回`0`”，然而之前实现系统调用分配堆区的时候讲义并没有让我们使用这个函数，它是第一次在讲义中出现。

### 5.6 关于调试

做到PA3和4阶段我和很多同学最害怕的一种bug就是`address out of bound`，它出现的原因实在是太多了，PA3那里我因为NEMU中的指令错误导致这一错误，非常难以定位。在PA4部分，尤其是虚拟/物理内存相关，`loader` `map`等不同模块的错误，最终的表现可能都是地址越界。所以讲义中的建议：在`page_translate()`中加入assert是非常有必要的，它在地址转换阶段就拦截了错误，方便调试。

### 5.7 鸣谢

感谢张祎扬同学在PA4.2阶段提供的帮助。