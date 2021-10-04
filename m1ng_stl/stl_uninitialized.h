
#ifndef __STL_UNINITIALIZED_H__
#define __STL_UNINITIALIZED_H__


#include "stl_config.h"
#include "stl_iterator_base.h"
#include "type_traits.h"
#include "stl_construct.h"
#include "stl_algobase.h"
//���ﳭ���Ը��
/*
															�ػ�
															____>___uninitialized_copy_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_copy() -----> ����_>__uninitialized_copy -- is POD ��
															|
															|YES _true_type --> copy
															|
															|_�ػ�_>__uninitialized_copy_aux(,_true_type)
 �����POD type�����ǻ�����������(�����������ͻ���ͨ�ṹ����(û��ָ��ĳ�Ա����)��)��ô��ֱ�ӿ�������
 �������POD type ����Ҫ���ε��ù��캯����������
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
//һЩ�����������Ϳ�������ڴ��ȫ�ֺ����� �������/���������ڴ�����/�ͷ��Ƿ���ʵ�ֵ�
template<class _InputIter, class _ForwardIter>
inline _ForwardIter uninitialized_copy(_InputIter __first, _InputIter __last, _ForwardIter __result) {
	return __uninitialized_copy(__first, __last, __result, __VALUE_TYPE(__result));
}


/*
�ȿ���������������:

1.fill��ֱ�Ӷ�ÿ��Ԫ�ؽ������value.

2.uninitialized_fill(δ��ʼ�����) ��Ҫ����value���������ж�ʹ����һ�ַ�ʽ���,

�����POD����(�����������ͻ���ͨ�ṹ����(û��ָ��ĳ�Ա����)),��ֱ�ӵ���fill����.

����POD����ʱ,��Ҫ����ÿ��Ԫ�ؽ��й���(����construct����).Ϊʲô��?

������Ϊ*first = value; ��ĸ�ֵ����������Ҫ*first�Ķ����Ѿ�����.

uninitialized_fillһ�㶼������δ�������,Ҳ����˵�ڴ��������û�ж������.

������صĻ�������: uninitialized_fill_n, uninitialized_copy.ԭ��һ��
*/
/*
															�ػ�
															____>__uninitialized_fill_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_fill() -----> ����_>__uninitialized_fill -- is POD ��
															|
															|YES _true_type --> fill
															|
															|_�ػ�_>__uninitialized_fill_aux(,_true_type)
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
		//����Ĺ��죬��stl_construct ��д
		for (; __cur != __last; ++__cur) {
			construct(&*__cur, __x);
		}
	}
	catch (...) {
		destory(__first, __cur);
	}
}


//��POD ���Կ���ֱ�ӽ���fill
template <class _ForwardIter, class _Tp>
inline void
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last,
	const _Tp& __x, __true_type)
{
	fill(__first, __last, __x);
}
//������һ���ģ�ֻ�����ǳ�������n������uniniyialized_fill_n()��������˵Ŀ�ʼ���ĵ�����(�˷�����ڴ���δ�����)�ͳ�ʼ���ռ�Ĵ�С������Ҫ���������ָ����Χ�ڵ�δ������ڴ�ָ����ͬ�Ķ���

/*
															�ػ�
															____>__uninitialized_fill_n_aux(.,_false_type)
															|
															|NO _false_type --> _Construct
															|
uninitialized_fill_n() -----> ����_>__uninitialized_fill_n -- is POD ��
															|
															|YES _true_type --> fill
															|
															|_�ػ�_>__uninitialized_fill_n_aux(,_true_type)
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
