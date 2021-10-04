#pragma once
#include<assert.h>
template<class _T>
class list {//��ȫ�ǿյ�class��ûɶ����
	//���»��ߣ�Ϊ���ڲ�ʹ��
//typedef _Node _Nodeptr

public:
	struct _Node;
	typedef _Node* _Nodeptr;
	struct _Node {
		_T _Value;
		_Nodeptr _Next, _Prev;
	};
	struct _Acc {//accר�ŷ���ǰ������̣��ڵ�ֵ
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
	//������ȡ�������õ��ļ�������
	typedef size_t             size_type;
	typedef _T                value_type;
	typedef _T*            pointer_type;
	typedef const _T*   const_pointer_type;
	typedef _T&                reference;
	typedef const _T&   const_reference;//���ã��൱�ڱ����������õĲ�����ԭ�������Ĵ˲�����ȫһ��
	typedef int difference_type;//��������

	//������
	class itrator;//��ͨ������
	class const_iterator;//const���͵�����

	class const_iterator {
		//��������;���ǣ��Ƚ��Ƿ���ͬ����μӼӣ���μ��٣����ȡֵ
	public:

		const_iterator() {//���캯��

		}
		//��һ�������Ĺ��캯��
		const_iterator(_Nodeptr _P):_Ptr(_P) {

		}
		//�������캯��,
		//!!!!!!! һ��Ҫע�⣬������const
		const_iterator(const const_iterator & _X) :_Ptr(_X._Ptr) {//����ֵ��������ʹ�õ�

		}

		const_reference operator *()const {//����Ǻŵ����ز�����
			return _Acc::_Value(_Ptr);
		}

		//����������൱�ڣ�
		//https:\//blog.csdn.net/naibozhuan3744/article/details/93532268
		//classA+classB==classA.operator+(classB);
		const_iterator operator++() {//ǰ�Ӽӣ�ϲ��ǰ�Ӽ�
			//�ȼӺ����
			_Ptr = _Acc::_Next(_Ptr);
			return *this;
		}

		const_iterator operator++(int) {//��Ӽӣ�������ϲ���
			const_iterator _tmp = *this;//��Ҫ����tmp����
			++* this;//�ȵ��ã����
			return _tmp;
		}

		const_iterator operator--()//ǰ�ü���
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}

		const_iterator operator--(int)//���ü���
		{
			const_iterator _Tmp = *this;
			--*this;
			return *_Tmp;
		}

		//ͨ����д�Ⱥţ����ж��Ƿ�������������ͬ
		bool operator==(const const_iterator & _X)const {
			return _Ptr == _X._Ptr;
		}
		bool operator!=(const const_iterator & _X)const {
			return !(*this == _X);//������д��==����һ����̾�ż���
		}
		_Nodeptr _Mynode()const {//Ϊ�˷���ptr
			return _Ptr;
		}
	protected:
		_Nodeptr _Ptr;//����
	};




public://���캯��
	class iterator :public const_iterator//��ͨ�������̳��Գ�������������ʹ�ù��м̳У����м̳п���ֱ��ʹ�ü̳е����ԣ��Լ�����
	{
	public:
		iterator() {//��ͨ�Ĺ��췽��

		}
		iterator(_Nodeptr _P):const_iterator(_P) {//һ�������Ĺ��췽��
			
		}
		reference operator*()const {
			return _Acc::_Value(_Ptr);
		}
		iterator operator++() {//ǰ�Ӽӣ�ϲ��ǰ�Ӽ�
			_Ptr = _Acc::_Next(_Ptr);
			return *this;
		}

		iterator operator++(int) {//��Ӽӣ�������ϲ���
			iterator _tmp = *this;//��Ҫ����tmp����
			++* this;
			return _tmp;
		}

		iterator operator--()//ǰ�ü���
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}

		iterator operator--(int)//���ü���
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
	explicit list() :_Head(_Buynode()),_Size()//��ʽת��
		//�๹�캯����ʼ���б�
		//���캯����ʼ���б���һ��ð�ſ�ʼ���������Զ��ŷָ������ݳ�Ա�б�ÿ�����ݳ�Ա�����һ�����������еĳ�ʼ��ʽ�����磺
	//��ʽת����ʽת��https://www.cnblogs.com/lijiaxin/p/10659157.html
	{
	}
	//��ֵ��С�Ĺ���
	explicit list(size_type _N, const _T& _V = _T()) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _N, _V);
	}
	//����Ĺ���
	list(const _T* _F, const _T* L) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _F, L);
	}      
	size_type size()const {
		return _Size;
	}
	bool empty() const {
		return (size == 0);
	}
	//����ͷ�ڵ�ָ�룬��ַ
	reference front() {
		return *begin();
	}
	reference back() {
		return *--end();//--�Ѿ���д����
	}
	const_iterator begin()const//��дһ��begin������Ϊ������
	{
		return const_iterator(_Acc::_Next(_Head));//���ص���һ�����캯�������ṹ��һ���࣬��������֮ǰ������Ҳ��Ҫ������
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



	//β��
	void push_back(const _T &v) {
		_Nodeptr _s = _Buynode(_Head,_Acc::_Prev(_Head));//ֱ�ӵ��÷��������ٽڵ�
		_Acc::_Vref(_s) = v;
														 //_s->_Value = v;
		//_s->_Next = _Head;//�½ڵ����һ��ָ��ͷ�ڵ㣨˫��ѭ��ô��
		//_s->_Prev = _Head->_Prev;//���½ڵ��ǰ��ָ��ͷ�ڵ��ǰ����
		//_Head->_Prev->_Next = _s;
		//_Head->_Prev = _s;
		//���ϰ汾Ҳ���ԣ���������ϲ����߶˰汾
		_Acc::_Next(_Acc::_Prev(_s)) = _s;
		_Acc::_Prev(_Head) = _s;
		_Size++;
	}
	//ͷ��
	void push_front(const _T & v) {
		insert(begin(), v);//ͷ��
	}
	//βɾ
	void pop_back() {
		erase(--end());
		//end���ص���������һ��Ԫ�أ�����Ҫ����
	}
	//ͷɾ
	void pop_front() {
		erase(begin());
	}

	void clear() {
		erase(begin(), end());
	}
	//��ֵ
	void assign(size_type _N,const _T& _X = _T()) {
		erase(begin(), end());
		insert(begin(), _N, _X);
	}
	//����д���뷽��
	public:
		//����λ�ã��������ͣ����룬
		/*
		iterator insert(iterator _P, const _T& _X = _T())//��int a = int() ����Ҳ���� int *a = new int();
		{
			_Nodeptr _Ptr = _P._Mynode();//��������mynode�����������ڵ��ָ��_Ptrָ�����¼ӵ���һ����Ҳ���ǵ�ǰλ��
			_Nodeptr _S = _Buynode(_Ptr,_Acc::_Prev(_Ptr));//����ڵ�,s�ĺ��,��һ����̣��ڶ���ǰ����s��ǰ������ptr��ǰ��
			//�����ֻ���������ߣ�����������
			_Acc::_Prev(_Ptr) = _S;
			_Acc::_Next(_Acc::_Prev(_S)) = _S;
			//��ֵ
			_Acc::_Value(_X);
			//����ĸ�ֵ����ȫ
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
			_Nodeptr _S = (_P++)._Mynode();//�����Ѿ������ɾ��������һ��
			_Acc::_Next(_Acc::_Prev(_S)) = _Acc::_Next(_S);
			_Acc::_Prev(_Acc::_Next(_S)) = _Acc::_Prev(_S);
			//�����ɾ��ͬ������ȫ
			free(_S);
			--_Size;
			return _P;
		}
		//ɾ������ĺ���
		iterator erase(iterator _F, iterator _L) {
			while (_F != _L) {
				erase(_F++);
			}
			return _F;
		}

		//����λ�ã�������С������ֵ
		void insert(iterator _P, size_type _M, const _T& _X) {//������ٸ�
			for (; 0 < _M; --_M) {
				insert(_P, _X);
			}
		}
		//insert ����һ������
		void insert(iterator _P, const _T * _F, const _T &_X)
		{
			for (; _F != _X;++_F) {
				insert(_P, *_F);
			}
		}
	//������Ա�Ŀɷ��ʷ�Χ��˽�г�Ա�󣬱ȹ��г�ԱС���ܷ���˽�г�Ա�ĵط����ܷ��ʱ�����Ա
protected:
	//_Narg��̲�����_Pargǰ������
	_Nodeptr _Buynode(_Nodeptr _Narg=0,_Nodeptr _Parg=0) {//����ͷ�ڵ㣬����ǰһ��ָ��ָ���Լ���һ��ָ��ֻ���Լ�
		_Nodeptr _S = (_Nodeptr)malloc(sizeof(_Node));
		assert(_S != NULL);//����
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
//ȱ���������������ܽ��ڴ��ͷţ�ֻ�ܽ��ڵ��ͷţ�ֻ�ǰ�ָ�뵱ǰ���ڴ��ͷŵ�����û�а�ָ��Ŀռ��ͷŵ�
