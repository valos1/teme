#pragma once
#include<cassert>
#include<type_traits>
#include<initializer_list>
#include<limits>
#include<istream>
template<typename T, unsigned rows, unsigned cols>
class matrix{
	public:
		static_assert(std::is_arithmetic_v<T>, "type must be integral or floating-point");//daca T nu e numar intreg sau fractionar, eroare de compilare
		constexpr matrix():matrix(T{}){}//constructor delegat
		constexpr explicit matrix(T const val):data{}{//la fel ca mai sus, doar ca se da ca argument valoarea cu care se vor initializa elementele
			for(unsigned i{0}; i < rows; ++i)//explicit inseamna ca nu se va face conversie implicita de la un numar la o matrice, deci daca
				for(unsigned j{0}; j < cols; ++j)//f ia ca argument o matrice de int si 5 linii si coloane, nu se va putea apela f(val); ,
					data[i][j] = val;//ci f(matrix<int, 5, 5>(val));
		}
		constexpr matrix(std::initializer_list<T>vals):data{}{//se insereaza rows * cols numere in matrice
			assert(vals.size() == rows * cols);
			auto it{vals.begin()};//numarul de elemente trebuie sa fie rows * cols
			for(unsigned i{0}; i < rows; ++i)
				for(unsigned j{0}; j < cols; ++j)
					data[i][j] = *it++;
		}
		template<typename U>
		matrix(matrix<U, rows, cols>const&other){//se poate initializa cu o matrice de alt tip, daca nu se face narrowing conversion, sa nu se piarda biti
			static_assert(std::is_same_v<T, double> || std::is_same_v<T, long double> ||
				std::is_same_v<T, float> && !std::is_same_v<U, double> && !std::is_same_v<U, long double>||
				std::is_integral_v<T> && std::is_integral_v<U> &&
				std::numeric_limits<T>::min() <= std::numeric_limits<U>::min() && 
				std::numeric_limits<T>::max() >= std::numeric_limits<U>::max(), "narrowing conversions not allowed");
			for(unsigned i{0}; i < rows; ++i)
				for(unsigned j{0}; j < cols; ++j)
					data[i][j] = other(i, j);
		}
		constexpr unsigned no_rows()const{//evaluat la compilare
			return rows;
		}
		constexpr unsigned no_cols()const{//evaluat la compilare
			return cols;
		}
		T& operator()(unsigned const row, unsigned const col){
			return data[row][col];
		}
		T const& operator()(unsigned const row, unsigned const col) const{//pentru obiecte constante care nu sunt marcate mutable, trebuie returnata referinta constanta,
			return data[row][col];//altfel s-ar putea modifica obiectul prin o referinta neconstanta
		}
		void transpose(){// se inverseaza pozitiile elementelor simetrice fata de diagonala principala
			static_assert(rows == cols, "non-square stack-allocated matrix cannot be transposed");//trebuie sa fie patratica
			for(unsigned i{0}; i < rows; ++i)
				for(unsigned j{i + 1}; j < cols; ++j)
					std::swap(data[i][j], data[j][i]);
		}
		void invert(){//inversare matrice
			static_assert(rows == cols, "matrix not invertable");//trebuie sa fie patratica
	    	T const det {determinant(*this)};//este nevoie de argument pentru a putea apela recursiv
	    	assert (det != 0);
	    	matrix<T, rows, rows>adjoint;//adjuncta are aceleasi dimensiuni ca matricea originala
	    	compute_adjoint(adjoint);
	    	for (unsigned i{0}; i < rows; ++i)
	        	for (unsigned j{0}; j < rows; ++j) 
	         	   data[i][j] = adjoint(i, j) / static_cast<double>(det);//se face narrowing conversion pe fiecare element daca T este intreg
		}
		template<typename U, typename Q, unsigned rows1, unsigned cols1>
		friend constexpr auto operator+(matrix<Q, rows1, cols1> const&, matrix<U, rows1, cols1> const&);//se da acces la array-ul data pentru ca operator()
		template<typename U, typename Q, unsigned rows1, unsigned cols1, unsigned cols2>//nu poate fi folosit intr-un context constexpr
		friend constexpr auto operator*(matrix<Q, rows1, cols1> const&, matrix<U, cols1, cols2> const&);//la fel
	private:
		template<unsigned size>
		void submatrix(matrix<T, size, size> const&full, matrix<T, size - 1, size - 1>& sub, unsigned const r, unsigned const c)const{ 
    		unsigned i{0}, j{0};//se insereaza in sub submatricea lui full care nu contine linia r si coloana c
	    	for (unsigned row {0}; row < size; ++row)
	        	for (unsigned col {0}; col < size; ++col)
	            	if (row != r && col != c){
	                	sub(i, j++) = full(row, col);
	                	if (j == size - 1){ 
	                    	j = 0;
	                    	++i;
	                	}
	            }
	    }
	    template<unsigned size>
		T determinant(matrix<T, size, size> const& full)const{//se da size ca parametru pentru a fi dedus
			if constexpr(size == 1)
	        	return full(0, 0);
	    	else{
	    		T det{};
	    		matrix<T, size - 1, size - 1> temp;
	    		signed char sign {1};
	    		for (unsigned i {0}; i < size; ++i){
	        		submatrix(full, temp, 0, i);
	        		det += sign * full(0, i) * determinant(temp);//algoritmul e recursiv, dar fiind pe o valoare stiuta la compilare,
	        		sign = -sign;//recursia se face pe parametrul size, in loc sa se dea inca un argument functiei. se vor face instantieri recursive 
	    		}//de functii la compilare. conditia de oprire a recursiei pe parametrul size se face prin if constexpr, care compileaza doar codul ce corespunde
	   		return det;//conditiei constante size == 1. O alternativa ar fi fost sa fac o specializare pentru intreaga functie cu argumentul matrix<T, 1, 1>const&
			}//dar asa pare mai clean
		}
		void compute_adjoint(matrix& adjoint)const{//se calculeaza adjuncta
	    	if constexpr(adjoint.no_rows() == 1){
	        	adjoint(0, 0) = 1;
	        	return;
	    	}
	    	else{
	    		signed char sign;
	    		matrix<T, adjoint.no_rows() - 1, adjoint.no_rows() - 1>temp; 
	    		for (unsigned i{0}; i < adjoint.no_rows(); ++i) 
	        		for (unsigned j{0}; j < adjoint.no_rows(); ++j){ 
	            		submatrix(*this, temp, i, j);
	            		sign = ((i + j) & 1) == 0? 1: -1;//& 1 da ultimul bit al lui i + j, adica restul impartirii la 2
	            		adjoint(j, i) = sign * determinant(temp);
	        		}
	    	}
		}
		T data[rows][cols];
};

template<typename U, typename Q, unsigned rows1, unsigned cols1>// validarea dimensiunilor se face prin parametri template (acelasi nr de linii si coloane)
constexpr auto operator+(matrix<Q, rows1, cols1> const& first, matrix<U, rows1, cols1> const& second){//pun auto ca sa nu mai scriu tot tipul
	matrix<decltype(U{} + Q{}), rows1, cols1> result;//tipul elementelor rezultante e dat de tipul expresiei neevaluate U{} + Q{}, prin decltype
	for(unsigned i{0}; i < rows1; ++i)
		for(unsigned j{0}; j < cols1; ++j)
			result.data[i][j] = first.data[i][j] + second.data[i][j];
	return result;
}

template<typename U, typename Q, unsigned rows1, unsigned cols1, unsigned cols2>//numarul de coloane de la prima e numarul de linii de la a doua
constexpr auto operator*(matrix<Q, rows1, cols1> const& first, matrix<U, cols1, cols2> const& second){
	matrix<decltype(U{} * Q{}), rows1, cols2> result;//tipul elementelor rezultante e dat de tipul expresiei neevaluate U{} * Q{}, prin decltype
	for(unsigned i{0}; i < rows1; ++i)//pentru result, numarul de linii este cel de la first si cel de coloane e cel de al second
		for(unsigned j{0}; j < cols2; ++j)
			for(unsigned k{0}; k < cols1; ++k)
				result.data[i][j] += first.data[i][k] * second.data[k][j];
	return result;
}

template<typename T, unsigned rows1, unsigned cols1>
std::istream& operator>>(std::istream& in, matrix<T, rows1, cols1>& mat){
	for(unsigned i{0}; i < rows1; ++i)
		for(unsigned j{0}; j < cols1; ++j)
			in >> mat (i, j);
	return in;
}

template<typename T, unsigned rows1, unsigned cols1>
std::ostream& operator<<(std::ostream& out, matrix<T, rows1, cols1> const& mat){
	for(unsigned i{0}; i < rows1; ++i){
		for(unsigned j{0}; j < cols1; ++j)
			out << mat (i, j) <<' ';
		out<<'\n';
	}
	return out;
}