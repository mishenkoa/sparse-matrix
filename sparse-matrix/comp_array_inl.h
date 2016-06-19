//  
// Created  : 10.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once
#define self_t			comp_array< TVal, TIdx >
#define _TMPL_DECL_		template< typename TVal, typename TIdx >

_TMPL_DECL_		self_t::comp_array(TIdx size, TVal * full_array)
	: size_real(size)
	, comp_elms(std::map<TIdx, TIdx>())
{
	stable_vector<TVal>		res;
	TVal last_viewed = full_array[0];
	TIdx last_viewed_length = 1;
	TIdx last_viewed_idx = 0;
	res.push_back(last_viewed);

	for (TIdx i = 1; i < size; ++i) {
		if (last_viewed != full_array[i]) {
			res.push_back(full_array[i]);
			if (last_viewed_length != 1) {
				comp_elms[last_viewed_idx] = last_viewed_length;
			}
			last_viewed_idx = i;
			last_viewed_length = 1;
			last_viewed = full_array[i];
		}
		else {
			++last_viewed_length;
		}
	}
	if (last_viewed_length != 1) {
		comp_elms[last_viewed_idx] = last_viewed_length;
	}
	size_comp = res.size();
	arr = new TVal[size_comp];
	for (TIdx i = 0; i < size_comp; ++i) {
		arr[i] = res[i];
	}
}

_TMPL_DECL_ template < typename TContainer, typename TMap >
TVal* self_t::comp_arr(TIdx size, TContainer const& comp_array, TMap const& map)
{
	size_comp = comp_array.size();
	arr = new TVal[size_comp];
	for (TIdx i = 0; i < size_comp; ++i) {
		arr[i] = comp_array[i];
	}
	return arr;
}

_TMPL_DECL_
TVal			self_t::operator[](const TIdx idx) const {
	TIdx j = 0, i = 0;
	do {
		++i;
		auto find = comp_elms.find(j);
		j += (find != comp_elms.end()) ? find->second : 1;
	} while (j <= idx);
	return arr[i - 1];
}

_TMPL_DECL_ template < typename F >								
ICF void		self_t::iterate(F&& func) const {		// func : pair<TIdx : index, TVal : value> -> void
	if (size_real == 0) return;
	TIdx j = 0, i = 0;
	do {
		func(std::pair<TIdx, TVal>(j, arr[i]));
		++i;
		auto find = comp_elms.find(j);
		j += (find != comp_elms.end()) ? find->second : 1;
	} while (j <= size_real - 1);
}

_TMPL_DECL_
self_t&			self_t::out( std::ostream& ostream) const {
	iterate([&](pair<TIdx, TVal> mew)
	{
		ostream << format("(idx: %1%, val: %2%)\n") % mew.first % mew.second;
	});
	return const_cast<self_t&>(*this);
}


#undef self_t
#undef _TMPL_DECL_