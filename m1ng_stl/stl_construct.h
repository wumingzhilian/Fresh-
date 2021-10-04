#pragma once
#ifndef __STL_CONSTRUCT_H__
#define __STL_CONSTRUCT_H__

#include<new>
#include "stl_config.h"
#include "type_traits.h"
#include "stl_iterator_base.h"

__STL_BEGIN_NAMESPACE
//https:\//blog.csdn.net/u014645632/article/details/78795466

//关于placement new 是new开辟内存空间的第四种方式，new算一个函数，可以进行重载
/*不懂得见这里
	   https:\//www.cnblogs.com/igloo1986/archive/2013/02/01/2888775.html
	*/

template<class _T1,class _T2>
inline void _Construct(_T1 *__p, const _T2 &__value) {
	//构造函数，在*p指针的位置，开辟空间，第二个是参数
	new((void *)__p) _T1(__value);//重新在p的内存上初始化，使用它T1的构造函数，placement new 相当于T1::T1（value）

}
template<class _T1>//这里是只有一个参数的构造
inline void _Construct(_T1 *__p) {
	new((void *) __p) _T1();
}
//析构函数
//https://\www.cnblogs.com/zsq1993/p/5838034.html
//显示调用析构函数，只会执行析构函数中的内容，不会释放栈内存，也不会摧毁对象（当然除非你写了）
//隐式调用析构函数，没有提供析构函数的时候，编译器会始终声明一个。
//https://\www.apiref.com/cpp-zh/cpp/language/destructor.html
//https://\www.cnblogs.com/zsq1993/p/5838034.html
template<class _Tp>
inline void _Destory(_Tp *__pointer) {
	destroy_one(__pointer, typename __type_traits<_Tp>::has_trivial_destructor());
	//第二个参数是萃取，去问萃取机，你这个答案是什么，有不重要的构造函数呢？
	//回答有/没有,trivial destructor 就是隐式析构函数，true当然什么事儿都没有，如果是false trivial destructor那么就会调用它本身的显示析构函数
	//使用typename 编译器确认他是一个类型，而不是一个变量。
}
template<class _Tp>
void destroy_one(_Tp * , __true_type){

}
template<class _Tp>
void destroy_one(_Tp *__pointer, __false_type) {
	if (__pointer != nullptr) {
		__pointer->~_Tp();//显示调用析构函数
	}
	//空指针类型关键字 nullptr
}
//aux辅助函数
template<class _ForwardIterator>
inline void _destory_aux(_ForwardIterator __first, _ForwardIterator __last, __false_type) {
	for (; __first != __last; ++__first) {
		_Destory(&*__first);//*一个迭代器，就表示迭代器指向的元素，迭代器只不过是一个指针而已，&引用，形参实参一起变（这里是逐个调用析构函数）
	}
}
//list迭代器，一个一个往下走对吧，也就是前向迭代器
//这里命名是前向迭代器，方便识别，其他还有随机存取迭代器，双向迭代器，逆向迭代器（他是一个配接器）
//https:|//blog.csdn.net/nestler/article/details/25806567
template<class _ForwardIterator>
inline void __destory_aux(_ForwardIterator, _ForwardIterator, __true_type) {
	//查阅资料，这里确实不写形参，

}


template<class _ForwardIterator,class _Tp>
inline void __destory(_ForwardIterator __first,_ForwardIterator __last,_Tp *) {
	typedef typename __type_traits<_Tp>::has_trivial_destructor _Trivial_destructor;//起了一个新名字
	__destory_aux(__first, __last, _Trivial_destructor());
	//类型直接加括号，相当于临时对象，没名称
}

template<class _ForwardIterator>
inline void _Destory(_ForwardIterator __first, _ForwardIterator __last) {
	__destory(__first, __last, __VALUE_TYPE(__first));
	//typedef void value_type; in stl_iterator.h
}

/*

	    这是泛化的函数模板，如果是基本类型的指针，显然无法调用析构函数，书上说有基本类型指针的特化函数模板，但实际上源码里是非模板函数，由于非模板函数优先级高于模板函数，遇到类似int*的指针编译器会调用非模板函数，并且不做任何操作，如下所示。
*/
inline void _Destroy(char*, char*) {}
inline void _Destroy(int*, int*) {}
inline void _Destroy(long*, long*) {}
inline void _Destroy(float*, float*) {}
inline void _Destroy(double*, double*) {}

//接下来是构造
template<class _T1,class _T2>
inline void construct(_T1 *__p,const _T2 __value) {
	_Construct(__p, __value);
}
template<class _T1>
inline void construct(_T1 *_p) {
	_Construct(_p);
}

//接下来是析构
template<class _Tp>
inline void destory(_Tp *__pointer) {
	_Destory(__pointer);
}

template<class _ForwardIterator>
inline void destory(_ForwardIterator __first, _ForwardIterator __last) {
	_Destory(__first, __last);
}
__STL_END_NAMESPACE
#endif // !_STL_CONSTRUCT_H_

