# <center>PA2实验报告</center>

**<center>2019年10月31日</center>**

## 1. 实验进度

​	完成了所有内容。

## 2. 必答题

### 2.1 RTFSC：请整理一条指令在NEMU中的执行过程。

一条指令被NEMU保存到全局译码信息`decinfo`的成员`seq_pc`中，再被作为参数送入`isa_exec()`函数中。`isa_exec()`先完成取值操作，即调用函数`instr_fetch()`。

`instr_fetch()`提取出`opcode`进行译码操作，对于`x86`，`opcode`就是前1或2个字节。在译码查找表`opcode_table`中索引得到一个`opcodeEntry`类型的元素，用这个元素调用`idex()`函数，`idex()`函数会根据该参数的信息调用其译码（如果有的话）和解码函数。

译码函数由`make_DHelper()`生成，它又会进一步调用`make_DopHelper()`生成的译码辅助函数，译码辅助函数则对应了特定的操作数类型和数据流向，最终完成译码操作。译码得到的信息被记录在`decinfo`的`src` `src2`和`dest`成员中，供解码函数使用。

解码函数由`make_EHelper()`生成，它们对应到具体的汇编指令类型，并进一步调用`rtl`命令完成具体的指令操作。

### 2.2 编译与链接：关于`static`和`inline`关键字

#### 2.2.1 回答

对于比较简单的函数，例如`rtl_li()`，去掉`static`或`inline`中的一个，编译可以通过。当同时去掉两者时，无法通过编译，提示函数被重定义。

对于不那么简单的函数，例如`rtl_div64_q()`,仅去掉`statinc`时编译可以通过，当去掉`inline`时，无法通过编译，提示函数被定义但没有使用(defined but not used)。若同时去掉`static`和`inline`，无法通过编译，提示函数被重定义。

#### 2.2.2 解释

如果只有`static`，该函数在所有包含`rtl.h`的模块中都是局部静态变量，属于本地符号。本地符号只在一个模块中被使用，符号解析时没有强弱符号的概念，不存在重定义的问题，每个模块里的该函数都是彼此独立存在的。然而这会带来另一个问题：有的模块中并没有使用该函数，这本来是一个`warning`，但在`-Werror`编译选项的作用下被视为`error`，导致编译失败。而对于比较简单的函数，`gcc`在`-O2`优化下将其做了内联处理，因此不存在这个问题。

如果只有`inline`，该函数被要求内联展开，即将函数体直接在调用处展开以简化函数调用过程。此时函数调用的过程已经不存在了，没有了对这个函数的引用，也就不存在重定义的问题了。

如果`static`和`inline`都不存在，这个函数在每个包含`rtl.h`的模块中都被作为强符号定义了一次，因此会产生重定义，无法通过编译。

#### 2.2.3 实验

定义`a.c` `b.c` `c.h`如下：

```c
//a.c
#include <stdio.h>
#include "c.h"
int main()
{
    foo();
    return 0;
}
```

```c
//b.c
#include "c.h"
int fun()
{
    return foo();
}
```

```c
static int foo()
{
    return 0;
}
```

查看汇编结果发现：

```assembly
00000000000005fa <foo>:
	...
0000000000000605 <main>:
 	...
000000000000061a <foo>:
	...
```

`gcc`生成了两个地址不同的`foo()`函数，印证了`static`会导致函数被本地化，只在一个模块中使用的结论。

将`c.h`更改如下：

```c
inline int foo()
{
    return 0;
}
```

编译错误，提示`foo()`未定义。

在`a.c`中增加函数声明`int foo();`，编译通过，反汇编结果如下：

```assembly
00000000000005fa <foo>:
	...
	
0000000000000605 <main>:
	...
 60e:	e8 e7 ff ff ff       	callq  5fa <foo>
	...

000000000000061a <fun>:
    ...
 623:	e8 d2 ff ff ff       	callq  5fa <foo>
    ...
```

可见，现在只有一个`foo()`函数，它已经是全局符号，然而`main()`和`fun()`仍然调用了`foo()`，`inline`没有生效。

再次修改`c.h`，增加强制内联属性`__attribute__((always_inline))`：

```c
__attribute__((always_inline)) inline int foo()
{
    return 0;
}
```

编译，反汇编，结果如下：

```assembly
0000000000000605 <main>:
 605:	55                   	push   %rbp
 606:	48 89 e5             	mov    %rsp,%rbp
 609:	b8 00 00 00 00       	mov    $0x0,%eax
 60e:	5d                   	pop    %rbp
 60f:	c3                   	retq   

0000000000000610 <fun>:
 610:	55                   	push   %rbp
 611:	48 89 e5             	mov    %rsp,%rbp
 614:	b8 00 00 00 00       	mov    $0x0,%eax
 619:	90                   	nop
 61a:	5d                   	pop    %rbp
 61b:	c3                   	retq   
 61c:	0f 1f 40 00          	nopl   0x0(%rax)
```

可见此时`main()`和`fun()`都没有调用`foo()`，而是有一句`mov    $0x0,%eax` 指令，这说明`inline`属性生效。

####  2.2.4 进一步实验

为什么对于简单的函数`rtl_li()`，去掉`inline`，保留`static`也能通过编译？

定义`a.c` `c.h`如下：

```c
//a.c
#include <stdio.h>
#include "c.h"
void foo();
int main()
{
    foo();
    return 0;
}
```

```c
//c.h
void foo()
{
    printf("foo\n");
}
```

不带优化选项编译时：

```assembly
000000000000064d <main>:
 64d:	55                   	push   %rbp
 64e:	48 89 e5             	mov    %rsp,%rbp
 651:	b8 00 00 00 00       	mov    $0x0,%eax
 656:	e8 df ff ff ff       	callq  63a <foo>
 65b:	b8 00 00 00 00       	mov    $0x0,%eax
 660:	5d                   	pop    %rbp
 661:	c3                   	retq   
 662:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
 669:	00 00 00 
 66c:	0f 1f 40 00          	nopl   0x0(%rax)
```

在`-O2`优化等级下编译时：

```assembly
0000000000000530 <main>:
 530:	48 8d 3d bd 01 00 00 	lea    0x1bd(%rip),%rdi        # 6f4 <_IO_stdin_used+0x4>
 537:	48 83 ec 08          	sub    $0x8,%rsp
 53b:	e8 d0 ff ff ff       	callq  510 <puts@plt>
 540:	31 c0                	xor    %eax,%eax
 542:	48 83 c4 08          	add    $0x8,%rsp
 546:	c3                   	retq   
 547:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
 54e:	00 00 
```

可见，低优化下`main()`调用了`foo()`，但在`-O2`优化下`foo()`被内联在了`main()`中，即便我们并没有指定`inline`。

#### 2.2.5 进两步实验

观察`static inline`函数定义与`inline`的不同：

```c
//a.c
#include <stdio.h>
#include "c.h"
void foo();
int main()
{
    foo();
    return 0;
}
```

```c
//c.h with static
static inline void foo()
{
    printf("foo\n");
}
```

```c
//c.h without static
inline void foo()
{
    printf("foo\n");
}
```

在`gcc -O2`选项下编译，发现两者的`main()`函数都做了内联处理，区别是`static inline`前缀下汇编代码中没有`foo`的函数块，而`inline`前缀下该函数块是存在的。

```assembly
# Attention: Only appear in 'inline' definition
0000000000000660 <foo>:
 660:	48 8d 3d 8d 00 00 00 	lea    0x8d(%rip),%rdi        # 6f4 <_IO_stdin_used+0x4>
 667:	e9 a4 fe ff ff       	jmpq   510 <puts@plt>
 66c:	0f 1f 40 00          	nopl   0x0(%rax)
```

对该现象我的理解是：加了`static`前缀，编译器知道该函数只在本模块内被使用，而且可以全部内联展开，不需要生成供调用的汇编函数体。而不加`static`时，编译器不确定其他模块是否会调用这个函数，为防止某些情况下该函数在其他模块里不能内联展开，出于保险起见，生成了可供调用的汇编函数体。

#### 2.2.6 结论

1. 带`inline`前缀的只是函数的定义，不具有函数声明的作用。

2. `inline`是一个对编译器的请求，编译器可以选择不实现这个请求，除非优化等级较高（如`-O2`），或者有强制内联属性`__attribute__((always_inline))`。此外，即使没有显式指出`inline`，`gcc`在较高优化等级下也会对简单的函数进行内联处理。

3. 如前所述，函数定义中只有`static`显然是不合理的，这不仅会导致函数在每个模块里都被汇编一份，对于复杂函数还会导致`defined but not used`这一错误；但只有`inline`为什么不行呢？为什么要两者同时存在？

   我认为这个问题的答案就是“进两步实验”的结论：加了`static`还能保证该函数的汇编函数体不被生成，从而更进一步地压缩了汇编代码的长度，减少冗余。

### 2.3 编译与链接：关于变量

#### 2.3.1

在`nemu/include/common.h`中添加`volatile static int dummy`，编译得到可执行文件`build/x86-nemu`。

使用命令`nm build/x86-nemu |grep dummy`,发现其中有2条记录是与我们定义的dummy无关的。

```
00000000000087f0 t frame_dummy
0000000000213a30 t __frame_dummy_init_array_entry
```

使用命令`nm build/x86-nemu |grep dummy |wc`，输出39。减去2条无关记录得到`dummy`变量实体的个数是37个。 

#### 2.3.2

在`nemu/include/debug.h`中再次添加`volatile static int dummy`，用上问的方法得到`dummy`变量实体的个数仍然是37个，与上题一致。显然，`debug.h`中包含了`common.h`,因此每个包含`debug.h`的模块里实际上定义了两次`volatile static int dummy`。我本来想用强弱符号的概念来解释这一现象，但因为这是`static`定义的局部变量，局部变量是不存在强弱符号的区别的，因此这样解释是不对的。通过查阅资料，我得知C语言中存在**"tentative definition"**的概念，即允许一个变量被多次定义，只要各定义不冲突（类型必须相同），且未初始化超过一次，就是不违反规则的，编译器会把这些定义合并为一条定义，最后也只存在一个变量。如果编译器在作用域里最终找到了一个"actual definition"，所有的tentative definition会被视为变量的声明。否则，它们将被视为变量的定义。

C语言中这一规则的存在，是为了解决C89标准之前的历史遗留问题，在C++中这么做是不合法的。

参考资料：https://stackoverflow.com/questions/3095861/about-tentative-definition

#### 2.3.3

此时编译报错`redefinition of dummy`。这是因为此时`dummy`被初始化了两次（尽管是同一个值），这违背了tentative definition的要求，因此编译失败。

### 2.4 了解Makefile

#### 2.4.1 一些符号

| 符号 | 作用                               |
| ---- | ---------------------------------- |
| $@   | 规则中的目标文件集                 |
| $<   | 规则中的依赖目标集                 |
| =    | 赋值，在整个文件展开后才确定值     |
| +=   | 追加赋值                           |
| :=   | 立即赋值，值取决于在文件中的位置   |
| ?=   | 赋值。如果该变量已经被赋值，忽略。 |

#### 2.4.2 工作方式

Makefile最基本的工作方式可以写作

```
target ... : prerequisites...
	command
	...
	...
```

这里target是目标文件，prerequisites是依赖文件，command是需要执行的命令。如果prerequisites比target更新，就需要执行command以更新target。这个过程是递归进行的，prerequisites可能是另一条规则的target。

允许在Makefile中定义变量，效果类似于C/C++的宏。

在`nemu/Makefile`中：

```makefile
ifneq ($(MAKECMDGOALS),clean) # ignore check for make clean
ISA ?= x86
ISAS = $(shell ls src/isa/)
$(info Building $(ISA)-$(NAME))

ifeq ($(filter $(ISAS), $(ISA)), ) # ISA must be valid
$(error Invalid ISA. Supported: $(ISAS))
endif
endif
```

指定了`make clean`和ISA名不合法的情形。`MAKECMDGOALS`是终极目标的列表，即输入`make`命令时后面的参数。

```makefile
ifdef SHARE
SO = -so
SO_CFLAGS = -fPIC -D_SHARE=1
SO_LDLAGS = -shared -fPIC
endif
```

指定了动态编译链接所用的选项。

```makefile
OBJ_DIR ?= $(BUILD_DIR)/obj-$(ISA)$(SO)
BINARY ?= $(BUILD_DIR)/$(ISA)-$(NAME)$(SO)
```

用宏展开的方式定义了`.o`文件的目录，和生成的可执行文件的名称。

```makefile
# Files to be compiled
SRCS = $(shell find src/ -name "*.c" | grep -v "isa")
SRCS += $(shell find src/isa/$(ISA) -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)
```

用`$()`调用shell命令`find`，指出源文件（即*.c）文件的集合。第一句用了`grep -v`剔除掉ISA相关的源文件，第二句再根据用户定义的ISA变量引入这些文件，使`SRCS`不包含其他ISA中文件。第三句定义了`.o`文件的集合，这句话的语法是把`SRCS`中所有字符串"src/%.c"替换为字符串"$(OBJ_DIR)/%.o"。这里%是通配符，匹配任意多个字符。

```makefile
# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $@ $<
```

这是一条规则定义：目标是OBJ_DIR目录下的所有`.o`文件，它们依赖对应的`.c`文件。要执行的命令的@前缀代表这条命令本身不会在终端输出。这部分完成提示信息的输出，目录创建，并使用给定的编译器和编译选项生成所有可重定位目标文件。

```makefile
$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -rdynamic $(SO_LDLAGS) -o $@ $^ -lSDL2 -lreadline -ldl
```

这也是一条规则定义：目标是最终的可执行文件，它依赖所有`.o`文件，使用给定的链接器完成动态链接。编译器和链接器在这里都是gcc。

```
.DEFAULT_GOAL = app
app: $(BINARY)
```

这里指出当我们仅输入`make`时，默认使用`app`伪目标，它会生成最终的可执行文件。

```
clean:
	-rm -rf $(BUILD_DIR)
	$(MAKE) -C tools/gen-expr clean
	$(MAKE) -C tools/qemu-diff clean
```

`make clean`时的操作，`rm`前的`-`代表忽略报错，即使BUILD_DIR不存在也没有问题。`$(MAKE)`变量代表用户输入的`make`命令及其参数，这里在两个子目录下调用了`make clean`。

```makefile
include Makefile.git
$(call git_commit, "run")

# Makefile.git
define git_commit
	-@git add .. -A --ignore-errors
	-@while (test -e .git/index.lock); do sleep 0.1; done
	-@(echo "> $(1)" && echo $(STUID) && id -un && uname -a && uptime && (head -c 20 /dev/urandom | hexdump -v -e '"%02x"') && echo) | git commit -F - $(GITFLAGS)
	-@sync
endef
```

此外值得注意的是，`Makefile`文件还使用`include`语句包含了`Makefile.git`，并使用call命令调用了过程`git_commit`来完成git相关的版本跟踪工作。

#### 2.4.3 编译与链接过程

当输入`make`时，`.DEFAULT_GOAL = app`使得make去查找目标`app`，发现它依赖`$(BINARY)`，其又依赖`$(OBJS)`，`$(OBJS)`依赖`$(SRCS)`。`$(SRCS)`到`$(OBJS)`代表了编译过程，而`$(OBJS)`到`$(BINARY)`代表了（动态）链接过程。

编译链接过程所需要的`cc` `ld` `CFLAGS` 等参数以及路径都是由之前定义的变量展开得到的，便于维护。

`$(SRCS)`是如何包含所需要的`.c`文件的已经在前面说明了，而`.c`文件包含`.h`文件是由gcc在预编译过程中进行的。

## 3. 思考题

### 3.1 立即数背后的故事2

可以把32位常数拆分成高位和低位两个16位常数，通过位运算和移位操作可以得到原先的32位常数。因此可以用寄存器分别存储低位和高位，并在需要时用它们运算出原先的32位常数。

### 3.2 指令执行的共性部分

每种ISA架构都有译码和执行分离的设计，而又共享同样的执行过程。事实上，类似`make_EHelper()` `Operand` `idex()`这些函数和结构体都是ISA无关的，对于所有的ISA，指令执行都可以分为译码和执行（体现在`idex()`中），区别仅仅在于译码和执行辅助函数的不同。这样做的好处是把问题分解到了不同的抽象层次，减少了代码冗余。当我们迁移到新的ISA，只需要重写译码和执行辅助函数，不用重新设计指令的执行过程。另外，同类型指令对译码/执行辅助函数的共享也降低了debug的难度，避免重复和不必要的copy-paste.

### 3.3 为什么执行了未实现指令会出现上述报错信息

以x86为例。未实现指令在`opcode_table`中标示为`EMPTY`，它的宏定义位于`exec.h`中：

```c
#define EMPTY              EX(inv)
```

可见，遇到EMPTY时实际执行了由`make_EHelper()`创建的函数`exec_inv()`，其定义于`special.c`中：

```c
make_EHelper(inv)
{
    /* invalid opcode */
    uint32_t temp[2];
    *pc     = cpu.pc;
    temp[0] = instr_fetch(pc, 4);
    temp[1] = instr_fetch(pc, 4);
    uint8_t* p = (void*)temp;
    printf("invalid opcode(PC = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n",
           cpu.pc, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
    display_inv_msg(cpu.pc);
    rtl_exit(NEMU_ABORT, cpu.pc, -1);
    print_asm("invalid opcode");
}
```

在这个函数里，先取出无效处8字节的指令反馈给用户，并调用`display_inv_msg()`打印错误信息（包括logo，它是一个长字符串）。随后调用了一条特殊的rtl指令`rtl_exit()`（定义于`cpu-exec.c`中）：

```c
void interpret_rtl_exit(int state, vaddr_t halt_pc, uint32_t halt_ret) {
  nemu_state = (NEMUState) { .state = state, .halt_pc = halt_pc, .halt_ret = halt_ret };
}
```

`NEMUState`变量被置为`NEMU_ABORT`。这一修改的效果体现在`cpu_exec()`中：

```c
switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }
```

因此NEMU输出错误信息后终止。

### 3.4 这又能怎么样呢？

我认为把程序和架构解耦，通过库和定义的`API`来使用运行时环境的做法还有一个好处：方便开发和调试。

比如当我们实现架构时，可以用在`native`上经检验正确无误的程序进行比对，反之亦然。还可以参照native的正确实现来实现nemu。事实上，在PA2.3中，我确实这么做了。另外，一旦出现bug，这样的设计也方便我们使用控制变量法来检查到底是程序还是nemu架构的实现出了问题。

### 3.5 阅读Makefile(nexus-am)

本题我将说明运行时环境是如何被编译的。关于特定的程序如何被编译到am中，将在3.1.7题说明。

表中列出了在nexus-am目录下输入`make`会涉及到的`Makefile`：

| 文件               | 作用                                         |
| ------------------ | -------------------------------------------- |
| `Makefile`         | 整个am的顶层`Makefile`,在am目录下调用`make`  |
| `Makefile.check`   | 检查环境变量，ARCH等是否合法;包含了`ARCH.mk` |
| `Makefile.compile` | 完成`.c` `.cpp` `.S`到`.o`的编译工作         |
| `Makefile.lib`     | 负责用`ar`命令从`.o`生成`.a`文件             |
| `am/Makefile`      | 被顶层`Makefile`调用，包含了`Makefile.lib`   |
| `am/arch/ARCH.mk`  | 设置具体架构的源文件和编译选项               |

输入`make`时，首先被检查的是`Makefile`，它先用`Makefile.check`检查环境变量和ARCH名，同时也顺便包含了`ARCH.mk`，之后`Makefile`设置环境变量并在`am`目录下执行了`make`语句，它会检查`am/Makefile`。`am/Makefile`根据`ARCH.mk`中的源文件设置了`SRCS`，并引入`Makefile.lib`。`Makefile.lib`先引入`Makefile.compile`完成编译，再用`ar`命令创建archive文件。

另外值得一提的是`make clean`的执行。`Makefile`用`find . -mindepth 2 -name "Makefile"`找到了除自己之外的其他`Makefile`文件，然后在他们所在的目录中执行`make clean`。

### 3.6 指令名对照

以x86为例，我可以在附录A的opcode map中找到相应字节码所对应的指令，查看该指令的效果。如果效果与AT&T格式指令一致，我就有理由认为它们指代的是同一条指令。这个方法有效是因为Intel和AT&T格式只是反汇编结果形式上的区别，它们都基于x86架构，因此相同指令码一定对应了同一条指令。

### 3.7 如何生成native的可执行文件

#### 3.7.1 分析Makefile

当我们在`cputest`目录下执行`make`时，`cputest/Makefile`先被检索。

```makefile
include $(AM_HOME)/Makefile.check
```

一上来，它就包含了`Makefile.check`文件。这个文件主要负责检查环境变量，ARCH等是否合法，在它的最后，它还引入了对应ARCH的`.mk`文件，这个文件是与ARCH有关的编译信息，在这里，是native相关的信息。

```
include $(AM_HOME)/am/arch/$(ARCH).mk
```

最后对可执行文件的编译工作也在这个`native.mk`中。

```
image:
	@echo + LD "->" $(BINARY_REL)
	@g++ -pie -o $(BINARY) -Wl,--whole-archive $(LINK_FILES) -Wl,-no-whole-archive -lSDL2 -lGL -lrt
```

回到`cputest/Makefile`中：

```makefile
ALL = $(basename $(notdir $(shell find tests/. -name "*.c")))
$(ALL): %: Makefile.%
Makefile.%: tests/%.c latest
	@/bin/echo -e "NAME = $*\nSRCS = $<\nLIBS += klib\ninclude $${AM_HOME}/Makefile.app" > $@
	-@make -s -f $@ ARCH=$(ARCH) $(MAKECMDGOALS)
	-@rm -f Makefile.$*
```

​	这一部分把`ALL`默认定义为所有测试样例的集合，特别地，如果我们调用时有`ALL`这个参数，会覆盖掉这里的默认定义。把它的依赖设置为`Makefile.filename`这个文件。当输入`make all=filename`时，创建一个临时的`Makefile.filename`，输入编译信息并包含`Makefile.app`这个文件，然后以`Makefile.filename`再次调用`make`，这次的调用完成了实际的编译工作。

如果我们指定了ARCH，它也会在这次调用中被传入。否则传入的是我们在`Makefile.check`中定义的默认值。当输入`ARCH=native`时，按照`Makefile.app`中的定义：

```makefile
APP_DIR ?= $(shell pwd)
INC_DIR += $(APP_DIR)/include/
DST_DIR ?= $(APP_DIR)/build/$(ARCH)/
BINARY  ?= $(APP_DIR)/build/$(NAME)-$(ARCH)
BINARY_REL = $(shell realpath $(BINARY) --relative-to .)
```

这个ARCH已经被定义在了`DST_DIR`和`BINARY`变量中，因此我们看到的可执行文件名会出现native的字样。

在`Makefile.app`中又包含了实际完成编译操作的`Makefile.compile`，这里是一些ARCH无关的编译信息。

总的来说，`make`不是顺序执行Makefile的命令的。它先读入了所有被include的`Makefile`，再按照目标之间的依赖关系执行。

#### 3.7.2 `make`命令执行过程

当键入`make ALL=dummy ARCH=native`时，产生了多次`make`调用：


| 目标                | 依赖             | 目标所在文件       |
| ------------------- | ---------------- | ------------------ |
| `$(ALL)` 即 "dummy" | `Makefile.dummy` | `cputest/Makefile` |
| `Makefile.dummy`    | `tests/dummy.c`  | `cputest/Makefile` |

之后，目标`Makefile.dummy`的规则以`make -s -f Makefile.dummy ARCH=native run`开始了第二次`make`调用：

| 目标                    | 依赖/行为                          | 目标所在文件       |
| ----------------------- | ---------------------------------- | ------------------ |
| run                     | default                            | `Makefile.app`     |
| default                 | image                              | `Makefile.app`     |
| image                   | `$(OBJS) am $(LIBS) prompt`        | `Makefile.app`     |
| `$(OBJS)`（一堆.o文件） | `.c .cpp .S`文件                   | `Makefile.compile` |
| am                      | 在`AM_HOME`下调用`make`编译am      | `Makefile.compile` |
| `$(LIBS)`（`klib`等）   | 在`AM_HOME/libs`下调用`make`编译库 | `Makefile.compile` |
| prompt                  | 输出一句提示信息                   | `Makefile.compile` |

  在`AM_HOME`下调用`make`编译am的具体行为在3.1.5节中说明了，它沿着`Makefile`->`am/Makefile`->`Makefile.lib`   ->`Makefile.check`->`am/arch/native.mk`的包含顺序最终包含到了对应ARCH的`Makefile`文件。`native.mk`中image目标下的`g++ -pie -o $(BINARY) -Wl,--whole-archive $(LINK_FILES) -Wl,-no-whole-archive -lSDL2 -lGL -lrt`命令最终完成了可执行文件的链接，生成了名为`dummy-native`的可执行文件。

### 3.8 奇怪的错误码

#### 3.8.1 为什么错误码是1

在`tests/cputest/include/trap.h`中：

```c
void nemu_assert(int cond) {
  if (!cond) _halt(1);
}
```

`_halt()`定义于`am/src/native/trm.c`中：

```
void _halt(int code) {
  printf("Exit (%d)\n", code);
  exit(code);
}
```

`exit()`是C内置函数，它结束程序运行，并且参数就是程序的返回值。0表示正常退出，1表示出现错误（非正常退出）。因此程序返回值（错误码）是1。

#### 3.8.2 `make`如何得到错误码

shell命令是有返回值的，`make`在出错前执行的最后一句代码是`native.mk`中的第20行`$(Binary)`，它在shell中执行了出错的可执行文件。上面已经说明了这个可执行文件返回1，代表错误。因此make从最后一句代码的返回值1得到了错误码。

### 3.9 理解volatile关键字

`_end`是一个外部变量的声明，它可能会在其他模块里被改变。

```assembly
#volatile
0000000000000630 <fun>:
 630:	48 8d 15 e1 09 20 00 	lea    0x2009e1(%rip),%rdx        # 201018 <_end>
 637:	c6 05 da 09 20 00 00 	movb   $0x0,0x2009da(%rip)        # 201018 <_end>
 63e:	66 90                	xchg   %ax,%ax
 640:	0f b6 02             	movzbl (%rdx),%eax
 643:	3c ff                	cmp    $0xff,%al
 645:	75 f9                	jne    640 <fun+0x10>
 647:	c6 05 ca 09 20 00 33 	movb   $0x33,0x2009ca(%rip)        # 201018 <_end>
 64e:	c6 05 c3 09 20 00 34 	movb   $0x34,0x2009c3(%rip)        # 201018 <_end>
 655:	c6 05 bc 09 20 00 86 	movb   $0x86,0x2009bc(%rip)        # 201018 <_end>
 65c:	c3                   	retq   
 65d:	0f 1f 00             	nopl   (%rax)
 
 #no volatile
 0000000000000610 <fun>:
 610:	c6 05 01 0a 20 00 00 	movb   $0x0,0x200a01(%rip)        # 201018 <_end>
 617:	eb fe                	jmp    617 <fun+0x7>
 619:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
```

可见没有`volatile`时，整个函数体被优化的只剩下一个无限循环。

如果p地址被映射到一个设备寄存器（比如状态寄存器），那么万一设备的状态被改变（外部变量_end），CPU也无从得知。因为连`*p != 0xff `的循环条件也被优化没了，循环永远不会终止。

### 3.10 native如何实现main函数的参数传递?

当我们输入`make mainargs=H run`时，实际上定义了一个值为`H`的`mainargs`变量。

在`am/arch/platform/nemu.mk`中：

```makefile
ifdef mainargs
MAINARGS = -a $(mainargs)
endif
NEMU_ARGS = -b $(MAINARGS) -l $(shell dirname $(BINARY))/nemu-log.txt $(BINARY).bin
```

变量`mainargs`被展开并被作为变量`NEMU_ARGS`的一部分，在伪目标`run`中：

```makefile
make -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMU_ARGS)"
```

以该变量为参数，在NEMU的根目录下调用`make`，`NEMU_HOME`是在调用`init.sh`下载框架代码时就设置好的。

在`nemu/Makefile`中：

```makefile
# Command to execute NEMU
IMG :=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)
```

可见当我们输入run时，实际上执行了`NEMU_EXEC`变量中的命令。这条命令运行了可执行文件，并把`ARGS`中的参数内容跟在后面（包括`mainargs`)，`mainargs`被`main()`函数读进了`args[0]`。

### 3.11 神秘的MAP宏

MAP宏定义为：`#define MAP(c, f) c(f)`，即以`f`为参数调用`c`。

在`nemu/src/device/keyboard.c`中如此调用：

```c
enum {
  _KEY_NONE = 0,
  MAP(_KEYS, _KEY_NAME)
};
```

而`_KEYS(f)`定义为对每个键调用`f`，`_KEY_NAME`定义为：

```c
#define _KEY_NAME(k) _KEY_##k
```

因此`MAP(_KEYS, _KEY_NAME)`最终在`enum`里被展开成`_KEY_KEYNAMES`(`KEYNAMES`为所有键名的集合)，它为每个键创建了一个映射值。

### 3.12 如何检测多个键同时被按下?

PS/2协议下键被按下时不断发送通码，抬起时才会发送断码。因此在收到断码前主机都认为某个键是被按下的。

USB协议下，键盘每隔固定时间向主机发送当前按键的状态，主机据此得知哪些按键被按下。

## 4. 遇到的问题

### 4.1 提示`exec_call()`函数未定义？

在`nemu/src/isa/x86/exec/all-instr.h`中补充函数声明。

### 4.2 i386手册中，opcode后面的/5 /7是什么意思？

查阅手册，在17.2.2.1节有说明：

/digit: (digit is between 0 and 7) indicates that the ModR/M byte of the instruction uses only the r/m (register or memory) operand. The reg field contains the digit that provides an extension to the instruction's opcode. 

### 4.3 `rtl_set_OF`的参数是一个`rtlreg_t`变量的地址，如何设置OF为0呢？

因为不能对常量取地址，我在`cpu.c`中的`rtl`临时寄存器后面定义了一个ZERO变量，这个变量值为0.对这个变量取地址就可以作为`rtl_set_OF`的参数了。

似乎有更优美的做法：可以用`rtl_xor()`异或相同寄存器中的值得到0。

### 4.4 `HIT GOOD TRAP`和`HIT BAD TRAP`是怎么来的？`nemu_assert()`是如何工作的？

`d6`指令调用了名为`exec_nemu_trap` 的执行辅助函数，它最终会调用`rtl_exit(NEMU_END, cpu.pc, cpu.eax)`指令，这一指令的第三个参数（即`eax`的值)若为0，打印GOOD TRAP，否则打印BAD TRAP。`nemu_assert()`在`eax`里`push   $0x1`，因此会导致BAD TRAP。

我在实现`rtl_push()`时出现了bug，导致`push $0x1`一句失败，因此`nemu_assert()`无法正确的导致BAD TRAP，所有的程序都显示GOOD TRAP，这是一个非常难以定位的bug。

## 5. 实验心得

### 5.1 C语言中`#`和`##`的用法

`#`将宏参数转变为字符串，`##`将两个宏参数贴合在一起。

```c
#define STR(s)     #s
printf(aaa)  //输出aaa
#define CONS(a,b)  int(a##e##b)
printf("%d\n", CONS(2,3)); //输出2000
```

### 5.2  设置条件码的方式

设置条件码的方式很巧妙：一条`set_cc`和`set_ncc`只差在最后一位，只要单独看这一位，决定是否要取反，就可以减少一半的`switch`分支。

### 5.3 关于`make_Dophelper(SI)`

`instr_fetch()`返回`uint32_t`结果，因此如果我们用它读一个1字节的数，得到的结果已经是零扩展过的。要想得到符号扩展的结果，我们必须先把它转为`int8_t`类型，再转为`int32_t`类型。

### 5.4 关于`sbb`函数中`OF`的计算

```
 rtl_is_sub_overflow(&s0, &s1, &id_dest->val, &id_src->val, id_dest->width);
```

这句话为什么把`s1`放在result的位置上？(`s1`在之前计算出是`CF`的值)

原来这里的`dest`是`s0+CF`的值，`src`是输入时的被减数-减数的值，而`s0`是被减数-减数的值，这个式子化简之后相当于计算`CF==CF`的overflow情况，这样写不用把两次减法孤立开来去求OF。

### 5.5 关于`snprintf()`

值得注意的一点是：它在被要求输出的字符数超过限制时，返回值应该是被要求输出的字符数，而非实际能够输出的字符数。

### 5.6 拒绝copy-paste

在类`printf`函数中，我实际实现的是`vsprintf()`，`printf()` `sprintf()` `snprintf()`都调用了`vsprintf()`。

### 5.7 四舍五入

`native`的`timer.c`中有`uptime->io=seconds*1000+(useconds+500)/1000`。这句话里先`+500`再`/1000`巧妙地实现了四舍五入。

