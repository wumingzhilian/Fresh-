#include "stl_vector.h"
#include<iostream>
#include<stdio.h>
//这里一定要指定namespace
using namespace ding_stl;
int main() {
	vector<char> v2;
	v2.push_back('h');
	v2.push_back('e');
	v2.push_back('l');
	v2.push_back('l');
	v2.push_back('o');
	v2.push_back(',');
	v2.push_back('w');
	v2.push_back('o');
	v2.push_back('r');
	v2.push_back('l');
	v2.push_back('d');
	printf("%d",v2.capacity());
	printf("\n");
	printf("%d", v2.empty());
	printf("\n");
	printf("%d", v2.size());
	printf("\n");
	for (vector<char>::iterator m = v2.begin(); m != v2.end(); m++)    
	{
		std::cout << *m ; 
	}
	return 0;
}