#include"pool.hpp"
#include<iostream>
#include<memory>
struct x{
	int i,j,f;
	x()=default;
	x(int i, int j, int f):i{i},j{j},f{f}{}
	x(int i, int j):i{i},j{j}{f=2;}
};
struct y{
	int f{1};
};
int main(){
	heap_pool<x>p(128);
	auto l = [&p](x* ptr){
			p.deallocate(ptr);
		};
	std::unique_ptr<x, decltype(l)> pt{p.allocate(1,2,3), l};//variadic template allocate
	std::cout<<pt->i<<' '<<pt->j<<' '<<pt->f<<'\n';

	stack_pool<x, 128>z;
	auto v = [&z](x* ptr){
			z.deallocate(ptr);
		};
	std::unique_ptr<x, decltype(v)> ptu{z.allocate(1,2), v};//variadic template allocate
	std::cout<<ptu->i<<' '<<ptu->j<<' '<<ptu->f<<'\n';

	stack_pool<y, 128>ll;
	auto vv = [&ll](y* ptr){
			ll.deallocate(ptr);
		};
	std::unique_ptr<y, decltype(vv)> ptq{ll.allocate(), vv};
	std::cout<<ptq->f;
}