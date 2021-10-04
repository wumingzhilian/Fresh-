#pragma once

#ifndef __STL_ITERATOR_H__
#define __STL_ITERATOR_H__

#include "stl_config.h"
#include "stl_iterator_base.h"

__STL_BEGIN_NAMESPACE
//http:\//ibillxia.github.io/blog/2014/06/21/stl-source-insight-2-iterators-and-traits/
/*
在 stl_iterator.h 文件中，设计了 back_insert_iterator, front_insert_iterator, insert_iterator, reverse_bidirectional_iterator, reverse_iterator, istream_iterator, ostream_iterator, 等标准的迭代器，其中前3中都使用 output_iterator 的只写特性（只进行插入操作，只是插入的位置不同而已），而第4种使用的是 bidirectional_iterator 的双向访问特性，第5种使用的是 random_access_iterator 的随机访问特性。而最后两种标准迭代器分别是使用 input_iterator 和 output_iterator 特性的迭代器。从这几个标准的迭代器的定义中可以看出，主要是实现了 operator=, operator*, operator->, operator==, operator++, operator--, operator+, operator-, operator+=, operator-= 等指针操作的标准接口。根据定义的操作符的不同，就是不同类型的迭代器了。
*/
//https:\//blog.csdn.net/weixin_42709632/article/details/104860823?utm_medium=distribute.pc_relevant.none-task-blog-baidujs-1
//后插
template<class _Container>
class back_insert_iterator
{
protected:
	_Container * container;
public:
	typedef _Container container_type;
	typedef output_iterator_tag iterator_category;
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;
	/*
	explicit的主要用法就是放在单参数的构造函数中,防止隐式转换, 导致函数的入口参数, 出现歧义.

	如果可以使用A构造B, 未加explicit的构造函数, 当使用B进行参数处理时, 就可以使用A, 使得接口混乱.

	为了避免这种情况, 使用explicit避免隐式构造, 只能通过显示(explicit)构造.

	*/

	explicit back_insert_iterator(_Container &__x) : container(&__x) {}
	/*
	insert_iterator不支持–运算符重载，可以看出它是单向迭代器。另外ostream_iterator和insert迭代器一样，*、++等运算符都存在，也可以使用，但是都不对对象做任何操作，只是返回对象自身，这样做的原因就是为了迎合其他迭代器的口味，与迭代器这个大家庭的用法一致。方便阅读和修改。
	*/
	back_insert_iterator<_Container> &operator = (const typename _Container::value_type &__value) {
		container->push_back(__value);
		return *this;
	}
	back_insert_iterator<_Container> &operator*() { return *this; }

	back_insert_iterator<_Container> &operator++() { return *this; }

	back_insert_iterator<_Container> &operator++(int) { return *this; }
};
//前插
	template<class _Container>
	class front_insert_iterator {
	protected:
		_Container *container;
	public:
		typedef _Container container_type;
		typedef output_iterator_tag iterator_category;
		typedef void value_type;
		typedef void pointer;
		typedef void difference_type;
		typedef void reference;
		
		explicit front_insert_iterator(_Container &__x) : container(&__x){}
		front_insert_iterator<_Container> &operator= (const typename _Container::value_type &__value) {
			container->push_front(__value);
		}
		front_insert_iterator<_Container> &operator*() { return *this; }

		front_insert_iterator<_Container> &operator++() { return *this; }

		front_insert_iterator<_Container> &operator++(int) { return *this; }
	};
	template<class _Container>
	inline front_insert_iterator<_Container> front_inserter(_Container &__x) {
		return front_insert_iterator<_Container>(__x);
	}

	template<class _Container>
	class insert_iterator {
	protected:
		_Container *container;
		typename _Container::iterator iter;
	public:
		//常规五要素
		typedef _Container container_type;
		typedef output_iterator_tag iterator_category;
		typedef void value_type;
		typedef void difference_type;
		typedef void pointer;
		typedef void reference;

		insert_iterator(_Container &__x,typename _Container::iterator __i):container(&__x),iter(__i){}

		insert_iterator<_Container> &operator=(const typename _Container::value_type &__value) {
			iter = container->insert(iter, __value);//将元素插入到iter指向的元素之前的位置
			++iter;//递增，指向原来的元素
			return *this;//谁访问地，你返回什么东西。
		}
		//同样的insert迭代器，没有*，之类的，也就没有运算符重载
		insert_iterator<_Container> &operator*() { return *this; }

		insert_iterator<_Container> &operator++() { return *this; }

		insert_iterator<_Container> &operator++(int) { return *this; }
	};
	//这个应该相当于对外的接口
	template<class _Container, class _Iterator>
	inline insert_iterator<_Container> inserter(_Container &__x, _Iterator __i) {
		typedef typename _Container::iterator __iter;
		return insert_iterator<_Container>(__x, __iter(__i));
	}
	//反向迭代器
	template<class _Iterator>
	class reverse_iterator {
	protected:
		_Iterator current;
	public:
		typedef typename iterator_traits<_Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<_Iterator>::value_type        value_type;
		typedef typename iterator_traits<_Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<_Iterator>::pointer           pointer;
		typedef typename iterator_traits<_Iterator>::reference         reference;
		typedef _Iterator                                              iterator_type;
		typedef reverse_iterator<_Iterator>                            _Self;
	//习惯问题，定义函数另写一个public也可，public这里可写可不写
	public:
		reverse_iterator() {}

		explicit reverse_iterator(iterator_type __x) : current(__x) {}

		reverse_iterator(const _Self &__x) : current(__x.current) {}

		template<class _Iter>
		reverse_iterator(const reverse_iterator<_Iter> &__other) : current(__other.base()) {}

		iterator_type base() const {
			return current;
		}
		// 该迭代器是从后面end()开始，需要往前一步，才可以获取到有效的迭代器位置
		reference operator*() const {
			_Iterator __tmp = current;
			return *--__tmp;
		}
		pointer operator->() const {
			return &(operator*());
		}
		//反向，加加就相当于减减，下面都是一些运算符重载了
		_Self &operator++() {
			--current;
			return *this;
		}

		_Self operator++(int) {
			_Self __tmp = *this;
			--current;
			return __tmp;
		}

		_Self &operator--() {
			++current;
			return *this;
		}

		_Self operator--(int) {
			_Self __tmp = *this;
			++current;
			return __tmp;
		}

		_Self operator+(difference_type __n) const {
			return _Self(current - __n);
		}

		_Self operator-(difference_type __n) const {
			return _Self(current + __n);
		}

		_Self &operator+=(difference_type __n) {
			current -= __n;
			return *this;
		}

		_Self &operator-=(difference_type __n) {
			current += __n;
			return *this;
		}

		reference operator[](difference_type __n) const {
			//        base()[-n-1]
			return *(*this + __n);
		}
	};

__STL_END_NAMESPACE
#endif // !__STL_ITERATOR_H__

