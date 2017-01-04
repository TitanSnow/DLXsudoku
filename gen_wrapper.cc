/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

*/

const char timeout_error[]=
"--TIMEOUT. MAY LOWLINE BE SET TOO HIGH-----\n"\
"!!!!-----------------------------------!!!!\n"\
"----!!!!---------------------------!!!!----\n"\
"--------!!!!-------------------!!!!--------\n"\
"------------!!!!-----------!!!!------------\n"\
"----------------!!!!---!!!!----------------\n"\
"-------------------!!!!!-------------------\n"\
"----------------!!!!---!!!!----------------\n"\
"------------!!!!-----------!!!!------------\n"\
"--------!!!!-------------------!!!!--------\n"\
"----!!!!---------------------------!!!!----\n"\
"!!!!-----------------------------------!!!!\n";

#include"rander.h"
#include<iostream>
#include<cctype>
#include<thread>
#include<chrono>
#include<mutex>

void pr(const std::vector<std::string>& vs) {
	for(int i=0; i!=9; ++i) {
		for(int j=0; j!=9; ++j)
			std::cout.put(std::isupper(vs[i][j])?vs[i][j]-16:vs[i][j]);
		std::cout.put('\n');
	}
	//std::cout.flush();
}
void single_doit(uintmax_t lowline,int seed)
{
	rand_maker rm(seed);
	rander<9> rd(rm);
	pr(rd.generate(lowline));
	std::cout<<"--SEED "<<dynamic_cast<const rand_maker&>(static_cast<const rander<9> >(rd).get_randomer()).get_seed()<<"-----\n";
}
void doit(uintmax_t lowline,int seed=-1)
{
	if(seed!=-1) {
		single_doit(lowline,seed);
		return;
	}
	std::mutex mtx,mtx2;
	mtx.lock();
	unsigned hwc=std::thread::hardware_concurrency();
	if(hwc==0) hwc=4;
	for(unsigned i=0;i!=hwc;++i){
		std::thread([&mtx,lowline,&mtx2]() {
			rand_maker rm;
			rander<9> rd(rm);
			rander<9>::board_t ans(rd.generate(lowline));
			mtx2.lock();
			if(!mtx.try_lock()) {
				pr(ans);
				std::cout<<"--SEED "<<dynamic_cast<const rand_maker&>(static_cast<const rander<9> >(rd).get_randomer()).get_seed()<<"-----\n";
			}
			mtx.unlock();
			mtx2.unlock();
		}).detach();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	mtx.lock();
}
int main()
{
	std::ios::sync_with_stdio(false);
	std::cout<<"Input seed (or enter): ";
	char ch;
	if(!std::cin.get(ch)) return 0;
	int seed=-1;
	if(std::isdigit(ch)) {
		std::cin.unget();
		std::cin>>seed;
	}
	std::cout<<"Input lowline: ";
	uintmax_t lowline;
	std::cin>>lowline;
	std::thread([]() {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout<<timeout_error;
		std::cout.flush();
		std::terminate();
	}).detach();
	doit(lowline,seed);
	return 0;
}
