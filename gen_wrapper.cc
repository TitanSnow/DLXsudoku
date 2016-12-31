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

void pr(const std::vector<std::string>& vs) {
	for(int i=0; i!=9; ++i) {
		for(int j=0; j!=9; ++j)
			std::cout.put(std::isupper(vs[i][j])?vs[i][j]-16:vs[i][j]);
		std::cout.put('\n');
	}
	//std::cout.flush();
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
	rander<9> rd;
	if(seed!=-1) rd.randomer=rand_maker(seed);
	std::thread([]() {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout<<timeout_error;
		std::cout.flush();
		std::terminate();
	}).detach();
	pr(rd.generate(lowline));
	std::cout<<"--SEED "<<rd.randomer.get_seed()<<"-----\n";
	return 0;
}