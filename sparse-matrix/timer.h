// 
// Created : 12.06.2016
// Author  : Veyroter
// Copyright (C) AshyCat 2016
// This product are licensed under MICROSOFT REFERENCE SOURCE LICENSE(MS-RSL).
// 
#pragma once
// #include <intrin.h>
#include <ctime>
#include "types.h"

class timer {
    u64			m_start_time; 

public:
	IC			timer				();
	IC void		start				();
	IC u64		get_elapsed_ticks	() const;
	IC f64		get_elapsed_ms		() const;
	IC float	get_elapsed_sec		() const = delete;
	IC void		reset				();
    IC void     out( std::ostream& ostream ) const;
};


#pragma region IC implementation


IC u64      get_clocks() {
	return              ( clock() );
}

IC          timer::timer() {
	m_start_time        = 0;
}

IC void     timer::start() {
	m_start_time        = get_clocks();
}

IC u64      timer::get_elapsed_ticks() const {
	return              ( get_clocks() - m_start_time );
}

IC void     timer::reset() {
	m_start_time        = 0;
}

IC void     timer::out( std::ostream& ostream ) const {
    ostream             << "time elapsed: " << get_elapsed_ticks() << "\n";
}

IC f64		timer::get_elapsed_ms() const {
    return              ( float ( get_elapsed_ticks() ) / CLOCKS_PER_SEC );
}

#pragma endregion
