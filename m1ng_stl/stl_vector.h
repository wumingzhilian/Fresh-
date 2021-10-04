#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include "stl_config.h"
#include "stl_alloc.h"
#include "stl_iterator.h"
#include "stl_uninitialized.h"

__STL_BEGIN_NAMESPACE

/*
 vector�����ݽṹΪ���������Կռ䣬�ֱ���start��finishָ�������ռ����ѱ�ʹ�õķ�Χ����end_of_storageָ�������ռ��β�����������ÿռ䣩��
 //https://\www.kancloud.cn/digest/stl-sources/177267
 */
	template<class _Tp, class _Alloc>
class _Vector_base {
public:
	typedef _Alloc allocator_type;
	allocator_type get_allocator() const { return allocator_type(); }

	//_Alloc get_allocator() const { return allocator_type(); }


	_Vector_base(const _Alloc &)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {}//ð�ź����൱�ڸ�ֵ��c++����д��

	//�ػ��汾
	_Vector_base(size_t __n, const _Alloc &)
		: _M_start(0), _M_finish(0), _M_end_of_storage(0) {
		_M_start = _M_allocate(__n);
		_M_finish = _M_start;
		_M_end_of_storage = _M_start + __n;
	}
	//��������
	~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }
	//��������ָ��
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

//Alloc��SGI STL�Ŀռ���������Ĭ���ǵڶ���������
template<class _Tp, class _Alloc = alloc>
//�����Ǽ̳�
class vector : protected _Vector_base<_Tp, _Alloc> {
private:
	typedef _Vector_base<_Tp, _Alloc> _Base;
public://vector����Ƕ�ͱ���,��iterator_traits<I>���������
	typedef _Tp value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef value_type *iterator;//vector�����ĵ���������ָͨ��
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
	//�����൱��ֱ��using namespace std;�԰ɣ�Ҳ������ֱ�ӵ����������һЩ����
	using _Base::_M_allocate;
	using _Base::_M_deallocate;
	using _Base::_M_end_of_storage;
	using _Base::_M_finish;
	using _Base::_M_start;
protected:
	void _M_insert_aux(iterator __position, const _Tp & __x);

	void _M_insert_aux(iterator __position);
public:
	//����Ϊvector�ĵ�����
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
	//�����������rbegin
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
	//��������������
	size_type size() const {
		return size_type(end() - begin());
	}

	size_type max_size() const {
		return size_type(-1) / sizeof(_Tp);
	}

	size_type capacity() const {
		return size_type(_M_end_of_storage - begin());
	}
	//�ж�Ϊ��
	bool empty() const {
		return begin() == end();
	}
	//������������ָ�룬Ҳ�������൱���������ͽ��ж�ȡ
	reference operator[](size_type __n) {
		return *(begin() + __n);
	}
	const_reference operator[](size_type __n) const {
		return *(begin() + __n);
	}
	//�������ж��Ƿ�Խ��
	void _M_range_check(size_type __n) const {
		if (__n >= size())
			//todo
		{
			throw;
		};
	}
	//at()�����߽��飬������try catch�����쳣��������Բ�ֱ���˳���������һ��Ԫ�ط��ʣ�����vecint.at(3)
	reference at(size_type __n) {
		_M_range_check(__n);
		return (*this)[__n];
	}

	const_reverse_iterator at(size_type __n) const {
		_M_range_check(__n);
		return (*this)[__n];
	}
	//�����vector�����Ĺ��캯���г������⣬��Ҫ������ƽ��ʹ��vector����ʱ������ҪҪ������Ӧ�������������ԣ�������Ƕ�vector�����Ĺ��캯���˽�Ƚ�͸��ʱ����Ӧ�õ��оͻ�Ƚϵ���Ӧ�֡�������Դ���ǰ���һ��ܽ��vector�����Ĺ��캯������ʹ�÷�����
	//Ĭ�ϵĹ��캯��
	explicit vector(const allocator_type &__a = allocator_type()) : _Base(__a) {}
	//���г�ʼֵ��������С�Ĺ��캯��
	vector(size_type __n, const _Tp &__value, const allocator_type &__a = allocator_type()) : _Base(__n, __a) {
		_M_finish = uninitialized_fill_n(_M_start, __n, __value);
	}
	//ֻ��������С�Ĺ��캯��
	explicit vector(size_type __n) : _Base(__n, allocator_type()) {
		_M_finish = uninitialized_fill_n(_M_start, __n, _Tp());
	}

	////�������캯������ָ���Ļ���Ĭ�ϸ��ƹ��캯��Ҳ����Ĭ�Ͽ������캯����ע�⣬Ĭ�Ϲ��캯�������޲ι��캯������һ�����ڣ����Ǹ��ƹ��캯�����ǻ���ڡ�
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
			Complex c2 (cl);  //�ø��ƹ��캯����ʼ��c2
			cout<<c2.real<<","<<c2.imag;  //��� 1,2
			return 0;
		}

	*/
	vector(const vector<_Tp, _Alloc> &__x) : _Base(__x.size(), __x.get_allocator()) {
		_M_finish = uninitialized_copy(__x.begin(), __x.end(), _M_start);
	}

	//�����������������ʾ������С�Ĺ��캯��
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

	//��������
	~vector() {
		destory(_M_start, _M_finish);
	}
	vector<_Tp, _Alloc> &operator=(const vector<_Tp, _Alloc> &__x);

	//��Ҫ���ӵĿռ���ڱ��ÿռ䣬����Ҫ�ú��������µ�vector�ռ䣬�ò������ͷŵ�֮ǰ�Ŀռ䣬����֮ǰ������copy���µĿռ䡣
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
	//assign ����ֵ��������vector�е�Ԫ�ظ�ֵ���͹��������ƣ����԰��յ���������ֵ��ve1.assign(ve2.begin(), ve2.end());�ܺ���⣬���Ǹ��Ʋ�������������ve1����Ϊve2��ǰ����Ԫ�ؿ���������ve1.assign(ve2.begin(), ve2.begin() + 3);
	//�����������滻Ϊn����ʼֵΪvalue
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

	//front���������ص�һ��
	reference front() {
		return *begin();
	}

	const_reference front() const {
		return *begin();
	}
	//back�������������һ����Ҳ����ָ���ǰһ��
	reference back() {
		return *(end() - 1);
	}

	const_reference back() const {
		return *(end() - 1);
	}
	//push back ���Ӻ������
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

	/*��������������
	*����ʹ�õķ����ǽ�����������ָ�ĵ�ַ
	*/
	void swap(vector<_Tp, _Alloc> &__x) {
		if (this != &__x) {
			ding_stl::swap(_M_start, __x._M_start);
			ding_stl::swap(_M_finish, __x._M_finish);
			ding_stl::swap(_M_end_of_storage, __x._M_end_of_storage);
		}
	}

	//��xֵ���뵽ָ����λ��
	iterator insert(iterator __position, const _Tp & __x) {
		size_type __n = __position - begin();
		//����ֺܶ����
		/*
		v.insert(v.begin(),8);//����ǰ�������Ԫ�ء�
		v.insert(v.begin()+2,1);//�ڵ������еڶ���Ԫ��ǰ������Ԫ��
		v.insert(v.end(),3);//������ĩβ׷����Ԫ�ء�
		v.insert(v.end(),4,1);//��β������4��1

		*/
		//��һ���ǲ��뵽vector��ĩλ
		if (_M_finish != _M_end_of_storage && __position == end()) {
			construct(_M_finish, __x);
			++_M_finish;
		}
		//���뵽�����ط�
		else {
			_M_insert_aux(__position, __x);
		}
		return begin() + __n;

	}
	//�����Ҹо���ƫ�ػ��������int��
	template<class _InputIterator>
	void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
		typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
		_M_insert_dispatch(__pos, __first, __last, _Integral());
	}
	template<class _Integer>
	void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val, __true_type) {
		_M_fill_insert(__pos, (size_type)__n, (_Tp)__val);
	}
	//�����֪����ʲô����ȥ����ȡ��
	template<class _InputIterator>
	void _M_insert_dispatch(iterator __pos, _InputIterator __first, _InputIterator __last, __false_type) {
		_M_range_insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
	}
	void insert(iterator __pos, size_type __n, const _Tp &__x) {
		_M_fill_insert(__pos, __n, __x);
	}
	void _M_fill_insert(iterator __pos, size_type __n, const _Tp &__x);
	//Ҳ���Ǵ����һ���ط���
	void pop_back() {
		--_M_finish;
		destroy(_M_finish);
	}
	//erase�����������в��������ǲ������������֮���Ԫ�أ�����û�л����ڴ�ռ䣬ֻ�ǰ��ڴ�ռ���Ϊ���ÿռ䣬���ȿ��¸ú�����Դ���룬�������溯���Ķ��壬���ǿ���֪����������start��end_of_storage��û�иı䣬ֻ�ǵ���������finish��������������Ԫ�ض���
	iterator erase(iterator __position) {
		//���position���滹��Ԫ�أ���Ҫ����;���position�����һ��Ԫ�أ������û��Ԫ�أ�ֱ��destroy����
		if (__position + 1 != end()) {
			copy(__position + 1, _M_finish, __position);
		}
		--_M_finish;
		destory(_M_finish);
		return __position;
	}
	//erase������
	//https://\blog.csdn.net/dongyanxia1000/article/details/52838922
	//��last��ʼ��������finish��Ŀ�꿪ͷ��first
	iterator erase(iterator __first, iterator __last) {
		iterator __i = copy(__last, _M_finish, __first);
		destory(__i, _M_finish);
		_M_finish = _M_finish - (__last - __first);
		return __first;
	}
	/*
	 resize(n��t)
��һ������t������������ӵ�Ԫ�س�ʼ��Ϊt��
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
	//���
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
	// This function is only called by the constructor.���ҵ���⣬Ӧ���ǣ���ʼ�����ʱ����г�ʼ���õ�
	template<class _ForwardIterator>
	void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag) {
		size_type __n = 0;
		distance(__first, __last, __n);
		_M_start = _M_allocate(__n);
		_M_end_of_storage = _M_start + __n;
		_M_finish = uninitialized_copy(__first, __last, _M_start);
	}
	//�����е������ĺ���
	template<class _InputIterator>
	void _M_range_insert(iterator __pos, _InputIterator __first, _InputIterator __last, input_iterator_tag);

	template<class _ForwardIterator>
	void _M_range_insert(iterator __pos, _ForwardIterator __first, _ForwardIterator __last, forward_iterator_tag);

};
//�������ǲ���������
template<class _Tp, class _Alloc>
inline bool operator==(const vector<_Tp, _Alloc> &__x, const vector<_Tp, _Alloc> &__y) {
	return __x.size() == __y.size() && equal(__x.begin(), __x.end(), __y.begin());
}
//���㷨����ʵ��lexixxx���Ǹ��ȽϺ���
template<class _Tp, class _Alloc>
inline bool operator<(const vector<_Tp, _Alloc> &__x, const vector<_Tp, _Alloc> &__y) {
	return lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}
//����=
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
//������Ӧ��ȫ������insert���������
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
	//���size > __n(__last-__first)��������ӳ���__n����Ԫ��
	if (__first == __last)
		erase(__cur, end());
	//���size < __n, �����������˵�Ԫ��
	else
		insert(end(), __first, __last);
}

template<class _Tp, class _Alloc>
template<class _ForwardIter>
void vector<_Tp, _Alloc>::_M_assign_aux(_ForwardIter __first, _ForwardIter __last, forward_iterator_tag) {
	size_type __len = 0;
	//ͳ�ƴ�__First��__last��Ԫ�ظ���
	distance(__first, __last, __len);
	if (__len > capacity()) {
		//���³�ʼ��������Ԫ�ش�first��last
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
		//ʣ��ռ��㹻���������¿���
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