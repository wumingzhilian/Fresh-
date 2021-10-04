#pragma once
#ifndef __STL_CONSTRUCT_H__
#define __STL_CONSTRUCT_H__

#include<new>
#include "stl_config.h"
#include "type_traits.h"
#include "stl_iterator_base.h"

__STL_BEGIN_NAMESPACE
//https:\//blog.csdn.net/u014645632/article/details/78795466

//����placement new ��new�����ڴ�ռ�ĵ����ַ�ʽ��new��һ�����������Խ�������
/*�����ü�����
	   https:\//www.cnblogs.com/igloo1986/archive/2013/02/01/2888775.html
	*/

template<class _T1,class _T2>
inline void _Construct(_T1 *__p, const _T2 &__value) {
	//���캯������*pָ���λ�ã����ٿռ䣬�ڶ����ǲ���
	new((void *)__p) _T1(__value);//������p���ڴ��ϳ�ʼ����ʹ����T1�Ĺ��캯����placement new �൱��T1::T1��value��

}
template<class _T1>//������ֻ��һ�������Ĺ���
inline void _Construct(_T1 *__p) {
	new((void *) __p) _T1();
}
//��������
//https://\www.cnblogs.com/zsq1993/p/5838034.html
//��ʾ��������������ֻ��ִ�����������е����ݣ������ͷ�ջ�ڴ棬Ҳ����ݻٶ��󣨵�Ȼ������д�ˣ�
//��ʽ��������������û���ṩ����������ʱ�򣬱�������ʼ������һ����
//https://\www.apiref.com/cpp-zh/cpp/language/destructor.html
//https://\www.cnblogs.com/zsq1993/p/5838034.html
template<class _Tp>
inline void _Destory(_Tp *__pointer) {
	destroy_one(__pointer, typename __type_traits<_Tp>::has_trivial_destructor());
	//�ڶ�����������ȡ��ȥ����ȡ�������������ʲô���в���Ҫ�Ĺ��캯���أ�
	//�ش���/û��,trivial destructor ������ʽ����������true��Ȼʲô�¶���û�У������false trivial destructor��ô�ͻ�������������ʾ��������
	//ʹ��typename ������ȷ������һ�����ͣ�������һ��������
}
template<class _Tp>
void destroy_one(_Tp * , __true_type){

}
template<class _Tp>
void destroy_one(_Tp *__pointer, __false_type) {
	if (__pointer != nullptr) {
		__pointer->~_Tp();//��ʾ������������
	}
	//��ָ�����͹ؼ��� nullptr
}
//aux��������
template<class _ForwardIterator>
inline void _destory_aux(_ForwardIterator __first, _ForwardIterator __last, __false_type) {
	for (; __first != __last; ++__first) {
		_Destory(&*__first);//*һ�����������ͱ�ʾ������ָ���Ԫ�أ�������ֻ������һ��ָ����ѣ�&���ã��β�ʵ��һ��䣨�����������������������
	}
}
//list��������һ��һ�������߶԰ɣ�Ҳ����ǰ�������
//����������ǰ�������������ʶ���������������ȡ��������˫������������������������һ���������
//https:|//blog.csdn.net/nestler/article/details/25806567
template<class _ForwardIterator>
inline void __destory_aux(_ForwardIterator, _ForwardIterator, __true_type) {
	//�������ϣ�����ȷʵ��д�βΣ�

}


template<class _ForwardIterator,class _Tp>
inline void __destory(_ForwardIterator __first,_ForwardIterator __last,_Tp *) {
	typedef typename __type_traits<_Tp>::has_trivial_destructor _Trivial_destructor;//����һ��������
	__destory_aux(__first, __last, _Trivial_destructor());
	//����ֱ�Ӽ����ţ��൱����ʱ����û����
}

template<class _ForwardIterator>
inline void _Destory(_ForwardIterator __first, _ForwardIterator __last) {
	__destory(__first, __last, __VALUE_TYPE(__first));
	//typedef void value_type; in stl_iterator.h
}

/*

	    ���Ƿ����ĺ���ģ�壬����ǻ������͵�ָ�룬��Ȼ�޷�������������������˵�л�������ָ����ػ�����ģ�壬��ʵ����Դ�����Ƿ�ģ�庯�������ڷ�ģ�庯�����ȼ�����ģ�庯������������int*��ָ�����������÷�ģ�庯�������Ҳ����κβ�����������ʾ��
*/
inline void _Destroy(char*, char*) {}
inline void _Destroy(int*, int*) {}
inline void _Destroy(long*, long*) {}
inline void _Destroy(float*, float*) {}
inline void _Destroy(double*, double*) {}

//�������ǹ���
template<class _T1,class _T2>
inline void construct(_T1 *__p,const _T2 __value) {
	_Construct(__p, __value);
}
template<class _T1>
inline void construct(_T1 *_p) {
	_Construct(_p);
}

//������������
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

