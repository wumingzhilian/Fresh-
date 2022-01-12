# Data labs

## 实验附件

- README

  http://csapp.cs.cmu.edu/3e/README-datalab

- Writeup

  http://csapp.cs.cmu.edu/3e/datalab.pdf

- 版本历史

  http://csapp.cs.cmu.edu/3e/datalab-release.html

- 自学材料

  http://csapp.cs.cmu.edu/3e/datalab-handout.tar



## 实验简介

学生可以实现简单的逻辑函数、二进制补码和浮点函数，但必须使用 C 语言的一个高度受限的子集。例如，可能会要求仅用位级运算和直线代码（straightline code）来计算一个数的绝对值。该实验帮助学生理解 C 语言数据类型的位级表示和数据操作的位级行为。

###  tips

前期准备这里都有

https://hansimov.gitbook.io/csapp/labs/data-lab/readme-student





### 第一题

```
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return 2;
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {







  return 2;
}
```

第一题要求，使用 ~ 和 & 实现异或，

![image-20211124183831918](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124183831918.png)

make的时候，因为是64位，无法编译32位，我们需要装库

```
可以执行sudo apt-get install libc6-dev-i386语句联网安装32位库文件。
```

![image-20211124183918482](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124183918482.png)

成功

![image-20211124184108270](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124184108270.png)

btest 功能测试成功

### 第二题

> 使用位运算获取 对2补码     的最小 `int` 值。

 ```
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {

  return 2;

}
 ```

也就是相当于 2 的补码，然后 求他的补码的最小值，也就是

0000 0000 0000 0000 0000 0000 0000 0010 变成了 1000 0000 0000 0000 0000 0000 0000 0000 0000

（32位，因为是int）

![image-20211124211956440](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124211956440.png)

![image-20211124212022699](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124212022699.png)

###  第三题

通过位运算计算是否是补码最大值。

```
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  return 2;
}
```

那么，补码的最大值也就是，当然是针对int类型的，最大值就是符号位为0，其他位为1，

判断给的值是不是补码的最大值

！~都是非

![image-20211124214001855](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124214001855.png)

![image-20211124214008670](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124214008670.png)

![image-20211124214015550](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124214015550.png)

![image-20211124213900716](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124213900716.png)

双逻辑非，能够方便的返回布尔值

![image-20211124214541488](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124214541488.png)

![image-20211124215756845](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124215756845.png)

这是为什么呢?

![image-20211124220052876](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124220052876.png)

![image-20211124220058605](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124220058605.png)

这个代码也就是

x+1 与 ~x进行异或，如果是最大的，那么结果为0 逻辑非，变成true，！！ 将他的值变为1，排除了因为原始值为0导致的错误

![image-20211124220931720](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211124220931720.png)

### 第四题

```
题目：

目标：如果 x 的二进位的所有奇数位全位 1，则返回 1，否则返回 0。 注：二进制最低位是第 0 位。
例子：allOddBits (0xFFFFFFFD) = 0, allOddBits (0xAAAAAAAA) = 1
限制：只能使用操作符！~ & ^ | + <<>>
最大操作次数： 12
难度：2
```

判断所有奇数位是否都为1，这里的奇数指的是位的阶级是2的几次幂。重在思考转换规律，如何转换为对应的布尔值。

由于这里不能使用大常量，因此我们这儿需要自己生成0xAAAAAAAA 

为什么呢？

因为这里A 的二进制代表了1010   因此可以用来判断技术位的1

同时，因为x^x = 0 这里就可以进行判断了

`a==b` 等价于 !（a^b）

```
int allOddBits(int x) {
  int temp = 0xAA;
  temp = temp << 8 | temp; //0xAAAA
  temp = temp << 16 | temp; //0xAAAAAAAA
  return !((x&temp)^temp);
}

```

![image-20211125112317540](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211125112317540.png)

### 第五题

题目：

- 目标：返回 - x
- 限制：只能使用操作符！~ & ^ | + <<>>
- 最大操作次数：5
- 难度：2



```
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
```

因为 取反+1 表示 负数

这里我们使用42来举例

负数均为补码存访

```
正数的42
00101010
求他的负数也就是
11010110（补）
那么，如果要显示，就是除符号位，取反-1
也就是
10101010
```

![image-20211125120726255](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211125120726255.png)

### 第六题

```
目标： return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters ‘0’ to ‘9’)
例子：
isAsciiDigit(0x35) = 1.
isAsciiDigit(0x3a) = 0.
isAsciiDigit(0x05) = 0.
限制：只能使用操作符！~ & ^ | + <<>>
最大操作次数：15
难度：3
```

0x39 二进制表示为 `00111001`

0x30 为 `00110000`

那么，如果在这个范围内，那么必定会满足，x-0x30 >=0同时 x-0x3a <0

```
int isAsciiDigit(int x) {
    return !((x+(~0x30+1))>>31)&((x+(~0x3a+1))>>31);
}
```

右移 31 位，获取了 int类型的符号位



![image-20211125173723057](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211125173723057.png)

### 第七题

```
题目：

目标：实现三目运算符 same as x ? y : z
限制：只能使用操作符！~ & ^ | + <<>>
最大操作次数：16
难度：3

/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  return 2;
}
```

三目运算符 `? :  `

也就是 

```
x > 0 return y 
else return z



```

我们将题分解的话，我们就能够进行列出式子

![image-20211125180650151](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211125180650151.png)

当x为1 则值为y

当x为0 则值为z

也就是说，我们的二进制中，也需要一个0 1 的参数

也就是说`00000000000000000`  和 `0xff ff ff ff`

```
int conditional(int x, int y, int z) {
  int flag = !!x; //flag =1 or flag = 0
  int negative = ~0x1 + 1;//0xffffffff
  return (~(flag + negative) & y) | ((flag + negative) & z);
}
```

我们可以当flag=1 和flag=0的时候，进行计算，得出结论

![image-20211125182452255](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211125182452255.png)

### 第八题

```
题目：
也就是实现x<=y

目标：如果 x<=y ，则返回 1， 否则返回 0.
限制：只能使用操作符！~ & ^ | + <<>>
最大操作次数：24
难度：3

/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  return 2;
}

tips：
若二者异号，直接判断 x<0,返回1,x>0，返回0
y-x>=0判断符号位
若2者异号，相减会出现相加情况(相当于两个正数或者两个负数相加)，可能出现溢出，不适合通过结果的符号位判断
故若二者异号，直接判断 x<0,返回1,x>0，返回0
```

````
int isLessOrEqual(int x, int y) {
   
   int isSameSign= !( ((x^y)>>31) & 0x1 );//&0x1将不需要的位置0，以免影响结果
   //如上 是确认 他两个相同的符号
   //如下，y+一个x的负数 右移31 接着判断符号位，也就是异号的情况下
   int e= (( y+(~x+1) ) >> 31) & 0x1;
  return ( isSameSign & !e ) | ( !isSameSign & ( (x>>31)&0x1) );
}
````

![image-20211201124213141](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201124213141.png)





### 第九题

题目：使用位级运算求逻辑非 !

```
! :代表逻辑取反，即：把非0的数值变为0，0变为1;

题目：

目标：实现！操作
例子： logicalNeg (3) = 0, logicalNeg (0) = 1
限制：只能使用操作符～& ^ | + <<>>
最大操作次数：12
难度：4
解法：

//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  return 2;
}
```



0的存储  正0 就是000000 负0 就是 取反+1 也就是0xfffffff+1 =0

`~x+1>>31` 的情况 x为正数的时候-1 x为0和负数的时候全为0

`x |(~x+1>>31)` 如果为-1 则表示x！=0 为 0 则表示x=0

`>>`31变成全0或全1，+1后，0变1,全1溢出为0，返回

```
(((~x+1)|x)>>31) +1
```

![image-20211201140127197](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20211201140127197.png)

### 第十题



### 第十一题



### 第十二题



### 第十三题





