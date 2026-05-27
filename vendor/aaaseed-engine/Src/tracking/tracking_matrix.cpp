#include "tracking_matrix.h"
#include "err.h"
#include <set>
#include <deque>
#include <vector>
#include <algorithm>


class metric_less
{ 
	public: FINLINE bool operator() ( c_metric_point& a, c_metric_point& b )  CONST NOEXCEPT
			{	return a._value < b._value; }
};

FINLINE	void c_metric_line::sort_by_metric()		{	sort( _line.begin(), _line.end(), metric_less() ); }

FINLINE void c_tracking_matrix::push_solution( std::vector<INT32>& dst, std::vector<INT32>& src, REAL error, INT32 iteration_count )
{
	if( _b_verbose )
	{
		//todo
		VERBOSE_PRINT_STRING( "%3d : ite %10d err %8.2f | ", _solution_cnt, iteration_count, error );
		for( UINT32 i = 0; i < src.size(); ++i )
			VERBOSE_PRINT_STRING( "%3d ", src[i] );
		VERBOSE_PRINT_STRING( "\n" );
	}
	if( dst.size() != src.size() )
		debug_break();
	dst = src;
	++_solution_cnt;
}

//todo scale absolute and square pb
#define GO_BACK()\
{\
	if( --depth < 0 )\
	break;\
	i = stack_i[depth];\
	err = stack_err[depth];\
	if( sol[depth] >= -0 )\
		prev_free[sol[depth]] = true;\
}
#define GO_NEXT()\
{\
	stack_i[depth] = i;\
	stack_err[depth] = err;\
	++depth;\
	err = new_err;\
	i = 0;\
}

void c_tracking_matrix::tree_explore
		(
		std::vector<INT32>&				solution,
		std::vector<c_metric_line*>&	rows
		)
{
	//	other input
	//		_metrics
	//		_history.back().size() for the number of prev blob
	UINT32	size_next = (UINT32)rows.size();

	solution.assign( size_next, -1 );

	std::vector<INT32>	sol;
	std::vector<INT32>	stack_i;
	std::vector<REAL>	stack_err;

	sol.assign( size_next, -1 );
	stack_i.assign( size_next, 0 );
	stack_err.assign( size_next, 0. );

	std::vector<REAL>	tree_cost_min;
	tree_cost_min.assign( size_next, 0. );

	REAL tmp = 0;
	for( INT32 i = size_next - 1; i >= 0; --i )
	{
		tree_cost_min[i] = tmp;
		tmp += rows[i]->get_first_value();
	}

	std::vector<bool>	prev_free;
	prev_free.assign( _cols.size(), true );

	INT32						depth		= 0;
	CONST	UINT32				dump		= size_next-1;
	REAL						best_error	= 10000000.;
	REAL						err			= 0;
	REAL						new_err;
	c_metric_point				metric_away( -1, -1, _penality_for_new );
	c_metric_point				metric_back( -2, -2, _penality_for_new );
	c_metric_point*				metric;

	INT32				loop_count	= 0;
	_solution_cnt = 0;

	for( INT32 i=0; ; )
	{
		if( i<_branch_try_max )
			metric = rows[depth]->get_metric_point(i);
		else if( i==_branch_try_max )
			metric = &metric_away;
		else
			metric = &metric_back;

		if( metric->_prev == -2 )
			GO_BACK()
		else if( metric->_prev == -1 )	// new ID
		{
			new_err = err + metric->_value;
			if( (new_err+tree_cost_min[depth]) >= best_error )	//the next at this level will be worse so skip
				GO_BACK()
			else
			{
				++i;
				if( depth != dump )
				{
					sol[depth] = -1;
					GO_NEXT();
				}
				else if( new_err < best_error )
				{
					best_error = new_err;
					sol[depth] = -1;
					push_solution( solution, sol, best_error, loop_count );
				}
			}
		}
		else
		{	
			++i;
			if( prev_free[metric->_prev] )
			{
				new_err = err + metric->_value;
				if( new_err+tree_cost_min[depth] >= best_error )	//the next at this level will be worse so skip
					GO_BACK()
				else 
				{	
					if( depth != dump )
					{
						sol[depth] = metric->_prev;
						prev_free[metric->_prev] = false;
						GO_NEXT();
					}
					else if( new_err < best_error )
					{
						best_error = new_err;
						sol[depth] = metric->_prev;			
						push_solution( solution, sol, best_error, loop_count );
						if( best_error == 0.)
							break;
					}
				}
			}
		}
		if( ++loop_count >= _loop_count_max ) //	&& _solution_cnt!=0 )
		{
			GOOD_PRINT_STRING( "%s() tree too complex at %d solution %d iterations skipping the rest ", __FUNCTION__, _solution_cnt, loop_count );
			break;
		}
	}
	if( !_solution_cnt )
		err_print( "%s() didn't found any solution", __FUNCTION__ );
};

void c_tracking_matrix::init_size( INT32 row_nb, INT32 col_nb )
{
	c_metric_line			empty_line;

	_rows.assign( row_nb, empty_line );

	for( INT32 i = (INT32)_rows.size()-1; i >= 0; --i )
		_rows[i].set_matrix_index(i);

	_cols.assign( col_nb, empty_line );
	for( INT32 i = (INT32)_cols.size()-1; i >= 0; --i )
		_cols[i].set_matrix_index(i);
}

void	c_tracking_matrix::add( INT32 i, INT32 j, REAL error )
{
	c_metric_point	point( i, j, error );
	_rows[i].add( point );
	_cols[j].add( point );
}

//worked
void	c_tracking_matrix::solve_global()
{
	std::vector< c_metric_line* >	next_to_track;

	INT32	nb = (INT32)_rows.size();
	for( INT32 i=0; i<nb; ++i )
	{
		c_metric_line& row = _rows[i];
		if( !row.is_empty() )
		{
			row.sort_by_metric();
			//introduce new id
			row.add( c_metric_point( i, -1, _penality_for_new ) );//_reject_dist));
			//add stop
			row.add( c_metric_point( i, -2, 1000000. ) );
			next_to_track.push_back( &row );
		}
	}
	if( next_to_track.size() > 0 )
	{	
		//	sort again to start first by the smallest in the tree explore
		sort( next_to_track.begin(), next_to_track.end(), metric_less() );

		//	explore a tree of all the possible choices
		//		parsing and measuring at the same time
		//		to avoid as many branch as we can
		//		because this is an NP Complete problem.

		std::vector<INT32>	prevs;
		tree_explore( prevs, next_to_track );
		//	_prevs are ordered in terms of next_to_track
		for( UINT32 i = 0; i < next_to_track.size(); ++i )
		{
			_solution[ next_to_track[i]->get_matrix_index() ] = prevs[i];
		}
	}
}

void	c_tracking_matrix::solve_by_subset()
{
	//	keep track of row/next to process
	std::set<INT32>					nexts;	
	INT32	size = (INT32)_rows.size();
	for( INT32 i=0; i<size; ++i )
		if( !_rows[i].is_empty() )	//empty row are new and so skipped
			nexts.insert(i);

	//	keep track of col/prev still unused
	std::set<INT32>					prevs;	
	size = (INT32)_cols.size();
	for( INT32 i=0; i<size; ++i )
		if( !_cols[i].is_empty() )	//empty col are not accessed so we keep the structure smaller by testing here
			prevs.insert(i);

	//	intermediate to build the subset
	std::vector<INT32>				next_to_add;
	std::vector<INT32>				prev_to_add;
	INT32							prev_count;	//todo use ?
	std::vector< c_metric_line*>	next_to_solve;
	c_metric_line*					line;
	
	while( !nexts.empty() )
	{
		//get the first next to process and remove it
		INT32	next = *nexts.begin();
		nexts.erase(next);
		prev_count = 0;
		next_to_solve.clear();
		//	loop on each row to read the colums
		for(;;)
		{
			//	add row
			line = &_rows[next];
			next_to_solve.push_back( line );
			//	find the prev concerned
			size = line->get_size();
			for( INT32 i=0; i<size; ++i )
			{
				INT32	index = line->get_prev(i);
				if( prevs.erase( index ) )	//	erase return 1 if found so we add only once
				{
					prev_to_add.push_back( index );
					++prev_count;
				}
			}
			//	use it to find the next they intersect and store it
			size = (INT32)prev_to_add.size();
			if( size )
			{
				for( INT32 i=0; i<size; ++i )
				{
					line = &_cols[prev_to_add[i]];
					INT32	size_col = line->get_size();
					for( INT32 j=0; j<size_col; ++j )
					{
						INT32	index = line->get_next(j);
						if( nexts.erase( index ) )	//	erase return 1 if found so we add only once
							next_to_add.push_back( index );
					}
				}
				prev_to_add.clear();
			}
			if( next_to_add.empty() )
				break;
			next = next_to_add.back();		
			next_to_add.pop_back();		
		}
		
		size = (INT32)next_to_solve.size();
		if( size==0 )
		{
			debug_break();
			continue;
		}
		else if( size==1 )
		{
			c_metric_line* row = next_to_solve[0];
			row->sort_by_metric();
			_solution[ row->get_matrix_index() ] = row->get_prev(0);

		}
/*	hereb we should sort the col
		else if( prev_count==1 )
		{
			c_metric_line* row = next_to_solve[0];
			c_metric_line* col = _cols[row->get_prev(0)];
			_solution[ row->get_matrix_index() ] = col->get_matrix_index();
		}
		*/
		else
		{		
			INT32 nb = (INT32)next_to_solve.size();
			for( INT32 i=0; i<nb; ++i )
			{
				c_metric_line* row = next_to_solve[i];
				row->sort_by_metric();
				//introduce new id
				row->add( c_metric_point( i, -1, _penality_for_new ) );//_reject_dist));
				//add stop
				row->add( c_metric_point( i, -2, 1000000. ) );
			}
			//sort again to start first by the smallest in the tree explore
			sort( next_to_solve.begin(), next_to_solve.end(), metric_less() );

			// explore a tree of all the possible choices
			//	parsing and measuring at the same time
			//	to avoid as many branch as we can
			//	because this is an NP Complete problem.

			std::vector<INT32>	sol;
			tree_explore( sol, next_to_solve );
			//_prevs are ordered in terms of next_to_solve
			for( UINT32 i = 0; i < next_to_solve.size(); ++i )
			{
				_solution[ next_to_solve[i]->get_matrix_index() ] = sol[i];
			}
		}
	}
}

void	c_tracking_matrix::solve()
{
	//	the basic solution is only new finger
	_solution.assign( _rows.size(), -1 );
	//	then we try to solve finger not rejected

	if( _b_by_subset )
		solve_by_subset();
	else
		solve_global();
}