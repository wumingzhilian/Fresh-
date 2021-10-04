#pragma once
#ifndef __STL_ITERATOR_BASE_H__
#define __STL_ITERATOR_BASE_H__
#include<stddef.h>
#include "stl_config.h"

__STL_BEGIN_NAMESPACE
//使用以下五个标签进行优化，目的是为了让编译器在执行时就确定使用那种方法
//https:\//cloud.tencent.com/developer/section/1011037
//https :\//www.cnblogs.com/xiaoshiwang/p/11937275.html
//https :\//zhuanlan.zhihu.com/p/51999289


struct input_iterator_tag {

};
struct output_iterator_tag {

};
//public继承，因为forward就必须完全支持input的所有功能更对吧，

struct forward_iterator_tag :public input_iterator_tag {

};
struct bidirectional_iterator_tag : public forward_iterator_tag {

};
struct random_access_iterator_tag :public bidirectional_iterator_tag {

};




/*
这里他想知道value_type 值的类型对吧

diffrenecetype 是两个iterator之间距离用什么表示

iterator萃取要要萃取出算法的提问，

一共五种

reference pointer 这两个没有使用

iterator_category()

Difference_type

Value_type

必须要提出五种


*/
// 为什么是五种，以及，到底几个，见侯捷stl，array，vector 等iterator 不是类传进来，而是指针，所以我们需要写偏特化，指针的偏特化

/*
紧接着定义了一个类模板 iterator 。该类模板有五个模板形参, _Category 是上面定义的五种迭代器标记之一，用来标识迭代器的类型。_Tp 表示迭代器指向的元素。 。两个迭代器之间可以计算其差值，_Distance 表示用什么类型的数据存储其差值。_Distance 的缺省实参为 ptrdiff_t ，其中 ptrdifff_t 是 signed long 的类型别名。迭代器可以通常都可以索引某个元素， _Pointer 表示可以指向这种类型的元素的指针，_Reference 表示对这种类型的元素的引用。

*/

template<class _Tp, class _Distance>
struct input_iterator
{
	typedef input_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Tp *pointer;
	typedef _Tp &reference;
};

struct output_iterator
{
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;
};

template<class _Tp, class _Distance>
struct forward_iterator
{
	typedef forward_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Tp *pointer;
	typedef _Tp &reference;
};

template<class _Tp, class _Distance>
struct bidirectional_iterator {
	typedef bidirectional_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Tp *pointer;
	typedef _Tp &reference;
};

template<class _Tp, class _Distance>
struct random_access_iterator {
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Tp *pointer;
	typedef _Tp &reference;
};


template<class _Category, class _Tp, class _Distance = ptrdiff_t, class _Pointer = _Tp * , class _Reference = _Tp & >
struct iterator {
	typedef _Category iterator_category;
	typedef _Tp value_type;
	typedef _Distance difference_type;
	typedef _Pointer pointer;
	typedef _Reference reference;
};

template<class _Iterator>
struct iterator_traits {
	typedef typename _Iterator::iterator_categroy iterator_category;
	typedef typename _Iterator::value_type value_type;
	typedef typename _Iterator::difference_type difference_type;
	typedef typename _Iterator::pointer pointer;
	typedef typename _Iterator::reference reference;
};
//对于指针类型的偏特化
template<class _Tp>
struct iterator_traits<_Tp *> {
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef ptrdiff_t difference_type;
	typedef _Tp *pointer;
	typedef _Tp &reference;
};

template<class _Tp>
struct iterator_traits<const _Tp *> {
	typedef random_access_iterator_tag iterator_category;
	typedef _Tp value_type;
	typedef ptrdiff_t difference_type;
	typedef const _Tp *pointer;
	typedef const _Tp &reference;
};

//既然收到普通指针，指针没法回答，直接value-type就是T 或者const T
//只解释一个，这个是萃取机的回答，传来一个迭代器，萃取出类型（例如：random_access_iterator_tag），然后直接定义了一个_iterator_category函数，供下面使用，最后输出他的类型
template<class _Iter>
//const _Iter &和const _Iter & __i 一样，只不过一个是占位符，一个没
inline typename iterator_traits<_Iter>::iterator_category __iterator_category(const _Iter &) {
	typedef typename iterator_traits<_Iter>::iterator_category _Category;
	return _Category();
	//或者可以省略为：return typename iterator_traits<_Iter>::iterator_category()
}
//下面这个应该是供外部使用，
template<class _Iter>
inline typename iterator_traits<_Iter>::iterator_category iterator_category(const _Iter &__i) {
	return __iterator_category(__i);
}


//static_cast 静态转换符 类型转换使用 static_cast <type-id> ( expression )
//https:\//e8859487.pixnet.net/blog/post/402001658-%5Bc%2B%2B%5D-static_cast-%E7%94%A8%E6%B3%95%E8%AA%AA%E6%98%8E---%28%E5%9F%BA%E7%A4%8E%E7%AF%87%29
//https:\//docs.microsoft.com/zh-cn/cpp/cpp/static-cast-operator?view=msvc-160
//https:\//bytes.com/topic/c/answers/475446-when-use-null-when-use-static_cast-some_pointer_type-0-a
/*
这里使用static_cast<pointer>(0) 表示null空指针，因为在 C++ 中，常量 0 总是可以自由转换为任何
指针类型
*/
template<class _Iter>
inline typename iterator_traits<_Iter>::difference_type *__distance_type(const _Iter &) {
	return static_cast<typename iterator_traits<_Iter>::difference_type *>(0);
}

template<class _Iter>
inline typename iterator_traits<_Iter>::difference_type *distance_type(const _Iter &__i) {
	return __distance_type(__i);
}
//value_type 同理
template<class _Iter>
inline typename iterator_traits<_Iter>::value_type *__value_type(const _Iter &) {
	return static_cast<typename iterator_traits<_Iter>::value_type *>(0);
}

template<class _Iter>
inline typename iterator_traits<_Iter>::value_type *value_type(const _Iter &__i) {
	return __value_type(__i);
}
//接下来搞宏定义，下面的不能被上面的代码调用
#define __ITERATOR_CATEGORY(__i) __iterator_category(__i)
#define __DISTANCE_TYPE(__i)     __distance_type(__i)
#define __VALUE_TYPE(__i)        __value_type(__i)


//advance()迭代器就是将迭代器it，移动n位。如果it是随机访问迭代器，那么函数进行1次运算符计算操作，否则函数将对迭代器进行n次迭代计算操作。
//distance()可处理迭代器之间的距离
//iter_swap()可交换两个迭代器所指内容（表示所指元素的内容）
//https:\//blog.csdn.net/xuanyin235/article/details/78526919
/*
函数模板 __distance 用来计算两个迭代器之间的差值，模板形参 _InputIterator 表示某种类型的迭代器，在该类型的迭代器中应该要对 operator++ 进行重载 (具体在定义迭代器时会对一些运算符函数进行重载，后续对容器进行介绍时，一并进行介绍)。

while 循环中对 __first 迭代器应用后置自增操作，使得迭代器 __first 朝着迭代器 __last 移动。直到 __first 和 __last 相等。自增运算作用的次数就是两个迭代器的差值。

第三个函数形参限定了 _InputIterator 为 input_iterator 类型的迭代器 。即 _InputIterator 中的 iterator_category 为 input_iterator_tag (或者为 input_iterator 的派生类)


*/
/*看到如下可能会觉得意外，这里是一个GUN2.9 旧版（目测），因为它可以自己控制n，然后下面的偏特化有不需要控制n的
特别全的博客
http:\//wujiantao.github.io/2013/11/07/iterator_base.html
*/
template<class _InputIterator, class _Distance>
inline void __distance(_InputIterator __first, _InputIterator __last, _Distance &__n, input_iterator_tag) {
	while (__first != __last) {
		++__first;
		++__n;
	}
}
//如下都相当于是偏特化了,随机存储，直接相减即可
template<class _RandomAccessIterator, class _Distance>
inline void __distance(_RandomAccessIterator __first, _RandomAccessIterator __last, _Distance &__n,random_access_iterator_tag) {
	__n += __last - __first;
}

//这个是外界调用的函数，函数 distance 根据迭代器的类型来决定采用上面定义的哪种 __distance 来计算两个迭代器的差值。
//__STL_REQUIRES 是stl种用来座模板参数约束的宏实现过于复杂，这里不做学习
template<class _InputIterator, class _Distance>
inline void distance(_InputIterator __first, _InputIterator __last, _Distance &__n) {
	__distance(__first, __last, __n, iterator_category(__first));
}
//如果編譯器支持 partial specialization of class templates（模板類的部分特化），
//       就定義 __STL_CLASS_PARTIAL_SPECIALIZATION.
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
template<class _InputIterator>
inline typename iterator_traits<_InputIterator>::difference_type __distance(_InputIterator __first, _InputIterator __last, input_iterator_tag) {
	typename iterator_traits<_InputIterator>::difference_type __n = 0;
	while (__first != __last) {
		++__first;
		++__n;
	}
	return __n;
}

template<class _RandomAccessIterator>
inline typename iterator_traits<_RandomAccessIterator>::difference_type __distance(_RandomAccessIterator __first, _RandomAccessIterator __last, random_access_iterator_tag) {
	return __last - __first;
}

template<class _InputIterator>
inline typename iterator_traits<_InputIterator>::difference_type distance(_InputIterator __first, _InputIterator __last) {
	typedef typename iterator_traits<_InputIterator>::iterator_category _Category;
	return __distance(__first, __last, _Category());
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
//advance同上，这里是跳转，需要写距离，input迭代器，只需要向后
template<class _InputIterator, class _Distance>
inline void __advance(_InputIterator &__i, _Distance __n, input_iterator_tag) {
	while (__n--) {
		++__i;
	}
}

//双向迭代器，需要考虑正负
template<class _BidirectionalIterator, class _Distance>
inline void __advance(_BidirectionalIterator &__i, _Distance __n, bidirectional_iterator_tag) {
	if (__n > 0) {
		while (__n--) {
			++__i;
		}
	}
	else {
		while (__n++) {
			--__i;
		}
	}
}

//随机存储，（调用了+=的重载）
template<class _RandomAccessIterator, class _Distance>
inline void __advance(_RandomAccessIterator &__i, _Distance __n, random_access_iterator_tag) {
	__i += __n;
}
//外部调用的advance
template<class _InputAccessIterator, class _Distance>
inline void advance(_InputAccessIterator &__i, _Distance __n) {
	__advance(__i, __n, __iterator_category(__i));
}



__STL_END_NAMESPACE
#endif // !__STL_ITERATOR_BASE_H__
