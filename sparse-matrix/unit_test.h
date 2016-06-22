//  
// Created  : 21.06.2016
// Author   : Veyroter
// Copyright (C) AshyCat, 2016 
//  
#pragma once
#include <boost/container/stable_vector.hpp>
#include <boost/function.hpp>
#include "types.h"

using namespace boost::container;
using namespace boost;

#define __add_unit_test_func(FNAME)         t_func.push_back( &FNAME );  t_func_name.push_back( #FNAME );

class unit_test {
protected:
    stable_vector< function< bool () > >    t_func;
    stable_vector< pcstr >                  t_func_name;
    stable_vector< pcstr >                  failed;
    stable_vector< f64 >                    t_func_time;

public:
    void    run             ();
    void    out_results     ( std::ostream& out );

};