//  
// Created  : 22.06.2016
// Author   : Veyroter
// Copyright (C) AshyCat, 2016 
//  
#pragma once
#include "unit_test.h"
#include "comp_array.h"
#include "csr_mat.h"
#include <limits>

namespace testing {

bool comp_arr_test_1        () {
    auto p                  = __new( f32, 5 );
    for( u8 i = 0; i < 5; i++ ) {
        p[i]                = (f32) i + 0.25f;
    }
    auto arr                = comp_array<f32, u8>( 5, p );
    for( u8 i = 0; i < 5; i++ ) {
        if( arr[i] != ( f32 ) i + 0.25f ) {
            return          ( false );
        }
    }
    s32 m = 0;
    for( auto it = arr.get_comp_iterator(); it.next(); ) {
        m++;
    }
    if( m != 5 ) {
        return              ( false );
    }

    return                  ( true );
}

bool comp_arr_test_2        () {
    auto p                  = __new( s32, 5 );
    for( u8 i = 0; i < 5; i++ ) {
        p[i]                = 0;
    }
    auto arr                = comp_array<s32, u8>( 5, p );
    for( u8 i = 0; i < 5; i++ ) {
        if( arr[i] != 0 ) {
            return          ( false );
        }
    }
    s32 m = 0;
    for( auto it = arr.get_comp_iterator(); it.next(); ) {
        m++;
    }
    if( m != 1 ) {
        return              ( false );
    }

    return                  ( true );
}

bool comp_arr_test_3        () {
    auto p                  = __new( s32, 1 );
    p[0]                    = 0;
    auto arr                = comp_array<s32, u8>( 1, p );
    if( arr[0] != 0 ) {
        return              ( false );
    }
    s32 m = 0;
    for( auto it = arr.get_comp_iterator(); it.next(); ) {
        m++;
    }
    if( m != 1 ) {
        return              ( false );
    }
    m = 0;
    for( auto it = arr.get_uncomp_iterator(); it.next(); ) {
        m++;
    }
    if( m != 1 ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_sub_and_construct() {
    typedef csr_mat<s32, s32>				int_mat;
    typedef csr_mat<s32, s32>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 60 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };
    container::vector<int_mat_data> v2 = {
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 70 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 3, 3, 80 },
        int_mat_data        { 3, 5, 80 },
    };
    container::vector<int_mat_data> res1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 3, -10 },
        int_mat_data        { 2, 4, 0 },
        int_mat_data        { 5, 5, 123 },
        int_mat_data        { 3, 3, -80 },
        int_mat_data        { 3, 5, -80 },
    };
    container::vector<int_mat_data> res2 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 3, -10 },
        int_mat_data        { 5, 5, 123 },
        int_mat_data        { 3, 3, -80 },
        int_mat_data        { 3, 5, -80 },
    };

    auto mat1               = int_mat( 10, 10, v1 );
    auto mat2               = int_mat( 10, 10, v2 );

    mat1.sub( mat2 );

    try {
        int_mat( 10, 10, res1 );
    } catch( std::invalid_argument& ) { }

    if( !( mat1.eq( int_mat( 10, 10, res2 ) ) ) ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_transp    () {
    typedef csr_mat<s32, s32>				int_mat;
    typedef csr_mat<s32, s32>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 3, 50 },
        int_mat_data        { 5, 5, 11 },
    };
    container::vector<int_mat_data> v2 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 1, 0, 20 },
        int_mat_data        { 3, 2, 50 },
        int_mat_data        { 5, 5, 11 },
    };

    auto mat1               = int_mat( 10, 10, v1 ).transp();

    if( !( mat1.eq( int_mat( 10, 10, v2 ) ) ) ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_mul       () {
    typedef csr_mat<s32, s32>				int_mat;
    typedef csr_mat<s32, s32>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 3, 50 },
        int_mat_data        { 5, 5, 11 },
    };
    container::vector<int_mat_data> v2 = {
        int_mat_data        { 0, 0, 100 },
        int_mat_data        { 0, 1, 200 },
        int_mat_data        { 5, 5, 121 },
    };

    auto mat1               = int_mat( 10, 10, v1 );
    mat1.mul                ( int_mat( 10, 10, v1 ) );

    if( !( mat1.eq( int_mat( 10, 10, v2 ) ) ) ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_add       () {
    typedef csr_mat<s32, u64>				int_mat;
    typedef csr_mat<s32, u64>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };
    container::vector<int_mat_data> v2 = {
        int_mat_data        { 2, 2, -50 },
        int_mat_data        { 2, 3, 70 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 3, 3, 80 },
        int_mat_data        { 3, 5, 80 },
    };
    container::vector<int_mat_data> res2 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 3, 100 },
        int_mat_data        { 3, 3, 80 },
        int_mat_data        { 3, 5, 80 },
        int_mat_data        { 2, 4, 140 },
        int_mat_data        { 5, 5, 123 },
    };

    u64 max                 = 10000000000000000000;
    auto mat1               = int_mat( max, max, v1 );
    auto mat2               = int_mat( max, max, v2 );

    mat1.add( mat2 );

    if( !( mat1.eq( int_mat( max, max, res2 ) ) ) ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_get_ref   () {
    typedef csr_mat<s32, s32>				int_mat;
    typedef csr_mat<s32, s32>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };

    auto mat               = int_mat( 100000000, 100000000, v1 );
    
    if( mat.get( 0, 0 ) != 10 ) {
        return              ( false );
    }
    if( mat.get( 2, 3 ) != 30 ) {
        return              ( false );
    }
    if( mat.get( 9, 9 ) != 0 ) {
        return              ( false );
    }
    if( mat.get( 0, 5 ) != 0 ) {
        return              ( false );
    }
    auto ref = mat.get_ref( 2, 3 );
    *ref = 3;
    if( mat.get( 2, 3 ) != 3 ) {
        return              ( false );
    }

    return                  ( true );
}

bool csr_mat_test_neq       () {
    typedef csr_mat<s32, s32>				int_mat;
    typedef csr_mat<s32, s32>::csr_data		int_mat_data;

    container::vector<int_mat_data> v1 = {
        int_mat_data        { 0, 0, 10 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };
    container::vector<int_mat_data> v2 = {
        int_mat_data        { 0, 0, 11 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };
    container::vector<int_mat_data> v3 = {
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
        int_mat_data        { 5, 5, 123 },
    };
    container::vector<int_mat_data> v4 = {
        int_mat_data        { 5, 5, 123 },
        int_mat_data        { 0, 1, 20 },
        int_mat_data        { 2, 2, 50 },
        int_mat_data        { 2, 3, 30 },
        int_mat_data        { 2, 4, 70 },
    };

    auto mat1               = int_mat( 100000000, 100000000, v1 );
    auto mat2               = int_mat( 100000000, 100000000, v2 );
    auto mat3               = int_mat( 100000000, 100000000, v3 );
    auto mat4               = int_mat( 100000000, 100000000, v4 );
    
    if( mat1.eq( mat2 ) ) {
        return              ( false );
    }
    if( mat1.eq( mat3 ) ) {
        return              ( false );
    }
    if( mat2.eq( mat3 ) ) {
        return              ( false );
    }
    if( ! mat4.eq( mat4 ) ) {
        return              ( false );
    }

    return                  ( true );
}

} // namespace testing


class project_unit_tests : public unit_test {
public:
    project_unit_tests() {
        __add_unit_test_func    ( testing::comp_arr_test_1 );
        __add_unit_test_func    ( testing::comp_arr_test_2 );
        __add_unit_test_func    ( testing::comp_arr_test_3 );
        __add_unit_test_func    ( testing::csr_mat_test_sub_and_construct );
        __add_unit_test_func    ( testing::csr_mat_test_transp );
        __add_unit_test_func    ( testing::csr_mat_test_mul );
        __add_unit_test_func    ( testing::csr_mat_test_add );
        __add_unit_test_func    ( testing::csr_mat_test_get_ref );
        __add_unit_test_func    ( testing::csr_mat_test_neq );
    }
};