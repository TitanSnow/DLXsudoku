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
#include<queue>
#include<cstdint>

#ifndef SUDOKU_CHOOSER_H
#define SUDOKU_CHOOSER_H

template<typename cov_t> typename cov_t::steps_t choose_least_guessing(cov_t& cvt) {
	typename cov_t::solution_tree_node_ptr p(cvt.get_solution_tree());
	struct status {
		typename cov_t::solution_tree_node_ptr cur;
		std::uint_fast64_t c_gs;
		status() {}
		status(typename cov_t::solution_tree_node_ptr cur,std::uint_fast64_t c_gs):cur(cur),c_gs(c_gs) {}
	};
	std::uint_fast64_t min_gs=UINT_FAST64_MAX;
	std::queue<status> q;
	typename cov_t::steps_t ans;
	q.push(status(p,1));
	while(!q.empty()) {
		status sa=q.front();q.pop();
		if(sa.cur->p_solution.get()) {
			if(sa.c_gs<min_gs) {
				min_gs=sa.c_gs;
				ans=*(sa.cur->p_solution);
			}
		}
		for(typename std::vector<typename cov_t::solution_tree_node_ptr>::size_type i=0; i!=sa.cur->children.size(); ++i)
			q.push(status(sa.cur->children[i],sa.c_gs*sa.cur->children.size()));
	}
	return ans;
}

#endif