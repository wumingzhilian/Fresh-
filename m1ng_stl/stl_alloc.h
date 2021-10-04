#pragma once
#ifndef __STL_ALLOC_H__
#define __STL_ALLOC_H__

#include<cstdio>
#include<cstdlib>
#include <cstddef>

#include "stl_config.h"
#include "stl_construct.h"

/*
stdlib.h��C��׼�������ͷ�ļ�����������ֵ���ַ���ת������, α��������ɺ���, ��̬�ڴ���亯��, ���̿��ƺ����ȹ��������� C++����Ӧ���õȼ۵�cstdlibͷ�ļ�.
���õĺ�����malloc()��calloc()��realloc()��free()��system()��atoi()��atol()��rand()��srand()��exit()�ȵȡ� ��������ݿ��Դ򿪱�������includeĿ¼�����stdlib.hͷ�ļ��鿴��
*/
__STL_BEGIN_NAMESPACE


//��һ����������ֱ��ʹ��malloc realloc freeʵ�ֹ���
template<int _inst>
class __malloc_alloc_template {
public:
	static void *allocate(size_t __n) {//unsignted int == size_t
		void *__result = malloc(__n);//(void * ���Ա�ʾ����ָ��)
		if (0 == __result) {
			//C �⺯�� int fprintf(FILE *stream, const char *format, ...) ���͸�ʽ��������� stream �С�
			fprintf(stderr, "out of memory\n");
			//�����൱�ڰ�out xxx д����stderr
			//stderr ��׼����
			/*��׼������
				��׼�������Ǵ�����Ϣ��������Ͼ����Ĭ��Ŀ�ꡣ��stdoutһ������ͨ��ҲĬ�϶����ı�����̨��ͨ������Ļ�ϣ���
					stderr���������κκ����Ĳ������ú�������FILE*���͵Ĳ������������������fputs��fprintf��
			*/
			exit(1);
		}
		return __result;
	}
	static void deallocate(void *__p,size_t /*__n*/) {
		free(__p);
	}
	//C++ �е� realloc() �������·�����ǰ���䵫��δ�ͷŵ��ڴ�顣
	static void *reallocate(void *__p, size_t /* old_size*/,size_t __new_sz) {
		void *__result = realloc(__p, __new_sz);//ֱ��ʹ����realloc��
		if (0 == __result) {//û������
			fprintf(stderr, "out of memory\n");
			exit(1);
		}
		return __result;
	}
};
//���������Ա������ʼ
//����ʹ��typedefһ�£��������ʹ�ã�ͬʱ��inst������û�������ã���������Ϊ0
typedef __malloc_alloc_template<0> malloc_alloc;

//https:\//github.com/jasonblog/note/blob/master/c%2B%2B/c%2B%2B_stl_xue_xi_zong_7d5028_quan_976229.md
//��װ���൱����������simple_allocʹ��alloc�߱���׼�ӿ�
//�ڶ����������������ѡ��ʹ���ĸ�������
//��C++�У��û�����ռ�������������͵ģ��е�������ռ䣬�������ռ䣬ÿ�����û��Լ���������ռ��ܴ�С���Ǻ��Ѻã����SGI-STL���ռ������������ٷ�װ��һ�㣺
//https:\//cloud.tencent.com/developer/article/1688439
template<class _Tp, class _Alloc>
class simple_alloc {
public:
	static _Tp *alloc(size_t __n) {
		//// ����n��T���Ͷ����С�Ŀռ�
		return 0 == __n ? 0 : (_Tp*)_Alloc::allocate(__n * sizeof(_Tp));
		//����ֻ����һ�����൱��if�ж϶԰ɣ��жϣ�size��0ô���ǵĻ�������0���Ǿͷ���n�����ʹ�С
	}
	//�����Ϊallocate����д��void�ˣ���bug��gan
	static _Tp *allocate(size_t __n) {
		//����һ��T���Ͷ����С�Ŀռ�
		return (_Tp *)_Alloc::allocate(sizeof(_Tp));
	}
	// �ͷ�һ��T���Ͷ����С�Ŀռ�
	static void deallocate(_Tp *__p) {
		_Alloc::deallocate(__p, sizeof(_Tp));
	}

	//�ͷ�n��T���Ͷ����С�Ŀռ�
	static void deallocate(_Tp *__p, size_t __n) {
		if (0 != __n) {
			_Alloc::deallocate(__p, __n * sizeof(_Tp));
		}
	}
};

//��alloc����Ϊһ��������
typedef malloc_alloc alloc;

__STL_END_NAMESPACE
#endif // !__STL_ALLOC_H__
