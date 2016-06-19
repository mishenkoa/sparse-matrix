//  
// Created  : 10.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once

#include <boost/container/allocator_traits.hpp>
#include "types.h"
#include <map>

using namespace boost::container;

template<typename TVal, typename TIdx>
struct comp_array {
//---------------------------------- Typedefs

	typedef comp_array<TVal, TIdx> self_t;

//---------------------------------- Nested structures

	struct comp_iterator {
		TIdx i = -1, 
			 j = -1;
		self_t * arr;

		comp_iterator(self_t * arr) : arr(arr) { }

		std::pair<TIdx, TVal> current() {
			return std::pair<TIdx, TVal>(j, arr->arr[i]);
		}
		
		// returns false when it ends
		bool next() {
			++i;
			auto find = arr->comp_elms.find(j);
			j += (find != arr->comp_elms.end()) ? find->second : 1;
			return !end();
		}

		bool end() {
			return j >= arr->size_real;
		}
	};

	struct uncomp_iterator {
		TIdx i = -1, 
			 j = -1, 
			 strike = 0;
		self_t * arr;

		uncomp_iterator(self_t * arr) : arr(arr) { }

		std::pair<TIdx, TVal> current() {
			return std::pair<TIdx, TVal>(j, arr->arr[i]);
		}

		// returns false when it ends
		bool next() {
			++j;
			if (strike > 0) {
				--strike;
			} else {
				++i;
				auto find = arr->comp_elms.find(j);
				if (find != arr->comp_elms.end()) {
					strike = find->second - 1;
				} 
			}
			return !end();
		}

		bool end() {
			return j >= arr->size_real;
		}
	};

//---------------------------------- Data
private:
	TVal *					arr;			// values
	TIdx					size_real;		// real size of array
	TIdx					size_comp;		// compressed size of array
	std::map<TIdx, TIdx>	comp_elms;		// elemets what was compressed

//---------------------------------- Constructors
public:
	comp_array(TIdx size, TVal * full_array);
//---------------------------------- Methods

	template < typename TContainer, typename TMap >
	static TVal * comp_arr(TIdx size, TContainer const& full_array, TMap const& map);
	template < typename F > 
	ICF void				iterate(F&& func) const;		// func : pair<TIdx : index, TVal : value> -> void
	self_t&					out( std::ostream& ostream) const;
	TVal					operator[](const TIdx idx) const;
	comp_iterator			get_comp_iterator()				{ return comp_iterator(this); }
	uncomp_iterator			get_uncomp_iterator()			{ return uncomp_iterator(this); }
	size_t					get_uncompressed_size() const	{ return size_real * 4; }					// in bytes
	size_t					get_size() const				{ return sizeof(*this) + 4 * size_comp; }	// in bytes
};


#include "comp_array_inl.h"