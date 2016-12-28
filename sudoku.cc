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

#include<cstdlib>
#include"sudoku.h"

handler::~handler() {}

dlx::dlx():c_line(0),IGN((event_listener_t)0) {
	event_listener=IGN;
	root.l=&root;
	root.r=&root;
	root.u=&root;
	root.d=&root;
	root.c=&root;
	root.s=0;
	root.line=0;
}
dlx::~dlx() {
	for(node* i=root.r; i!=&root;) {
		for(node* j=i->d; j!=i;) {
			node* next=j->d;
			std::free(j);
			j=next;
		}
		node* next=i->r;
		std::free(i);
		i=next;
	}
}
node* dlx::add_col() {
	node* pn=(node*)std::malloc(sizeof(node));
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
int dlx::add_row(const std::vector<node*>& vs) {
	node* first=NULL;
	node* pre=NULL;
	++c_line;
	for(std::vector<node*>::const_iterator it(vs.begin()); it!=vs.end(); ++it) {
		node* pn=(node*)std::malloc(sizeof(node));
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
void dlx::add_event_listener(event_listener_t el) {
	event_listener=el;
}
void dlx::rmcol(node* c) {
	c->l->r=c->r;
	c->r->l=c->l;
	for(node* i=c->d; i!=c; i=i->d)
		for(node* j=i->r; j!=i; j=j->r) {
			j->u->d=j->d;
			j->d->u=j->u;
			j->c->s-=1;
		}
}
void dlx::recol(node* c) {
	for(node* i=c->u; i!=c; i=i->u)
		for(node* j=i->l; j!=i; j=j->l) {
			j->c->s+=1;
			j->d->u=j;
			j->u->d=j;
		}
	c->r->l=c;
	c->l->r=c;
}
bool dlx::solve() {
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
void dlx::dfs_solution_tree(bool dfs_col,bool dfs_row) {
	if(root.r==&root) {
		if(event_listener!=IGN) event_listener->operator()(dep_end_success,0);
		if(dfs_row) return;
		throw success_exception();
	}

	node* c=root.r;
	if(!dfs_col)
		for(node* i=root.r; i!=&root; i=i->r)
			if(i->s<c->s)
				c=i;

	bool b_throw=false;
	for(int i=0; (dfs_col?c!=&root:i!=1); ++i,c=c->r) {
		rmcol(c);
		if(dfs_col&&event_listener!=IGN) event_listener->operator()(select_col,0);
		bool continue_loop=true;
		for(node* i=c->d; i!=c&&continue_loop; i=i->d) {
			for(node* j=i->r; j!=i; j=j->r) rmcol(j->c);
			if(event_listener!=IGN) event_listener->operator()(select_line,i->line);
			try{
				dfs_solution_tree(dfs_col,dfs_row);
			} catch (success_exception) {
				continue_loop=false;
			}
			for(node* j=i->l; j!=i; j=j->l) recol(j->c);
			if(event_listener!=IGN) event_listener->operator()(unselect_line,i->line);
		}
		recol(c);
		if(dfs_col&&event_listener!=IGN) event_listener->operator()(unselect_col,0);

		if(!continue_loop)
			b_throw=true;
	}
	if(!b_throw) return;
	throw success_exception();
}
