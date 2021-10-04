#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_uninitialized.h"

__STL_BEGIN_NAMESPACE

/*
 vector的数据结构为简单连续线性空间，分别用start和finish指向连续空间中已被使用的范围，而end_of_storage指向整个空间的尾部（包括备用空间）。
 //https://\www.kancloud.cn/digest/stl-sources/177267
 */
	template<class _Tp, class _Alloc>
class _Vector_base {
public:
	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	//_Alloc get_allocator() const { return allocator_type(); }


	_Vector_base(const _Alloc &)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {}//冒号后面相当于赋值，c++特有写法

	//特化版本
	_Vector_base(size_t __n, const _Alloc &)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {
		_M_start = _M_allocate(__n);
		_M_finish = _M_start;
		_M_end_of_storage = _M_start + __n;
	}
	//析构函数
	~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }
	//定义三个指针
protected:
	_Tp *_M_start;
	_Tp *_M_finish;
	_Tp *_M_end_of_storage;

	typedef simple_alloc<_Tp, _Alloc> _M_data_allocator;

	_Tp * _M_allocate(size_t __n) {
		return _M_data_allocator::allocate(__n);
	}
	void _M_deallocate(_Tp *__p, size_t __n) {
		_M_data_allocator::deallocate(__p, __n);
	}
};

//Alloc是SGI STL的空间配置器，默认是第二级配置器
template<class _Tp, class _Alloc = alloc>
//这里是继承
class vector : protected _Vector_base<_Tp, _Alloc> {
private:
	typedef _Vector_base<_Tp, _Alloc> _Base;
public://vector的内嵌型别定义,是iterator_traits<I>服务的类型
	typedef _Tp value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type *iterator;//vector容器的迭代器是普通指针
	typedef const value_type *const_iterator;
	typedef value_type &reference;
	typedef const value_type &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef typename _Base::allocator_type allocator_type;
	//typedef typename _Base::_Alloc allocator_type
	allocator_type get_allocator() const { return _Base::get_allocator(); }

	typedef reverse_iterator<const_iterator> const_reverse_iterator;
	typedef reverse_iterator<iterator> reverse_iterator;
protected:
	//这里相当于直接using namespace std;对吧，也就是他直接调用了上面的一些属性
	using _Base::_M_allocate;
	using _Base::_M_deallocate;
	using _Base::_M_end_of_storage;
	using _Base::_M_finish;
	using _Base::_M_start;
protected:
	void _M_insert_aux(iterator __position, const _Tp & __x);

	void _M_insert_aux(iterator __position);
public:
	//以下为vector的迭代器
	iterator begin() {
		return _M_start;
	}

	const_iterator begin() const {
		return _M_start;
	}

	iterator end() {
		return _M_finish;
	}

	const_iterator end() const {
		return _M_finish;
	}
	//反向迭代器，rbegin
	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const {
		return reverse_iterator(end());
	}

	reverse_iterator rend() {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const {
		return reverse_iterator(begin());
	}
	//下面是三个容量
	size_type size() const {
		return size_type(end() - begin());
	}

	size_type max_size() const {
		return size_type(-1) / sizeof(_Tp);
	}

	size_type capacity() const {
		return size_type(_M_end_of_storage - begin());
	}
	//判断为空
	bool empty() const {
		return begin() == end();
	}
	//迭代器是特殊指针，也可以用相当于数组类型进行读取
	reference operator[](size_type __n) {
		return *(begin() + __n);
	}
	const_reference operator[](size_type __n) const {
		return *(begin() + __n);
	}
	//好像是判断是否越界
	void _M_range_check(size_type __n) const {
		if (__n >= size())
			//todo
		{
			throw;
		};
	}
	//at()会做边界检查，可以用try catch捕获异常，程序可以不直接退出。，是另一种元素访问，比如vecint.at(3)
	reference at(size_type __n) {
		_M_range_check(__n);
		return (*this)[__n];
	}

	const_reverse_iterator at(size_type __n) const {
		_M_range_check(__n);
		return (*this)[__n];
	}
	//这里把vector容器的构造函数列出来讲解，主要是我们平常使用vector容器时，首先要要定义相应的容器对象，所以，如果我们对vector容器的构造函数了解比较透彻时，在应用当中就会比较得心应手。在以下源码的前面我会总结出vector容器的构造函数及其使用方法。
	//默认的构造函数
	explicit vector(const allocator_type &__a = allocator_type()) : _Base(__a) {}
	//具有初始值和容器大小的构造函数
	vector(size_type __n, const _Tp &__value, const allocator_type &__a = allocator_type()) : _Base(__n, __a) {
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}
	//只有容器大小的构造函数
	explicit vector(size_type __n) : _Base(__n, allocator_type()) {
		_M_finish = uninitialized_fill_n(_M_start, __n, _Tp());
	}

	////拷贝构造函数，不指定的话，默认复制构造函数也就是默认拷贝构造函数，注意，默认构造函数（即无参构造函数）不一定存在，但是复制构造函数总是会存在。
	/*
		#include<iostream >
		using namespace std;
		class Complex
		{
		public:
			double real, imag;
			Complex(double r, double i) {
				real= r; imag = i;
			}
		};
		int main(){
			Complex cl(1, 2);
			Complex c2 (cl);  //用复制构造函数初始化c2
			cout<<c2.real<<","<<c2.imag;  //输出 1,2
			return 0;
		}

	*/
	vector(const vector<_Tp, _Alloc> &__x) : _Base(__x.size(), __x.get_allocator()) {
		_M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start);
	}

	//用两个迭代器区间表示容器大小的构造函数
	template<class _InputIterator>
	vector(_InputIterator __first, _InputIterator __last, const allocator_type &__a = allocator_type()) : _Base(__a) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_initialize_aux(__first, __last, _Integral());
	}

	template<class _Integer>
	void _M_initialize_aux(_Integer __n, _Integer __value, __true_type) {
		_M_start = _M_allocate(__n);
		_M_end_of_storage = _M_start + __n;
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}

	template<class _InputIterator>
	void _M_initialize_aux(_InputIterator __first, _InputIterator __last, __false_type) {
		_M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
	}

	//析构函数
	~vector() {
		destory(_M_start, _M_finish);
	}
	vector<_Tp, _Alloc> &operator=(const vector<_Tp, _Alloc> &__x);

	//当要增加的空间大于备用空间，则需要该函数配置新的vector空间，该操作会释放掉之前的空间，并将之前的内容copy到新的空间。
	void reserve(size_type __n) {
		if (capacity() < __n) {
			const size_type __old_size = size();
			iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
			destory(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __tmp;
			_M_finish = __tmp + __old_size;
			_M_end_of_storage = _M_start + __n;

		}
	}
	//assign ，赋值函数，对vector中的元素赋值，和构造相类似，可以按照迭代器来赋值，ve1.assign(ve2.begin(), ve2.end());很好理解，就是复制操作，如果你想把ve1复制为ve2的前三个元素可以这样：ve1.assign(ve2.begin(), ve2.begin() + 3);
	//把容器内容替换为n个初始值为value
	void assign(size_type __n, const _Tp &__val) {
		_M_fill_assign(__n, __val);
	}
	void _M_fill_assign(size_type __n, const _Tp & __val);

	template<class _InputIterator>
	void assign(_InputIterator __first, _InputIterator __last) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_assign_dispatch(__first, __last, _Integral());
	}

	template<class _Integer>
	void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type) {
		_M_fill_assign((size_type)__n, (_Tp)__val);
	}

	template<class _InputIterator>
	void _M_assign_dispatch(_InputIterator __first, _InputIterator __last, __false_type) {
		_M_assign_aux(__first, __last, __ITERATOR_CATEGORY(__first));
	}

	template<class _InputIterator>
	void _M_assign_aux(_InputIterator __first, _InputIterator __last, input_iterator_tag);

	template<class _ForwardIterator>
	void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag);

	//front函数，返回第一个
	reference front() {
		return *begin();
	}

	const_reference front() const {
		return *begin();
	}
	//back函数，返回最后一个，也就是指针的前一个
	reference back() {
		return *(end() - 1);
	}

	const_reference back() const {
		return *(end() - 1);
	}
	//push back ，从后面插入
	void push_back(const _Tp &__value) {
		if (_M_finish != _M_end_of_storage) {
			construct(_M_finish, __value);
			++_M_finish;
		}
		else {
			_M_insert_aux(end(), __value);
		}
	}

	void push_back() {
		if (_M_finish != _M_end_of_storage) {
			construct(_M_finish);
			++_M_finish;
		}
		else {
			_M_insert_aux(end());
		}
	}

	/*交换容器的内容
	*这里使用的方法是交换迭代器所指的地址
	*/
	void swap(vector<_Tp, _Alloc> &__x) {
		if (this != &__x) {
			ding_stl::swap(_M_start, __x._M_start);
			ding_stl::swap(_M_finish, __x._M_finish);
			ding_stl::swap(_M_end_of_storage, __x._M_end_of_storage);
		}
	}

	//把x值插入到指定的位置
	iterator insert(iterator __position, const _Tp & __x) {
		size_type __n = __position - begin();
		//插入分很多情况
		/*
		v.insert(v.begin(),8);//在最前面插入新元素。
		v.insert(v.begin()+2,1);//在迭代器中第二个元素前插入新元素
		v.insert(v.end(),3);//在向量末尾追加新元素。
		v.insert(v.end(),4,1);//在尾部插入4个1

		*/
		//第一种是插入到vector的末位
		if (_M_finish != _M_end_of_storage && __position == end()) {
			construct(_M_finish, __x);
			++_M_finish;
		}
		//插入到其他地方
		else {
			_M_insert_aux(__position, __x);
		}
		return begin() + __n;

	}
	//这里我感觉像偏特化，这个是int的
	template<class _InputIterator>
	void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_insert_dispatch(__pos, __first, __last, _Integral());
	}
	template<class _Integer>
	void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val, __true_type) {
		_M_fill_insert(__pos, (size_type)__n, (_Tp)__val);
	}
	//如果不知道是什么，就去问萃取机
	template<class _InputIterator>
	void _M_insert_dispatch(iterator __pos, _InputIterator __first, _InputIterator __last, __false_type) {
		_M_range_insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
	}
	void insert(iterator __pos, size_type __n, const _Tp &__x) {
		_M_fill_insert(__pos, __n, __x);
	}
	void _M_fill_insert(iterator __pos, size_type __n, const _Tp &__x);
	//也就是从最后一个地方出
	void pop_back() {
		--_M_finish;
		destroy(_M_finish);
	}
	//erase，擦除，其中擦除函数是擦除输入迭代器之间的元素，但是没有回收内存空间，只是把内存空间作为备用空间，首先看下该函数的源代码，根据上面函数的定义，我们可以知道，迭代器start和end_of_storage并没有改变，只是调整迭代器finish，并析构待擦除元素对象
	iterator erase(iterator __position) {
		//如果position后面还有元素，需要拷贝;如果position是最后一个元素，则后面没有元素，直接destroy即可
		if (__position + 1 != end()) {
			copy(__position + 1, _M_finish, __position);
		}
		--_M_finish;
		destory(_M_finish);
		return __position;
	}
	//erase的例子
	//https://\blog.csdn.net/dongyanxia1000/article/details/52838922
	//从last开始，拷贝到finish，目标开头到first
	iterator erase(iterator __first, iterator __last) {
		iterator __i = copy(__last, _M_finish, __first);
		destory(__i, _M_finish);
		_M_finish = _M_finish - (__last - __first);
		return __first;
	}
	/*
	 resize(n，t)
多一个参数t，将所有新添加的元素初始化为t。
	*/
	void resize(size_type __new_size, const _Tp &__x) {
		if (__new_size < size()) {
			erase(begin() + __new_size, end());
		}
		else {
			insert(end(), __new_size - size(), __x);
		}
	}

	void resize(size_type __new_size) {
		resize(__new_size, _Tp());
	}
	//清除
	void clear() {
		erase(begin(), end());
	}
protected:
	template<class _ForwardIterator>
	iterator _M_allocate_and_copy(size_type __n, _ForwardIterator __first, _ForwardIterator __last) {
		iterator __result = _M_allocate(__n);
		try {
			uninitialized_copy(__first, __last, __result);
			return __result;
		}
		catch (...) {
			_M_deallocate(__result, __n);
		}
	}

	template<class _InputIterator>
	void _M_range_initialize(_InputIterator __first, _InputIterator __last, input_iterator_tag) {
		for (; __first != __last; ++__first) {
			push_back(*__first);
		}
	}
	// This function is only called by the constructor.按我的理解，应该是，开始构造的时候进行初始化用的
	template<class _ForwardIterator>
	void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag) {
		size_type __n = 0;
		distance(__first, __last, __n);
		_M_start = _M_allocate(__n);
		_M_end_of_storage = _M_start + __n;
		_M_finish = uninitialized_copy(__first, __last, _M_start);
	}
	//上面有调用他的函数
	template<class _InputIterator>
	void _M_range_insert(iterator __pos, _InputIterator __first, _InputIterator __last, input_iterator_tag);

	template<class _ForwardIterator>
	void _M_range_insert(iterator __pos, _ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag);

};
//接下来是操作符重载
template<class _Tp, class _Alloc>
inline bool operator==(const vector<_Tp, _Alloc> &__x, const vector<_Tp, _Alloc> &__y) {
	return __x.size() == __y.size() && equal(__x.begin(), __x.end(), __y.begin());
}
//在算法中有实现lexixxx的那个比较函数
template<class _Tp, class _Alloc>
inline bool operator<(const vector<_Tp, _Alloc> &__x, const vector<_Tp, _Alloc> &__y) {
	return lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}
//重载=
template<class _Tp, class _Alloc>
vector<_Tp, _Alloc> & vector<_Tp, _Alloc>::operator=(const vector<_Tp, _Alloc> &__x) {
	if (this != &__x) {
		const size_type __xlen = __x.size();
		if (__xlen > capacity()) {
			iterator __tmp = _M_allocate_and_copy(__xlen, __x.begin(), __x.end());
			destory(_M_start, _M_finish);
			destory(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __tmp;
			_M_end_of_storage = _M_start + __xlen;
		}
		else if (__xlen <= size()) {
			iterator __i = copy(__x.begin(), __x.end(), begin());
			destory(__i, _M_finish);
		}
		else {
			copy(__x.begin(), __x.begin() + size(), _M_start);
			uninitialized_copy(__x.begin() + size(), __x.end(), _M_finish);
		}
		_M_finish = _M_start + __xlen;
	}
	return *this;
}
//接下来应该全部都是insert的其他情况
template<class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp &__val) {
	if (__n > capacity()) {
		vector<_Tp, _Alloc> __tmp(__n, __val, get_allocator());
		__tmp.swap(*this);
	}
	else if (__n > size()) {
		fill(begin(), end(), __val);
		_M_finish = uninitialized_fill_n(_M_finish, __n - size(), __val);
	}
	else {
		//size() >= __n
		erase(fill_n(begin(), __n, __val), end());
	}
}

template<class _Tp, class _Alloc>
template<class _InputIter>
void vector<_Tp, _Alloc>::_M_assign_aux(_InputIter __first, _InputIter __last, input_iterator_tag) {
	iterator __cur = begin();
	for (; __first != __last && __cur != end(); ++__cur, ++__first)
		*__cur = *__first;
	//如果size > __n(__last-__first)，则擦除从超过__n个的元素
	if (__first == __last)
		erase(__cur, end());
	//如果size < __n, 则向后插入少了的元素
	else
		insert(end(), __first, __last);
}

template<class _Tp, class _Alloc>
template<class _ForwardIter>
void vector<_Tp, _Alloc>::_M_assign_aux(_ForwardIter __first, _ForwardIter __last, forward_iterator_tag) {
	size_type __len = 0;
	//统计从__First到__last的元素个数
	distance(__first, __last, __len);
	if (__len > capacity()) {
		//重新初始化并拷贝元素从first到last
		iterator __tmp = _M_allocate_and_copy(__len, __first, __last);
		destroy(_M_start, _M_finish);
		_M_deallocate(_M_start, _M_end_of_storage - _M_start);
		_M_start = __tmp;
		_M_end_of_storage = _M_finish = _M_start + __len;
	}
	else if (size() >= __len) {
		iterator __new_finish = copy(__first, __last, _M_start);
		destroy(__new_finish, _M_finish);
		_M_finish = __new_finish;
	}
	else {
		// size < __len <=capacity
		_ForwardIter __mid = __first;
		advance(__mid, size());
		copy(__first, __mid, _M_start);
		_M_finish = uninitialized_copy(__mid, __last, _M_finish);
	}
}
template<class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position, const _Tp &__x) {
	if (_M_finish != _M_end_of_storage) {
		construct(_M_finish, *(_M_finish - 1));
		++_M_finish;
		_Tp __x_copy = __x;
		copy_backward(__position, _M_finish - 2, _M_finish - 1);
		*__position = __x_copy;
	}
	else {
		const size_type __old_size = size();
		const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
		iterator __new_start = _M_allocate(__len);
		iterator __new_finish = __new_start;
		try {
			__new_finish = uninitialized_copy(_M_start, __position, __new_start);
			construct(__new_finish, __x);
			++__new_finish;
			__new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
		}
		catch (...) {
			destory(__new_start, __new_finish);
			_M_deallocate(__new_start, __len);
		}
		destory(begin(), end());
		_M_deallocate(_M_start, _M_end_of_storage - _M_start);
		_M_start = __new_start;
		_M_finish = __new_finish;
		_M_end_of_storage = __new_start + __len;
	}
}
template<class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position) {
	if (_M_finish != _M_end_of_storage) {
		construct(_M_finish, *(_M_finish - 1));
		++_M_finish;
		copy_backward(__position, _M_finish - 2, _M_finish - 1);
		*__position = _Tp();
	}
	else {
		const size_type __old_size = size();
		const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
		iterator __new_start = _M_allocate(__len);
		iterator __new_finish = __new_start;
		try {
			__new_finish = uninitialized_copy(_M_start, __position, __new_start);
			construct(__new_finish);
			++__new_finish;
			__new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
		}
		catch (...) {
			destroy(__new_start, __new_finish);
			_M_deallocate(__new_start, __len);
		}
		destroy(begin(), end());
		_M_deallocate(_M_start, _M_end_of_storage - _M_start);
		_M_start = __new_start;
		_M_finish = __new_finish;
		_M_end_of_storage = __new_start + __len;
	}
}
template<class _Tp, class _Alloc>
void vector<_Tp, _Alloc>::_M_fill_insert(iterator __position, size_type __n,
	const _Tp &__x) {
	if (__n != 0) {
		//剩余空间足够，无需重新开辟
		if (size_type(_M_end_of_storage - _M_finish) >= __n) {
			_Tp __x_copy = __x;
			const size_type __elems_after = _M_finish - __position;
			iterator __old_finish = _M_finish;
			if (__elems_after > __n) {
				uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
				_M_finish += __n;
				copy_backward(__position, __old_finish - __n, __old_finish);
				fill(__position, __position + __n, __x_copy);
			}
			else {
				uninitialized_fill_n(_M_finish, __n - __elems_after, __x_copy);
				_M_finish += __n - __elems_after;
				uninitialized_copy(__position, __old_finish, _M_finish);
				_M_finish += __elems_after;
				fill(__position, __old_finish, __x_copy);
			}
		}
		else {
			const size_type __old_size = size();
			const size_type __len = __old_size + max(__old_size, __n);
			iterator __new_start = _M_allocate(__len);
			iterator __new_finish = __new_start;
			try {
				__new_finish = uninitialized_copy(_M_start, __position, __new_start);
				__new_finish = uninitialized_fill_n(__new_finish, __n, __x);
				__new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
			}
			catch (...) {
				destory(__new_start, __new_finish);
				_M_deallocate(__new_start, __len);
			}
			destory(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __new_start;
			_M_finish = __new_finish;
			_M_end_of_storage = __new_finish + __len;
		}
	}
}
template<class _Tp, class _Alloc>
template<class _InputIterator>
void vector<_Tp, _Alloc>::_M_range_insert(iterator __pos,
	_InputIterator __first,
	_InputIterator __last,
	input_iterator_tag) {
	for (; __first != __last; ++__first) {
		__pos = insert(__pos, *__first);
		++__pos;
	}
}

template<class _Tp, class _Alloc>
template<class _ForwardIterator>
void vector<_Tp, _Alloc>::_M_range_insert(iterator __position,
	_ForwardIterator __first,
	_ForwardIterator __last,
	forward_iterator_tag) {
	if (__first != __last) {
		size_type __n = 0;
		distance(__first, __last, __n);
		if (size_type(_M_end_of_storage - _M_finish) >= __n) {
			const size_type __elems_after = _M_finish - __position;
			iterator __old_finish = _M_finish;
			if (__elems_after > __n) {
				uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
				_M_finish += __n;
				copy_backward(__position, __old_finish - __n, __old_finish);
				copy(__first, __last, __position);
			}
			else {
				_ForwardIterator __mid = __first;
				advance(__mid, __elems_after);
				uninitialized_copy(__mid, __last, _M_finish);
				_M_finish += __n - __elems_after;
				uninitialized_copy(__position, __old_finish, _M_finish);
				_M_finish += __elems_after;
				copy(__first, __mid, __position);
			}
		}
		else {
			const size_type __old_size = size();
			const size_type __len = __old_size + max(__old_size, __n);
			iterator __new_start = _M_allocate(__len);
			iterator __new_finish = __new_start;
			try {
				__new_finish = uninitialized_copy(_M_start, __position, __new_start);
				__new_finish = uninitialized_copy(__first, __last, __new_finish);
				__new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
			}
			catch (...) {
				destroy(__new_start, __new_finish);
				_M_deallocate(__new_start, __len);
			}
			destroy(_M_start, _M_finish);
			_M_deallocate(_M_start, _M_end_of_storage - _M_start);
			_M_start = __new_start;
			_M_finish = __new_finish;
			_M_end_of_storage = __new_start + __len;
		}
	}
}


__STL_END_NAMESPACE


#endif // !__STL_VECTOR_H__