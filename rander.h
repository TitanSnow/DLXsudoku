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
#include<cstdlib>
#include<ctime>
#include<cstdint>
#include<iterator>
#include<algorithm>

#ifndef RANDER_H
#define RANDER_H

class rand_maker {
protected:
	int seed;
public:
	explicit rand_maker(int s=(int)std::time(0)%(int)std::clock()) {
		seed=s;
		std::srand(seed);
	}
	virtual const int& get_seed() const{
		return seed;
	}
	virtual double frand() const{
		double rm=std::rand();
		rm/=RAND_MAX;
		return rm;
	}
	virtual int randint(int min,int max) const{
		return frand()*(max-min)+min;
	}
	virtual ~rand_maker() {}
};

template<int N> class rander {
public:
	rand_maker randomer;
protected:
	virtual const rand_maker& get_randomer() const{
		return randomer;
	}
	virtual rand_maker& get_randomer() {
		return randomer;
	}
public:
	typedef typename covter<N>::board_t board_t;
private:
	std::uintmax_t level(board_t vs) const{
		std::uintmax_t ans=1;
		typedef typename covter<N>::solution_tree_node_ptr solution_tree_node_ptr;
		covter<N> cvt(vs,true,true,false,false);
		solution_tree_node_ptr p=cvt.get_solution_tree();
		while(!p->children.empty()){
			ans*=(p->children.size()+1);
			p=p->children.back();
		}
		return ans;
	}
public:
	board_t generate(std::uintmax_t lowline) const{
		board_t vs,prev,cp;
		std::fill_n(std::back_inserter(vs),N,std::string(N,'-'));
		do {
			prev=vs;
			int fi,fj,fa;
			fi=get_randomer().randint(0,N);
			fj=get_randomer().randint(0,N);
			fa=get_randomer().randint(0,N);
			vs[fi][fj]=fa+'A';
			cp=vs;
		} while(covter<N>(cp,false).get_numof_solutions());
		vs=prev;
		covter<N>(vs,false);
		while(level(vs)<lowline) {
			board_t vs2;
			for(;;) {
				vs2=vs;
				int mi,mj;
				mi=get_randomer().randint(0,N);
				mj=get_randomer().randint(0,N);
				vs2[mi][mj]='-';
				board_t vs3(vs2);
				covter<N> cvt(vs3,false,true);
				if(cvt.get_numof_solutions()==1) break;
			}
			vs=vs2;
		}
		return vs;
	}
};

#endif
