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

using namespace std;

struct handler {
	virtual void operator() (int,int) =0;
	virtual ~handler() {}
};

struct node {
	node *l,*r,*u,*d,*c;
	int s,line;
};

#include<cstdlib>
#include<vector>
class dlx {
	node root;
	int c_line;
public:
	enum event {select_line,unselect_line};
	typedef handler *event_listener_t;
	const event_listener_t IGN;
private:
	event_listener_t event_listener;
public:
	dlx():c_line(0),IGN((event_listener_t)0) {
		event_listener=IGN;
		root.l=&root;
		root.r=&root;
		root.u=&root;
		root.d=&root;
		root.c=&root;
		root.s=0;
		root.line=0;
	}
	~dlx() {
		// of course it is needed, however, i just don't wanna write it
	}
	node* add_col() {
		node* pn=(node*)malloc(sizeof(node));
		pn->l=root.l;
		pn->r=&root;
		pn->u=pn;
		pn->d=pn;
		pn->c=pn;
		pn->s=0;
		root.l->r=pn;
		root.l=pn;
		pn->line=0;
		return pn;
	}
	int add_row(const vector<node*>& vs) {
		node* first=NULL;
		node* pre=NULL;
		++c_line;
		for(vector<node*>::const_iterator it(vs.begin()); it!=vs.end(); ++it) {
			node* pn=(node*)malloc(sizeof(node));
			if(first==NULL)
				first=pn;
			pn->l=pre;
			if(pre!=NULL)
				pre->r=pn;
			pn->u=(*it)->u;
			pn->d=*it;
			(*it)->u->d=pn;
			(*it)->u=pn;
			pn->line=c_line;
			pn->c=*it;
			pn->s=0;
			(*it)->s+=1;
			pre=pn;
		}
		first->l=pre;
		pre->r=first;
		return c_line;
	}
	void add_event_listener(event_listener_t el) {
		event_listener=el;
	}
private:
	void rmcol(node* c) {
		c->l->r=c->r;
		c->r->l=c->l;
		for(node* i=c->d; i!=c; i=i->d)
			for(node* j=i->r; j!=i; j=j->r) {
				j->u->d=j->d;
				j->d->u=j->u;
				j->c->s-=1;
			}
	}
	void recol(node* c) {
		for(node* i=c->u; i!=c; i=i->u)
			for(node* j=i->l; j!=i; j=j->l) {
				j->c->s+=1;
				j->d->u=j;
				j->u->d=j;
			}
		c->r->l=c;
		c->l->r=c;
	}
public:
	bool solve() {
		if(root.r==&root)
			return true;

		node* c=root.r;
		for(node* i=root.r; i!=&root; i=i->r)
			if(i->s<c->s)
				c=i;

		rmcol(c);
		for(node* i=c->d; i!=c; i=i->d) {
			for(node* j=i->r; j!=i; j=j->r) rmcol(j->c);
			if(event_listener!=IGN) event_listener->operator()(select_line,i->line);
			if(solve()) return true;
			for(node* j=i->l; j!=i; j=j->l) recol(j->c);
			if(event_listener!=IGN) event_listener->operator()(unselect_line,i->line);
		}
		recol(c);

		return false;
	}
};

#include<string>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<stdexcept>
#include<iterator>
template<int N> class covter {
	const int subN;
	dlx solver;
	node* mpC[4][N][N];
	struct decision {
		int i,j;
		char t;
		decision() {}
		decision(int ni,int nj,char nt):i(ni),j(nj),t(nt) {}
	};
	vector<decision> dcs;
	int get_sub(int i,int j) {
		return (i/subN)*subN+j/subN;
	}
	char vs[N][N+1];
	vector<vector<string> > steps;
	vector<string> get_this_step() {
		vector<string> vs(N);
		for(int i=0; i!=N; ++i)
			copy(this->vs[i],this->vs[i]+N,back_inserter(vs[i]));
		return vs;
	}
	void event_listener(int e,int line) {
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
	covter(vector<string>& vs):subN(sqrt(N)) {
		if(subN*subN!=N) throw invalid_argument("covter: N is not a square");

		memset(mpC,0,sizeof(mpC));
		for(int i=0; i!=N; ++i)
			strcpy(this->vs[i],vs[i].c_str());

		for(int i=0; i!=N; ++i)
			for(int j=0; j!=N; ++j)
				if(vs[i][j]=='-') {
					node* t=solver.add_col();
					mpC[0][i][j]=t;
				}

		for(int i=0; i!=N; ++i) {
			bool a[100];
			memset(a,0,sizeof(a));
			for(int j=0; j!=N; ++j)
				a[vs[i][j]]=true;
			for(int t='A'; t!='A'+N; ++t)
				if(!a[t]) {
					node* tt=solver.add_col();
					mpC[1][i][t-'A']=tt;
				}
		}

		for(int j=0; j!=N; ++j) {
			bool a[100];
			memset(a,0,sizeof(a));
			for(int i=0; i!=N; ++i)
				a[vs[i][j]]=true;
			for(int t='A'; t!='A'+N; ++t)
				if(!a[t]) {
					node* tt=solver.add_col();
					mpC[2][j][t-'A']=tt;
				}
		}

		for(int i=0; i+subN<=N; i+=subN)
			for(int j=0; j+subN<=N; j+=subN) {
				bool a[100];
				memset(a,0,sizeof(a));
				for(int i1=i; i1!=i+subN; ++i1)
					for(int j1=j; j1!=j+subN; ++j1)
						a[vs[i1][j1]]=true;
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
						vector<node*> r;
						if(mpC[0][i][j]!=0) r.push_back(mpC[0][i][j]);
						else continue;
						if(mpC[1][i][t-'A']!=0) r.push_back(mpC[1][i][t-'A']);
						else continue;
						if(mpC[2][j][t-'A']!=0) r.push_back(mpC[2][j][t-'A']);
						else continue;
						if(mpC[3][get_sub(i,j)][t-'A']!=0) r.push_back(mpC[3][get_sub(i,j)][t-'A']);
						else continue;
						int line=solver.add_row(r);
						dcs.resize(max(line+1,(int)dcs.size()));
						dcs[line]=decision(i,j,t);
					}

		my_handler el;
		el.pare=this;
		solver.add_event_listener(&el);
		solver.solve();

		for(int i=0; i!=N; ++i)
			copy(this->vs[i],this->vs[i]+N,vs[i].begin());
	}

	const vector<vector<string> >& get_steps() {
		return steps;
	}
};

#include<iostream>
#include<cctype>
class wrapper {
	const vector<string>* pvs;
	void pr() {
		const vector<string>& vs=*pvs;
		for(int i=0; i!=9; ++i) {
			for(int j=0; j!=9; ++j)
				cout.put(isupper(vs[i][j])?vs[i][j]-16:vs[i][j]);
			cout.put('\n');
		}
		//cout.flush();
	}
public:
	wrapper() {
		ios::sync_with_stdio(false);
		//cin.tie(0);
		int kase=0;
		while(1) {
			vector<string> vs;
			this->pvs=&vs;
			for(int i=0; i!=9; ++i) {
				string st;
				do {
					cin>>st;
					if(!cin) {
						if(st.size()==9) break;
						return;
					}
				} while(st.size()!=9);
				for(int i=0;i!=9;++i)
					if(isdigit(st[i]))st[i]+=16;
				vs.push_back(st);
			}
			covter<9> cvt(vs);
			if(kase++!=0) cout.put('\n');
			pr();
			char ch;
			while(cin.get(ch)&&isspace(ch));
			if(!cin) return;
			if(ch!='?')
				cin.unget();
			else {
				cin.get();
				if(!cin) return;
				const vector<vector<string> >& steps=cvt.get_steps();
				for(int i=0; i!=steps.size(); ++i) {
					this->pvs=&(steps[i]);
					pr();
					cout<<"--PRESS-----\n";
					cin.get();
					if(!cin) return;
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
