#pragma once
#ifndef __STL_ALLOC_H__
#define __STL_ALLOC_H__

#include<cstdio>
#include<cstdlib>
#include <cstddef>

#include "stl_config.h"
#include "stl_construct.h"

/*
stdlib.h是C标准函数库的头文件，声明了数值与字符串转换函数, 伪随机数生成函数, 动态内存分配函数, 进程控制函数等公共函数。 C++程序应调用等价的cstdlib头文件.
常用的函数如malloc()、calloc()、realloc()、free()、system()、atoi()、atol()、rand()、srand()、exit()等等。 具体的内容可以打开编译器的include目录里面的stdlib.h头文件查看。
*/
__STL_BEGIN_NAMESPACE


//第一级配置器，直接使用malloc realloc free实现功能
template<int _inst>
class __malloc_alloc_template {
public:
	static void *allocate(size_t __n) {//unsignted int == size_t
		void *__result = malloc(__n);//(void * 可以表示任意指针)
		if (0 == __result) {
			//C 库函数 int fprintf(FILE *stream, const char *format, ...) 发送格式化输出到流 stream 中。
			fprintf(stderr, "out of memory\n");
			//这里相当于把out xxx 写入了stderr
			//stderr 标准错误
			/*标准错误流
				标准错误流是错误消息和其他诊断警告的默认目标。与stdout一样，它通常也默认定向到文本控制台（通常在屏幕上）。
					stderr可以用作任何函数的参数，该函数采用FILE*类型的参数，期望输出流，如fputs或fprintf。
			*/
			exit(1);
		}
		return __result;
	}
	static void deallocate(void *__p,size_t /*__n*/) {
		free(__p);
	}
	//C++ 中的 realloc() 函数重新分配先前分配但尚未释放的内存块。
	static void *reallocate(void *__p, size_t /* old_size*/,size_t __new_sz) {
		void *__result = realloc(__p, __new_sz);//直接使用了realloc，
		if (0 == __result) {//没分配上
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		return __result;
	}
};
//类结束，成员函数开始
//这里使用typedef一下，方便后续使用，同时，inst在如下没有起到作用，这里设置为0
typedef __malloc_alloc_template<0> malloc_alloc;

//https:\//github.com/jasonblog/note/blob/master/c%2B%2B/c%2B%2B_stl_xue_xi_zong_7d5028_quan_976229.md
//包装，相当于适配器，simple_alloc使得alloc具备标准接口
//第二个参数是让你进行选择使用哪个分配器
//在C++中，用户所需空间可能是任意类型的，有单个对象空间，有连续空间，每次让用户自己计算所需空间总大小不是很友好，因此SGI-STL将空间配置器重新再封装了一层：
//https:\//cloud.tencent.com/developer/article/1688439
template<class _Tp, class _Alloc>
class simple_alloc {
public:
	static _Tp *alloc(size_t __n) {
		//// 申请n个T类型对象大小的空间
		return 0 == __n ? 0 : (_Tp*)_Alloc::allocate(__n * sizeof(_Tp));
		//这里只解释一个，相当于if判断对吧，判断，size是0么，是的话，返回0不是就分配n个类型大小
	}
	//这儿因为allocate里面写成void了，出bug了gan
	static _Tp *allocate(size_t __n) {
		//申请一个T类型对象大小的空间
		return (_Tp *)_Alloc::allocate(sizeof(_Tp));
	}
	// 释放一个T类型对象大小的空间
	static void deallocate(_Tp *__p) {
		_Alloc::deallocate(__p, sizeof(_Tp));
	}

	//释放n个T类型对象大小的空间
	static void deallocate(_Tp *__p, size_t __n) {
		if (0 != __n) {
			_Alloc::deallocate(__p, __n * sizeof(_Tp));
		}
	}
};

//将alloc定义为一级分配器
typedef malloc_alloc alloc;

__STL_END_NAMESPACE
#endif // !__STL_ALLOC_H__
