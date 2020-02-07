# <center>PA3实验报告</center>

**<center>2019年11月29日</center>**

## 1. 实验进度

​	完成了所有必做内容。

## 2. 必答题

请结合代码解释仙剑奇侠传, 库函数, libos, Nanos-lite, AM, NEMU是如何相互协助, 来分别完成游戏存档的读取和屏幕的更新。

### 2.1 游戏存档的读取

在`navy-apps/apps/pal/src/global/global.c`的`PAL_LoadGame()`中通过`fread()`读取游戏存档，在这里`fread()`是一个属于Newlib的库函数，定义在`navy-apps/libs/libc/src/stdio/fread.c`中，它会用缓存区暂存从文件读出的内容并一次性写入用户缓存区，因此要用到系统调用中的`_read()`和`_sbrk()`。

#### 2.1.1 关于_read()

在使用库函数`fopen()`打开文件的时候(`navy-apps/libs/libc/src/stdio/fopen.c`)，C库设置了文件描述符结构体的各属性，其中就包括读和写函数：

```c
fp->_file = f;
fp->_flags = flags;
fp->_cookie = (void *) fp;
fp->_read = __sread;
fp->_write = __swrite;
fp->_seek = __sseek;
fp->_close = __sclose;
```

其中`__sread`定义在`navy-apps/libs/libc/src/stdio/stdio.c`中：

```c
_READ_WRITE_RETURN_TYPE
__sread (struct _reent *ptr,
       void *cookie,
       char *buf,
       _READ_WRITE_BUFSIZE_TYPE n)
{
  register FILE *fp = (FILE *) cookie;
  register ssize_t ret;

#ifdef __SCLE
  int oldmode = 0;
  if (fp->_flags & __SCLE)
    oldmode = setmode (fp->_file, O_BINARY);
#endif

  ret = _read_r (ptr, fp->_file, buf, n);

#ifdef __SCLE
  if (oldmode)
    setmode (fp->_file, oldmode);
#endif

  /* If the read succeeded, update the current offset.  */

  if (ret >= 0)
    fp->_offset += ret;
  else
    fp->_flags &= ~__SOFF;	/* paranoia */
  return ret;
}
```

不难发现，执行实际读取工作的是第16行的`_read_r`函数，它被定义在`navy-apps/libs/libc/src/reent/readr.c`中：

```c
_ssize_t
_read_r (struct _reent *ptr,
     int fd,
     void *buf,
     size_t cnt)
{
  _ssize_t ret;

  errno = 0;
  if ((ret = (_ssize_t)_read (fd, buf, cnt)) == -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}
```

这是最后一层封装了，第10行的`_read`就是我们的系统调用接口。

#### 2.1.2 关于`_sbrk()`

`_sbrk()`定义在`navy-apps/libs/libc/src/syscalls/syssbrk.c`中：

```c
void *
sbrk (ptrdiff_t incr)
{
  return _sbrk_r (_REENT, incr);
}
```

它封装了定义在`navy-apps/libs/libc/src/reent/sbrkr.c`中的`_sbrk_r()`函数：

```c
void *
_sbrk_r (struct _reent *ptr,
     ptrdiff_t incr)
{
  char *ret;
  void *_sbrk(ptrdiff_t);

  errno = 0;
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
    ptr->_errno = errno;
  return ret;
}
```

这是最后一层封装，它使用了系统调用接口`_sbrk()`。



以上内容都是库函数对系统调用的进一步封装，它们最终调用的都是`libos/src/nanos.c`中的函数，也就是系统调用在用户层的封装。这些函数使用`_syscall_`，通过直接插入汇编码的形式将系统调用请求发往nanos-lite：

```c
intptr_t _syscall_(intptr_t type, intptr_t a0, intptr_t a1, intptr_t a2)
{
    register intptr_t _gpr1 asm(GPR1) = type;
    register intptr_t _gpr2 asm(GPR2) = a0;
    register intptr_t _gpr3 asm(GPR3) = a1;
    register intptr_t _gpr4 asm(GPR4) = a2;
    register intptr_t ret asm(GPRx);
    asm volatile(SYSCALL
                 : "=r"(ret)
                 : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
    return ret;
}
```

触发系统调用的本质是自陷指令，此时执行流会被切换到IDT中描述的地址，并完成了上下文保存和事件分发的工作，这部分在讲义中讲得很清楚了。注意上下文管理功能是由AM提供的（`nexus-am/am/include/arch/x86-nemu.h`），而在x86中自陷又涉及寄存器的保存，恢复等工作，这属于NEMU中的硬件范畴。

以`_read()`为例，对应的系统调用处理函数会使用`fs_read()`这个文件系统中的函数，它又使用了操作硬盘的函数`ramdisk_read()`。“硬盘”的本质是用“内存”模拟出来的，而内存的本质是NEMU中的数组。



### 2.2 屏幕的更新

在`navy-apps/apps/pal/src/hal/hal.c`的`redraw()`函数中，先使用`NDL_DrawRect()`把二维的屏幕信息转为一维的canvas数组，再使用`NDL_Render()`更新屏幕信息。`NDL_Render()`使用`fwrite()`写入显存设备，使用`fflush`刷新。

`fwrite`从库函数，libos再到nanos-lite中系统调用的过程与`fread`类似，区别在于这里写入的并不是一个普通文件，而是显存设备。因此nanos-lite使用的写函数是定义在`device.c`中，由我们完成的`fb_write()`。它又使用了IOE中封装好的屏幕更新函数`draw_rect()`，而IOE是am提供的，负责直接与NEMU中的硬件设备交互（利用架构无关的“抽象寄存器”）。NEMU中的显存其实还是内存映射，故本质仍然是一段数组。



总的来说，库函数是编程者能直接接触到的API，它封装了与操作系统有关的系统调用，libos则是系统调用在用户层的接口。nanos-lite完成系统调用的具体细节（上下文管理，事件分发），这里上下文管理与ISA有关，是经过AM抽象的。此外，涉及到串口、时钟、键盘、VGA等设备的交互也由AM中的IO-extension抽象。无论是对设备还是内存的修改，最终都体现在硬件设备NEMU上，硬盘，显存，内存都是NEMU用数组模拟出来的。

## 3. 思考题

### 3.1 诡异的x86代码

我们知道调用`__am_irq_handle()`前压入的最后一个值就是函数调用的第一个参数，这里是把%esp的值作为`__am_irq_handle()`的参数传入，也就是`_Context* c`。

### 3.2 堆和栈在哪里?

因为操作系统中可能同时运行多个程序，如果把堆和栈的位置写死在每个可执行文件里，它们同时执行时就可能发生冲突。因此必须要由操作系统来分配管理堆区和栈区。

### 3.3 如何识别不同格式的可执行文件?

Windows下可执行文件采用PE格式而非elf格式，因此exe文件没有ELF头，linux试图执行时发现找不到ELF的magic字节就会报格式错误。

### 3.4 冗余的属性?

FileSiz是文件中段的大小，MemSiz是内存中段的大小。一般来说MemSiz比较大，因为未初始化的局部变量在文件中不需要分配空间储存，但是在内存中必须要占空间。

### 3.5 为什么要清零?

清零的这部分其实是.bss段，按照C规定未初始化的全局变量要被初始化为0。

### 3.6 不再神秘的秘技

1. 本意让李逍遥拿钱去请法师，但玩家可以用这个钱去干别的事，回来又能拿钱。这属于业务逻辑没有写好。

2. 财产是个无符号数，每次使用乾坤一掷`-=5000`，最后发生了wrap around溢出成了大整数。
3. 正常逻辑是99级不能通过打怪等方式获得经验，但是吃道具可能是特判的，直接在经验值上做加法，同样业务逻辑没有写好导致可以继续升级。我推测每个等级需要的升级经验、可学到的技能等信息都是用等级作为数组下标索引的，而几个人物的信息恰好是连续存储的（比如李逍遥==0，灵儿==1，算下标的时候用人物*100+人物等级），因此李逍遥等级溢出之后变成了灵儿的1级，因此升级经验很少而且能学到灵儿的法术。

## 4. 遇到的问题

### 4.1 实现了缓冲区，但`printf()`还在逐字符输出

通过RTFSC我发现在`syscall.c`中设置GPRx寄存器还不是最终的返回值，最终的返回值是`_write()`返回的。我已开始让`_write()`返回0，觉得这个无所谓。但是这就导致`printf()`始终认为缓冲区申请失败。

### 4.2 `readelf -l`指令无法读`ramdisk`文件

在一开始做PA3.3时，我发现使用`readelf -l`指令无法读`ramdisk`文件，因为找不到`elf`的magic文件头。后来我发现虽然各文件是在`ramdisk`里按顺序存储的，但第一个文件并不是`elf`文件而是一个文本文件，因此无法使用`readelf`指令读出它的内容。

### 4.3 奇怪的问题

在完成PA3.2后，我发现`printf`会试图调用尚未完成的`_close()`系统调用并触发`BAD TRAP`，同时无法打印出不带换行符的字符串。实现`_close()`后这两个问题都解决了，但是我不太知道为什么`printf`会去使用文件关闭的系统调用，以及这个问题的出现为什么会与字符串末尾的换行符有联系。

### 4.4 要命的bug

在测试`bmptest`我遇到了一个bug，它具有以下特点：

​	1. 如果我注释掉代码的一部分调试剩余部分，它可能会消失，也可能触发位置发生改变。

 2. 在一段正常运行的代码后添加其他代码，正常的代码会出错，尽管后添加的代码根本还没执行到。

 3. 第一次读写一个数组时没有任何问题，但第二次读写时就会发生段错误。

 4. 在native中执行时，bug的行为也基本相似，不过在nemu中出错的程序在native中可能正确。

    

这个bug的行为是如此诡异，以至于我甚至无法确定出错的到底是软件还是硬件。后来我使用同学的正确实现进行二分替换，发现问题出在loader中：为了确保读入数据不会越界，我开了一个大小为文件大小的临时数组。当文件太大时，这个数组过大就会导致地址错误。对于小文件这个bug则根本不会出现，这也就是1和2两个特点的出现原因——当我注释掉一部分代码时，可执行文件变小。我觉得没有替换法帮助我是很难定位这个问题所在的。

### 4.5 另一个bug

另一个bug是启动仙剑奇侠传时出现黑屏，在native中正常。这次可以确定问题在nemu中，同样使用“正确”代码二分替换找到问题所在，但修正后的代码却无法通过某些前面的测试样例，需要继续修改。这个故事告诉我测试样例集是非常有限的，尽管在PA2中通过那么多样例，却还是存在没有覆盖的漏网之鱼，到后面才会暴露出来。事实上在nemu中我找出了3个可见的错误，分别是`cmp` `update_ZF`和`cltd`，尽管错的比较低级（比如，在`cltd`中我用`<0`判断一个无符号数的最高位是否为1），但这些错误都直到现在才初次出现。

## 5. 实验心得

### 5.1 如何修改`_Context`的顺序?

`__am_irq_handle`是被汇编代码调用的，`_Context`的元素顺序应该刚好与自陷过程的压栈顺序相反。

用简易调试器查看_Context处的内存，然后让元素顺序与内存中的顺序对起来即可。

### 5.2 关于未完全实现的函数

`printf()`之前没有实现完全，比如`%p`。如果让遇到`%p`的时候`assert`一下，就比什么都不做来得好，否则都不知道遇到了没有实现的格式符，定位不了问题。这其实是PA2中就提醒我们思考的，当时我没有这么做，给debug带来了困惑，花费了没有必要的时间在定位问题上。

### 5.3 如何实现`loader`（在实现文件系统之前）

`ramdisk`里存放的是ELF文件，一上来（地址为0）应该就是ELF的文件头。其实我一开始没意识到这一点，后来发现前3个字节刚好是0x464C45，即"ELF"的ascii码

根据ELF文件头中的`e_phoff` `e_phentsize` `e_phnum`三个量（程序头表的起始位置，大小，数目）即可读出所有的程序头表，根据每个程序头表按照要求设置内存，最后设置跳转地址为`e_entry`。

缓冲区的大小我选用了`ramdisk`的大小，虽然声明数组时，长度只能是常量，但这不代表不能使用`get_ramdisk_size()`作为长度，因为这个函数太简单，被内联处理了，就相当于一个常量。当然后来我发现直接用硬盘大小这样简单粗暴的做法会带来灾难（4.x）。

### 5.4 只有一个`_write()`

本质上其实输出到文件和输出到串口是同一回事，它们都使用`SYS_write`的系统调用。

