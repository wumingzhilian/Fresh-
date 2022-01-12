# Bomb Labs

\* 阶段1：字符串比较

\* 阶段2：循环

\* 阶段3：条件/分支

\* 阶段4：递归调用和栈

\* 阶段5：指针

\* 阶段6：链表/指针/结构

## phase1

![image-20211201154145110](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201154145110.png)

我们查看bomb.c 的源码，这里就是我们第一关的开始

![image-20211201182235020](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201182235020.png)

我们 使用gdb

r 运行代码

`objdump -d bomb >> bomb.asm`

我们使用objdump 将他反汇编

我们找到我们的第一关

![image-20211201182738666](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201182738666.png)

或者我们可以使用gdb动态调试

gdb 加`-tui`  并用 `layout asm` 命令切换到汇编指令模式

就可以在调试的时候查看对应的汇编了

![image-20211201185150585](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201185150585.png)

![image-20211201185325769](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201185325769.png)

`0x400da0` 就是我们的main的开始

我们向下查看

![image-20211201200741992](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201200741992.png)

`0x400ee0` 就是我们的phase1

![image-20211201200852461](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201200852461.png)

这时候就要用到`gdb`的指令了，在汇编模式下的指令与普通模式有一些不同。我们可以使用`ni`(`next instruction`)和`si`(`step into`)来实现普通模式下的单步向下执行与步入操作。

打断点需要使用`b <func_name>`或`b *<address>`来进行比如我们可以看到调用`phase_1`函数的`call`指令的地址是`0x400e3a`，所以我们可以使用`b phase_1`或`b *0x400e3a`来打断点的，这两条命令有一点不同就在于断在地址会停在地址 上也就是`call`指令的位置，断在函数名会进入函数中，相当于再进行了一次`si`操作。

![image-20211201202817562](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201202817562.png)

如果出现乱码，当然就是refresh

```
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq  
```



这里，我们拿出来的是objdump 反汇编的代码，方便查看

第二行，0x402400给了esi

然后调用了strigns_not_equal函数

![image-20211201203922243](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201203922243.png)

我们使用步入，步过，到达函数strings_not_equal

mov %rdi,%rbx

mov %rsi,%rbp

这两句，把rdi，rsi复制到了 rbx，rbp，然后调用了string_length

也就是说，strings_not_equal 的参数可能就是两个，rbx,rbp

```
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
```

返回后，判断了eax中的值

eax相等的时候，跳转到400ef7

如果相等，就会继续执行，，也就走到了 explode_bomb 爆炸

也就是说，我们需要string_not_qeual 判断的test 返回0



![image-20211201210019597](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201210019597.png)

我们重新打了断点，发现，之前调用了readline 去读取我们的输入

并给了%rax

我们可以推测出这个函数的作用是读取一行输入

根据返回值一般存放在`rax`中的约定，`rax`中应该就是读入的数据的地址，第二句`0x400e37`中我们把这个值复制到了`rdi`中。

根据这里，那么我们此时应该干什么呢? 对，就是i找到rsi中的值，因为我们输入的要和他相等

我们查看rsi中存放的值



![image-20211201210734355](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201210734355.png)

rsi 我们在之前的esi中已经给了值

也就是`0x402400`

我们使用gdb查看地址的值，使用x命令，并以字符串展示

```
x /s 0x402400
```

![image-20211201211149822](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201211149822.png)

这个也就是我们的答案

````
Border relations with Canada have never been better.
````



![image-20211201211355374](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201211355374.png)

## phase2

![image-20211206205817737](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206205817737.png)

首先我们去400efc处查看一下

![image-20211206205847475](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206205847475.png)

也就是这里

简单观察一下

他从开始pushpush 然后callq了一个read_six_number 函数

字面意思，读取了六个数字，

那么我们尝试打断点，进入看看

我们把断点搭载phase开始

```
b *0x400efc
```



```
$0x28 ;表示一个十六进制数28
```

x 命令，用来查看的（可以在gdb中x help 查看）

打印寄存器的值

![image-20211206211937459](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206211937459.png)

我们查看一下read之后是什么情况，也就是400f0a

info break 查看断点，然后delete删除



![image-20211206212854286](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206212854286.png)

![image-20211206213709387](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206213709387.png)

执行完之后，我们查看rsp寄存器的值

```
x /32xb $rsp
```

因为我们输入的是

```
1 2 3 4 5 6 
```

那么，这里的值也就是，

0x01 0x02 0x03 0x4 0x05 0x06

我们打出了`rsp`开始32字节的内容，发现栈中依次存放了输入的6个数，之后就是返回的地址。那么我们可以确定读取的数值就是依次存放在栈中的。

说明，read six number 就是放入栈中进行读取

之后，他进行了将rsp中的值与1进行比较

跳转条件见这里

```
https://blog.csdn.net/andiao1218/article/details/101192369
https://blog.csdn.net/weixin_41890599/article/details/104848164
```

![image-20211206214146456](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206214146456.png)

如果相等则跳过爆炸代码，

然后跳转到400f30

![image-20211206214229835](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206214229835.png)

![image-20211206214313930](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206214313930.png)

将`rsp+0x4`与`rsp+0x18`的值分别存放到了`rbx`与`rbp`

。下一行又进行了一次跳转，来到了400f17

![image-20211206214410938](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206214410938.png)

![image-20211206214427234](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206214427234.png)

将`rbx`的地址减4中存放的内容复制到了`eax`中

rbx-4     `rbx`的地址减4也就意味着与`rsp`相等

因为之前去了+4  

rsp+4 = rbx 

然后rbx-4 = rsp

%eax中它的值也就是第一个读入的值

下一行将`eax`的值乘二

接下来将乘二后的值与`rbx`也就是第二个值进行比较，rbx此时已经是下一个值了，如果相同则不炸

也就是说，第二个数的值是第一个值的两倍就不会炸



为什么rbx+0x4呢 因为一个int 是四个字节，也就指向了下一个单元



![image-20211206215038847](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206215038847.png)

那么，这里rbp的值是用来干什么的呢？

rbp = rsp+0x18 也就是24个字节，也就是6个int

相当于是一个满足的判断

for循环的满足条件一样

![image-20211206215320030](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206215320030.png)

这里，上面我们加完之后会和rbp进行比较， 防止超过for循环限制的次数

所以答案就很显而易见了

也就是`1 2 4 8 16 32`

![image-20211206215600263](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206215600263.png)

没问题

### tips 

这里的**`read_six_numbers `** 实际上调用了sscanf函数

`sscanf`与`scanf`类似，都是用于输入的，只是后者以屏幕(`stdin`)为输入源，前者以固定字符串为输入源。



其中的`format`可以是一个或多个：`{%[*][width][{h|l|I64|L}]type|' '|'\t'|'\n'|非%符号}`

- `*` 亦可用于格式中即(`%*d、%*s`) 加了[星号](https://www.zhihu.com/search?q=星号&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"article"%2C"sourceId"%3A370029604}) (`*`) 表示跳过此数据不读入(也就是不把此数据读入参数中)；
- `{a|b|c}`表示`a/b/c`中选一; `[d]`: 表示可以有`d`也可以没有`d`。
- `width`: 宽度，一般可以忽略，用法如：

```cpp
const  char sourceStr[] = "hello, world";
    char buf[10] = {0};
    sscanf(sourceStr, "%5s", buf);   //%5s，只取5个字符
    cout << buf<< endl;

    //结果为:hello
```



## phase3

```
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq 
```

上来%rsp 减了18，开了空间（我觉得是开栈，然后最后add了，栈平衡）

然后%rcx = %rsp+0xc 也就是加了12

然后%rdx = %rsp+0x8 也就是加了8

然后给%esi赋值 0x4025cf

然后给%eax赋值0x0

调用了sscanf函数读取输入的内容

然后比较%eax和1的值

sscanf 库函数的参数中需要一个格式化字符串

我们执行的时候看看$0x4025cf esi的值到底是什么呢？

![image-20211206220906388](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206220906388.png)

可以看到这就是格式化字符串，读入的是两个整型值。这两个值存放在哪里呢？我们想到之前把rsp+0xc与rsp+0x8的值分别给rcx与rdx，这是两个地址值，



我们可以用之前的方法验证栈中存放的确实是我们读入的这两个值。

简单画一个栈

![image-20211206221353625](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206221353625.png)

`eax`一般用于存放函数返回值

而`sscanf` 的返回值是成功读入的数值个数

![image-20211206221551893](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206221551893.png)

![image-20211206221602263](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211206221602263.png)

也就是说，大于1 的都跳转，不会被炸，跳到400f6a

![image-20211207153500893](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207153500893.png)

也就是跳转到了另一个比较的地方，去让0x7 和%rsp+0x8 的值进行比较，大于7则 跳转到400fad 然后引爆炸弹![image-20211207153957455](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207153957455.png)

![image-20211207153853484](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207153853484.png)

因此这里的数不能大于7，

我们给%eax赋值 %rsp+0x8



![image-20211207155303814](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207155303814.png)

`*0x402470(,%rax,8)`，这就是一个典型的`switch`语句的实现,使用间接跳转，：直接跳转到索引*位移的指令位置。

这个有三个参数，第一个是%rdx，第二个是%rax，第三个是8



```
jmp *0x402470(,%rax,8) means jmp *(0x402470(,%rax,8)) and * means the data at the address
```



![image-20211207155751957](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207155751957.png)

看了很多文章，也没有具体说明 `*0x402470(,%rax,8)` 这个具体如何去计算，

### tips

```
This operation jmpq *0x402390(,%rax,8) is for jumping directly to the absolute address stored at
8 * %rax + 0x402390

 jump = 0x402470 + 8 *i (i <= 7)
```

这个jmpq 是直接跳转到存储在8*%rax+0x402390的绝对地址，

这个是switch跳转表

```
x/16gx 0x402470
（检查从 0x402390 开始的十六进制中的 16 个“字”），您会发现地址表如下所示
```

![image-20211207171611900](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207171611900.png)

![image-20211207170128023](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207170128023.png)

````
x /x *(0x402470)
这里可能有人会问，为什么需要* 直接0x20470 不可以么，
取※是为了取地址，可以去见源文档，*0x402470 获得他的地址
````



![image-20211207170610539](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207170610539.png)

当然，表里的，和我们框住的一样

![image-20211207171300611](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207171300611.png)

我们接着查看

```
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
```

%eax 赋值为 0x2c3 也就是707

![image-20211207154342122](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207154342122.png)

![image-20211207154526021](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207154526021.png)

```
x = 0
400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
x = 2
400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
x = 3
400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
x = 4
400f91:	b8 85 01 00 00       	mov    $0x185,%eax
400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
x = 5
400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
x = 6
400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
x = 7
400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>

400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
x = 1
400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
400fc2:	74 05                	je     400fc9 <phase_3+0x86>

400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
400fc9:	48 83 c4 18          	add    $0x18,%rsp
400fcd:	c3                   	retq   

```



![image-20211207171718190](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207171718190.png)

将`eax`中的值与`rsp+0xc`也就是我们读入的第二个数进行判断，如果相等的话跳过引爆代码

那咱们选择，3 对应的第二个也就是 0x100 也就是256

答案：

````
3 256
````



![image-20211207172034749](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207172034749.png)

nice mo问题

## phase4

![image-20211207175617289](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207175617289.png)

这次是递归调用 和栈，让我来康康

首先，同样是调用了sscanf函数

```
 $0x4025cf 我们查看一下他的格式化字符串是多少
 
```

![image-20211207180056615](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207180056615.png)

读入了两个值，存放在了%rcx与%rdx

%eax 不等于0x2就跳



jbe 低于或相等

![image-20211207181104322](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207181104322.png)

（%rsp+0x8）也就是%rdx的值 低于或等于0xe 就跳转 然后爆炸

这里给%edx赋值 0xe 

给%esi赋值 0x0



![image-20211207181312610](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207181312610.png)

![image-20211207181728374](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207181728374.png)

然后给%edi= %rsp+0x8的值

接下来调用了fun4

我们先不看fun4 我们看最后结果

je 相等就跳

不相等就爆炸

![image-20211207180405590](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207180405590.png)

也就是说%rsp+0xc的值是0x0



我们接下来看fun4

![image-20211207181927980](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211207181927980.png)

![image-20211208205621332](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211208205621332.png)

直接扔IDA

v3 = (a3-a2)/2 +a2

如果v3 > a1 递归调用2*func4（a1,a2）

如果v3<a1 递归调用2*fun4（a1,v3+1）+1

也就是说v3 = a1 的时候，return result

0LL  *//long long*

那么我们跑一下

![image-20211208211817679](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211208211817679.png)



也就是说，第一部分func4要返回0，第二个参数也是0

所以答案任选，`0/1/3/7 第二个为0`

![image-20211208212007204](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211208212007204.png)





这个逆向的代码和ida的有些不同

```
#include<stdio.h>
int func4(int a1, int a2, int x){
    int b = (a1 - a2) >> 31;
    int result = ((a1-a2) + b) >> 1;
    b = result + a2;
    if(b == x) return 0;
    if(b < x) {
        result = func4(a1, b + 1, x);
        return result * 2 + 1;
    }else{
        result = func4(b - 1, a2, x);
        return result * 2;
    }
}

int main(){
	for(int i=0;i<=14;i++){
		if(func4(14,0,i) == 0  )
		printf("%d",i);
	}
	return 0;
}
```

### tips

这里我们针对，func4进行一下分析，ida逆向出来的并不能直接进行运行，很离谱，我们针对汇编进行一下分析吧

````
  400fce: 48 83 ec 08           sub    $0x8,%rsp  //分配栈帧
  400fd2: 89 d0                 mov    %edx,%eax  // %eax=0xe
  400fd4: 29 f0                 sub    %esi,%eax  // %eax=%eax-0=0xe
  400fd6: 89 c1                 mov    %eax,%ecx  // %ecx=%eax=0xe
  400fd8: c1 e9 1f              shr    $0x1f,%ecx //对%ecx的值逻辑右移31位
  400fdb: 01 c8                 add    %ecx,%eax  //%eax=%eax+%ecx=0xe
  400fdd: d1 f8                 sar    %eax       //%eax=%eax>>1=0x7
  400fdf: 8d 0c 30              lea    (%rax,%rsi,1),%ecx //%ecx=0x7+0x0=0x7
  400fe2: 39 f9                 cmp    %edi,%ecx  //%ecx-%edi=0x7-a
  400fe4: 7e 0c                 jle    400ff2 <func4+0x24>  //if <=0 则跳转
  也就是说，如果第一个参数大于7的时候，就会跳转400ff2
````



![image-20211209134544204](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209134544204.png)

```
 400ff2:  b8 00 00 00 00        mov    $0x0,%eax    //返回值为0
 400ff7:  39 f9                 cmp    %edi,%ecx    //再一次比较%ecx-%edi=0x7-a 
 400ff9:  7d 0c                 jge    401007 <func4+0x39> // if a<=7 则跳出
 400ffb:  8d 71 01              lea    0x1(%rcx),%esi  // 否则我们把%esi++重写执行
 400ffe:  e8 cb ff ff ff        callq  400fce <func4> //递归回去
 401003:  8d 44 00 01           lea    0x1(%rax,%rax,1),%eax
 401007:  48 83 c4 08           add    $0x8,%rsp
 40100b:  c3                    retq   
```

这个就是典型的循环调用，也就是递归

这下面是超哥带我之后的分析

这里直接使用ida的反汇编，需要注意的是，有一些指令，ida反汇编的并不能很好的兼容，就比如func的参数

```

int func(int a1,int a2,int a3){
	int v3;
	int result=0;
	v3 = (a3-a2)/2+a2;
	if( v3 > a1){
		return 2*func(a1,a2,v3-1);
	}
	if( v3 < a1){
		return 2*func(a1,v3+1,a3)+1;
	}
	return result;
}
int main(){
	for(int i=0;i<=14;i++){
		if(func(i,0,14) == 0  )
		printf("%d",i);
	}
	return 0;
}
```

因为a3是从%edx传递参数，rcx中存储的就是v3，这里v3-1 因此我们参数也是v3-1



![img](https://gitee.com/dingpengs/image/raw/master/imgwin/%7B1D8E1134-D264-3386-BAD6-E54AC7F84654%7D.jpg)

ida调用的时候，参数放在了最后一个，所以，需要我们改一下顺序

![img](https://gitee.com/dingpengs/image/raw/master/imgwin/%7B78B99897-166A-3EF5-2B50-4CC1792D1738%7D.jpg)

这个函数使用 rdi rsi edx传参数

## phase_5

```
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx
  401096:	83 e2 0f             	and    $0xf,%edx
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
  4010a4:	48 83 c0 01          	add    $0x1,%rax
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   

```

首先，第四行把输入的地址，rdi给了rbx，第五第七则向栈中压入了一个金丝雀值

![image-20211209192730209](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209192730209.png)

![image-20211209192950422](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209192950422.png)

这里，清空了eax中的值，然后调用stringlength函数，统计字符串的个数，并放在%eax中，

然后eax中的值和0x6进行比较，等于则不炸，我们进入

![image-20211209193243228](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209193243228.png)

他将eax赋值为空，然后跳转到40108b

![image-20211209193325902](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209193325902.png)

看起来像一个循环

movzbl命令，将rbx输入开始的第rax个位置的第一个字节赋值给ecx的低16位

%cl 就是cx的低8位，%dl就是dx的低8位

接下来，把cl中的值赋值给了%rsp里面，再把%rsp中的值赋值到%rdx中，

第四行，使用与运算，和0xf 也就是` 0000 1111` 取了%edx的后四位，



也就是说，读入的字符串中的rax位置的字符，并取他的低四位放在%edx中，



然后，将地址` 0x4024b0+rdx` 中的一个字节放入edx的低16位中，

然后将这16位赋值到了` rsp+0x10+rax` 的位置中，



然后我们给%rax+0x1

然后进行比较，如果不行，就跳回去，在+1，直到等于6



循环执行了6次，也就是读取了输入的6个字符，记录6个字符的低8位作为索引rdx，

从`0x4024b0+rdx`的位置赋值一个字节到	`rsp+0x10` 开始的6字节中。结束之后，`rsp+0x10` 开始存放了6个字符

![image-20211209200650670](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209200650670.png)

这里给%rsp+0x16 的位置，也就是6个字符之后，添加0x0也就是终止符`\0`

![image-20211209200842734](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209200842734.png)

接下来将`0x40245e`这个地址赋给`esi`，把`rsp+0x10`这个地址赋给`rdi`，接下来调用`strings_not_equal`这个函数，`esi`与`rdi`就是要比较的两个字符串的首地址。如果两个字符串不相同就引爆炸弹。

我们先查看0x40245e位置的字符串是什么是flyers



![image-20211209203243349](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209203243349.png)

这个也就是我们要构造的，放在rsp+0x10 的字符，

这里我们在maduiersnofotybuso中找flyers，

也就是9 15 14 5 6 7

![image-20211209210407361](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209210407361.png)

那么我们再将 后四位是这几个的字符输入即可

```

for i in range(65, 123):
    # if (bin(i).replace('0b','')[-4::]) == bin(9).replace('0b',''):
    #     print(chr(i),end=',')
    # if (bin(i).replace('0b', '')[-4::]) == bin(15).replace('0b',''):
    #     print(chr(i),end=',')
    if (bin(i).replace('0b', '')[-4::]) == bin(14).replace('0b',''):
        print(chr(i),end=',')
    # if (bin(i).replace('0b', '')[-3::]) == bin(5).replace('0b',''):
    #     print(chr(i),end=',')
    # if (bin(i).replace('0b', '')[-3::]) == bin(6).replace('0b',''):
    #     print(chr(i),end=',')
    # if (bin(i).replace('0b', '')[-3::]) == bin(7).replace('0b',''):
    #     print(chr(i),end=',')
```

![image-20211209223419541](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209223419541.png)

第一组

![image-20211209223434179](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209223434179.png)

第二组

![image-20211211213847485](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211213847485.png)

第三组

![image-20211209223447216](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209223447216.png)

第四组

![image-20211209223500179](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209223500179.png)

第五组

![image-20211209223514023](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211209223514023.png)

第六组

我们任意挑选

```
这里我们使用第0个 也就是IONEFG
```



![image-20211211214100170](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211214100170.png)

## phase_6

```
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  40110b:	49 89 e6             	mov    %rsp,%r14
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  4011ba:	48 89 d9             	mov    %rbx,%rcx
  4011bd:	48 8b 10             	mov    (%rax),%rdx
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)
  4011d9:	00 
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
```

6比较长，我们分段查看

![image-20211211214307450](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211214307450.png)

首先，压入栈6个数字

然后栈帧移动

%r13 = %rsp

%rsi = %rsp

两个mov猜测是read的参数，rsi就是我们的栈帧，然后调用read——six

![image-20211211214449865](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211214449865.png)

前面几行，是一系列的赋值，

%r14 = %rsp

%r12d = 0x0

%rbp = %r13

%eax = %r13+0x0 也就是第一个参数

eax中的值是rsp位置存放的值

然后突然给%eax减0x1



比较，%eax和0x5的大小关系，也就是说，判断%eax-6 的大小关系

jbe也就是当%eax-6 < = 0 的时候跳转



不小于6就爆炸

之前，%r12d 为0，

![image-20211211215819586](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211215819586.png)

之后呢，我们将%r12d的值增加1，然后与6进行比较，如果相等则跳转，（一会儿分析）

![image-20211211220055422](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211220055422.png)

如果不相等，那么则，%ebx = %r12d

又赋值给了%rax = %ebx

然后，将`rsp+rax*4` 中的值，也就是 第`rax+1` 个读入的int值（一个int这里应该是按4个字节） 给了%eax；

接下来，%eax中的值，与rbp+0x0地址指向的值进行比较

如果相同则爆炸，不同则跳走

![image-20211211220457784](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211220457784.png)

也就是值要不同，然后接下来，%ebx+0x1 再和0x5进行比较，如果小于5 则跳转到401135 再去栈中取下一个新的int值 和rbp中的进行比较

![image-20211211220546776](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211220546776.png)

这一段就是不断地去获取栈中的值与rbp的值进行比较

这两个则是 外循环，他更新了%r13的值，令r13 指向下一个int值，

然后跳到

![image-20211211220745683](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211220745683.png)

然后跳回去



![image-20211211220843787](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211220843787.png)

也就是，调回去用r13的值，更新了rbp的值，也就是把比较的对象往后移动了一个，（类似于链表）



这一段的代码，就是读入6个数字，必须小于等于6 而且不能互相相等

也就是说，我们输入的肯定就是654321 ，那么具体是什么顺序呢，我们接着往下看



![image-20211211221229958](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211221229958.png)

第一行，将%rsp+0x18的值取出来给了%rsi

第二行，%rax = %r14  %r14的值是之前保存的rsp

第三行，%ecx = 0x7 

第四行，%edx = %ecx

然后，将%edx值减去%rax的值（0x7-%rax），然后将%rax = %edx

也就是更新一下%rax作为剩余的部分，

接着%rax增加0x4

也就是指向下一个int值

接着与之前设定的%rsi中的值进行比较

如果不相同，就重复这个过程，rsi实际上指向的是6个int值之后的位置，作为一个标记使用

这一段的话，也就是将栈中的值变成 7-x



![image-20211211223034682](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211223034682.png)



我们来看下一段，首先将esi置为0x0

然后跳转到401197

![image-20211211223134436](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211223134436.png)

这里从%rsp+%rsi*1 的位置 取出一个数 给了%ecx，接下来对这个数进行判断，

如果它小于等于1 则跳转到 401183

![image-20211211223257372](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211223257372.png)



这里，%edx = 0x6032d0

然后，将%edx的值赋给了`rsp+2*%rsi+0x20 ` 

这里我们可以知道rsi的作用是索引，

因为，下面一行%rsi+0x4 

说明从%rsp+0x20 开始存访8个字节的数据，

然后将%rsi的值与0x18 做比较 ，说明整个过程要进行6次（0x18 / 0x4 ），相等后，跳转到了4011ab

![image-20211211223812861](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211211223812861.png)

如果不等的时候，就将下一个int值给了rcx，



如果 rcx的值不小于等于1 那么接着往下，给%eax赋值0x1

然后给%edx赋值 0x6032d0

然后强行跳转到401176



![image-20211212142702173](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212142702173.png)

我们看到，有三个绿色，前三行是一体的，

把%rdx+0x8 的值赋给了 %rdx，也就是，往下一个单元移动了8个字节



我们可以查看一下最基本的0x6032d0的开始

![image-20211212144327927](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212144327927.png)

```
x /12xg 0x6032d0 12是为了查看12个
x /xg 0x6032d0 只查看一个

```

这我们查看，那么肯定就是一个链表的结构，一个node中偏移8个字节中，存储的就是下一个节点的地址，前面的8个字节就是节点存储的数据

![image-20211212145209655](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212145209655.png)

我们再看之前很难理解的部分，也就是`p  = p->next` 

指向下一个节点，【也就是为什么，我们需要理解0x8(rdx),rdx】



整理思路，从栈中读取存放的6个数据，放入了rcx，然后根据rcx值找到链表中对应的解点，然后把节点的地址放入rsp+0x20 开始对应位置中

![image-20211212150426924](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212150426924.png)

这里，分别

将rsp+0x20 值给了%rbx，

%rsp+0x28 值给了%rax

%rsp+0x50 值给了%rsi

将%rbx的值给了%rcx

然后第五行，将rax（rsp+0x20）中存放的地址复制到了%rdx

然后，%rdx中的值给了%rcx+0x8（也就是，rbx，*（rsp+0x20））解点的指针域

然后%rax+0x8 移向下一个，最后和%rsi进行比较，%rsi就是最后一个也就是边界值

如果rax超出末端则跳出这段代码

![image-20211212164619364](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212164619364.png)

跳转到了4011d2

然后给%rdx+0x8 赋值为0x0 也就是把表尾的指针赋值为null

如果不跳，那就是把%rdx赋值给%rcx，然后跳转到之前进行重复操作，

这个其实也就是按照链表节点在栈中的位置重新将链表连接起来



![image-20211212171015427](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212171015427.png)

首先给%ebp赋值 0x5

把%rbx+0x8值赋给%rax

rbx之前的值是rsp+0x20 那么 这个rbx+0x8 中这个地址存放的值就是下一个节点的地址，给了%rax

第四行，rax代表的节点的数据取出来，赋值eax，再和

然后将 %eax，%rbx，进行对比，



这里为什么和eax比呢？因为eax占32位，也就是说，在十六进制里占8个，和实际看到的一致



也就是说，下一个节点的值和上一个节点的值进行比较

如果前一个节点的值大于等于后一个节点的值 则跳转到4011ee

否则爆炸

![image-20211212213015619](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212213015619.png)

还是这样，跳过爆炸，给%rbx+0x8

指向下一个结点

![image-20211212213340369](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212213340369.png)

%ebp减小1

然后在进行判断，保证循环五次

也就是说，要使得新的链表中前一个存放数据的数据值都大于后一个节点

我们找一下顺序

![image-20211212213747957](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212213747957.png)

那么实际算来，大小也就是

`3 > 4 > 5 > 6 > 1 >2 `

又因为之前又x = 7-x 重新排列了一次，

也就是说

`4 > 3 > 2 > 1 > 6 > 5`

![image-20211212214555840](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212214555840.png)



## secret_phase



![image-20211212214812212](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212214812212.png)

当然，在fun7之后，存在了一个secret_phase 

首先，我们怎么进去呢？

![image-20211212214910991](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212214910991.png)

搜索之后，只有前面这里有跳转

![image-20211212214948295](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212214948295.png)

也就是说，在phase_defused里面

![image-20211212215112307](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212215112307.png)

这里在main里面有两个跳转，

![image-20211212215710317](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211212215710317.png)

我们回到phase

这里，有一个调用的地方，numinput strings

这里，把输入的长度和0x6进行了一次比较如果不等于6，那么直接跳转到结束

![image-20211216131919579](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216131919579.png)

如果等于6 ，那么接下来去取出一堆地址，

把%rsp+0x10 的地址给了%r8

%rsp+0xc的地址给了%rcx

%rsp+0x8的地址给了%rdx



然后把0x402619给了 %esi，%esi是格式化字符串的首地址

这是sscanf调用的过程，我们查看一下内容



![image-20211216140247396](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216140247396.png)

说明输入的是%d %d %s

![image-20211216140628402](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216140628402.png)

esi给了 一个地址，也就是格式化字符串

那么edi是what呢？

因为之前所有的edi是读取的第一个字符串的位置，这里默认猜测也是第一个的位置

![image-20211216151331347](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216151331347.png)

因为我们之前，有一个输入 0 0 通关，同时，这里两个%d，说明还少一个字符串

![image-20211216151927383](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216151927383.png)

我们这里看到

他和0x3进行比较，

如果不够3 那么则跳过，不使用phasedefused

然后紧接着给 esi赋值 0x402622

![image-20211216152316273](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216152316273.png)

相当于是strings—not—equal的调用，

也就是说，他的参数是esi和edi，esi给了地址的值，edi可能就是第三个字符串的地址值，

rdi = %rsp+0x10

那么我们需要比较什么呢？

![image-20211216154843223](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216154843223.png)

我们去查看，0x402622 

得知，他是DrEvil

```
0000000000401242 <secret_phase>:
  401242:	53                   	push   %rbx
  401243:	e8 56 02 00 00       	callq  40149e <read_line>
  401248:	ba 0a 00 00 00       	mov    $0xa,%edx
  40124d:	be 00 00 00 00       	mov    $0x0,%esi
  401252:	48 89 c7             	mov    %rax,%rdi
  401255:	e8 76 f9 ff ff       	callq  400bd0 <strtol@plt>
  40125a:	48 89 c3             	mov    %rax,%rbx
  40125d:	8d 40 ff             	lea    -0x1(%rax),%eax
  401260:	3d e8 03 00 00       	cmp    $0x3e8,%eax
  401265:	76 05                	jbe    40126c <secret_phase+0x2a>
  401267:	e8 ce 01 00 00       	callq  40143a <explode_bomb>
  40126c:	89 de                	mov    %ebx,%esi
  40126e:	bf f0 30 60 00       	mov    $0x6030f0,%edi
  401273:	e8 8c ff ff ff       	callq  401204 <fun7>
  401278:	83 f8 02             	cmp    $0x2,%eax
  40127b:	74 05                	je     401282 <secret_phase+0x40>
  40127d:	e8 b8 01 00 00       	callq  40143a <explode_bomb>
  401282:	bf 38 24 40 00       	mov    $0x402438,%edi
  401287:	e8 84 f8 ff ff       	callq  400b10 <puts@plt>
  40128c:	e8 33 03 00 00       	callq  4015c4 <phase_defused>
  401291:	5b                   	pop    %rbx
  401292:	c3                   	retq   
  401293:	90                   	nop
  401294:	90                   	nop
  401295:	90                   	nop
  401296:	90                   	nop
  401297:	90                   	nop
  401298:	90                   	nop
  401299:	90                   	nop
  40129a:	90                   	nop
  40129b:	90                   	nop
  40129c:	90                   	nop
  40129d:	90                   	nop
  40129e:	90                   	nop
  40129f:	90                   	nop
```

![image-20211216161634637](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216161634637.png)

首先，调用了readline，逐行读取，然后，我们给read_line的返回值给了rdi，并调用了strtol函数，这个标准库的作用是 把字符串转换成对应的长整型，返回值存放在rax中，第8行，将rax-1 赋值给了eax

然后与0x3e8进行比较，如果小于，则跳过引爆，

所以，我们还需要一个小于1001的值

接下来，我们把ebx赋值给了esi，

也就是我们输入的rax的值，

然后，将0x6030f0给了edi

然后callq了fun7

函数执行完成后，比较了eax和2的区别，

如果相等，则跳过爆炸

![image-20211216162809157](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216162809157.png)

那么我们查看一下fun7的函数

![image-20211216162834877](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216162834877.png)

首先，我们对输入的数字做判断，如果相等，跳转到401238

![image-20211216162925676](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216162925676.png)

同时，返回-1，这肯定不行，那么，我们如果不跳转，把%rdi的值给了%edx，

同时在接下来，将我们输入的进行比较，如果小于我们读入的书，则继续执行

![image-20211216163300278](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216163300278.png)

将eax改为0

在进行一次，相同的比较，然后如果相同，那么则跳转到20行，然后返回

![image-20211216163431019](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216163431019.png)

如果不相等则继续执行，

我们这里查看一下rdi到底是什么

因为这里有点像链表的感觉

![image-20211216163711221](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216163711221.png)

这个其实是二叉树，

每个节点第一个8字节，第二个8字节存访左子树地址，第三个8字节存访右子树位置，并且命令也有规律，nab，a代表层数，b代表从左至右第b个节点，我们抄大佬的树

![image-20211216163901272](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216163901272.png)

所以，我们的0x10(rdi),rdi 就是将他移动到右子树的位置，接着调用fun7,在返回后，令eax=2*rax+1

![image-20211216164739020](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216164739020.png)

```
也就是说，需要我们的输入和树进行比较
如果两者相等：返回0
如果前者大于后者：rdi移至左子树，返回2 * rax
如果后者大于前者：rdi移至右子树，返回2 * rax + 1
```

那么我们想要在最后一次调用返回0，倒数第二次调用返回2*rax+1

第一次调用返回2*rax，这个数应该在第三层，比父节点大，且比根节点小，唯一的答案就是

0x16(22)

```
(gdb) x/120 0x6030f0
0x6030f0 <n1>:	0x00000024	0x00000000	0x00603110	0x00000000
0x603100 <n1+16>:	0x00603130	0x00000000	0x00000000	0x00000000
0x603110 <n21>:	0x00000008	0x00000000	0x00603190	0x00000000
0x603120 <n21+16>:	0x00603150	0x00000000	0x00000000	0x00000000
0x603130 <n22>:	0x00000032	0x00000000	0x00603170	0x00000000
0x603140 <n22+16>:	0x006031b0	0x00000000	0x00000000	0x00000000
0x603150 <n32>:	0x00000016	0x00000000	0x00603270	0x00000000
0x603160 <n32+16>:	0x00603230	0x00000000	0x00000000	0x00000000
0x603170 <n33>:	0x0000002d	0x00000000	0x006031d0	0x00000000
0x603180 <n33+16>:	0x00603290	0x00000000	0x00000000	0x00000000
0x603190 <n31>:	0x00000006	0x00000000	0x006031f0	0x00000000
0x6031a0 <n31+16>:	0x00603250	0x00000000	0x00000000	0x00000000
0x6031b0 <n34>:	0x0000006b	0x00000000	0x00603210	0x00000000
0x6031c0 <n34+16>:	0x006032b0	0x00000000	0x00000000	0x00000000
0x6031d0 <n45>:	0x00000028	0x00000000	0x00000000	0x00000000
0x6031e0 <n45+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x6031f0 <n41>:	0x00000001	0x00000000	0x00000000	0x00000000
0x603200 <n41+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x603210 <n47>:	0x00000063	0x00000000	0x00000000	0x00000000
0x603220 <n47+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x603230 <n44>:	0x00000023	0x00000000	0x00000000	0x00000000
0x603240 <n44+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x603250 <n42>:	0x00000007	0x00000000	0x00000000		0x00000000
0x603260 <n42+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x603270 <n43>:	0x00000014	0x00000000	0x00000000	0x00000000
0x603280 <n43+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x603290 <n46>:	0x0000002f	0x00000000	0x00000000	0x00000000
0x6032a0 <n46+16>:	0x00000000	0x00000000	0x00000000	0x00000000
0x6032b0 <n48>:	0x000003e9	0x00000000	0x00000000	0x00000000
0x6032c0 <n48+16>:	0x00000000	0x00000000	0x00000000	0x00000000
```

![image-20211216171728926](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216171728926.png)

这些情况，直接会退出来，结束条件，edx = 0

因为我们定义过初始的eax为0

![image-20211216172823538](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216172823538.png)



### tips

成功进入

![image-20211216184930974](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216184930974.png)

因为没做出来，重新捋一下思路

首先，我们必须在树节点中，找一个节点，否则直接会跳转到0xffffff

````
如果两者相等：返回0
如果前者大于后者：rdi移至左子树，返回2 * rax
如果前者小于后者：rdi移至右子树，返回2 * rax + 1
````

也就是说，我们输入的值，

首先要经过 2*rax+1

然后经过     2*rax， 

然后返回0，我们的rax才能够成为2

也就是说

因为他是递归

我们需要首先 再叶子节点的最下面，，我们画一个图，包含顺序



![image-20211216190028309](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216190028309.png)

![image-20211216192249685](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216192249685.png)

22 也就是0x16

![image-20211216192425379](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211216192425379.png)

完结撒花