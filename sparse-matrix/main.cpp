#include "csr_mat.h"
#include "types.h"
#include <iostream>
#include <boost/container/vector.hpp>
#include "comp_array.h"
#include "timer.h"

int main() {
	typedef csr_mat<s32, s32>				int_mat;
	typedef csr_mat<s32, s32>::csr_data		int_mat_data;

	//container::vector<int_mat_data> v = {
	//	int_mat_data{ 2, 2, 50 },
	//	int_mat_data{ 2, 3, 60 },
	//	int_mat_data{ 2, 4, 70 },
	//	int_mat_data{ 3, 3, 80 },
	//	int_mat_data{ 3, 5, 80 },
	//};

	//container::vector<int_mat_data> v1 = {
	//	int_mat_data{ 0, 0, 10 },
	//	int_mat_data{ 0, 1, 20 },
	//	int_mat_data{ 2, 2, 50 },
	//	int_mat_data{ 2, 3, 60 },
	//	int_mat_data{ 2, 4, 70 },
	//	int_mat_data{ 5, 5, 123 },


	//};

	//auto mat = int_mat(1000000, 1000000, v);
	//auto mat2 = int_mat(10, 10, v1);

	//mat.out( std::cout );
	  
	//mat.eq(mat2);

	//mat.mul( mat2 ).out( cout );

    //mat.add( mat ); .out( std::cout );//.out( cout ).iterate( []( int_mat_data a ) {cout << a.to_string() << " "; } );
	//cout << endl;
	//mat2.transp().out( cout );//.iterate( []( int_mat_data a ) {cout << a.to_string() << " "; } );;
	//mat.iterate( []( int_mat_data a ) {cout << a.val << " "; } );

	//mat.mul(mat2).out( cout );

	/*auto a = comp_array<s32, s32>(19, new s32[19]{ 1,1,1,1,1,1,1,1,1,1,2,3,3,3,234,2,34,2,34 });
	a.out(std::cout);
	auto it = a.get_uncomp_iterator();
	while (it.next()) {
		std::cout << format("(idx: %1%, val: %2%)\n") % it.current().first % it.current().second;
	}*/


    container::vector<int_mat_data> v = {
		int_mat_data{ 2, 2, 50 },
		int_mat_data{ 2, 3, 60 },
		int_mat_data{ 2, 4, 70 },
		int_mat_data{ 3, 3, 80 },
		int_mat_data{ 3, 5, 80 },
	};

    timer t; t.start();
    auto mat = int_mat( 100000000, 100000000, v );
    t.out( std::cout );
    
    t.start();
    mat.add( mat );
    t.out( std::cout );

	system( "pause" );
	return 0;
}
