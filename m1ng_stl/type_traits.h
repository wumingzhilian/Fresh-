#pragma once
#ifndef __STL_TYPE_TRAITS_H__
#define __STL_TYPE_TRAITS_H__
//两个true，flase结构体，作为是否重要的参数，方便萃取的时候使用

struct __true_type {

};
struct __false_type {

};
//模板类，萃取作为回答
template<class _Tp>
struct __type_traits {
	typedef __true_type this_dummy_member_must_be_first;
	/* Do not remove this member. It informs a compiler which automatically specializes __type_traits that this __type_traits template is special. It just makes sure that things work if an implementation is using a template called __type_traits for something unrelated. */

	/*
不要移除该成员。它通知自动专一__type_traits的编译器这个__type_traits模板是特殊的。它只是确保如果一个实现使用了一个名为__type_traits的模板来处理一些不相关的事情，那么事情能够正常工作。*/
/*
为了让编译器自动生成该类的特定类型专门化，应该遵守以下限制:

-如果你愿意，你可以重新排列下面的成员

-如果您愿意，您可以删除以下任何成员

—不能在没有创建相应成员的情况下重命名成员

编译器中的名称更改

-你添加的成员将像普通成员一样对待，除非

在编译器中添加适当的支持。
*/
	//默认构造器 不重要吗，  不是，所以是重要的，
	typedef __false_type has_trivial_default_constructor;
	typedef __false_type has_trivial_copy_constructor;
	typedef __false_type has_trivial_assignment_operator;
	typedef __false_type has_trivial_destructor;
	typedef __false_type is_POD_type;
};

//以下都是一些偏特化版本/特化版本，定义
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

//一个模板函数
template<class _Tp> 
struct _Is_integer {
	typedef __false_type _Integral;
};
//剩余是他的特化和偏特化
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

