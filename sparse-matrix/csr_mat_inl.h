//  
// Created  : 10.05.2016
// Author   : Compiles
// © AshyCat, 2016 
//  
#pragma once

_TMPL_DECL_ 
template <typename TContainer>
self_t::csr_mat(TIdx rows, TIdx cols, TContainer& pairs, bool sorted)
	: rows(rows)
	, cols(cols) 
{
	if( ! sorted ) {
		sort(pairs, [&](csr_data a, csr_data b) { return index(a) < index(b); });
	}

	size_a				= pairs.size();
	a					= shared_array<TVal>(new TVal[size_a]);
	ja					= shared_array<TIdx>(new TIdx[size_a]);
	TIdx *  ia_temp		= (TIdx * ) malloc( sizeof( TIdx ) * ( rows + 1 ) );
	ASSERT( ia_temp );  // действительно ли выделили память
	size_t i			= 0;
	size_t ilast		= 0;

	ia_temp[0] = TIdx(0);

	std::for_each(pairs.begin(), pairs.end(), [&](csr_data pair) {
		a[i]	= pair.val;
		ja[i]	= pair.j;

		if (ilast != pair.i) {
			for (int k = ilast + 1; k <= pair.i; k++) { // заполняем пробелы
				ia_temp[k] = i;
			}
			ilast = pair.i;
		}
		i++;
	});

	for (int k = ilast + 1; k <= rows; k++) { // заполняем пробелы
		ia_temp[k] = i;
	}

	nnz = size_a;
	ia_temp[rows] = nnz;

	ia = new comp_array<TIdx, TVal>(rows + 1, ia_temp);
	free(ia_temp);
}

_TMPL_DECL_ template < typename F >
ICF				void		self_t::iterate(F&& func) const {
	for (TIdx i = 0; i < rows; ++i) {
		TIdx tj_from	= ia[i];
		TIdx tj_to		= (*ia)[i + 1] - 1;

		while (tj_from != tj_to + 1) {
			func(csr_data{ i, ja[tj_from], a[tj_from] });
			++tj_from;
		}
	}
}

_TMPL_DECL_		self_t&		self_t::transp() {
	stable_vector<csr_data> res;

	iterate([&](csr_data data) {
		res.push_back(csr_data{ data.j, data.i, data.val });
	});

	return *this = *(new csr_mat(cols, rows, res, false));
}

_TMPL_DECL_		ICF TIdx	self_t::index(const csr_data& a) const {
	return a.i * cols + a.j;
}

_TMPL_DECL_		ICF TVal*	self_t::get_ref(TIdx row, TIdx col) const {
	ASSERT(row >= 0 && row < rows && col >= 0 && col < cols);

	auto idx_from	= (*ia)[row],
		 idx_to		= (*ia)[row + 1] - 1;

	if ( idx_to < 0 ) {
		return nullptr;
	}

	auto idx = static_cast<TIdx*>(
		bsearch(
			&col,
			ja.get() + idx_from,
			idx_to - idx_from + 1,
			sizeof TIdx,
			[](const void* a, const void* b) {	return static_cast<s32>(*static_cast<const TIdx*>(a) - *static_cast<const TIdx*>(b));  }
			)
		);
	return idx == nullptr ? nullptr : &a[idx - ja.get()];
}

_TMPL_DECL_		TVal		self_t::get(TIdx row, TIdx col) const {
	TVal* ref = get_ref(row, col);
	return ref == nullptr ? TVal(0) : *ref;
}

_TMPL_DECL_		self_t&		self_t::add(const self_t &a) {
	ASSERT(a.rows == rows && a.cols == cols);

	stable_vector<csr_data> res;

	for (TIdx i = 0; i < rows; ++i) {
		TIdx tj_from	= ia[i],
			 tj_to		= ia[i + 1] - 1;
		TIdx aj_from	= a.ia[i],
			 aj_to		= a.ia[i + 1] - 1;
		bool t_end		= tj_from >= tj_to + 1;
		bool a_end		= aj_from >= aj_to + 1;
		TIdx to_add_j;
		TVal to_add_val;

		while (!(t_end && a_end)) {
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
				res.push_back(csr_data{ i, to_add_j, to_add_val });
			}
			t_end = tj_from >= tj_to + 1;
			a_end = aj_from >= aj_to + 1;
		}
	}
	return *this = *(new csr_mat(rows, cols, res, true));
}

_TMPL_DECL_		self_t&		self_t::sub(const self_t &a) {
	ASSERT(a.rows == rows && a.cols == cols);

	stable_vector<csr_data> res;

	for (TIdx i = 0; i < rows; ++i) {
		TIdx tj_from	= ia[i],
			tj_to		= ia[i + 1] - 1;
		TIdx aj_from	= a.ia[i],
			aj_to		= a.ia[i + 1] - 1;
		bool t_end		= tj_from >= tj_to + 1;
		bool a_end		= aj_from >= aj_to + 1;
		TIdx to_add_j;
		TVal to_add_val;

		while (!(t_end && a_end)) {
			if (!t_end && (a_end || ja[tj_from] < a.ja[aj_from])) {
				to_add_val = this->a[tj_from];
				to_add_j = ja[tj_from];
				++tj_from;
			}
			else if (!a_end && (t_end || ja[tj_from] > a.ja[aj_from])) {
				to_add_val = -a.a[aj_from];
				to_add_j = a.ja[aj_from];
				++aj_from;
			}
			else {
				to_add_val = this->a[tj_from] - a.a[aj_from];
				to_add_j = ja[tj_from];
				++tj_from;  ++aj_from;
			}
			if (to_add_val != TVal(0)) {
				res.push_back(csr_data{ i, to_add_j, to_add_val });
			}
			t_end = tj_from >= tj_to + 1;
			a_end = aj_from >= aj_to + 1;
		}
	}
	return *this = *(new csr_mat(rows, cols, res, true));
}

_TMPL_DECL_		self_t&		self_t::mul(const self_t &a) {
	ASSERT(cols == a.rows);
	self_t b = a;
	b.transp();

	stable_vector<csr_data> res;

	for (TIdx i = 0; i < rows; ++i) {
		for (TIdx j = 0; j < cols; ++j) {
			TIdx tj_from	= ia[i],
				 tj_to		= ia[i + 1] - 1;
			TIdx aj_from	= b.ia[j],
			 	 aj_to		= b.ia[j + 1] - 1;

			TVal sum = TVal(0);
			while ((tj_from <= tj_to) && (aj_from <= aj_to)) {
				if (ja[tj_from] < b.ja[aj_from]) {
					++tj_from;
				} else if (ja[tj_from] > b.ja[aj_from]) {
					++aj_from;
				} else {
					sum += this->a[tj_from] * b.a[aj_from];
					++tj_from;  ++aj_from;
				}
				if (sum != TVal(0)) {
					res.push_back(csr_data{ i, j, sum });
				}
			}
		}
	}
	return *this = *(new csr_mat(rows, a.cols, res, true));

}

_TMPL_DECL_		ICF bool	self_t::eq(const self_t &a) {
	if (cols == a.cols && rows == a.rows && nnz == a.nnz) {
		for (size_t i = 0; i < size_a; i++) {
			if (this->a[i] != a.a[i] || ja[i] != a.ja[i]) {
				return false;
			}
		}
		for (size_t i = 0; i < rows + 1; i++) {
			if (ia[i] != a.ia[i]) {
				return false;
			}
		}
		return true;
	}
	return false;
}

_TMPL_DECL_		TIdx		dignum(TVal num) {
	return (num != abs(num))
		? dignum<TVal, TIdx>(abs(num)) + 1
		: (num /= TIdx(10))
			? TIdx(1) + dignum<TVal, TIdx>(num)
			: TIdx(1);
}

_TMPL_DECL_		self_t&		self_t::out(std::ostream &ostream) const {
	auto maxdig = new TIdx[cols];

	for (int j = 0; j < this->cols; j++) {
		maxdig[j] = 1;
		for (int i = 0; i < this->rows; i++) {
			if (dignum<TVal, TIdx>(get(i, j)) > maxdig[j]) {
				maxdig[j] = dignum<TVal, TIdx>(get(i, j));
			}
		}
	}

	for (int i = 0; i < this->rows; i++) {
		ostream << "|";
		for (int j = 0; j < this->cols; j++) {
			ostream << std::setw(maxdig[j] + 1) << std::setfill(' ') << this->get(i, j);
		}
		ostream << " |" << std::endl;
	}

	delete[] maxdig;

	std::cout << "A: ";
	for (size_t i1 = 0; i1 < size_a; i1++) {
		ostream << a[i1] << " ";
	} 	ostream << std::endl;
	std::cout << "JA: ";
	for (size_t i1 = 0; i1 < size_a; i1++) {
		ostream << ja[i1] << " ";
	} 	ostream << std::endl;
	std::cout << "IA: ";
	for (size_t i1 = 0; i1 < rows + 1; i1++) {
		ostream << (*ia)[i1] << " ";
	} 	ostream << std::endl;

	return const_cast<self_t&>(*this);
}

_TMPL_DECL_		self_t&		self_t::operator=(const self_t& a) {
	ja		= a.ja;
	ia		= a.ia;
	this->a = a.a;
	nnz		= a.nnz;
	rows	= a.rows;
	cols	= a.cols;
	size_a	= a.size_a;
	return *this;
}