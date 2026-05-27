
#ifdef AAA_OBJ_UI_H
#error "OBJ_UI_H included more than once."
#endif
#define AAA_OBJ_UI_H 1


#ifndef AAA_OBJ_H
#	include "obj.h"
#endif
#ifndef AAA_FACTORY_H
#	include "../factory/factory.h"
#endif	
#ifndef AAA_PARAMS_H
#	include "../param/params.h"
#endif
#ifndef AAA_AAA_FILE_H
#	include "file/aaa_file.h"
#endif
#ifndef AAA_AAA_MEM_H
#	include "aaa_mem.h"
#endif


#if 1
	namespace osc
	{
		class	ReceivedMessage;
		class	ReceivedBundle;
		class	ReceivedPacket;
	};
#else
#	ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#		include "obj_ui/com/osc/OscReceivedElements.h"
#	endif
#endif


class c_root;
class c_namer;
class c_bdd;
class c_connex;

class	c_obj_ui
{
	friend class c_namer;
	friend class c_trax;
public:
	typedef c_obj_ui	SELF;
	typedef c_obj		SUPER;
	typedef	UINT32		OBJ_UI_ID;

	friend	c_factory_abstract<SELF>;
	static	c_factory_abstract<SELF>&	the_factory();

private:
	static	c_obj_ui**	id_to_obj;
	static	o_str		o_sum_up;
	static	c_root*		the_global_obj_ui_root;

public:
	FINLINE static	c_root* get_the_root()		{	return the_global_obj_ui_root;	}

	FINLINE	bool		is_the_root()  CONST	{	return this == (c_obj_ui*)get_the_root();	}

	static	void		c_init();
	static	void		c_deinit();

	static	bool		b_verbose_load;
	static	bool		b_verbose_save;

	static	bool		b_aaa_exiting;
	static	bool		b_aaa_exiting_fast;
	static	bool		b_aaa_exiting_hack;

	static	void		class_dealloc();
	static	void		check_all();
	static	void		assign_orphan();

			bool		do_dialog_forget( C_PCHAR_C what, INT32 CONST index );
	virtual c_obj_ui*	get_obj_sub_by_index( INT32 CONST index ) CONST ;	// used for modules/module/layers/layer and perhaps traxs later (2024 June)

	//	these find by name_symbo
	//		these search from the top
	static	c_obj_ui*	find_from_top_by_name_symbo(					C_PCHAR_C		name					);
	static	c_obj_ui*	find_from_top_by_name_symbo(					o_str CONST &	name					);
	static	c_obj_ui*	find_from_top_by_class_and_name_symbo(			C_PCHAR_C		class_name,				o_str CONST & o_sym		);
	static	c_obj_ui*	find_from_top_by_class_start_and_name_symbo(	C_PCHAR_C		class_name, INT32 nb,	o_str CONST & o_sym		);
	//		these search down then up from the object
			c_obj_ui*	find_by_name_symbo(								o_str CONST &	name					);
			c_obj_ui*	find_by_class_name(								C_PCHAR_C		class_name				);
			c_obj_ui*	find_by_class_and_name_symbo(					C_PCHAR_C		class_name,				o_str CONST & o_sym		);
			c_obj_ui*	find_by_class_and_name_symbo_except(			C_PCHAR_C		class_name,				o_str CONST & o_sym,	c_obj_ui* o_no );
			c_obj_ui*	find_by_class_start_and_name_symbo(				C_PCHAR_C		class_name, INT32 nb,	o_str CONST & o_sym		);
	//		these search down from the object
			c_obj_ui*	find_down_by_name_symbo(						o_str CONST &	name					);
			c_obj_ui*	find_down_by_class_name(						C_PCHAR_C		class_name				);
			c_obj_ui*	find_down_by_class_and_name_symbo(				C_PCHAR_C		class_name,				o_str CONST & o_sym		);
			c_obj_ui*	find_down_by_class_and_name_symbo_except(		C_PCHAR_C		class_name,				o_str CONST & o_sym,	c_obj_ui* o_no );
			c_obj_ui*	find_down_by_class_start_and_name_symbo(		C_PCHAR_C		class_name, INT32 nb,	o_str CONST & o_sym		);
	//		these search up from the object
			c_obj_ui*	find_up_by_name_symbo(							o_str  CONST &	name					);
			c_obj_ui*	find_up_by_class_name(							C_PCHAR_C		class_name				);
			c_obj_ui*	find_up_by_factory(								c_factory_base CONST * CONST factory	);
template<class T>
			FINLINE	T*	find_up_by_class()								{	return	(T*)find_up_by_factory( (c_factory_base CONST *)&T::the_factory() );	}

			c_obj_ui*	find_up_by_class_and_name_symbo(				C_PCHAR_C		class_name,				o_str CONST & o_sym	);
			c_obj_ui*	find_up_by_class_start_and_name_symbo(			C_PCHAR_C		class_name, INT32 nb,	o_str CONST & o_sym	);

	static	void		set_searchable_by_filename( bool b );
//	static	c_obj_ui*	find_by_filename(								C_PCHAR_C		filename								);
	static	c_obj_ui*	find_from_top_by_name_search(					C_PCHAR_C		filename								);
	static	c_obj_ui*	find_first_by_class_name(						C_PCHAR_C		class_name								);
//	static	c_obj_ui*	find_from_top_by_class_name_and_param(			C_PCHAR_C		class_name, c_param CONST * CONST par	);

	static	c_obj_ui*	find_from_top_by_name(							C_PCHAR_C		name									);
			c_bdd*		find_bdd_by_name_symbo(							o_str CONST &	o_sym									);
//
//	ID and REF
//

//todo move to 64 bit ?
//todo separate type for ref, type as class or structure
// _obj_ui_id store a unique number for each object start at 1, 0 is not valid
// ref fill up an array (2022 Oct) document this later
public:
	static	CONSTEXPR INT32		ID_BIT_NB	=	20;
	static	CONSTEXPR UINT32	ID_MASK		=	(1<<ID_BIT_NB) - 1;
	static	CONSTEXPR UINT32	ID_MIN		=	1;	
	static	CONSTEXPR UINT32	ID_MAX		=	ID_MASK;	

	static	CONSTEXPR UINT32	SIGNATURE_OBJ_EXIST			=	0x33;
	static	CONSTEXPR UINT32	SIGNATURE_OBJ_EXIST_SHIFT	=	26;
	static	CONSTEXPR UINT32	SIGNATURE_OBJ_EXIST_MASK	=	0x3f << SIGNATURE_OBJ_EXIST_SHIFT;

	static	CONSTEXPR INT32		FLAGS_SHIFT		=	24;
	static	CONSTEXPR UINT32	MASK_REF		=	(1<<FLAGS_SHIFT) - 1;

	static	CONSTEXPR UINT32	FLAG_REF_OBJ	=	0x1;
	static	CONSTEXPR UINT32	FLAG_REF_PARAM	=	0x2;
	static	CONSTEXPR UINT32	FLAGS_REF_SHIFT	=	24;
	static	CONSTEXPR UINT32	FLAGS_REF_MASK	=	0x3 << FLAGS_REF_SHIFT;


private:
	static	FINLINE CONSTEXPR OBJ_UI_ID compose_id_and_flag( OBJ_UI_ID id, UINT32 flag )	{ return (id & ID_MASK) | (flag << ID_BIT_NB);  }
	static	FINLINE CONSTEXPR UINT32	extract_flag(		UINT32 id )						{ return id >> ID_BIT_NB;  }
	static	FINLINE CONSTEXPR UINT32	clear_obj_exist(	UINT32& ref )					{ return ref &= ~SIGNATURE_OBJ_EXIST_MASK;  }
	static	FINLINE CONSTEXPR UINT32	is_obj_exist(		UINT32 id )						{ return (id >> SIGNATURE_OBJ_EXIST_SHIFT) == SIGNATURE_OBJ_EXIST;  }
			FINLINE			  bool		is_valid_obj_ui()	CONST							{ return is_obj_exist(_obj_ui_id); }	//	we remove this when we delete obj


//	static	FINLINE CONSTEXPR void		set_ref_type(		UINT32& ref, UINT32 type )		{ ref = (ref & ~FLAGS_REF_MASK) | (type << FLAGS_REF_SHIFT);  }
	static	FINLINE CONSTEXPR void		set_ref_obj(		UINT32& ref )					{ ref = (ref & ~FLAGS_REF_MASK) | (FLAG_REF_OBJ << FLAGS_REF_SHIFT);  }
	static	FINLINE CONSTEXPR void		set_ref_param(		UINT32& ref )					{ ref = (ref & ~FLAGS_REF_MASK) | (FLAG_REF_PARAM << FLAGS_REF_SHIFT);  }
//	static	FINLINE CONSTEXPR UINT32	extract_ref_type(	UINT32 ref )					{ return ref >> REF_FLAGS_SHIFT;  }
	static	FINLINE CONSTEXPR bool		is_ref_obj(			UINT32 ref )					{ return ref>>FLAGS_REF_SHIFT == FLAG_REF_OBJ;  }
	static	FINLINE CONSTEXPR bool		is_ref_param(		UINT32 ref )					{ return ref>>FLAGS_REF_SHIFT == FLAG_REF_PARAM;  }

			OBJ_UI_ID	_obj_ui_id;

	static	UINT32		obj_ui_nb_created;
	static	UINT32		obj_ui_nb;

	static	UINT32		ref_obj_nb;
	static	UINT32		ref_param_nb;
	static	o_str		ref_error;

public:
	static FINLINE	bool is_id_valid(	OBJ_UI_ID CONST id_in	)
	{
		return id_in <= ID_MAX;
	}
#if AAA_DEBUG()
	static			c_obj_ui*	get_from_id(	OBJ_UI_ID CONST id_in	);
#else
	static FINLINE	c_obj_ui*	get_from_id(	OBJ_UI_ID CONST id_in	)
	{
		return (id_in <= ID_MAX) ? *(id_to_obj+id_in) : nullptr;	//case 0 should be handled by a nullpte at the beginning of id_to_obj
	}
#endif

	static	FINLINE	UINT32*		get_obj_ui_nb_pt()				{	return &obj_ui_nb; }
	static	FINLINE	UINT32*		get_obj_ui_nb_created_pt()		{	return &obj_ui_nb_created; }

			FINLINE	OBJ_UI_ID	get_obj_ui_id()	 CONST 			{	return _obj_ui_id & ID_MASK; }

public:
	static	void				set_ref_error( C_PCHAR_C str )	{	ref_error.set(str);		}
	static	UINT32*				get_ref_obj_nb_pt()				{	return &ref_obj_nb;		}
	static	UINT32*				get_ref_param_nb_pt()			{	return &ref_param_nb;	}
	static	C_PCHAR_C			get_ref_error_str()				{	return ref_error.get(); }

			UINT32				get_obj_ref();		
			void				remove_obj_ref();	

	static	bool				is_obj_ref(					UINT32 ref );	
	static	c_obj_ui*			get_obj_from_ref(			UINT32 ref );
	static	c_obj_ui*			get_obj_from_ref_direct(	UINT32 ref );	//unused maa 2023 Jan

	static	UINT32				get_param_ref(				c_param * CONST param );
	static	void				remove_param_ref(			c_param * CONST param );
	static	bool				is_param_ref(				UINT32 ref );
	static	c_param*			get_param_from_ref(			UINT32 ref );
	static	c_param*			get_param_from_ref_direct(	UINT32 ref );	//unused maa 2023 Jan

private:
	bool							_b_obj_active_ui;

	c_params						_params;

	c_factory_base*					_factory;
	c_obj_ui*						_root;
	c_list_pt_active< c_obj_ui >*	_branch;	//todo perhaps we should move to a list of ID here too

//	c_connex::LIST*					_outs;
//	c_connex::LIST*					_ins;
	o_str*							_name_ui;		//	used for the ui and messages
//todo make it a pointer too but this should function with param too 
	o_str							_name_sym;		//	symbolic name used to reference object	
													//		have to be readable by human
													//		but have no other purpose 
	o_str							_my_filename;	//	the name the obj is loaded with,
													//		or saved if not loaded,
													//		or null if not loaded or saved yet.
													//	my_filename is relative to the cur dir when loaded or saved				
	o_str							_name_search;	//	use by the AAASeed "language" (command and lua for now)
													//		this is a unique name build from my_filename (which can't be the same for different object)
													//		but the path is relative to the data directory of AAASeed
	//hack and the search should be optimized
	INT32							_file_version;



	REAL							_cell_pos[3];
	REAL							_cell_size;
	INT32							_cell_render_frame;

	c_namer*						_namer;
	void*							_lua_ud;

public:
	c_obj_ui( c_factory_base* factory );
	virtual ~c_obj_ui() = 0;

	FINLINE	c_factory_base*	get_factory()								CONST	{	return _factory; }
	FINLINE	C_PCHAR_C		get_class_name()							CONST	{	return get_factory()->get_class_name();					}
	FINLINE	bool			is_class_name(				C_PCHAR_C str )	CONST	{	return get_factory()->is_class_name( str );				}
	FINLINE	bool			is_class_match(				C_PCHAR_C str )	CONST	{	return get_factory()->is_class_match( str );			}
	FINLINE	bool			is_inherited_from(			C_PCHAR_C str )	CONST	{	return get_factory()->is_in_get_hierarchic_name( str );	}
	FINLINE	o_str CONST &	get_name_human()							CONST	{	return get_factory()->get_name_human();					}
	virtual C_PCHAR_C		get_fname_ext()								CONST	{	return get_factory()->get_file_ext().get();				}
	FINLINE	bool			is_same_class( c_obj_ui* src )				CONST	{	return get_factory() == src->get_factory();				}
	template < class T >
	FINLINE	bool			is_class()									CONST	{	return T::is_instance(this);							}
	FINLINE	bool			is_obj_first()								CONST	{	return get_factory()->is_obj_first();					}

	virtual void			set_active( bool CONST value );
	FINLINE	void			flip_active()										{	set_active( !_b_obj_active_ui );	}
	FINLINE	bool			is_active()									CONST	{	return _b_obj_active_ui;			}
	FINLINE	bool*			get_pt_active()										{	return &_b_obj_active_ui;			}

	virtual	void			before_exit()										{}

//todo redon using the spy stuff 
//			void			tbuf_add( INT32 channel_id, REAL val_in, CHAR* tex_in );
//			void			tbuf_inc( INT32 channel_id, REAL val_in, CHAR* tex_in );
//			void			tbuf_dec( INT32 channel_id, REAL val_in, CHAR* tex_in );

private:
			void			dbg_print_before() CONST;
			void			err_print_before() CONST;
			void			 ui_print_before() CONST;

public:
	virtual	void			print_string(						C_PCHAR_C fmt,				...	) CONST;
	virtual	void			dbg_print(							C_PCHAR_C fmt = nullptr,	...	) CONST;
	virtual	void			err_print(					C_PCHAR_C fmt = nullptr,	...	) CONST;
			void			err_print_method_unimplemented(		C_PCHAR_C fn_name				) CONST;
			void			err_print_unused(					C_PCHAR_C lib_name, C_PCHAR_C fn_name ) CONST;
	virtual	void			ui_print(					C_PCHAR_C fmt = nullptr,	...	) CONST;
	virtual	void			box_err(							C_PCHAR_C fmt = nullptr,	...	) CONST;

	virtual	INT32			get_file_version_save() CONST;
	FINLINE	INT32			get_file_version()	CONST			{ return _file_version;		}
	FINLINE	void			set_file_version( INT32 version )	{ _file_version = version;	}

	virtual void			update()				{}
	virtual void			draw()					{}
	virtual void			restart()				{}
	virtual	void			update_then_draw();

	virtual	void			update_lua();
	virtual	bool			update_then_draw_lua();

//NAME
			void			set_name(		o_str CONST & name_in	);
			void			set_name(		C_PCHAR_C name_in		);
			void			set_name_with(	C_PCHAR_C start_name,	INT32 CONST index );
			void			add_to_name(	C_PCHAR_C str			);
			void			init_name_with(	C_PCHAR_C start_name	);
			void			init_name_with(	C_PCHAR_C start_name,	INT32 CONST index );
			C_PCHAR_C		get_name_str() CONST;
			o_str CONST & 	get_name_dbg() CONST;

	FINLINE	o_str CONST * 	get_name_ui(								) CONST	{	return _name_ui;							}

	FINLINE	o_str CONST &	get_name_symbo(								) CONST	{	return _name_sym;							}
	FINLINE	o_str&			get_name_symbo(								)		{	return _name_sym;							}
	FINLINE	bool			is_name_symbo(	o_str CONST & name			) CONST	{	return _name_sym.is_equal( name );			}
	FINLINE	bool			is_name_symbo(	C_PCHAR_C name, INT32 len	) CONST	{	return _name_sym.is_str_equal( name, len );	}
	FINLINE	void			set_name_symbo(	C_PCHAR_C name				)		{	_name_sym.set( name );	}

	FINLINE	o_str CONST & 	get_name_search()		CONST	{	return _name_search;				}
	FINLINE	C_PCHAR_C		get_name_search_str()	CONST	{	return _name_search.get();			}
//FILENAME

	FINLINE	bool			is_my_filename(		)	CONST	{	return !_my_filename.is_empty();	}
			C_PCHAR_C		get_my_filename(	)	CONST;
			C_PCHAR_C		set_my_filename(	o_str CONST  &	filename );

//PARAM and UI
private:
			void			set_param(		c_param * CONST pt, INT32 CONST nb );

public:
//	virtual	void			build_sum_up( o_str* o )	{}
	virtual	bool			is_draw_sum_up()			{ return false; }
	virtual	void			draw_sum_up()				{}
	virtual	void			draw_icon()					{}

			void			param_init_with( c_param_def CONST * CONST pt, INT32 CONST nb );
			void			param_dealloc();
	virtual	void			param_init();
	virtual	void			param_init_pt_static();			// called once at creation
	virtual	void			param_init_pt()				{}	// called to update param which needed too, accessible from lua too
	virtual	void			prepare_for_ui();			// called by c_param::draw() when about to be displayed

	virtual bool			param_do_action( c_param * CONST par, aaa::param::ACTION CONST action );

	FINLINE	c_param*		get_param_begin()			CONST							{	return _params.get_param_begin();				}
	FINLINE	INT32			get_param_nb_allocated()	CONST							{	return _params.get_param_nb_allocated();		}
	FINLINE	INT32			get_param_nb_used()			CONST							{	return _params.get_param_nb_used();				}
	FINLINE	void			set_param_nb_used(			INT32 nb )						{	_params.set_param_nb_used( nb );				}
	FINLINE	c_param*		get_param(					INT32 index )	CONST			{	return _params.get( index );					}
	FINLINE	void			swap_param(					INT32 a, INT32 b )				{	_params.swap_param( a, b );						}
	FINLINE	void			swap_param_expand(			INT32 a, INT32 b )				{	_params.swap_param_expand( a, b );				}

	FINLINE	void			param_read_from_mem(		CHAR CONST * CONST src )					{ _params.param_read_from_mem(		 src );		}
	FINLINE	void			param_read_from_mem(		CHAR CONST * CONST src, INT32 CONST nb )	{ _params.param_read_from_mem(		 src, nb ); }
	FINLINE	CHAR*			param_write_to_mem(			CHAR * CONST dst )			       CONST	{ return _params.param_write_to_mem( dst );		}
	FINLINE	CHAR*			param_write_to_mem(			CHAR * CONST dst, INT32 CONST nb ) CONST	{ return _params.param_write_to_mem( dst, nb );	}

	FINLINE	c_param*		get_param_by_name(			C_PCHAR_C name_in )	CONST		{	return _params.get_param_by_name(		name_in );	}
	FINLINE	INT32			get_param_index_by_name(	C_PCHAR_C name_in )	CONST		{	return _params.get_param_index_by_name(	name_in );	}

	//	FINLINE	bool		is_param_belong( c_param CONST * param )		CONST	{	return _params.is_param_belong( param );	}
	//param nor sur we keep this except to check
	FINLINE	INT32			get_param_index( c_param CONST * param )		CONST		{	return _params.get_param_index( param );	}

	FINLINE	c_param*		get_param_header()		CONST								{	return _params.get_param_header();			}
			void			make_param_header_name();

			void			set_param_value(		C_PCHAR_C param_name, C_PCHAR_C param_value, bool CONST b_str );
private:
			void			param_init_to_ina();	// done only at init called by param_init_with()
public:
			void			param_set_to_ina();		// don't set the don't save param
			void			param_set_to_def();

#if	AAA_DEBUG()
			void			err_param_init_pt( INT32 nb_in );
#else
	FINLINE	void			err_param_init_pt( INT32 nb_in ) {}
#endif
						//hack this done for testing unstable
//								void	param_next_set_list(	INT32 h,		INT32 nb );

						FINLINE	void	param_set_pt_null(		INT32& h )										CONST { _params.set_pt_null(h); }
						FINLINE	void	param_set_pt_null_v2(	INT32& h )										CONST { _params.set_pt_null_v2(h); }
						FINLINE	void	param_set_pt_null_v3(	INT32& h )										CONST { _params.set_pt_null_v3(h); }
						FINLINE	void	param_set_pt_null_vn(	INT32& h,		INT32 nb )						CONST { _params.set_pt_null_vn(h,nb); }
template	<class T>	FINLINE	void	param_set_pt(			INT32& h,		T& pt )							CONST { _params.set_pt(h,pt); }
template	<class T>	FINLINE	void	param_set_pt(			INT32& h,		T* CONST pt )					CONST { _params.set_pt(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_no_inc(	INT32 CONST h,	T* CONST pt )					CONST { _params.set_pt_no_inc(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_even_null(	INT32& h,		T* CONST pt )					CONST
																												{
																													if(pt)
																														_params.set_pt(h,pt);
																													else
																														param_set_pt_null(h);
																												}
template	<class T>	FINLINE	void	param_set_pt_2(			INT32& h,		T* CONST pt )					CONST { _params.set_pt_v2(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_3(			INT32& h,		T* CONST pt )					CONST { _params.set_pt_v3(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_4(			INT32& h,		T* CONST pt )					CONST { _params.set_pt_v4(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_5(			INT32& h,		T* CONST pt )					CONST { _params.set_pt_v5(h,pt); }
template	<class T>	FINLINE	void	param_set_pt_n(			INT32& h,		T* CONST pt, INT32 CONST nb )	CONST { _params.set_pt_vn(h,pt,nb); }
// deal with Red Green Blue Factor Alpha																						
template	<class T>	FINLINE	void	param_set_pt_rgbfa(		INT32& h,		T pt )
						{
							param_set_pt_3(	h, pt );
							param_set_pt(	h, pt[4] );
							param_set_pt(	h, pt[3] );
						}

						FINLINE	void	param_set_unused(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused  ( h, b ); }
						FINLINE	void	param_set_unused_2(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused_2( h, b ); }
						FINLINE	void	param_set_unused_3(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused_3( h, b ); }
						FINLINE	void	param_set_unused_4(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused_4( h, b ); }
						FINLINE	void	param_set_unused_5(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused_5( h, b ); }
						FINLINE	void	param_set_unused_6(		INT32 CONST h, bool CONST b )					CONST { _params.set_unused_6( h, b ); }
						FINLINE	void	param_set_unused_n(		INT32 CONST h, bool CONST b, INT32 CONST nb )	CONST { _params.set_unused_n( h, b, nb ); }

						FINLINE	void	param_set_max_no_inc(		INT32 CONST h,	REAL r_in )				CONST { _params.set_max_no_inc(h,r_in); }
						FINLINE	void	param_attach_obj(			INT32& h,		c_obj_ui* CONST pt )	CONST { _params.attach_obj(h,pt); }
						FINLINE	void	param_attach_obj_no_inc(	INT32 CONST h,	c_obj_ui* CONST pt )	CONST { _params.attach_obj_no_inc(h,pt); }
template	<class T>	FINLINE	void	param_set_sum_up(			INT32 h,		T* pt,	bool b=true )	CONST
						{
							if( b && pt )
							{
								pt->build_sum_up( o_sum_up );
								get_param(h)->set_comment( o_sum_up );
							}
							else
								get_param(h)->clear_comment();
						}
template	<class T>	FINLINE	void	param_attach_sum_up(		INT32 h,		T* pt,	bool b=true )	CONST 
						{
							param_set_sum_up( h, pt, b );
							param_attach_obj_no_inc( h, pt );
						}
template	<class T>	FINLINE	void	param_set_pt_attach_obj(	INT32& h,		T* pt, c_obj_ui* CONST obj ) CONST
						{
							_params.attach_obj_no_inc( h, obj ); 
							_params.set_pt( h, pt );
						}
template	<class T>	FINLINE	void	param_set_pt_attach_obj(	INT32& h,		T& pt, c_obj_ui* CONST obj ) CONST
						{
							_params.attach_obj_no_inc( h, obj ); 
							_params.set_pt( h, pt );
						}

//	virtual bool		callback_change( void* data )	{	return false;	}

	virtual	o_str*		get_comment() CONST;

	virtual	bool		do_command(		C_PCHAR_C cmd );
	virtual	bool		enum_command(	o_str& o );

//CONNEX
// todo probably should be removed
//			void		param_connex_set();

//PARENT
//	FINLINE	bool		is_out()								CONST	{	return _outs->is_item();							}
//	FINLINE	INT32		get_out_nb()							CONST	{	return _outs->get_item_nb();						}
//	FINLINE	c_connex*	get_out_connex(		INT32 index )		CONST	{	return is_out() ? _outs->get_item(index) : nullptr;	}
			INT32		add_out_connex(		c_connex* con );
			c_connex*	remove_out_connex(	c_connex* con );
//			c_connex*	remove_out_index(	INT32 index );
			AAA_ERR		remove_out_all();
			AAA_ERR		unplug_out_all();
//CHILD
//	FINLINE	bool		is_in()									CONST	{	return _ins->is_item();								}
//	FINLINE	INT32		get_in_nb()								CONST	{	return _ins->get_item_nb();							}
//	FINLINE	c_connex*	get_in_connex(		INT32 index )		CONST	{	return is_in() ? _ins->get_item(index) : nullptr;	}
			INT32		add_in_connex(		c_connex* con );
			c_connex*	remove_in_connex(	c_connex* con );
//			c_connex*	remove_in_index(	INT32 index );
			AAA_ERR		remove_in_all();
			AAA_ERR		unplug_in_all();

//BRANCH
	FINLINE	bool		is_branch			()					CONST	{	return _branch && _branch->is_item();			}
	FINLINE	INT32		get_branch_nb		()					CONST	{	return _branch ? _branch->get_item_nb() : 0;	}
			INT32		get_branch_active_nb() CONST;
	FINLINE	c_obj_ui*	get_branch			( INT32 index )		CONST	{	return (index<get_branch_nb()) ? _branch->get_item(index) : nullptr;	}
	FINLINE	c_obj_ui*	get_branch_active	( INT32 index )		CONST	{	return (index<get_branch_nb() && _branch->is_item_active(index)) ? _branch->get_item(index) : nullptr; }
	FINLINE	INT32		get_branch_index_of	( c_obj_ui* obj )	CONST	{	return _branch ? _branch->found_index(obj) : -1;				}
//	FINLINE	bool		is_branch_active	( INT32 index )		CONST	{	return _branch->is_item_active(index);							}
//	FINLINE	bool*		get_branch_active_pt( INT32 index )		CONST	{	return (index<get_branch_nb()) ? _branch->get_item_active_pt(index) : nullptr;						}
	// safe if get_branch_nb() return > 0 before calling it
	FINLINE	bool*		get_branch_active_pt( INT32 index )		CONST	{	return _branch->get_item_active_pt(index);	}
			INT32		add_branch			( c_obj_ui* obj );

			c_obj_ui*	find_branch_by_name_symbo(	o_str& name											)	CONST;
			void		find_branchs_by_class_name(	C_PCHAR_C class_name,	std::vector<c_obj_ui*>& vec	)	CONST;
			c_obj_ui*	find_branch_by_class_name(	C_PCHAR_C class_name								)	CONST;
			c_obj_ui*	find_branch_by_my_filename(	C_PCHAR_C name										)	CONST;

	FINLINE	void 		insert_branch_before( c_obj_ui* obj, INT32 index )
						{
							if( _branch )
								_branch->insert_before(obj,index);
							else
								add_branch(obj);
						}
			c_obj_ui*	remove_branch		( c_obj_ui* obj );
			c_obj_ui*	remove_branch_index	( INT32 index );
			AAA_ERR		remove_branch_all	();
	//unused					
	//		AAA_ERR		destroy_branch		( c_obj_ui* obj );
	//		AAA_ERR		destroy_branch_index( INT32 index );
	//		AAA_ERR		destroy_branch_all	();

	FINLINE	c_obj_ui*	branch_prev() 		CONST	{	return _root ? _root->_branch->get_prev(this) : 0;	}
	FINLINE	c_obj_ui*	branch_next() 		CONST	{	return _root ? _root->_branch->get_next(this) : 0;	}

			void		set_root(			c_obj_ui* obj );
			void		set_root_direct(	c_obj_ui* obj );
	FINLINE	c_obj_ui* 	get_root()			CONST	{	return _root;	}

	static	void		show_orphan();	//	this should help make sure everybody have a root
	static	void		test_create_delete();



//DRAW AND UI
			void		set_focus();		//if virtual it crash with a null pt
	virtual	void		become_ui()			{}

	//this way we can track better
			void		mem_print( CHAR* str );
/*
			//void*		MALLOC( INT32 s, size_t alignment = 16 );
			void		FREE( void* memblock, size_t alignment = 16 );
			//void*		REALLOC( void* memblock, INT32 size, size_t alignment = 16 );
	template<class T>
	FINLINE	void		FREE_AND_NULL( T*& memblock, size_t alignment = 16 )
						{
							FREE( (void*) memblock, alignment );
							memblock =  nullptr;
						}
*/
//GL
	virtual	void			release_texture( c_obj_ui* obj_new, INT32 bind )			{}
//CELL DRAW
	virtual	void			cell_draw_obj( REAL CONST size );

	void					set_cell_pos_size_frame( REAL CONST* CONST pos, REAL CONST size, INT32 CONST frame );
//			void			set_cell_pos( REAL CONST* CONST src );
	FINLINE	REAL CONST *	get_cell_pos()						CONST	{	return _cell_pos;			}
//	FINLINE	void			set_cell_size( REAL in ) { _cell_size = in; }
	FINLINE	REAL			get_cell_size()						CONST	{	return _cell_size;			}
//	FINLINE	void			set_cell_render_frame(INT32 in)				{ _cell_render_frame = in; }
	FINLINE	INT32			get_cell_render_frame()				CONST	{	return _cell_render_frame;	}




//NAMER	will be the mechanism to store and get obj name
	FINLINE	c_namer*		get_namer()							CONST	{	return _namer;				}
	FINLINE	void			set_namer( c_namer* in)						{	_namer = in;				}

	FINLINE	bool			is_lua_ud()							CONST	{	return _lua_ud != nullptr;	}
	FINLINE	void*			get_lua_ud()						CONST	{	return _lua_ud;				}
	FINLINE	void			set_lua_ud( void* ud_in )					{	_lua_ud = ud_in;			}

	//hack potential trouble when target deleted but emergency FAF prod
//	virtual bool			set_target( INT32 slot,	c_obj_ui* target )	;	//slot start at 1
//	virtual c_obj_ui*		get_target( INT32 slot	)			CONST	;	//slot start at 1

//FILE	
private:
			bool			check_before_file_op(		C_PCHAR_C		filename ) CONST;
			bool			check_before_file_op(		o_str CONST &	filename ) CONST;	

			AAA_ERR			load_from_file_common(		o_str CONST &	filename );

	FINLINE	void			check_ext(					o_str CONST &	filename ) CONST;

public:
	virtual	AAA_ERR			save_to_this_file(			o_str CONST &	filename );
	virtual	AAA_ERR			load_from_existing_file(	o_str CONST &	filename );

	virtual AAA_ERR			load_do_before(				o_str CONST &	filename );
	virtual AAA_ERR			load_do_after(				o_str CONST &	filename );
	virtual AAA_ERR			save_do_before(				o_str CONST &	filename );
	virtual AAA_ERR			save_do_after(				o_str CONST &	filename );														

	FINLINE	AAA_ERR			load_from_file(				C_PCHAR_C		filename )	{ return load_from_file( o_str(filename) ); }
	FINLINE	AAA_ERR			save_to_file(				C_PCHAR_C		filename )	{ return save_to_file(   o_str(filename) ); }

	virtual AAA_ERR			load_from_file(				o_str CONST &	filename );
	virtual AAA_ERR			save_to_file(				o_str CONST &	filename );
			
			AAA_ERR			load_from_file_replace_ext(	o_str CONST &	filename );
			AAA_ERR			save_to_file_replace_ext(	o_str CONST &	filename );

			AAA_ERR			load_from_file_add_ext(		o_str CONST &	filename );
			AAA_ERR			save_to_file_add_ext(		o_str CONST &	filename, C_PCHAR_C ext );
			AAA_ERR			save_to_file_add_ext(		o_str CONST &	filename );

			AAA_ERR			load()						{	return load_from_file( get_my_filename() );	}
			AAA_ERR			save()						{	return save_to_file(   get_my_filename() );	}

			AAA_ERR			save_tree_to_file(			o_str CONST &	filename	);	//unused,	bool b_full );

	virtual	bool			is_default_save_always()	{	return false;	}
	
			c_obj_ui*		obj_get(							c_obj_ui*& obj,	c_factory_base* CONST pf							);
			void			obj_get_load_from_existing_file(	c_obj_ui*& obj, c_factory_base* CONST pf,	o_str CONST & filename	);
	FINLINE	void			try_obj_load_with_this_filename(	c_obj_ui*& obj, c_factory_base* CONST pf,	o_str CONST & filename	);
			void			try_obj_load_add_ext(				c_obj_ui*& obj, c_factory_base* CONST pf,	o_str CONST & filename	);

private:
			void			obj_new_private(			c_obj_ui* obj );

public:
	template<class T>	FINLINE		void	obj_delete(							T*& obj )
											{
												SAFE_DELETE( obj );
											//	DBG_HEAP_CHECK();
											}
	template<class T>	FINLINE		T*		obj_new(							T*& obj );
	template<class T>	FINLINE		T*		obj_get(							T*& obj );
	template<class T>	FINLINE		void	obj_get_load_from_existing_file(	T*& obj,			o_str CONST & filename	);
	template<class T>	FINLINE		void	try_obj_load_with_this_filename(	T*& obj,			o_str CONST & filename	);
	template<class T>	FINLINE		void	try_obj_load_add_ext(				T*& obj,			o_str CONST & filename	);


	virtual AAA_ERR			load_data_from_filename(	o_str CONST & filename, INT32 type_io );

/*
//REFERENCE
private:
	bool				_b_referenced;
public:
	FINLINE	void		clear_referenced()					{	_b_referenced = false;	}
	FINLINE	void		set_referenced()					{	_b_referenced = true;	}
	FINLINE	bool		is_referenced()						{	return	_b_referenced;	}
*/

#if AAA_STATE_COMPILE()
//STATE	this is a central mecanism to change state
			void		state_do_action();
#endif //AAA_STATE_COMPILE

//	operation with other object
			void		cpy_params_from(		c_obj_ui* src, INT32 index_begin=0, INT32 nb=-1 );
			void		cpy_params_from_skip(	c_obj_ui* src, INT32 nb_skip_at_begin = 0, INT32 nb_skip_at_end = 0 );

			bool		make_script_filename(	o_str& dst_filename, o_str CONST & src_name, C_PCHAR_C ext, o_str* CONST name_relative = nullptr );

			void		net_send_param(			INT32 CONST net_channel,	INT32 nb	);
			void		net_receive_param(		CHAR CONST * pt,			INT32 nb	);
			void		net_receive_param(		INT32 CONST net_channel,	INT32 nb	);

	virtual	void		osc_process_message(	osc::ReceivedMessage CONST & msg	);
			void		osc_process_bundle(		osc::ReceivedBundle  CONST & b		);
			void		osc_process(			osc::ReceivedPacket  CONST & p		);
			void		osc_process(			UINT8 CONST * pt, INT32 CONST size	);

			INT32		osc_take_and_process(	C_PCHAR start, bool b_verbose = false );
};



template<class T>
FINLINE	T*	c_obj_ui::obj_new( T*& obj )
{
	obj = new T;
	if( obj )
	{
		obj_new_private( obj );
		return	obj;
	}
	//todo get the class of T to refine error message
	debug_break( "%s() can't allocate object", __FUNCTION__ );
	return nullptr;
}

template<class T>
FINLINE	T*	c_obj_ui::obj_get( T*& obj )
{
	if( obj )
	{
#if AAA_DEBUG()
		mem::is_pointer_fucked( (void*)obj );
#endif
		return obj;
	}
	return obj_new( obj );
}


//todo	this introduce orphan objects
//	try this hack
//	if ok should make a temple x = class_set(this, x);
//	or return nullptr if none
/*
template<class T>
FINLINE	T&	c_obj_ui::obj_get_and_assign( T& obj )
{
	obj_get( obj );
	return obj;
}
*/

template<class T>
FINLINE void	c_obj_ui::obj_get_load_from_existing_file(	T*& obj, o_str CONST & filename )
{
	obj_get_load_from_existing_file(  (c_obj_ui*&)obj, &T::the_factory(), filename );
}

template<class T>
FINLINE void	c_obj_ui::try_obj_load_with_this_filename(	T*& obj, o_str CONST & filename )
{
	if( c_file::is_exist(filename) )
		obj_get_load_from_existing_file(  (c_obj_ui*&)obj, &T::the_factory(), filename );
}

template<class T>
FINLINE void	c_obj_ui::try_obj_load_add_ext(				T*& obj, o_str CONST & filename_in )
{
	try_obj_load_add_ext( (c_obj_ui*&)obj, &T::the_factory(), filename_in );
}

typedef c_obj_ui c_obj_active_ui;

//to get obj with the_root as root
template<class T>	FINLINE	T*		obj_new(				T*& obj )											{	return	((c_obj_ui*)c_obj_ui::get_the_root())->obj_new( obj );	}
template<class T>	FINLINE	T*		obj_get(				T*& obj )											{	return	((c_obj_ui*)c_obj_ui::get_the_root())->obj_get( obj );	}
template<class T>	FINLINE	void	if_obj_save_add_ext(	T* obj, o_str CONST &  filename )					{	if( obj )	obj->save_to_file_add_ext( filename );		}
template<class T>	FINLINE	void	if_obj_save_add_ext(	T* obj, o_str CONST &  filename, C_PCHAR_C ext )	{	if( obj )	obj->save_to_file_add_ext( filename, ext );	}
template<class T>	FINLINE	void	if_obj_load_add_ext(	T* obj, o_str CONST &  filename )					{	if( obj )	obj->load_from_file_add_ext( filename );	}
template<class T>	FINLINE	void	if_obj_update(			T* obj )											{	if( obj )	obj->update();								}
template<class T>	FINLINE	bool	is_obj_exist_and_active(T CONST * CONST obj )								{	return obj && obj->is_active();							}

/*	class	c_obj_active_ui: public c_obj_ui	{	FACTORY_ABSTRACT_DECLARE(c_obj_active_ui,c_obj_ui);	};	*/

FINLINE c_obj_ui*	get_obj_ui_from_id(	OBJ_UI_ID CONST id_in )	{	return c_obj_ui::get_from_id(id_in);	}
