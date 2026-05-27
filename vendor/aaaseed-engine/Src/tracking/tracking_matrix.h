
#ifdef AAA_TRACKING_MATRIX_H
#error "TRACKING_MATRIX_H included more than once."
#endif
#define AAA_TRACKING_MATRIX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

class	c_metric_point
{
public:
	INT32	_next;
	INT32	_prev;
	REAL	_value;

	c_metric_point( INT32 next, INT32 prev, REAL value )
		: _next(next), _prev(prev), _value(value)
	{}
};


class	c_metric_line
{
private:
	std::vector< c_metric_point >	_line;
	INT32							_matrix_index;

public:
	FINLINE	void			set_matrix_index(INT32 index)	{ _matrix_index = index; }
	FINLINE	INT32			get_matrix_index()				{ return _matrix_index; }

	FINLINE	bool			is_empty()						{ return _line.empty(); }
	FINLINE	INT32			get_size()						{ return (INT32)_line.size(); }

	FINLINE	void			add( c_metric_point point )		{ _line.push_back( point ); }

	FINLINE	REAL			get_first_value()				{ return _line[0]._value; }
	FINLINE	c_metric_point*	get_metric_point(INT32 index)	{ return &_line[index]; }
	FINLINE	INT32			get_prev(INT32 index)			{ return _line[index]._prev; }
	FINLINE	INT32			get_next(INT32 index)			{ return _line[index]._next; }

	FINLINE	void			sort_by_metric();
};

class	c_tracking_matrix
{
private:
	REAL	_penality_for_new;

	std::vector< c_metric_line >	_rows;
	std::vector< c_metric_line >	_cols;

	std::vector<INT32>				_solution;
protected:
	class metric_less
	{ 
	public:
		bool operator() ( c_metric_line* a, c_metric_line* b ) 
			{	return a->get_first_value() < b->get_first_value();		}
	};
public:
	bool	_b_verbose;
	INT32	_solution_cnt;
	INT32	_branch_try_max;
	INT32	_loop_count_max;
	bool	_b_by_subset;
public:
	FINLINE	UINT32	get_row_nb()			{	return (UINT32)_rows.size(); }
			INT32	get_solution(INT32	i)	{	return _solution[i]; }

	FINLINE void push_solution( std::vector<INT32>& solution, std::vector<INT32>& ids, REAL error, INT32 iteration_count );
	void	tree_explore
	(
		std::vector<INT32>&				solution,
		std::vector<c_metric_line*>&	rows
	);

	void	set_penality_for_new( REAL in )	{ _penality_for_new = in; }
	void	init_size( INT32 row_nb, INT32 col_nb );
	void	add( INT32 i, INT32 j, REAL error );
	void	close_row( INT32 row_index );
	void	solve_global();
	void	solve_by_subset();
	void	solve();	
};
