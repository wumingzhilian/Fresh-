#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
//定义行的属性
typedef struct{
    int valid_bits;//有效位
    unsigned tag;//标识位
    int stamp;//lruNumber牺牲行的时候，也就是需要替换的时候
}cache_line;

//定义参数
char* filepath = NULL;
int s,E,b,S;           // s is set ,E  is line,each line have 2^b bits ,S is 2^s set
int hit=0,miss=0,eviction=0;
cache_line** cache = NULL;

//函数实现
//初始化函数
void init(){
	cache = (cache_line**)malloc(sizeof(cache_line*)*S);             //malloc cache[S][E]
	for(int i=0;i<S;i++)
		*(cache+i) = (cache_line*)malloc(sizeof(cache_line)*E);
	for(int i=0;i<S;i++){
		for(int j=0;j<E;j++){
			cache[i][j].valid_bits = 0;      // set all valid_bits is zero
			cache[i][j].tag = 0xffffffff;           //no address
			cache[i][j].stamp = 0;            //time is 0;
		}
	}
}
//比较重要的更新函数
void update(unsigned address){
	unsigned s_address =(address>>b) & ((0xffffffff)>>(32-s));//set`s index
	unsigned t_address = address>>(s+b);                     //tag`s index
	for(int i=0;i<E;i++){
		if((*(cache+s_address)+i)->tag ==t_address){
			cache[s_address][i].stamp = 0;       //now ,this is used
			hit++;
			return;
		}
	}
	for(int i=0;i<E;i++){
		if(cache[s_address][i].valid_bits == 0){
			cache[s_address][i].tag = t_address;
			cache[s_address][i].valid_bits = 1;
			cache[s_address][i].stamp = 0;       //now ,this is load
			miss++;
			return;
		}
	}
	int max_stamp=0;
	int max_i;
	for(int i=0;i<E;i++){
		if(cache[s_address][i].stamp >max_stamp){
			max_stamp = cache[s_address][i].stamp;
			max_i = i;
		}
	}
	eviction++;
	miss++;
	cache[s_address][max_i].tag = t_address;
	cache[s_address][max_i].stamp = 0;
}
void time(){
	for(int i=0;i<S;i++){
		for(int j=0;j<E;j++){
			if(cache[i][j].valid_bits == 1)
				cache[i][j].stamp++;
		}
	}
}
int main(int argc,char *argv[])
{
	int opt;
	while((opt = getopt(argc,argv,"s:E:b:t:")) !=-1){           //parse command line arguments
		switch(opt){
		case 's':
			s=atoi(optarg);
			break;
		case 'E':
			E=atoi(optarg);
			break;
		case 'b':
			b=atoi(optarg);
			break;
		case 't':
			filepath = optarg;
			break;
		}
	}
	S = 1<<s;
	init();
	FILE* file=fopen(filepath,"r");
	if(file == NULL){     // read trace file
		printf("Open file wrong");
		exit(-1);
	}
	char operation;
	unsigned address;
	int size;
	while(fscanf(file," %c %x,%d",&operation,&address,&size)>0){
		switch(operation){
			case 'L':
				update(address);
				break;
			case 'M':
				update(address);
			case 'S':
				update(address);
				break;
		}
		time();
	}
	for(int i=0;i<S;i++)                  //free cache[S][E]
		free(*(cache+i));
	free(cache);
	fclose(file);	                //close file
    printSummary(hit,miss,eviction);
    return 0;
}
