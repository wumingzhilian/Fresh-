#pragma once
//这个函数是算法的基础库，包括algo，algobase
#ifndef __STL_ALGO_H__
#define __STL_ALGO_H__

#include "stl_config.h"
__STL_BEGIN_NAMESPACE
//lower_bound() 函数用于在指定区域内查找不小于目标值的第一个元素。
//这里只写一个lowerbound函数
// Binary search (lower_bound, upper_bound, equal_range, binary_search).
//返回指向范围 [first, last) 中首个不小于（即大于或等于） value 的元素的迭代器，或若找不到这种元素则返回 last 。
template<class _ForwardIter,class _Tp,class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last, const _Tp & __val, _Distance *) {
	_Distance __len = 0;
	distance(__first, __last, __len);//求出两个指向之间的距离
	_Distance __half;
	_ForwardIter __middle;
	while (__len > 0) {
		__half = __len >> 1;//位运算符，右移一位，相当于除2，先找到一半的位置
		__middle = __first;
		advance(__middle, __half);//前进后退，把当前的跳转一半的距离，然后进行比较。
		if (*__middle < __val) {//相当于一个小二分法。
			++__first;
			__len = __len - __half - 1;
		}
		else {
			__len = __half;
		}

	}
	return __first;
}
	//封装好的接口调用，输入开始结尾，和想要对比的值
	template<class _ForwardIter, class _Tp>
	inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,const _Tp &__val) {
		return __lower_bound(__first, __last, __val,
			__DISTANCE_TYPE(__first));
	}

//接下来是自带cmp函数的lower—bound
	template<class _ForwardIter, class _Tp, class _Compare, class _Distance>
	_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,const _Tp &__val, _Compare __comp, _Distance *) {
		_Distance __len = 0;
		distance(__first, __last, __len);//这里的len相当于会求出他的距离
		_Distance __half;
		_ForwardIter __middle;

		while (__len > 0) {
			__half = __len >> 1;
			__middle = __first;
			advance(__middle, __half);
			if (__comp(*__middle, __val)) {
				__first = __middle;
				++__first;
				__len = __len - __half - 1;
			}
			else
				__len = __half;
		}
		return __first;
	}

	template<class _ForwardIter, class _Tp, class _Compare>
	inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,const _Tp &__val, _Compare __comp) {
		return __lower_bound(__first, __last, __val, __comp,__DISTANCE_TYPE(__first));
	}

__STL_END_NAMESPACE
#endif // !__STL_ALGO_H__


