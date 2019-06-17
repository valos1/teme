#include"matrix.hpp"
#include<iostream>
#include<fstream>
int main(){
	std::ios_base::sync_with_stdio(false);//viteza de executie
	std::ifstream file{"matrice"};
	matrix<int, 2,2>f;
	matrix<float, 3,3>a;
	file >> a;
	std::cout<<a;
	constexpr matrix<int, 3,3>b{
		2,2,2,
		0,1,2,
		2,3,0};

	constexpr matrix<int, 2,3>c{
		1,2,3,
		0,1,5};
	constexpr matrix<int, 3,2>d{
		4,2,
		1,6,
		2,7};

	int e;
	do{
		std::cout<<"1:transpunere matrice a\n2:inversare matrice a\n3:adunare matrice a si b\n4:inmultire matrice c si d\nInput:";
		std::cin>>e;
		switch(e){
			case 1:a.transpose();
			for(int i{0};i < a.no_rows();++i){
				for(int j{0};j < a.no_cols();++j)
					std::cout<<a(i, j)<<' ';
				std::cout<<'\n';}
			break;
			case 2:
			a.invert();
			for(int i{0};i < a.no_rows();++i){
				for(int j{0};j < a.no_cols();++j)
					std::cout<<a(i, j)<<' ';
				std::cout<<'\n';}
			break;
			case 3:
			a=a+b;
			for(int i{0};i < a.no_rows();++i){
				for(int j{0};j < a.no_cols();++j)
					std::cout<<a(i, j)<<' ';
				std::cout<<'\n';}
			break;
			case 4:
			f=c*d;
			for(int i{0};i < f.no_rows();++i){
				for(int j{0};j < f.no_cols();++j)
					std::cout<<f(i, j)<<' ';
				std::cout<<'\n';}
			break;
			default:std::cout<<"invalid input";
			break;
		}
		std::cout<<"\n\n";
	}while(e!=0);
	std::cout<<"constexpr multiply:\n";
	constexpr matrix<int,2,2>g{c*d};
	for(int i{0};i < g.no_rows();++i){
		for(int j{0};j < g.no_cols();++j)
			std::cout<<g(i, j)<<' ';
		std::cout<<'\n';
	}
}