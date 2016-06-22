//  
// Created  : 10.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once
#include <exception>
#include <boost/concept_check.hpp>

_TMPL_DECL_ template < typename TContainer>
self_t::csr_mat( TIdx rows, TIdx cols, TContainer& pairs, bool sorted )
	: rows( rows )
	, cols( cols ) {

    BOOST_RANGE_CONCEPT_ASSERT( ( RandomAccessRangeConcept<TContainer> ) );

	if( ! sorted ) {
		sort( pairs, [&]( csr_data a, csr_data b ) { return index(a) < index(b); } );
	}

	size_a				= ( TIdx ) pairs.size();
	a					= val_array_t( new TVal[size_a] );
	ja					= idx_array_t( new TIdx[size_a] );
    auto ia_comp_vals   = stable_vector<TIdx>();    
    auto ia_comp_elms   = std::map<TIdx, TIdx>();   
    ia_comp_vals.push_back( TVal( 0 ) );

    TIdx i			    = 0;
    TIdx ilast		    = 0;
    
	std::for_each( pairs.begin(), pairs.end(), [&]( csr_data pair ) {
        if( pair.val == TVal( 0 ) ) {
            throw std::invalid_argument( "pairs" );                    // passing zeroes in constructor prohibited
        }
		a[i]	        = pair.val;
		ja[i]	        = pair.j;
		
        if( ilast != pair.i ) {

            auto count = pair.i - ilast;    
            if( count != 1 ) {              
                ia_comp_elms[ilast + 1] = count;    
            }                               
            ia_comp_vals.push_back( i );    

			ilast       = pair.i;
		}
		
        i++;
	} );

    auto count = rows - ilast;  
    if( count != 1 ) {          
        ia_comp_elms[ilast + 1] = count;
    }                           
    ia_comp_vals.push_back( i );

	nnz                 = size_a;
	ia                  = cm_array_t::construct( ia_comp_vals, rows + 1, ia_comp_elms );
}

_TMPL_DECL_ 
ICF				void		self_t::iterate( function<void ( csr_data )> func ) const {
    for( TIdx i = 0; i < rows; ++i ) {
        TIdx tj_from        = ( *ia )[i];
        TIdx tj_to          = ( *ia )[i + 1] - 1;

        while( tj_from != tj_to + 1 ) {
            func            ( csr_data{ i, ja[tj_from], a[tj_from] } );
            tj_from         += 1;
        }
    }
}

_TMPL_DECL_		self_t&		self_t::transp() {
    stable_vector<csr_data> res;
    iterate                 ( [&] ( csr_data data ) { res.push_back( csr_data{ data.j, data.i, data.val } ); } );

    return                  ( *this = *( new csr_mat( cols, rows, res, false ) ) );
}

_TMPL_DECL_		ICF TIdx	self_t::index( const csr_data& a ) const {

	return                  ( a.i * cols + a.j );
}

_TMPL_DECL_		ICF TVal*	self_t::get_ref( TIdx row, TIdx col ) const {

    ASSERT                  ( row >= 0 && row < rows && col >= 0 && col < cols );
    auto idx_from           = ( *ia )[row];
    auto idx_to             = ( *ia )[row + 1] - 1;

	if( idx_to < 0 )        { return nullptr; }

    auto comparator         = [] ( const void* a, const void* b ) {	
        return              TIdx( *( const TIdx* )( a ) - *( const TIdx* )( b ) );
    };

    auto idx = static_cast< TIdx* >(
        bsearch(
            &col,                   // what we are looking for
            ja.get() + idx_from,    // base pointer
            idx_to - idx_from + 1,  // number of elements
            sizeof TIdx,            // size of one element
            comparator              // comparator function
            )
        );
    return                  ( idx == nullptr ? nullptr : &a[idx - ja.get()] );
}

_TMPL_DECL_		TVal		self_t::get( TIdx row, TIdx col ) const {
	TVal*  ref               = get_ref( row, col );
	return                  ( ref == nullptr ? TVal( 0 ) : * ref );
}

_TMPL_DECL_	    self_t&		self_t::add( const self_t &a ) {
	ASSERT                  ( a.rows == rows && a.cols == cols );

	stable_vector<csr_data> res;
    
    auto ti_it              = this->ia->get_comp_iterator_prev();
    auto ai_it              = a.ia->get_comp_iterator_prev();
    ti_it.next              ();
    ai_it.next              ();

    TIdx    tj_from;
    TIdx    tj_to;
    TIdx    aj_from;
    TIdx    aj_to;
    bool    t_end;
    bool    a_end;
    TIdx    to_add_i;
    
    do {
        if( ti_it.current().get<0>() < ai_it.current().get<0>() ) {
            a_end           = true;
            t_end           = false;
            tj_from         = ti_it.current().get<2>();
            tj_to           = ti_it.current().get<1>() - 1;
            to_add_i        = ti_it.current().get<0>() - 1;
            ti_it.next      ();
        } else if( ti_it.current().get<0>() > ai_it.current().get<0>() ) {
            t_end           = true;
            a_end           = false;
            aj_from         = ai_it.current().get<2>();
            aj_to           = ai_it.current().get<1>() - 1;
            to_add_i        = ai_it.current().get<0>() - 1;
            ai_it.next      ();
        } else {
            tj_from         = ti_it.current().get<2>();
            tj_to           = ti_it.current().get<1>() - 1;
            aj_from         = ai_it.current().get<2>();
            aj_to           = ai_it.current().get<1>() - 1;
            t_end           = tj_from >= tj_to + 1; 
            a_end           = aj_from >= aj_to + 1; 
            to_add_i        = ti_it.current().get<0>() - 1;
            ti_it.next      ();
            ai_it.next      ();
        }

		TIdx to_add_j;
		TVal to_add_val;
        
		while ( ! ( t_end && a_end ) ) {
			if (!t_end && (a_end || ja[tj_from] < a.ja[aj_from])) {
				to_add_val	= this->a[tj_from];
				to_add_j	= ja[tj_from];
				++tj_from;
			}
			else if (!a_end && (t_end || ja[tj_from] > a.ja[aj_from])) {
				to_add_val	= a.a[aj_from];
				to_add_j	= a.ja[aj_from];
				++aj_from;
			}
			else {
				to_add_val	= this->a[tj_from] + a.a[aj_from];
				to_add_j	= a.ja[aj_from];
				++tj_from;  ++aj_from;
			}
			if (to_add_val != TVal(0)) {
				res.push_back(csr_data{ to_add_i, to_add_j, to_add_val });
			}
			t_end           = tj_from >= tj_to + 1;
			a_end           = aj_from >= aj_to + 1;
		}
    } while( ! ( ti_it.end() && ai_it.end() ) );

	return                  ( *this = *( new csr_mat( rows, cols, res, true ) ) );
}

_TMPL_DECL_		self_t&		self_t::sub( const self_t &a ) {
	ASSERT                  ( a.rows == rows && a.cols == cols );

	stable_vector<csr_data> res;

	auto ti_it              = this->ia->get_comp_iterator_prev();
	auto ai_it              = a.ia->get_comp_iterator_prev();
	ti_it.next              ();
	ai_it.next              ();

    TIdx    tj_from;
    TIdx    tj_to;
    TIdx    aj_from;
    TIdx    aj_to;
    bool    t_end;
    bool    a_end;
	TIdx    to_add_i;

	do {
		if (ti_it.current().get<0>() < ai_it.current().get<0>()) {
			a_end           = true;
			t_end           = false;
			tj_from         = ti_it.current().get<2>();
			tj_to           = ti_it.current().get<1>() - 1;
			to_add_i        = ti_it.current().get<0>() - 1;
			ti_it.next  ();
		} else if ( ti_it.current().get<0>() > ai_it.current().get<0>() ) {
			t_end           = true;
			a_end           = false;
			aj_from         = ai_it.current().get<2>();
			aj_to           = ai_it.current().get<1>() - 1;
			to_add_i        = ai_it.current().get<0>() - 1;
			ai_it.next      ();
		} else {
			tj_from         = ti_it.current().get<2>();
			tj_to           = ti_it.current().get<1>() - 1;
			aj_from         = ai_it.current().get<2>();
			aj_to           = ai_it.current().get<1>() - 1;
			t_end           = tj_from >= tj_to + 1; 
			a_end           = aj_from >= aj_to + 1; 
			to_add_i        = ti_it.current().get<0>() - 1;
			ti_it.next();
			ai_it.next();
		}

		TIdx    to_add_j;
		TVal    to_add_val;

        while( !( t_end && a_end ) ) {
            if( !t_end && ( a_end || ja[tj_from] < a.ja[aj_from] ) ) {
                to_add_val  = this->a[tj_from];
                to_add_j    = ja[tj_from];
                tj_from     += 1;

            } else if( !a_end && ( t_end || ja[tj_from] > a.ja[aj_from] ) ) {
                to_add_val  = -a.a[aj_from];
                to_add_j    = a.ja[aj_from];
                aj_from     += 1;

            } else {
                to_add_val  = this->a[tj_from] - a.a[aj_from];
                to_add_j    = ja[tj_from];
                tj_from     += 1;
                aj_from     += 1;
            }

            if( to_add_val != TVal( 0 ) ) {
                res.push_back( csr_data{ to_add_i, to_add_j, to_add_val } );
            }

            t_end           = tj_from >= tj_to + 1;
            a_end           = aj_from >= aj_to + 1;
        }

    } while( !( ti_it.end() && ai_it.end() ) );

    return                  ( *this = *( new csr_mat( rows, cols, res, true ) ) );
}

_TMPL_DECL_		self_t&		self_t::mul( const self_t &a ) {
	ASSERT                  (cols == a.rows);
	self_t b                = a;
	b.transp                ();

	stable_vector<csr_data> res;

    for( TIdx i = 0; i < rows; ++i ) {
        for( TIdx j = 0; j < cols; ++j ) {

            TIdx tj_from    = ( *ia )[i],
                 tj_to      = ( *ia )[i + 1] - 1;
            TIdx aj_from    = ( *b.ia )[j],
                 aj_to      = ( *b.ia )[j + 1] - 1;
            TVal sum        = TVal( 0 );

            while( ( tj_from <= tj_to ) && ( aj_from <= aj_to ) ) {

                if( ja[tj_from] < b.ja[aj_from] ) {
                    tj_from += 1;

                } else if( ja[tj_from] > b.ja[aj_from] ) {
                    aj_from += 1;

                } else {
                    sum     += this->a[tj_from] * b.a[aj_from];
                    tj_from += 1;  
                    aj_from += 1;
                }

                if( sum != TVal( 0 ) ) {
                    res.push_back( csr_data{ i, j, sum } );
                }

            }
        }
    }

    return                  ( *this = *( new csr_mat( rows, a.cols, res, true ) ) );
}

_TMPL_DECL_		bool	    self_t::eq( const self_t &a ) {

    if( cols != a.cols || rows != a.rows || nnz != a.nnz ) {
        return              ( false );
    }

    for( TIdx i = 0; i < size_a; i++ ) {
        if( this->a[i] != a.a[i] || ja[i] != a.ja[i] ) {
            return          ( false );
        }
    }

    if( ! ia->eq( *a.ia ) ) {
        return              ( false );
    }

    /*for( TIdx i = 0; i < rows + 1; i++ ) {
        if( ( *ia )[i] != ( * a.ia )[i] ) {
            return          ( false );
        }
    }*/

    return                  ( true );
}

_TMPL_DECL_		TIdx		dignum( TVal num ) {

    return                  ( num != abs( num ) ) 
                ?           dignum<TVal, TIdx>( abs( num ) ) + 1
                :           ( num /= TIdx( 10 ) )
                    ?       TIdx( 1 ) + dignum<TVal, TIdx>( num )
                    :       TIdx( 1 );
}

_TMPL_DECL_		self_t&		self_t::out( std::ostream &ostream ) const {

    TIdx * maxdig             = __new( TIdx, cols );

    for( int j = 0; j < this->cols; j++ ) {
        maxdig[j]           = 1;

        for( int i = 0; i < this->rows; i++ ) {
            if( dignum<TVal, TIdx>( get( i, j ) ) > maxdig[j] ) {
            
                maxdig[j]   = dignum<TVal, TIdx>( get( i, j ) );
            }
        }
    }
    
    for( int i = 0; i < this->rows; i++ ) {
        ostream             << "|";
        for( int j = 0; j < this->cols; j++ ) {
            ostream         << std::setw( maxdig[j] + 1 ) << std::setfill( ' ' ) << this->get( i, j );
        }
        ostream             << " |" << std::endl;
    }
	__delete                ( maxdig );

    ostream                 << "A: ";
    for( TIdx i1 = 0; i1 < size_a; i1++ ) {
        ostream             << a[i1] << " ";
    } 	ostream             << std::endl;

    ostream                 << "JA: ";

    for( TIdx i1 = 0; i1 < size_a; i1++ ) {
        ostream             << ja[i1] << " ";
    } 	ostream             << std::endl;
    ostream                 << "IA: ";
    for( TIdx i1 = 0; i1 < rows + 1; i1++ ) {
        ostream             << ( *ia )[i1] << " ";
    } 	ostream             << std::endl;
	
    return                  ( const_cast<self_t&>(*this) );
}

_TMPL_DECL_		self_t&		self_t::operator=( const self_t& a ) {
	ja		                = a.ja;
	ia		                = a.ia;
	this->a                 = a.a;
	nnz		                = a.nnz;
	rows	                = a.rows;
	cols	                = a.cols;
	size_a	                = a.size_a;

	return                  ( * this );
}