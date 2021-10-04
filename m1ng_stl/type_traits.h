#pragma once
#ifndef __STL_TYPE_TRAITS_H__
#define __STL_TYPE_TRAITS_H__
//����true��flase�ṹ�壬��Ϊ�Ƿ���Ҫ�Ĳ�����������ȡ��ʱ��ʹ��

struct __true_type {

};
struct __false_type {

};
//ģ���࣬��ȡ��Ϊ�ش�
template<class _Tp>
struct __type_traits {
	typedef __true_type this_dummy_member_must_be_first;
	/* Do not remove this member. It informs a compiler which automatically specializes __type_traits that this __type_traits template is special. It just makes sure that things work if an implementation is using a template called __type_traits for something unrelated. */

	/*
��Ҫ�Ƴ��ó�Ա����֪ͨ�Զ�רһ__type_traits�ı��������__type_traitsģ��������ġ���ֻ��ȷ�����һ��ʵ��ʹ����һ����Ϊ__type_traits��ģ��������һЩ����ص����飬��ô�����ܹ�����������*/
/*
Ϊ���ñ������Զ����ɸ�����ض�����ר�Ż���Ӧ��������������:

-�����Ը�⣬�����������������ĳ�Ա

-�����Ը�⣬������ɾ�������κγ�Ա

��������û�д�����Ӧ��Ա���������������Ա

�������е����Ƹ���

-����ӵĳ�Ա������ͨ��Աһ���Դ�������

�ڱ�����������ʵ���֧�֡�
*/
	//Ĭ�Ϲ����� ����Ҫ��  ���ǣ���������Ҫ�ģ�
	typedef __false_type has_trivial_default_constructor;
	typedef __false_type has_trivial_copy_constructor;
	typedef __false_type has_trivial_assignment_operator;
	typedef __false_type has_trivial_destructor;
	typedef __false_type is_POD_type;
};

//���¶���һЩƫ�ػ��汾/�ػ��汾������
template<> struct __type_traits<bool> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};


template<> struct __type_traits<char> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<signed char> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<unsigned char> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<short> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<unsigned short> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<int> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<unsigned int> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<long> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<unsigned long> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<long long> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<unsigned long long> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<float> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<double> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

template<> struct __type_traits<long double> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};

//һ��ģ�庯��
template<class _Tp> 
struct _Is_integer {
	typedef __false_type _Integral;
};
//ʣ���������ػ���ƫ�ػ�
template<>
struct _Is_integer<int> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<unsigned int> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<bool> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<char> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<signed char> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<unsigned char> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<short> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<unsigned short> {
	typedef __true_type _Integral;
};

template<>
struct _Is_integer<long> {
	typedef __true_type _Integral;
};


template<>
struct _Is_integer<unsigned long> {
	typedef __true_type _Integral;
};
#endif // !__STL_TYPE_TRAITS_H__

