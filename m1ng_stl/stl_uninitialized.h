
#ifndef __STL_UNINITIALIZED_H__
#define __STL_UNINITIALIZED_H__


#include "stl_config.h"
#include "stl_iterator_base.h"
#include "type_traits.h"
#include "stl_construct.h"
#include "stl_algobase.h"
//这里抄的淘哥的
/*
															特化
															____>___uninitialized_copy_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_copy() -----> 泛化_>__uninitialized_copy -- is POD ？
															|
															|YES _true_type --> copy
															|
															|_特化_>__uninitialized_copy_aux(,_true_type)
 如果是POD type，就是基本数据类型(就是内置类型或普通结构与类(没有指针的成员数据)等)那么就直接拷贝即可
 如果不是POD type 就需要依次调用构造函数创建数据
 */
__STL_BEGIN_NAMESPACE
template <class _InputIter, class _ForwardIter>
inline _ForwardIter __uninitialized_copy_aux(_InputIter __first, _InputIter __last, _ForwardIter __result, __false_type) {
	_ForwardIter __cur = __result;
	try {
		for (; __first != __last; ++__first, ++__cur) {
			_Construct(&*__cur, *__first);
		}
		return __cur;
	}
	catch (...) { _Destroy(__result, __cur); throw; }
}
template <class _InputIter, class _ForwardIter>
inline _ForwardIter __uninitialized_copy_aux(_InputIter __first, _InputIter __last, _ForwardIter __result, __true_type) {
	return copy(__first, __last, __result);

}
template<class _InputIter, class _ForwardIter, class _Tp>
inline _ForwardIter __uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result, _Tp *) {
	typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
	return __uninitialized_copy_aux(__first, __last, __result, _Is_POD());
}
//一些用于用于填充和拷贝大块内存的全局函数。 对象构造和/析构，与内存配置/释放是分离实现的
template<class _InputIter, class _ForwardIter>
inline _ForwardIter uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result) {
	return __uninitialized_copy(__first, __last, __result, __VALUE_TYPE(__result));
}


/*
先看看两个函数功能:

1.fill是直接对每个元素进行填充value.

2.uninitialized_fill(未初始化填充) 是要根据value的类型来判断使用哪一种方式填充,

如果是POD类型(就是内置类型或普通结构与类(没有指针的成员数据)),就直接调用fill函数.

不是POD类型时,就要遍历每个元素进行构造(调用construct函数).为什么呢?

那是因为*first = value; 类的赋值操作符必须要*first的对象已经生成.

uninitialized_fill一般都是用于未初化填充,也就是说内存区间根本没有对象可言.

此外相关的还有两个: uninitialized_fill_n, uninitialized_copy.原理一样
*/
/*
															特化
															____>__uninitialized_fill_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_fill() -----> 泛化_>__uninitialized_fill -- is POD ？
															|
															|YES _true_type --> fill
															|
															|_特化_>__uninitialized_fill_aux(,_true_type)
*/
template <class _ForwardIter, class _Tp>
inline void uninitialized_fill(_ForwardIter __first, _ForwardIter __last, const _Tp& __x) {
	__uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
}
template <class _ForwardIter, class _Tp, class _Tp1>
inline void __uninitialized_fill(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, _Tp1*) {
	typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
	return __uninitialized_fill_aux(__first, __last, __x, _Is_POD());
}
template<class _ForwardIter, class _Tp>
inline void __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, const _Tp& __x, __false_type) {
	_ForwardIter __cur = __first;
	try {
		//这里的构造，在stl_construct 有写
		for (; __cur != __last; ++__cur) {
			construct(&*__cur, __x);
		}
	}
	catch (...) {
		destory(__first, __cur);
	}
}


//是POD 所以可以直接进行fill
template <class _ForwardIter, class _Tp>
inline void
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last,
	const _Tp& __x, __true_type)
{
	fill(__first, __last, __x);
}
//和上面一样的，只不过是成了填入n个，，uniniyialized_fill_n()接受输入端的开始处的迭代器(此分配的内存是未构造的)和初始化空间的大小，最重要的是它会给指定范围内的未构造的内存指定相同的对象。

/*
															特化
															____>__uninitialized_fill_n_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_fill_n() -----> 泛化_>__uninitialized_fill_n -- is POD ？
															|
															|YES _true_type --> fill
															|
															|_特化_>__uninitialized_fill_n_aux(,_true_type)
*/
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n, const _Tp& __x, __false_type) {
	_ForwardIter __cur = __first;
	try {
		for (; __n > 0; --__n, ++__cur) {
			construct(&*__cur, __x);
		}
		//note
		return __cur;
	}
	catch (...) {
		destroy(__first, __cur);
	}
}
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n, const _Tp& __x, __true_type) {
	return fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
inline _ForwardIter __uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x, _Tp1*) {
	typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
	return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x) {
	return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
}
__STL_END_NAMESPACE
#endif // !__STL_UNINITIALIZED_H__

