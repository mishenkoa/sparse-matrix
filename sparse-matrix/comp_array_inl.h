//  
// Created  : 25.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once
#define self_t			        comp_array< TVal, TIdx >
#define _TMPL_DECL_		        template< typename TVal, typename TIdx >

_TMPL_DECL_		        self_t::comp_array  ( TIdx size, TVal * full_array )
    : size_real                 ( size )
    , comp_elms                 ( std::map<TIdx, TIdx>() ) 
{
    ASSERT                      ( size >= TIdx( 0 ) )
    stable_vector<TVal>	res;
    TVal last_viewed            = full_array[0];
    TIdx last_viewed_length     = 1;
    TIdx last_viewed_idx        = 0;

    res.push_back               ( last_viewed );

    for( TIdx i = 1; i < size; ++i ) {
        if( last_viewed        != full_array[i] ) {
            res.push_back       ( full_array[i] );

            if( last_viewed_length != 1 ) {
                comp_elms[last_viewed_idx] = last_viewed_length;
            }

            last_viewed_idx     = i;
            last_viewed_length  = 1;
            last_viewed         = full_array[i];

        } else {
            ++last_viewed_length;
        }
    }

    if( last_viewed_length != 1 ) {
        comp_elms[last_viewed_idx] = last_viewed_length;
    }
    size_comp                   = (TIdx) res.size();
    arr                         = __new( TVal, size_comp );
    for( TIdx i = 0; i < size_comp; ++i ) {
        arr[i]                  = res[i];
    }
}

_TMPL_DECL_             self_t::~comp_array () {
    __delete                 ( arr );
}

_TMPL_DECL_ TVal	    self_t::operator[]  ( const TIdx idx ) const {
    TIdx j                      = 0;
    TIdx i                      = 0;
    do {
        ++i;
        auto find               = comp_elms.find( j );
        j                      += ( find != comp_elms.end() )  ?  find->second  :  1;
    } while ( j <= idx );

    return arr[i - 1];
}

_TMPL_DECL_ ICF void	self_t::iterate     ( function<void ( TIdx, TVal )> func ) const {

    if( size_real == 0 )        { return; }
    TIdx j                      = 0;
    TIdx i                      = 0;
    do {
        func                    ( j, arr[i] );
        ++i;

        auto find               = comp_elms.find( j );
        j                      += ( find != comp_elms.end() )  ?  find->second  :  1;
    } while ( j <= size_real - 1 );
}

_TMPL_DECL_ self_t&		self_t::out         ( std::ostream& ostream ) {

    for( uncomp_iterator* it = new uncomp_iterator( this ); it->next(); ) {
        //ostream << format( "(idx: %1%, val: %2%)\n" ) % it->current().get<0>() % it->current().get<1>();
        ostream << it->current().get<1>() << " ";
    }

    return const_cast< self_t& >( *this );
}

_TMPL_DECL_ template<typename T>
static self_t*          self_t::construct   ( T _arr, TIdx _size_real, std::map<TIdx, TIdx> _comp_elms ) {
    auto result             = new comp_array();
    result->size_comp       = ( TIdx ) _arr.size();
    result->arr             = __new( TVal, result->size_comp );
    result->size_real       = _size_real;
    result->comp_elms       = _comp_elms;

    for( TIdx i = 0; i < result->size_comp; i++ ) {
        result->arr[i]      = _arr[i];
    }

    return result;
}

_TMPL_DECL_ bool	    self_t::eq          ( self_t& a ) {
    auto t_it = get_comp_iterator();
    auto a_it = a.get_comp_iterator();

    while( true ) {
        t_it.next();
        a_it.next();

        if( t_it.end() ) {
            return a_it.end();
        }
        if( t_it.current().get<0>() != a_it.current().get<0>() || 
            t_it.current().get<1>() != a_it.current().get<1>() ) {
            return false;
        }
    }
}


#undef self_t
#undef _TMPL_DECL_