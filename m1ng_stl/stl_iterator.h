#pragma once

#ifndef __STL_ITERATOR_H__
#define __STL_ITERATOR_H__

#include "stl_config.h"
#include "stl_iterator_base.h"

__STL_BEGIN_NAMESPACE
//http:\//ibillxia.github.io/blog/2014/06/21/stl-source-insight-2-iterators-and-traits/
/*
�� stl_iterator.h �ļ��У������ back_insert_iterator, front_insert_iterator, insert_iterator, reverse_bidirectional_iterator, reverse_iterator, istream_iterator, ostream_iterator, �ȱ�׼�ĵ�����������ǰ3�ж�ʹ�� output_iterator ��ֻд���ԣ�ֻ���в��������ֻ�ǲ����λ�ò�ͬ���ѣ�������4��ʹ�õ��� bidirectional_iterator ��˫��������ԣ���5��ʹ�õ��� random_access_iterator ������������ԡ���������ֱ�׼�������ֱ���ʹ�� input_iterator �� output_iterator ���Եĵ����������⼸����׼�ĵ������Ķ����п��Կ�������Ҫ��ʵ���� operator=, operator*, operator->, operator==, operator++, operator--, operator+, operator-, operator+=, operator-= ��ָ������ı�׼�ӿڡ����ݶ���Ĳ������Ĳ�ͬ�����ǲ�ͬ���͵ĵ������ˡ�
*/
//https:\//blog.csdn.net/weixin_42709632/article/details/104860823?utm_medium=distribute.pc_relevant.none-task-blog-baidujs-1
//���
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
	explicit����Ҫ�÷����Ƿ��ڵ������Ĺ��캯����,��ֹ��ʽת��, ���º�������ڲ���, ��������.

	�������ʹ��A����B, δ��explicit�Ĺ��캯��, ��ʹ��B���в�������ʱ, �Ϳ���ʹ��A, ʹ�ýӿڻ���.

	Ϊ�˱����������, ʹ��explicit������ʽ����, ֻ��ͨ����ʾ(explicit)����.

	*/

	explicit back_insert_iterator(_Container &__x) : container(&__x) {}
	/*
	insert_iterator��֧�֨C��������أ����Կ������ǵ��������������ostream_iterator��insert������һ����*��++������������ڣ�Ҳ����ʹ�ã����Ƕ����Զ������κβ�����ֻ�Ƿ��ض���������������ԭ�����Ϊ��ӭ�������������Ŀ�ζ���������������ͥ���÷�һ�¡������Ķ����޸ġ�
	*/
	back_insert_iterator<_Container> &operator = (const typename _Container::value_type &__value) {
		container->push_back(__value);
		return *this;
	}
	back_insert_iterator<_Container> &operator*() { return *this; }

	back_insert_iterator<_Container> &operator++() { return *this; }

	back_insert_iterator<_Container> &operator++(int) { return *this; }
};
//ǰ��
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
		//������Ҫ��
		typedef _Container container_type;
		typedef output_iterator_tag iterator_category;
		typedef void value_type;
		typedef void difference_type;
		typedef void pointer;
		typedef void reference;

		insert_iterator(_Container &__x,typename _Container::iterator __i):container(&__x),iter(__i){}

		insert_iterator<_Container> &operator=(const typename _Container::value_type &__value) {
			iter = container->insert(iter, __value);//��Ԫ�ز��뵽iterָ���Ԫ��֮ǰ��λ��
			++iter;//������ָ��ԭ����Ԫ��
			return *this;//˭���ʵأ��㷵��ʲô������
		}
		//ͬ����insert��������û��*��֮��ģ�Ҳ��û�����������
		insert_iterator<_Container> &operator*() { return *this; }

		insert_iterator<_Container> &operator++() { return *this; }

		insert_iterator<_Container> &operator++(int) { return *this; }
	};
	//���Ӧ���൱�ڶ���Ľӿ�
	template<class _Container, class _Iterator>
	inline insert_iterator<_Container> inserter(_Container &__x, _Iterator __i) {
		typedef typename _Container::iterator __iter;
		return insert_iterator<_Container>(__x, __iter(__i));
	}
	//���������
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
	//ϰ�����⣬���庯����дһ��publicҲ�ɣ�public�����д�ɲ�д
	public:
		reverse_iterator() {}

		explicit reverse_iterator(iterator_type __x) : current(__x) {}

		reverse_iterator(const _Self &__x) : current(__x.current) {}

		template<class _Iter>
		reverse_iterator(const reverse_iterator<_Iter> &__other) : current(__other.base()) {}

		iterator_type base() const {
			return current;
		}
		// �õ������ǴӺ���end()��ʼ����Ҫ��ǰһ�����ſ��Ի�ȡ����Ч�ĵ�����λ��
		reference operator*() const {
			_Iterator __tmp = current;
			return *--__tmp;
		}
		pointer operator->() const {
			return &(operator*());
		}
		//���򣬼ӼӾ��൱�ڼ��������涼��һЩ�����������
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

