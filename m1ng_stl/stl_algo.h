#pragma once
//����������㷨�Ļ����⣬����algo��algobase
#ifndef __STL_ALGO_H__
#define __STL_ALGO_H__

#include "stl_config.h"
__STL_BEGIN_NAMESPACE
//lower_bound() ����������ָ�������ڲ��Ҳ�С��Ŀ��ֵ�ĵ�һ��Ԫ�ء�
//����ֻдһ��lowerbound����
// Binary search (lower_bound, upper_bound, equal_range, binary_search).
//����ָ��Χ [first, last) ���׸���С�ڣ������ڻ���ڣ� value ��Ԫ�صĵ������������Ҳ�������Ԫ���򷵻� last ��
template<class _ForwardIter,class _Tp,class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last, const _Tp & __val, _Distance *) {
	_Distance __len = 0;
	distance(__first, __last, __len);//�������ָ��֮��ľ���
	_Distance __half;
	_ForwardIter __middle;
	while (__len > 0) {
		__half = __len >> 1;//λ�����������һλ���൱�ڳ�2�����ҵ�һ���λ��
		__middle = __first;
		advance(__middle, __half);//ǰ�����ˣ��ѵ�ǰ����תһ��ľ��룬Ȼ����бȽϡ�
		if (*__middle < __val) {//�൱��һ��С���ַ���
			++__first;
			__len = __len - __half - 1;
		}
		else {
			__len = __half;
		}

	}
	return __first;
}
	//��װ�õĽӿڵ��ã����뿪ʼ��β������Ҫ�Աȵ�ֵ
	template<class _ForwardIter, class _Tp>
	inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,const _Tp &__val) {
		return __lower_bound(__first, __last, __val,
			__DISTANCE_TYPE(__first));
	}

//���������Դ�cmp������lower��bound
	template<class _ForwardIter, class _Tp, class _Compare, class _Distance>
	_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,const _Tp &__val, _Compare __comp, _Distance *) {
		_Distance __len = 0;
		distance(__first, __last, __len);//�����len�൱�ڻ�������ľ���
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


