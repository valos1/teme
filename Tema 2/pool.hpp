#pragma once
#include<vector>
#include<cassert>
template<typename T, unsigned size>
class stack_pool{
	public:
		stack_pool(){//initial, toti indicii sunt liberi
			for(unsigned i{0}; i < size; ++i)
				free_indices[i] = i;
		}
		template<typename ...U>//variadic template folosit pentru a transfera argumentele unui constructor al clasei T
		T* allocate(U&&... args){
			assert(occupied < size);
			++occupied;
			data[free_indices[last_index_plus_one - 1]] = T{std::forward<U>(args)...};//forward tine cont de tipul valorii fiecarui argument, lvalue sau rvalue
			return data + free_indices[--last_index_plus_one];
		}
		void deallocate(T* ptr){//se adauga in lista de indici liberi indexul elementului ce va fi dealocat
			--occupied;
			free_indices[last_index_plus_one++] = ptr - data;//indexul se obtine prin diferenta intre adresa elementului
		}//sters si cea a primului element
	private:
		unsigned last_index_plus_one{size}, occupied{0}, free_indices[size];
		T data[size];
};

template<typename T>
class heap_pool{
	public:
		explicit heap_pool(unsigned const size){//se prealoca memoria necesara pentru data, fara sa se cheme constructori
			data.reserve(size);
			free_indices.reserve(size);
			for(unsigned i{size - 1}; i > 0; --i)//se adauga indicii in ordine inversa pentru ca atunci cand ocup un index din data,
				free_indices.push_back(i);//free_indices.back() imi va da ordinea crescatoare a indicilor, ca la o stiva.
			free_indices.push_back(0);//Cu pop_back(), sterg un index din free_indices.
		}//pop_front ar fi mutat tot continutul lui free_indices o pozitie la stanga.
		template<typename ...U>
		T* allocate(U&&... args){//transmit argumentele la un constructor al lui T
			assert(free_indices.size() > 0);
			unsigned const index{free_indices.back()};//folosesc un index liber
			free_indices.pop_back();
			if(index == data.size())//daca acest index nu s-a mai folosit, creez obiectul in interiorul memoriei alocate in constructor
				data.emplace_back(std::forward<U>(args)...);
			else
				data[index] = T{std::forward<U>(args)...};//altfel, creez obiectul in afara si folosesc operator=(T&&) dupa pentru a muta continutul
			return &data[index];
		}
		void deallocate(T* ptr){//se adauga in lista de indici liberi indexul elementului ce va fi dealocat
			free_indices.push_back(ptr - &data[0]);
		}
	private:
		std::vector<T>data;
		std::vector<unsigned>free_indices;
};