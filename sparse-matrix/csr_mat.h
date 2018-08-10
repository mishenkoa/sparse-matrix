//  
// Created  : 10.05.2016
// Author   : Veyroter
// � AshyCat, 2016 
//  
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
//---------------------------------- Typedefs

    typedef comp_array<TIdx, TIdx>  cm_array_t;
    typedef shared_array<TVal>      val_array_t;
    typedef shared_array<TIdx>      idx_array_t;

//---------------------------------- Nested structures
public:
	struct csr_data {

        TIdx        i;
        TIdx        j;
		TVal        val;

		            csr_data    ( TIdx i, TIdx j, TVal val ) : i( i ), j( j ), val( val ) {}
		auto        to_string   ()      sparse_ret ( format( "(%1%, %2%, %3%)" ) % i % j % val )
	};
	
//---------------------------------- Data
private:
    TIdx			rows;
    TIdx            cols;
	TIdx			nnz;
    idx_array_t	    ja;		    // column numbers for elements a
    cm_array_t *    ia;	        // elements count
    val_array_t     a;		    // matrix elements
	TIdx			size_a;     // size of shared array a

//---------------------------------- Constructors
public:
    template < typename TContainer > // TContainer : iterable< csr_data >
	                csr_mat     ( TIdx rows, TIdx cols, TContainer& pairs, bool sorted = false);

//---------------------------------- Methods

	TVal            get         ( TIdx row, TIdx col ) const;
    ICF TVal*	    get_ref     ( TIdx row, TIdx col ) const;
    self_t&		    transp      ();
	self_t&         add         ( const self_t& a );
	self_t&         sub         ( const self_t& a );
	self_t&         mul         ( const self_t& a );
	bool            eq          ( const self_t& a );
	self_t&         out         ( std::ostream& ostream ) const;
	self_t&		    operator=   ( const self_t& a );

	ICF void	    iterate     ( function< void ( csr_data ) > func ) const;

private:
	ICF TIdx	    index       ( const csr_data& a ) const;
};

#include "csr_mat_inl.h"

#undef self_t
#undef _TMPL_DECL_