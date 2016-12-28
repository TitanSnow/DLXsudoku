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

#include<vector>
#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<stdexcept>
#include<iterator>
#include<memory>

#ifndef SUDOKU_H
#define SUDOKU_H

struct handler {
	virtual void operator() (int,int) =0;
	virtual ~handler();
};

struct node {
	node *l,*r,*u,*d,*c;
	int s,line;
};

class dlx {
	node root;
	int c_line;
public:
	enum event
		{
			select_line,
			unselect_line,
			dep_end_success,
			loop_end
		};
	typedef handler *event_listener_t;
	const event_listener_t IGN;
private:
	event_listener_t event_listener;
public:
	dlx();
	~dlx();
	node* add_col();
	int add_row(const std::vector<node*>& vs);
	void add_event_listener(event_listener_t el);
private:
	void rmcol(node* c);
	void recol(node* c);
public:
	bool solve();
	void dfs_solution_tree();
};

template<int N> class covter {
public:
	typedef std::vector<std::vector<std::string> > solution_t;
	struct solution_tree_node_t;
	typedef std::shared_ptr<solution_tree_node_t> solution_tree_node_ptr;
	struct solution_tree_node_t {
		std::auto_ptr<solution_t> p_solution;
		solution_tree_node_ptr parent;
		std::vector<solution_tree_node_ptr> children;
	};
private:
	const int subN;
	dlx solver;
	node* mpC[4][N][N];
	struct decision {
		int i,j;
		char t;
		decision() {}
		decision(int ni,int nj,char nt):i(ni),j(nj),t(nt) {}
	};
	std::vector<decision> dcs;
	int get_sub(int i,int j) const{
		return (i/subN)*subN+j/subN;
	}
	char vs[N][N+1];
	solution_t steps;
	std::vector<std::string> get_this_step() const{
		std::vector<std::string> vs(N);
		for(int i=0; i!=N; ++i)
			std::copy(this->vs[i],this->vs[i]+N,std::back_inserter(vs[i]));
		return vs;
	}
	const bool record_step;
	const bool dfs_all;
	int c_ans;
	solution_tree_node_ptr ptroot;
	solution_tree_node_ptr ptcur;
	void event_listener(int e,int line) {
		if(record_step&&!dfs_all)
			switch(e) {
			case dlx::select_line: {
				const decision& dc=dcs[line];
				vs[dc.i][dc.j]=dc.t;
				steps.push_back(get_this_step());
			}
			break;
			case dlx::unselect_line: {
				steps.pop_back();
			}
			break;
			}
		else if(!record_step)
			switch(e) {
			case dlx::select_line: {
				const decision& dc=dcs[line];
				vs[dc.i][dc.j]=dc.t;
			}
			break;
			case dlx::dep_end_success: {
				++c_ans;
			}
			break;
			}
		else
			switch(e) {
			case dlx::select_line: {
				const decision& dc=dcs[line];
				vs[dc.i][dc.j]=dc.t;
				steps.push_back(get_this_step());
				ptcur->children.push_back(solution_tree_node_ptr());
				ptcur->children.back()->parent=ptcur;
				ptcur=ptcur->children.back();
			}
			break;
			case dlx::unselect_line: {
				steps.pop_back();
			}
			break;
			case dlx::loop_end: {
				ptcur=ptcur->parent;
			}
			break;
			case dlx::dep_end_success: {
				ptcur->p_solution.reset(new solution_t(steps));
				++c_ans;
			}
			break;
			}
	}
	struct my_handler:public handler {
	private:
		covter<N>* pare;
	public:
		void operator() (int p1,int p2) {
			pare->event_listener(p1,p2);
		}
		friend class covter<N>;
	};
	friend class my_handler;
public:
	explicit covter(std::vector<std::string>& vs,bool record_step=true,bool dfs_all=false):subN(std::sqrt(N)),record_step(record_step),dfs_all(dfs_all),c_ans(0),ptroot(new solution_tree_node_t) {
		ptcur=ptroot;

		if(subN*subN!=N) throw std::invalid_argument("covter: N is not a square");
		if(vs.size()!=N) throw std::invalid_argument("covter: vector size != N");
		for(int i=0; i!=N; ++i) {
			if(vs[i].size()!=N) throw std::invalid_argument("covter: string size != N");
			for(int j=0; j!=N; ++j)
				if(vs[i][j]!='-'&&(vs[i][j]<'A'||vs[i][j]>='A'+N))
					throw std::invalid_argument("covter: invalid char");
		}

		std::memset(mpC,0,sizeof(mpC));
		for(int i=0; i!=N; ++i)
			std::strcpy(this->vs[i],vs[i].c_str());

		for(int i=0; i!=N; ++i)
			for(int j=0; j!=N; ++j)
				if(vs[i][j]=='-') {
					node* t=solver.add_col();
					mpC[0][i][j]=t;
				}

		for(int i=0; i!=N; ++i) {
			bool a[100];
			std::memset(a,0,sizeof(a));
			for(int j=0; j!=N; ++j)
				a[(std::size_t)vs[i][j]]=true;
			for(int t='A'; t!='A'+N; ++t)
				if(!a[t]) {
					node* tt=solver.add_col();
					mpC[1][i][t-'A']=tt;
				}
		}

		for(int j=0; j!=N; ++j) {
			bool a[100];
			std::memset(a,0,sizeof(a));
			for(int i=0; i!=N; ++i)
				a[(std::size_t)vs[i][j]]=true;
			for(int t='A'; t!='A'+N; ++t)
				if(!a[t]) {
					node* tt=solver.add_col();
					mpC[2][j][t-'A']=tt;
				}
		}

		for(int i=0; i+subN<=N; i+=subN)
			for(int j=0; j+subN<=N; j+=subN) {
				bool a[100];
				std::memset(a,0,sizeof(a));
				for(int i1=i; i1!=i+subN; ++i1)
					for(int j1=j; j1!=j+subN; ++j1)
						a[(std::size_t)vs[i1][j1]]=true;
				for(int t='A'; t!='A'+N; ++t)
					if(!a[t]) {
						node* tt=solver.add_col();
						mpC[3][get_sub(i,j)][t-'A']=tt;
					}
			}

		for(int i=0; i!=N; ++i)
			for(int j=0; j!=N; ++j)
				if(vs[i][j]=='-')
					for(int t='A'; t!='A'+N; ++t) {
						std::vector<node*> r;
						if(mpC[0][i][j]!=0) r.push_back(mpC[0][i][j]);
						else continue;
						if(mpC[1][i][t-'A']!=0) r.push_back(mpC[1][i][t-'A']);
						else continue;
						if(mpC[2][j][t-'A']!=0) r.push_back(mpC[2][j][t-'A']);
						else continue;
						if(mpC[3][get_sub(i,j)][t-'A']!=0) r.push_back(mpC[3][get_sub(i,j)][t-'A']);
						else continue;
						int line=solver.add_row(r);
						dcs.resize(std::max(line+1,(int)dcs.size()));
						dcs[line]=decision(i,j,t);
					}

		my_handler el;
		el.pare=this;
		solver.add_event_listener(&el);
		if(!dfs_all)
			solver.solve();
		else
			solver.dfs_solution_tree();

		for(int i=0; i!=N; ++i)
			std::copy(this->vs[i],this->vs[i]+N,vs[i].begin());
	}

	const solution_t& get_steps() const{
		if(!record_step||dfs_all)
			throw std::logic_error("covter: has not recorded steps or recorded steps as a tree");
		return steps;
	}
	int get_numof_solutions() const{
		if(!dfs_all)
			throw std::logic_error("covter: has not dfs solution tree");
		return c_ans;
	}
	solution_tree_node_ptr get_solution_tree() {
		if(!ptroot)
			throw std::logic_error("covter: \"get_solution_tree\" cannot be called twice");
		solution_tree_node_ptr root(ptroot);
		ptroot=0;
		ptcur=0;
		return root;
	}
};

#endif
