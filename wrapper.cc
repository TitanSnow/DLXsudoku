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

#include"sudoku.h"
#include<iostream>
#include<cctype>
class wrapper {
	const std::vector<std::string>* pvs;
	void pr() const{
		const std::vector<std::string>& vs=*pvs;
		for(int i=0; i!=9; ++i) {
			for(int j=0; j!=9; ++j)
				std::cout.put(std::isupper(vs[i][j])?vs[i][j]-16:vs[i][j]);
			std::cout.put('\n');
		}
		//std::cout.flush();
	}
public:
	wrapper() {
		std::ios::sync_with_stdio(false);
		//std::cin.tie(0);
		int kase=0;
		while(1) {
			std::vector<std::string> vs;
			this->pvs=&vs;
			for(int i=0; i!=9; ++i) {
				std::string st;
				do {
					std::cin>>st;
					if(!std::cin) {
						if(st.size()==9) break;
						return;
					}
				} while(st.size()!=9);
				for(int i=0;i!=9;++i)
					if(std::isdigit(st[i]))st[i]+=16;
				vs.push_back(st);
			}
			covter<9> cvt(vs);
			if(kase++!=0) std::cout.put('\n');
			pr();
			char ch;
			while(std::cin.get(ch)&&std::isspace(ch));
			if(!std::cin) return;
			if(ch!='?')
				std::cin.unget();
			else {
				std::cin.get();
				if(!std::cin) return;
				const std::vector<std::vector<std::string> >& steps=cvt.get_steps();
				for(std::vector<std::vector<std::string> >::size_type i=0; i!=steps.size(); ++i) {
					this->pvs=&(steps[i]);
					pr();
					std::cout<<"--PRESS-----\n";
					std::cin.get();
					if(!std::cin) return;
				}
			}
		}
	}
};

int main()
{
	wrapper();
	return 0;
}
