## Cache lab 

cachelab 包含part A 和part B 

也就是

part A实现一个缓存，part B使用算法提高命中率

当然我们需要使用valgrind 工具来进行测试



## 实验附件

- 

  README

  http://csapp.cs.cmu.edu/3e/README-cachelab

- 

  Writeup

  http://csapp.cs.cmu.edu/3e/cachelab.pdf

- 

  版本历史

  http://csapp.cs.cmu.edu/3e/cachelab-release.html

- 

  自学材料

  http://csapp.cs.cmu.edu/3e/cachelab-handout.tar



### readme

```
This is the handout directory for the CS:APP Cache Lab. 

************************
Running the autograders:
************************

Before running the autograders, compile your code:
    linux> make

Check the correctness of your simulator:
    linux> ./test-csim

Check the correctness and performance of your transpose functions:
    linux> ./test-trans -M 32 -N 32
    linux> ./test-trans -M 64 -N 64
    linux> ./test-trans -M 61 -N 67

Check everything at once (this is the program that your instructor runs):
    linux> ./driver.py    

******
Files:
******

# You will modifying and handing in these two files
csim.c       Your cache simulator
trans.c      Your transpose function

# Tools for evaluating your simulator and transpose function
Makefile     Builds the simulator and tools
README       This file
driver.py*   The driver program, runs test-csim and test-trans
cachelab.c   Required helper functions
cachelab.h   Required header file
csim-ref*    The executable reference cache simulator
test-csim*   Tests your cache simulator
test-trans.c Tests your transpose function
tracegen.c   Helper program used by test-trans
traces/      Trace files used by test-csim.c
```

我们首先make一下

![image-20220109204417491](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109204417491.png)

## 概述

根据实验要求，我们需要完成csim.c文件，编译之后，能够实现如下功能

![image-20220109232829205](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109232829205.png)

要求我们的程序能够手动设置cache的set数量，line数量，block大小，读取

![image-20220109234626214](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109234626214.png)

每一行的第一个字母代表操作

| operator | what             | do what                   |
| -------- | ---------------- | ------------------------- |
| I        | instruction load |                           |
| L        | data load        | 读access cache            |
| S        | data store       | 写access cache            |
| M        | data modify      | 又读又写 access cache两次 |

I

L

S

M

![image-20220109235043742](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109235043742.png)

第二个数字代表16位内存地址，第三个代表该操作要对多大的byte内容进行

## Part A：Writing a Cache Simulator

使用LRU策略，编写csim.c，模拟cache的运行。cache为多路组相联，组数、每组行数、每行块数由调用参数给出，具体看WriteUp。材料中给出了一个名为csim-ref的可执行文件，要求我们程序最后执行的结果和它一样。

### getopt()

![image-20220109235828109](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109235828109.png)





要使用getopt函数解析命令行: 上面的讲义有写

![image-20220110000502124](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110000502124.png)

### fscanf()

读入测试文件的，自带，

```
FILE * pFile; //pointer to FILE object
pFile = fopen ("tracefile.txt",“r"); //open file for reading
char identifier;
unsigned address;
int size;
// Reading lines like " M 20,1" or "L 19,3"
while(fscanf(pFile,“ %c %x,%d”, &identifier, &address, &size)>0){
// Do stuff
}

fclose(pFile); //remember to close file when done
```

![image-20220110000444694](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110000444694.png)

### Malloc/Free

分配和释放内存空间的函数。

具体用法如下：

Some_pointer_you_malloced = malloc(sizeof(int));

Free(some_pointer_you_malloced);

分配了内存用完的时候记得释放，还有不要释放没有分配的内存。

![image-20220110000546377](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110000546377.png)







![image-20220110000649579](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110000649579.png)

大概就是告诉你它只是一个模拟缓存的行为并不是真实缓存，内存的数据不用存储，不使用块偏移，因此地址种b并不重要，

简单的计算hits、miss、evictions的值。

然后就是你的模拟器需要适用于不同的（s，b，E），同时给出运行时间。



最后就是要求你使用这个LRU最近最少使用策略，意思就是使用这个策略进行行的牺牲，因此cache发生miss的时候，就会从它的k+1层存储器读取新的拷贝，假设在空间已满的情况下（未满的时候不需要替换行）,新读取的数据需要存储在cache里面，但空间已满，只能选择驱逐（原文是驱逐的意思，也就是覆盖某一行），然后覆盖的方法就是这个LRU策略。



LRU策略就是：选择那个最后被访问的时间距离现在最远的块。代码体现的话就是给cache的每一行绑一个Lrunumber，这个值越小（你也可以设置为最大，随自己喜欢）表示它最后一次被访问的时间距离现在最远，可以作为牺牲行。



cache有组数S、一组包含的行数E，存储块的字节大小B，cache的容量C（C=S*E*B)

地址的构成：标识位t、组索引s、块偏移b（前面说了实验不使用块偏移，但计算标记位的时候需要用到）

![image-20220109235652317](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220109235652317.png)

首先是cache的构成:

cache有2^s组，每组有E行（E分为三种情况：E=1，称为直接映射高速缓存；1<E<C/B:组相联高速缓存；E=C/B，全相连高速缓存，也就是一组包含了所有的行），然后是每行的构成：

（1）有效位vild(取0和1，1表示存储了有效信息，0表示没有，判断命中的时候需要用到）

（2）标记位t=m-(s+b),当cpu要读取某个地址的内容时，就会将某个地址的第一个部分：标记位与它进行对比，匹配相等则命中，也就是锁定在某一行

（3）数据块B，B负责存储这个地址的内容，把B想象成字节数组就好，共有B-1个小块（别和B这个数据块搞混咯,下标从0开始，因此是B-1）。



在这里简单提下b的作用吧，虽然实验没用到。b理解为数据块B这个数组的下标就好，也就是你要从B[b]这个位置开始读取字。注意这里的数据都是2进制的。

首先我我们定义cache的结构体



```
详细函数见这里
https://blog.csdn.net/weixin_42294984/article/details/80738945
https://blog.csdn.net/weixin_41256413/article/details/81388351
```

个人抄的非常简略版本。实在不太理解

```
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
//定义行的属性
typedef struct{
    int valid_bits;//有效位
    unsigned tag;//标识位
    int stamp;//lruNumber牺牲行的时候，也就是需要替换的时候
}cache_line;

//定义参数
char* filepath = NULL;
int s,E,b,S;           // s is set ,E  is line,each line have 2^b bits ,S is 2^s set
int hit=0,miss=0,eviction=0;
cache_line** cache = NULL;

//函数实现
//初始化函数
void init(){
	cache = (cache_line**)malloc(sizeof(cache_line*)*S);             //malloc cache[S][E]
	for(int i=0;i<S;i++)
		*(cache+i) = (cache_line*)malloc(sizeof(cache_line)*E);
	for(int i=0;i<S;i++){
		for(int j=0;j<E;j++){
			cache[i][j].valid_bits = 0;      // set all valid_bits is zero
			cache[i][j].tag = 0xffffffff;           //no address
			cache[i][j].stamp = 0;            //time is 0;
		}
	}
}
//比较重要的更新函数
void update(unsigned address){
	unsigned s_address =(address>>b) & ((0xffffffff)>>(32-s));//set`s index
	unsigned t_address = address>>(s+b);                     //tag`s index
	for(int i=0;i<E;i++){
		if((*(cache+s_address)+i)->tag ==t_address){
			cache[s_address][i].stamp = 0;       //now ,this is used
			hit++;
			return;
		}
	}
	for(int i=0;i<E;i++){
		if(cache[s_address][i].valid_bits == 0){
			cache[s_address][i].tag = t_address;
			cache[s_address][i].valid_bits = 1;
			cache[s_address][i].stamp = 0;       //now ,this is load
			miss++;
			return;
		}
	}
	int max_stamp=0;
	int max_i;
	for(int i=0;i<E;i++){
		if(cache[s_address][i].stamp >max_stamp){
			max_stamp = cache[s_address][i].stamp;
			max_i = i;
		}
	}
	eviction++;
	miss++;
	cache[s_address][max_i].tag = t_address;
	cache[s_address][max_i].stamp = 0;
}
void time(){
	for(int i=0;i<S;i++){
		for(int j=0;j<E;j++){
			if(cache[i][j].valid_bits == 1)
				cache[i][j].stamp++;
		}
	}
}
int main(int argc,char *argv[])
{
	int opt;
	while((opt = getopt(argc,argv,"s:E:b:t:")) !=-1){           //parse command line arguments
		switch(opt){
		case 's':
			s=atoi(optarg);
			break;
		case 'E':
			E=atoi(optarg);
			break;
		case 'b':
			b=atoi(optarg);
			break;
		case 't':
			filepath = optarg;
			break;
		}
	}
	S = 1<<s;
	init();
	FILE* file=fopen(filepath,"r");
	if(file == NULL){     // read trace file
		printf("Open file wrong");
		exit(-1);
	}
	char operation;
	unsigned address;
	int size;
	while(fscanf(file," %c %x,%d",&operation,&address,&size)>0){
		switch(operation){
			case 'L':
				update(address);
				break;
			case 'M':
				update(address);
			case 'S':
				update(address);
				break;
		}
		time();
	}
	for(int i=0;i<S;i++)                  //free cache[S][E]
		free(*(cache+i));
	free(cache);
	fclose(file);	                //close file
    printSummary(hit,miss,eviction);
    return 0;
}

```

我们执行make后，进行测试



![image-20220110003828055](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110003828055.png)

## Part B: Optimizing Matrix Transpose

优化一个矩阵转置函数，使得cache miss尽可能少。

最后给分有三个给定的size，用csim-ref测试。

cache规模为：32组，直接映射，每行32字节数据

ppt中给出了这道题的方法：分块 + 重新组织遍历顺序

### PRE

矩阵转置

![image-20220110004003292](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110004003292.png)

![image-20220110004009564](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110004009564.png)

- 本实验要求

- - 只运行使用最多12个`int`局部变量
  - 不能使用递归函数
  - 不能对原始的矩阵A进行修改
  - 不能通过`malloc`申请空间

该高速缓存的架构为`s=5, E=1, b=5`。

评分标准如下。`m`指的是miss次数

![image-20220110004034720](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110004034720.png)

咱们只做32*32 能力有限。。。

### 为什么需要缓存

因为cpu和内存的速度不匹配

![image-20220110112006651](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112006651.png)

寄存器的速度会比主存的速度快的多。为了缓冲这种不匹配我们引入的`cache`也就是缓存思想。在主存和`cpu`之间引入新的机制。

![image-20220110112017064](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112017064.png)

### 缓存为什么起作用

局部性

我们知道程序具有局部性。正是局部性的存在才导致了缓存有作用

![image-20220110112037370](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112037370.png)

对于数组`a[i][j]`的访问是按照顺序的访问。我们先访问`a[0][0]`然后`a[0][1]`所以这体现了空间局部性。而对`sum`的访问则体现了时间局部性。因为每一次执行都会访问sum

下图就是`cache`的原理

![image-20220110112045117](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112045117.png)

我们上面的高速缓存，一般采用lru算法进行牺牲

- \1. cache未满

我们就可以把要访问的快放到cache里面。这样下次访问的时候就会命中

- \2. cache已满

这个时候我们需要选择一个牺牲块。把旧的块移出去。再把新的块移动进来。这里注意如果我们旧的块在`cach`中已经被修改过了。那么我们需要把它写到内存中，为了保持数据的一致性

![image-20220110112120180](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112120180.png)

![image-20220110112125112](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112125112.png)

### 矩阵乘法分析

我们都知道两个矩阵的乘法操作是如何进行的。

![image-20220110112141293](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110112141293.png)

```
c = (double *) calloc(sizeof(double), n*n);

/* Multiply n x n matrices a and b	*/
void mmm(double *a, double *b, double *c, int n) {  
   int i, j, k;
   for (i = 0; i < n; i++)  
     for (j = 0; j < n; j++)
        for (k = 0; k < n; k++)
           c[i*n + j] += a[i*n + k] * b[k*n + j];
}
```

**下面考虑如何用分块矩阵的方法来优化。**

####  先证明分块矩阵的乘法结果和不分块一致

![image-20220110113816961](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110113816961.png)

大概内容如上图。A矩阵的第一小块第一行和B矩阵的第一小块的第一列进行运算。然后加上A矩阵的第二小块的第一行和B矩阵第三小块的第一列进行运算。得到的结果就为`C[0][0]`。其他的运算也类似。可以发现分块矩阵乘法得到结果和之前是一样的。

#### **3.2 那么分块矩阵为什么有用那**

这里我们假设cache可以放下16*16的小矩阵。

![image-20220110113845840](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110113845840.png)

![image-20220110113901518](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110113901518.png)

![image-20220110113906696](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110113906696.png)

可以发现对于A矩阵的访问还是没有任何差距的。但是对于B矩阵由于能把整个B1矩阵全部放进去。虽然我们在访问`b[i][0]`（i=0～15）都会不命中。但是之后在访问`b[i][1~7]`的时候都会命中，这就和我们么有分块的时候形成了鲜明的对比。

```
void mmm(double *a, double *b, double *c, int n) 
{  int i, j,k;
   for (i = 0; i < n; i+=B) 
      for (j = 0; j < n; j+=B) 
          for (k = 0; k < n; k+=B) /* B x B mini matrix multiplications */  
	    for (i1 = i; i1 < i+B;i1++)
	      for (j1 = j; j1 < j+B; j1++) 
	         for (k1 = k; k1 < k+B; k1++) 
		    c[i1*n+j1] += a[i1*n + k1]*b[k1*n + j1];
}
```

## 开始

![image-20220110230920566](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110230920566.png)



一个int 4个字节，一个block 32字节，可以放8个int。

考虑 ![[公式]](https://www.zhihu.com/equation?tex=8\times8) 分块——所需组（行）数为 ![[公式]](https://www.zhihu.com/equation?tex=8%2B8%3D16) （组），足够

考虑 ![[公式]](https://www.zhihu.com/equation?tex=16\times16) 分块，所需组（行）数为 ![[公式]](https://www.zhihu.com/equation?tex=2\times16\times2%3D64) ,不够

根据PPT 或者web Aside的资料，我们可以算出 ![[公式]](https://www.zhihu.com/equation?tex=8\times8) 分块的理论miss次数：

![[公式]](https://www.zhihu.com/equation?tex=4\times4\times16%3D256) 次

（稍微解释一下：因为cache空间足够（组数足够），所以A 和B的正在计算的block的所有元素全部存在cache line中，一个block一共有 ![[公式]](https://www.zhihu.com/equation?tex=8%2B8%3D16) 行，所以会miss 16次。整个A（B）共有 ![[公式]](https://www.zhihu.com/equation?tex=4\times4%3D16) 个block。）

```
void trans_1(int M, int N, int A[N][M], int B[M][N])
{
	int ii, jj, i, j;
	for(jj = 0; jj < 32; jj += 8)
		for(ii = 0; ii < 32; ii += 8)
			for(i = ii; i < ii + 8; i++)
                        	for(j = jj; j < jj + 8; j++)
                                	B[i][j] = A[j][i];

 }
```



进一步观察会发现，这种冲突只会发生在处理对角线的块上。这就是WriteUp让我们注意对角线的原因。

WriteUp中提到了可以用不超过12个临时变量，那么我们可以空间换效率——把一行全部读到临时变量里，达到一次性读完的效果。

```c
void trans_1(int M, int N, int A[N][M], int B[M][N])
{
	int ii, jj, i, val1, val2, val3, val4, val5, val6, val7, val0;
	for(jj = 0; jj < 32; jj += 8)
		for(ii = 0; ii < 32; ii += 8)
		{
			for(i = ii; i < ii + 8; i++)
			{
				val0 = A[i][jj];
				val1 = A[i][jj + 1];
				val2 = A[i][jj + 2];
				val3 = A[i][jj + 3];
				val4 = A[i][jj + 4];
				val5 = A[i][jj + 5];
				val6 = A[i][jj + 6];
				val7 = A[i][jj + 7];
				B[jj][i] = val0;
				B[jj + 1][i] = val1;
				B[jj + 2][i] = val2;
				B[jj + 3][i] = val3;
				B[jj + 4][i] = val4;
				B[jj + 5][i] = val5;
				B[jj + 6][i] = val6;
				B[jj + 7][i] = val7;
			}
		}
}
```

我们直接修改trans.c即可

写一个自定义的

![image-20220110235228991](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110235228991.png)

在提交中进行跳转

![image-20220110235240834](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110235240834.png)

即可正常运行

我们获得了最基本的8分

![image-20220110235404399](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110235404399.png)

## 软件安装

```text
sudo apt install valgrind
```

在实验之前还需要装一下valgrind内存泄露检测工具。

![image-20220110235039019](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220110235039019.png)