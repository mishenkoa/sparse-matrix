#include "unit_test.h"
#include <iostream>
#include "types.h"

void unit_test::run() {
    ASSERT                  ( t_func.size() == t_func_name.size() );

    for( size_t i = 0; i < t_func.size(); i++ ) {
        if( t_func[i]() == false ) { failed.push_back( t_func_name[i] ); }
    }
}

void unit_test::out_results( std::ostream & out ) {
    if( failed.size() ) {
        out                 << "Unit tests failed! Errors in: " << std::endl;

        for each ( pcstr fname in failed ) {
            out             << "\t" << fname << std::endl;
        }
    } else {
        out                 << "Unit tests succeeded." << std::endl;
    }
}
