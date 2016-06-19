// 
// Created : 12.06.2016
// Author  : Veyroter
// Copyright (C) AshyCat 2016
// This product are licensed under MICROSOFT REFERENCE SOURCE LICENSE(MS-RSL).
// 
#include "timer.h"				//  includes order
#include <timeapi.h>			//  is very important!

u64	ashy::timing::g_qpc_per_second = 0;

void ashy::timing::initialize() {
	#pragma comment(lib, "winmm.lib")

	timeBeginPeriod( 1 );

	QueryPerformanceFrequency( (PLARGE_INTEGER)&g_qpc_per_second );

	errno_t const error = _controlfp_s( 0, _PC_24, MCW_PC );
	ASSERT( !error );
}

u64 ashy::timing::query_performance_counter	() {
	u64 result;
	QueryPerformanceCounter( (PLARGE_INTEGER)&result );
	return  result;
}