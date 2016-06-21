#pragma once
#include <iomanip>
#include <boost/shared_array.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <map>
#include "types.h"
#include "comp_array.h"

using namespace boost;
using namespace container;

#define self_t			csr_mat< TVal, TIdx >
#define _TMPL_DECL_		template< typename TVal, typename TIdx >

_TMPL_DECL_ class csr_mat {

//---------------------------------- Nested structures
public:
	struct csr_data {
		TIdx i, j;
		TVal val;

		csr_data( TIdx i, TIdx j, TVal val ) : i( i ), j( j ), val( val ) {}
		auto to_string() { return format( "(%1%, %2%, %3%)" ) % i % j % val; }
	};
	
//---------------------------------- Data
private:
    TIdx				    rows;
    TIdx                    cols;
	TIdx				    nnz;
	shared_array <TIdx>	    ja;		// column numbers for elements a
	comp_array<TIdx, TVal>* ia;	    // elements count
	shared_array <TVal>     a;		// matrix elements
	TIdx				    size_a; // size of shared array a

//---------------------------------- Constructors
public:
	template< typename TContainer > // : TContainer< tuple< TIdx, TIdx, TVal > >
	csr_mat( TIdx rows, TIdx cols, TContainer& pairs, bool sorted = false);

//---------------------------------- Methods

	TVal        get( TIdx row, TIdx col ) const;
	self_t&		transp();
	self_t&     add( const self_t& a );
	self_t&     sub( const self_t& a );
	self_t&     mul( const self_t& a );
	bool        eq( const self_t& a );
	self_t&     out( std::ostream& ostream ) const;
	self_t&		operator=( const self_t& a );

	template < typename F >
	ICF void	iterate( F&& func ) const;

private:
	ICF TIdx	index( const csr_data& a ) const;
	ICF TVal*	get_ref( TIdx row, TIdx col ) const;
};

#include "csr_mat_inl.h"

#undef self_t
#undef _TMPL_DECL_