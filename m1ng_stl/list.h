#pragma once
#include<assert.h>
template<class _T>
class list {//完全是空的class，没啥问题
	//加下划线，为了内部使用
//typedef _Node _Nodeptr

public:
	struct _Node;
	typedef _Node* _Nodeptr;
	struct _Node {
		_T _Value;
		_Nodeptr _Next, _Prev;
	};
	struct _Acc {//acc专门返回前驱，后继，节点值
		typedef _Nodeptr& _Nodepref;
		typedef _T& _Vref;
		static _Nodepref _Next(_Nodeptr _P) {
			return ((_Nodepref)(*_P)._Next);//_p->_next
		}
		static _Nodepref _Prev(_Nodeptr _P) {
			return ((_Nodepref)(*_P)._Prev);//_p->_Prev
		}
		static _Vref _Value(_Nodeptr _P) {
			return ((_Vref)(*_P)._Value);//_p->_Value
		}
	};
	//方便萃取，经常用到的几个类型
	typedef size_t             size_type;
	typedef _T                value_type;
	typedef _T*            pointer_type;
	typedef const _T*   const_pointer_type;
	typedef _T&                reference;
	typedef const _T&   const_reference;//引用，相当于别名，对引用的操作和原来变量的此操作完全一致
	typedef int difference_type;//距离类型

	//迭代器
	class itrator;//普通迭代器
	class const_iterator;//const类型迭代器

	class const_iterator {
		//迭代器用途就是，比较是否相同，如何加加，如何减少，如何取值
	public:

		const_iterator() {//构造函数

		}
		//有一个参数的构造函数
		const_iterator(_Nodeptr _P):_Ptr(_P) {

		}
		//拷贝构造函数,
		//!!!!!!! 一定要注意，这里是const
		const_iterator(const const_iterator & _X) :_Ptr(_X._Ptr) {//参数值访问类内使用点

		}

		const_reference operator *()const {//完成星号的重载操作符
			return _Acc::_Value(_Ptr);
		}

		//运算符重载相当于：
		//https:\//blog.csdn.net/naibozhuan3744/article/details/93532268
		//classA+classB==classA.operator+(classB);
		const_iterator operator++() {//前加加，喜欢前加加
			//先加后调用
			_Ptr = _Acc::_Next(_Ptr);
			return *this;
		}

		const_iterator operator++(int) {//后加加，里面加上参数
			const_iterator _tmp = *this;//需要返回tmp出来
			++* this;//先调用，后加
			return _tmp;
		}

		const_iterator operator--()//前置减减
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}

		const_iterator operator--(int)//后置减减
		{
			const_iterator _Tmp = *this;
			--*this;
			return *_Tmp;
		}

		//通过重写等号，来判断是否两个迭代器相同
		bool operator==(const const_iterator & _X)const {
			return _Ptr == _X._Ptr;
		}
		bool operator!=(const const_iterator & _X)const {
			return !(*this == _X);//利用重写的==，加一个感叹号即可
		}
		_Nodeptr _Mynode()const {//为了返回ptr
			return _Ptr;
		}
	protected:
		_Nodeptr _Ptr;//属性
	};




public://构造函数
	class iterator :public const_iterator//普通迭代器继承自常迭代器，我们使用公有继承，公有继承可以直接使用继承的属性，以及函数
	{
	public:
		iterator() {//普通的构造方法

		}
		iterator(_Nodeptr _P):const_iterator(_P) {//一个参数的构造方法
			
		}
		reference operator*()const {
			return _Acc::_Value(_Ptr);
		}
		iterator operator++() {//前加加，喜欢前加加
			_Ptr = _Acc::_Next(_Ptr);
			return *this;
		}

		iterator operator++(int) {//后加加，里面加上参数
			iterator _tmp = *this;//需要返回tmp出来
			++* this;
			return _tmp;
		}

		iterator operator--()//前置减减
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}

		iterator operator--(int)//后置减减
		{
			iterator _Tmp = *this;
			--*this;
			return *_Tmp;
		}
		bool operator==(const iterator& _X)const {
			return _Ptr == _X._Ptr;
		}
		bool operator!=(const iterator& _X)const {
			return !(*this == _X);
		}
	protected:
		_Nodeptr _Ptr;
	};
	explicit list() :_Head(_Buynode()),_Size()//显式转换
		//类构造函数初始化列表
		//构造函数初始化列表以一个冒号开始，接着是以逗号分隔的数据成员列表，每个数据成员后面跟一个放在括号中的初始化式。例如：
	//显式转换隐式转换https://www.cnblogs.com/lijiaxin/p/10659157.html
	{
	}
	//数值大小的构造
	explicit list(size_type _N, const _T& _V = _T()) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _N, _V);
	}
	//区间的构造
	list(const _T* _F, const _T* L) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _F, L);
	}      
	size_type size()const {
		return _Size;
	}
	bool empty() const {
		return (size == 0);
	}
	//返回头节点指针，地址
	reference front() {
		return *begin();
	}
	reference back() {
		return *--end();//--已经重写过了
	}
	const_iterator begin()const//先写一下begin，定义为常函数
	{
		return const_iterator(_Acc::_Next(_Head));//返回的是一个构造函数，他会构造一个类，所以我们之前的声明也需要是类名
	}

	iterator begin() {
		return iterator(_Acc::_Next(_Head));
	}
	iterator end() {
		return iterator(_Head);
	}
	const_iterator end() const
	{
		return const_iterator(_Head);
	}



	//尾插
	void push_back(const _T &v) {
		_Nodeptr _s = _Buynode(_Head,_Acc::_Prev(_Head));//直接调用方法，开辟节点
		_Acc::_Vref(_s) = v;
														 //_s->_Value = v;
		//_s->_Next = _Head;//新节点的下一个指向头节点（双向循环么）
		//_s->_Prev = _Head->_Prev;//（新节点的前驱指向，头节点的前驱）
		//_Head->_Prev->_Next = _s;
		//_Head->_Prev = _s;
		//以上版本也可以，但是他们喜欢你高端版本
		_Acc::_Next(_Acc::_Prev(_s)) = _s;
		_Acc::_Prev(_Head) = _s;
		_Size++;
	}
	//头插
	void push_front(const _T & v) {
		insert(begin(), v);//头插
	}
	//尾删
	void pop_back() {
		erase(--end());
		//end返回的是最后的下一个元素，所以要减减
	}
	//头删
	void pop_front() {
		erase(begin());
	}

	void clear() {
		erase(begin(), end());
	}
	//赋值
	void assign(size_type _N,const _T& _X = _T()) {
		erase(begin(), end());
		insert(begin(), _N, _X);
	}
	//我们写插入方法
	public:
		//给出位置，插入类型，插入，
		/*
		iterator insert(iterator _P, const _T& _X = _T())//和int a = int() 类似也就是 int *a = new int();
		{
			_Nodeptr _Ptr = _P._Mynode();//迭代器中mynode函数，表明节点的指向_Ptr指向了新加的上一个，也就是当前位置
			_Nodeptr _S = _Buynode(_Ptr,_Acc::_Prev(_Ptr));//申请节点,s的后继,第一个后继，第二个前驱，s的前驱就是ptr的前驱
			//到这儿只连了两根线，还有两根线
			_Acc::_Prev(_Ptr) = _S;
			_Acc::_Next(_Acc::_Prev(_S)) = _S;
			//赋值
			_Acc::_Value(_X);
			//这里的赋值不安全
			_Size++;
			return iterator(_S);
		}*/
		iterator insert(iterator _P, const _T& _X = _T()) {
			_Nodeptr _S = _P._Mynode();
			_Acc::_Prev(_S) = _Buynode(_S, _Acc::_Prev(_S));
			_S = _Acc::_Prev(_S);
			_Acc::_Next(_Acc::_Prev(_S)) = _S;
			///////////////////
			_Acc::_Value(_S) = _X;
			//////////////////////////
			_Size++;
			return iterator(_S);
		}

		iterator erase(iterator _P) {
			_Nodeptr _S = (_P++)._Mynode();//这里已经变成了删除结点的下一个
			_Acc::_Next(_Acc::_Prev(_S)) = _Acc::_Next(_S);
			_Acc::_Prev(_Acc::_Next(_S)) = _Acc::_Prev(_S);
			//这里的删除同样不安全
			free(_S);
			--_Size;
			return _P;
		}
		//删除区间的函数
		iterator erase(iterator _F, iterator _L) {
			while (_F != _L) {
				erase(_F++);
			}
			return _F;
		}

		//给出位置，给出大小，给出值
		void insert(iterator _P, size_type _M, const _T& _X) {//插入多少个
			for (; 0 < _M; --_M) {
				insert(_P, _X);
			}
		}
		//insert 插入一个区间
		void insert(iterator _P, const _T * _F, const _T &_X)
		{
			for (; _F != _X;++_F) {
				insert(_P, *_F);
			}
		}
	//保护成员的可访问范围比私有成员大，比公有成员小。能访问私有成员的地方都能访问保护成员
protected:
	//_Narg后继参数，_Parg前驱参数
	_Nodeptr _Buynode(_Nodeptr _Narg=0,_Nodeptr _Parg=0) {//开辟头节点，让他前一个指针指向自己下一个指针只想自己
		_Nodeptr _S = (_Nodeptr)malloc(sizeof(_Node));
		assert(_S != NULL);//断言
		//_S->_Next = _Narg != 0 ? _Narg : _S;
		//_S->_Prev = _Narg != 0 ? _Narg : _S;
		_Acc::_Next(_S) = _Narg != 0 ? _Narg : _S;
		_Acc::_Prev(_S) = _Parg != 0 ? _Parg : _S;
		return _S;
	}
private:
	_Nodeptr _Head;
	size_type _Size;
};
//缺少析构函数，不能将内存释放，只能将节点释放，只是把指针当前的内存释放掉，并没有把指向的空间释放掉
