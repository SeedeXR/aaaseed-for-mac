
#ifdef AAA_NODE_LIST_UI_H
#error "NODE_LIST_UI_H included more than once."
#endif
#define AAA_NODE_LIST_UI_H 1


#ifndef AAA_STRNUM_H
#	include "strnum.h"
#endif
#ifndef AAA_NAMER_H
#	include "infrastructure/namer.h"
#endif
#ifndef AAA_PARAM_DECLARE_H
#	include "infrastructure/param/param_declare.h"
#endif

template <class T>
class c_node_list_ui : public T
{
	FACTORY_ABSTRACT_DECLARE( c_node_list_ui<T>, T );
private:
	//todo get rid off this
	static INT32 CONST BRANCH_NB_MAX = 16;
	INT32	_index_cur;	// can be -1 if no current
protected:

public:
	virtual	void			update();
//	virtual	void			draw();

	FINLINE	C_PCHAR_C		get_name_str(		INT32 index )	{	return T::get_branch( index )->get_name_str();			}
	FINLINE	T*				get_item(			INT32 index )	{	return index>=0 ? (T*)T::get_branch( index ) : nullptr;	}
	FINLINE	INT32			get_index_nb()						{	return this ? T::get_branch_nb() : 0;						}
	FINLINE	INT32			get_index_cur()						{	return _index_cur;										}
	FINLINE	T*				get_item_cur()						{	return this ? get_item( _index_cur ) : nullptr;			}
	FINLINE	void			set_index_cur(		INT32 index )	{
																	IF_THIS_NULL_RETURN();
																	{
																		INT32 nb = T::get_branch_nb();
																		if( nb )
																			_index_cur = IMOD( index, nb );
																	}
																}
	FINLINE	void			inc_index_cur()						{	set_index_cur( _index_cur + 1 );	}	
	FINLINE	void			dec_index_cur()						{	set_index_cur( _index_cur - 1 );	}	


//	FINLINE	T*				remove(T* one)						{	return (T*)destroy_branch(one); }
	FINLINE	void			remove_and_destroy( INT32 index );
	FINLINE	void			remove_and_destroy_all();

	FINLINE	void			insert_before(				T* one,			INT32 index		);
			AAA_ERR			insert(						c_obj_ui* obj					);
			AAA_ERR			insert_from_file(			C_PCHAR_C		filename,		INT32 where );
	FINLINE	AAA_ERR			insert_at_tail(				C_PCHAR_C		filename )		{ return insert_from_file( filename, T::get_branch_nb() ); }
	virtual	AAA_ERR			save_to_this_file(			o_str CONST &	filename );
	virtual	AAA_ERR			load_from_existing_file(	o_str CONST &	filename );

			void			make_fname_branch(			o_str& fname,	c_obj_ui* obj );
public:
			c_param_def*	build_param_begin(			c_param_def* param	);
	virtual	INT32			get_param_extra_nb()								{ return 0; }
	virtual	void			param_init_pt_extra(		INT32& h			)	{}
			c_param_def*	build_param_branch(			c_param_def* param	);
			void			build_param();

			void			param_init_pt_start(		INT32& h			);
			void			param_init_pt_branch(		INT32& h			);

	virtual	c_param_def*	build_param_extra(			c_param_def* param	)	{ return param; }
public:
	virtual	void			param_init_pt();
};

template <class T>
FINLINE	void	c_node_list_ui<T>::insert_before( T* one, INT32 index )
{
	index = MAX0( index );
	if( _index_cur >= index || _index_cur == -1 )
		++_index_cur;
	T::insert_branch_before( one, index );
	//todo this root manipulation is dirty it avoid a double branch but should be done with insert_branch_before or lower
	//		in fact the def node free its branch in a lower mechanism
	one->set_root_direct( this );
}

template <class T>
FINLINE	void	c_node_list_ui<T>::remove_and_destroy( INT32 index )
{
	IF_THIS_NULL_RETURN();
	{
		T* to_del;
		if( _index_cur >= index )
		{
			if( _index_cur != index || (_index_cur+1) == T::get_branch_nb() )
				--_index_cur;
		}
		to_del = (T*)T::get_branch( index );
		//		to_del = (T*)remove_branch_index(index);
		//		to_del->set_root(nullptr);	//	symmetric of set_root_direct in insert_before
		delete to_del;
	}
}

template <class T>
FINLINE	void	c_node_list_ui<T>::remove_and_destroy_all()
{
	//		index_cur = -1;
	//		return (T*)destroy_branch_all();
	//		INT32	i;
	for( INT32 i = get_index_nb() - 1; i >= 0; --i )
		remove_and_destroy(i);
}



template <class T>
c_param_def*	c_node_list_ui<T>::build_param_begin( c_param_def* param )
{
	//	initialize active param
	//todo BOOL or bool
	param->set_all( TYPE_BOOL, "active", 0., 1., 0., 1. );	//par
	++param;
	//	initialize name_symbo
	param->set_all( TYPE_REF, "name_symbo", 0., 0., 0., 0. );		//par
	++param;
	return param;
}

template <class T>
c_param_def*	c_node_list_ui<T>::build_param_branch( c_param_def* param )
{
	CHAR	name[256];
	CHAR*	str;

	strcpy( name, T::get_factory()->get_property( "param_hook" ) );

	str = name + strlen( name);
	*str++ = '_';
	*(str+2) = 0;

	//	initialize all param and build strs
	for( INT32 i = 0; i < BRANCH_NB_MAX; ++i )
	{
		strnum::make( str, 2, i );	//todo this limit at 2 could be problematic
		//todo BOOL or bool
		param->set_all( TYPE_BOOL, name, 0., 1., 0., 1. );
		++param;
	}
	return param;
}

template <class T>
void	c_node_list_ui<T>::build_param()
{
	T::set_name( (CHAR*)T::get_class_name() );

	o_str& name_more = o_str::push_name( "_" );
		name_more.add( T::get_factory()->get_obj_created_nb() );
		T::add_to_name( name_more.get() );	//todo should look better
	o_str::pop_name();

	INT32 CONST	param_nb = 2 + get_param_extra_nb() + BRANCH_NB_MAX;
	if( T::is_obj_first() )	//todo check it work for every template
	{	
		//	class_name.set( param_name );
		//	class_name.add(  "_node");

		//	alloc params and string for it
		//now make it dynamic
		//todofranz dealloc
		//todo done with c_param_def but this a hack
		c_param_def* param_array = new c_param_def[param_nb];
		if( param_array )
		{
			c_param_def* param = param_array;

			param = build_param_begin(  param );
			param = build_param_extra(  param );
			param = build_param_branch( param );

			T::param_init_with( param_array, param_nb );
			SAFE_DELETE_ARRAY( param_array );
		}
		else
			ERR_PRINT_STRING( "Can't allocate params in c_node_list_ui" );
	}
	else
		T::param_init_with( nullptr, param_nb );	
}

template <class T>
c_node_list_ui<T>::c_node_list_ui(c_factory_base* factory) : SUPER( factory )
//c_node_list_ui<T>::c_node_list_ui()
,_index_cur(-1)
{
}

template <class T>
void	c_node_list_ui<T>::param_init_pt_start( INT32& h )
{
	T::param_set_pt( h, T::get_pt_active() );
	T::param_set_pt( h, T::get_name_symbo() );
}
template <class T>
void	c_node_list_ui<T>::param_init_pt_branch( INT32& h )
{
	INT32 len = T::get_branch_nb();
	for( INT32 i = 0; i < len; ++i )
	{
		auto br = T::get_branch( i );
		T::get_param(h)->set_comment( br->get_class_name() );
		T::param_attach_obj_no_inc( h, br );
		T::param_set_pt( h, T::get_branch_active_pt( i ) );
	}
}

template <class T>
void	c_node_list_ui<T>::param_init_pt()
{
	INT32		h = 0;
	param_init_pt_start( h );
	param_init_pt_extra( h );
	param_init_pt_branch( h );
	T::set_param_nb_used( h );
}

template <class T>
c_node_list_ui<T>::~c_node_list_ui()
{
}

template <class T>
void	c_node_list_ui<T>::update()
{
	if( T::is_active() )
	{
		INT32	len = T::get_branch_nb();
		T*		obj;
		for( INT32 i = 0; i < len; ++i )
		{
			obj = (T*)T::get_branch_active(i);
			if( obj )
			{
				if( obj->is_active() )
					obj->update();
			}
		}
	}
}

/*
template <class T>
void	c_node_list_ui<T>::draw()
{

INT32		i;
INT32		len = get_len();
T*	obj;
	if ( b_active )
		{
		for( i=0; i<len; ++i )
			{
			if ( get_branch_active(i) )
				{
				obj = (T*)get_branch(i);
				if ( obj->is_active() )
					obj->draw();
				}
			}
		}
}
*/

//todo refine where this i, should be in object
template <class T>
AAA_ERR	c_node_list_ui<T>::insert_from_file( C_PCHAR_C filename_in, INT32 where )
{
	if( *filename_in )
	{
		CHAR		cid[64];	//hack ok if cid don't exceed this size
		INT32		version;

		if( NOERR( c_param::load_file_type( filename_in, cid, &version ) ) )
		{
			//	I don't like these casting on one
			c_obj_ui*	one = c_factory_base::create_obj_by_cid( cid );
			if( one )
			{
				o_str& filename = o_str::push_name( filename_in );
					c_namer::set_file_only( one, filename );
					one->load_from_file( filename );
					insert_before( (T*)one, where );
				o_str::pop_name();
				return AAA_OK;
			}
		}
	}
	return ERR_ANY;
}

template <class T>
AAA_ERR	c_node_list_ui<T>::insert( c_obj_ui* obj )
{
	IF_THIS_NULL()
	{
	}
	else if( obj )
	{
		insert_before( (T*)obj, _index_cur+1 );
	}
	return AAA_OK;
}

template <class T>
void	c_node_list_ui<T>::make_fname_branch( o_str& fname, c_obj_ui* obj )
{
	fname.set( T::get_my_filename() );
	fname.drop_ext();
	INT32 i = T::get_branch_index_of( obj );
	CHAR tmp[] = "00";
	strnum::make( tmp, 2, i );
	fname.add( tmp );
	fname.replace_ext( obj->get_fname_ext() );
}

template <class T>
AAA_ERR	c_node_list_ui<T>::save_to_this_file( o_str CONST & filename )
{
	c_param::builder_push( this );
	c_obj_ui::save_to_this_file( filename );
	c_param::builder_pop();
	return AAA_OK;
}

template <class T>
AAA_ERR	c_node_list_ui<T>::load_from_existing_file( o_str CONST & filename )
{
	c_param::builder_push( this );
	c_obj_ui::load_from_existing_file( filename );
	c_param::builder_pop();
	return AAA_OK;
}

/*
template <class T>
virtual AAA_ERR	c_node_list_ui<T>::load_do_before( o_str CONST & filename )
{
	c_param::builder_push( this );
	return AAA_OK;
}

template <class T>
virtual AAA_ERR	c_node_list_ui<T>::load_do_after( o_str CONST & filename )
{
	c_param::builder_pop();
	return AAA_OK;
}
*/
/*
virtual AAA_ERR	c_node_list_ui<T>::save_do_before( o_str CONST & filename )
{
//CHAR	dir[AAA_PATH_MAX()];

//CHAR	loc_filename[AAA_PATH_MAX()];
CHAR*	str_num;

INT32	nb = get_branch_nb();
INT32	i;

	//	change directory
	fname::cpy_dir_name( dir, filename);
	c_file::dir_ch(dir);

	//	get relative name
	fname::get_fname_pure( loc_filename, filename);
	fname::drop_ext( loc_filename);

	//	point to number part
	str_num = loc_filename + strlen(loc_filename);
	*str_num++ = '_';

	//	finish the filename
	strcat( str_num, "00.node_base_name);

	for( i=0; i<nb; ++i )
		{
		strnum::make( str_num, 2, i);
		get_branch(i)->save_to_file(loc_filename);
		}

	c_file::dir_change_to_def();
	return RECODE_OK;
}

virtual AAA_ERR	c_node_list_ui<T>::load_do_after( o_str CONST & filename )
{
//CHAR	dir[AAA_PATH_MAX()];

//CHAR	loc_filename[AAA_PATH_MAX()];
CHAR*	str_num;

INT32	nb = 0;
INT32	i;
	
	remove_all();

		//	change directory
		fname::cpy_dir_name( dir, filename);
		c_file::dir_ch(dir);

		//	get relative name
		fname::get_fname_pure( loc_filename, filename);
		fname::drop_ext( def_filename);

		//	point to number part
		str_num = def_filename + strlen(def_filename);
		*str_num++ = '_';

		//	finish the filename
		strcat( str_num, "00.def");

		for( i=0; i<nb; ++i )
			{
			strnum::make( str_num, 2, i);
			insert_from_file( def_filename, i);
			}
		c_file::dir_change_to_def();
		return AAA_OK;
		}
	return ERR_ANY;
}
*/

