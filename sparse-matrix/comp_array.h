//  
// Created  : 25.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once

#include <boost/container/allocator_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/function.hpp>
#include "types.h"
#include <map>

using namespace boost::container;
using namespace boost;

template<typename TVal, typename TIdx>
struct comp_array {
//---------------------------------- Typedefs

    typedef comp_array<TVal, TIdx> self_t;

//---------------------------------- Nested structures

    struct comp_iterator {
        TIdx        i;
        TIdx        j;
        self_t *    arr;

        comp_iterator( self_t * arr ) : arr( arr ), i( -1 ), j( -1 ) { }

        // returns (idx, val)
        auto        current     ()  __ret( make_tuple( j, arr->arr[i] ) )
        bool        end         ()  __ret( j >= arr->size_real )

        // returns false when it ends
        bool        next        () {
            ++i;
            auto find = arr->comp_elms.find( j );
            j += ( find != arr->comp_elms.end() ) ? find->second : 1;
            return !end();
        }
    };

    // compressed iterator with previous element
    struct comp_iterator_prev {
        TIdx        i;
        TIdx        j;
        self_t *    arr;

        comp_iterator_prev( self_t * arr ) : arr( arr ), i( -1 ), j( -1 ) { }

        // returns (idx, val, previous val)
        auto        current     ()  __ret( make_tuple( j, arr->arr[i], arr->arr[i <= 0 ? i : i - 1] ) )
        bool        end         ()  __ret( j >= arr->size_real )

        // returns false when it ends
        bool        next        () {
            ++i;
            auto find = arr->comp_elms.find( j );
            j += ( find != arr->comp_elms.end() ) ? find->second : 1;
            return ! end();
        }
    };

    struct uncomp_iterator {
        TIdx        i;
        TIdx        j;
        TIdx        strike;
        self_t *    arr;

        uncomp_iterator ( self_t * arr ) : arr( arr ), i( -1 ), j( -1 ), strike( 0 ) { }

        // returns (idx, val)
        auto        current     ()  __ret( make_tuple( j, arr->arr[i] ) )
        bool        end         ()  __ret( j >= arr->size_real )

        // returns false when it ends
        bool        next        () {
            ++j;
            if( strike > 0 ) {
                --strike;
            } else {
                ++i;
                auto find = arr->comp_elms.find( j );
                if( find != arr->comp_elms.end() ) {
                    strike = find->second - 1;
                }
            }
            return !end();
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
                            comp_array              ( TIdx size, TVal * full_array );
                           ~comp_array              ();

//---------------------------------- Methods

    ICF void				iterate                 ( function<void ( TIdx, TVal )> func ) const;
    self_t&					out                     ( std::ostream& ostream ) const;
    TVal					operator[]              ( const TIdx idx ) const;
    comp_iterator			get_comp_iterator       ()          { return comp_iterator( this ); }
    comp_iterator_prev		get_comp_iterator_prev  ()          { return comp_iterator_prev( this ); }
    uncomp_iterator			get_uncomp_iterator     ()          { return uncomp_iterator( this ); }
    size_t					get_uncompressed_size   () const    { return size_real * 4; }					// in bytes
    size_t					get_size                () const    { return sizeof( *this ) + 4 * size_comp; }	// in bytes
};


#include "comp_array_inl.h"