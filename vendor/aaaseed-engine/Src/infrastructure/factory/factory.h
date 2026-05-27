
#ifdef AAA_FACTORY_H
#error "FACTORY_H included more than once."
#endif
#define AAA_FACTORY_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_REGISTRY_H
#	include "registry.h"
#endif
#ifndef _LIST_
#	include <list>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _SET_
#	include <set>
#endif

class c_obj_ui;
class c_param_data;
class c_param_def;
//class c_param;

class c_factory_base
{
private:
public:
	static	bool				b_verbose;
	static	INT32				count;
	static	INT32				count_abstract;
	static	INT32				count_concrete;
	static	bool				b_do_dealloc;
//static GET and CREATE	
	static	C_PCHAR_C			get_cid(				INT32 index );
	static	C_PCHAR_C			get_file_ext(			INT32 index );
	static	c_factory_base*		get_by_cid(				C_PCHAR_C cid );
	static	c_factory_base*		get_by_file_ext(		C_PCHAR_C ext );
	
	static	c_obj_ui*			create_obj_by_cid(		C_PCHAR_C cid );
	static	c_obj_ui*			create_obj_by_file_ext(	C_PCHAR_C ext );
//data members
private:
	c_factory_base*				_super_factory;		//	derived from class factory
	o_str						_class_name;		//todo should never change create a type for this
	o_str						_name_human;		//	readable name
	o_str						_file_ext;			//	file extension
	
	UINT32						_obj_count;			//	count existing instances (could be done by the list above too but faster)
	UINT32						_obj_created_nb;	//	count created instances
	std::list<c_obj_ui*>		_objs;				//	keep instance pointers //todo move to some kind of hash
	std::vector<c_param_data*>	_param_data;		//	pointer to allow polymorphic param_data
	
	c_registry<C_PCHAR_C>		_properties;

	std::set<c_factory_base*>	_derived_factories;	//	derived class factories

	virtual	c_obj_ui*			new_hidden()= 0;

protected:
public:
	virtual	c_obj_ui*			create_obj();
	virtual	void				delete_hidden( c_obj_ui* obj ) = 0;

	c_factory_base( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human=nullptr, C_PCHAR_C ext=nullptr, C_PCHAR_C props=nullptr );
	virtual	~c_factory_base();
	void						dealloc_dirty();
	void						dealloc();
//ANCESSOR
#if AAA_DEBUG() && defined(_MSC_VER)
	FINLINE	C_PCHAR_C			get_class_name()		CONST					{ return _class_name.get(); }
#else
	FINLINE	C_PCHAR_C			get_class_name()		CONST					{ return _class_name.get(); }
#endif
	FINLINE	bool				is_class_name(				C_PCHAR_C str )		{ return str_is_equal_bothnocase( get_class_name(), str ); }
			bool				is_class_match(				C_PCHAR_C str );	
	FINLINE	C_PCHAR_C			get_super_class_name()	CONST					{ return _super_factory ? _super_factory->get_class_name() : nullptr; }
			void				get_hierarchic_name(		o_str& dst );
			bool				is_in_get_hierarchic_name(	C_PCHAR_C str );
	FINLINE	c_factory_base*		get_super()										{ return _super_factory; }
	FINLINE	o_str CONST &		get_name_human()								{ return _name_human; }
//	FINLINE	CONST	o_str&		get_name_human_o()								{ return _name_human; }
//	FINLINE	C_PCHAR_C			get_file_ext()									{ return _file_ext.get(); }
	FINLINE	o_str CONST &		get_file_ext()									{ return _file_ext; }

			void				add_derived(			c_factory_base* derived );
			INT32				get_derived_nb();
			c_factory_base*		get_derived(			INT32 index );

			void				set_properties(			CHAR const * command );
			void				add_property(			C_PCHAR_C name, C_PCHAR_C value );
			C_PCHAR_C			get_property(			C_PCHAR_C name );
			C_PCHAR_C			get_menu_sub();
			C_PCHAR_C			get_file_filter();

			void					register_obj(	c_obj_ui* obj );
			void					unregister_obj(	c_obj_ui* obj );
	FINLINE	c_obj_ui*				get_obj_first()						{ return _objs.empty() ? nullptr : _objs.front(); }
	FINLINE	std::list<c_obj_ui*>*	get_objs_list()						{ return &_objs;	}
			void					delete_all_objs();
			void					before_exit_all_objs();

	FINLINE	UINT32				get_obj_count()			{	return _obj_count; }
	FINLINE	UINT32*				get_obj_count_pt()		{	return &_obj_count; }		//todo use for param but dangerous
	FINLINE	bool				is_obj_first()			{	return _obj_count == 1; }
	FINLINE	UINT32				get_obj_created_nb()	{	return _obj_created_nb; }
	FINLINE	UINT32*				get_obj_created_nb_pt()	{	return &_obj_created_nb; }
	virtual	bool				is_concrete() CONST = 0;
//			bool				is_abstract()			{	return !is_concrete(); }
//			void				dummy()					{}

	FINLINE	INT32				get_param_data_size()									{ return INT32(_param_data.size()); }
	FINLINE	c_param_data*		get_param_data(			INT32 index )					{ return _param_data[index]; }
			void				push_param_data(		c_param_def CONST * CONST st );
	
//			c_obj_ui*			find_obj_by_param(		c_param CONST * CONST param ) CONST;

			void				print_hierarchy(		bool b_detail=false, INT32 level=0 );
			void				check_hierarchy();
			void				build_derived_concrete(	std::list<c_factory_base*>& list_dc );

	static	void				print();
	static	void				test();

	static	void				print_hierarchy(		C_PCHAR_C name, bool b_detail=false, INT32 level=0 );
	static	void				check_hierarchy(		C_PCHAR_C name );
	static	void				check_all_obj();
	static	void				build_derived_concrete(	C_PCHAR_C name, std::list<c_factory_base*>& list_dc );
};


class c_factory_base_abstract : public	c_factory_base
{
private:
	virtual	c_obj_ui*	new_hidden() { return nullptr; }	//facto do something here
public:
	c_factory_base_abstract( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human, C_PCHAR_C ext, C_PCHAR_C props )
		: c_factory_base( cid, super_factory, human, ext, props )
	{
		++count_abstract;
	};
	virtual	~c_factory_base_abstract()	{ --count_abstract; }
	virtual	bool		is_concrete()	CONST { return false; }
	virtual void		delete_hidden( c_obj_ui* obj )	{}
};

template< class T>
class c_factory_abstract final : public	c_factory_base_abstract
{
public:
	c_factory_abstract<T>( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human=nullptr, C_PCHAR_C ext=nullptr, C_PCHAR_C props=nullptr )
		: c_factory_base_abstract( cid, super_factory, human, ext, props )
			{};
	virtual	~c_factory_abstract<T>() {};
};

class c_factory_base_concrete : public	c_factory_base
{
public:
	c_factory_base_concrete( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human, C_PCHAR_C ext, C_PCHAR_C props )
		: c_factory_base( cid, super_factory, human, ext, props )
	{
		++count_concrete;
	}
	virtual	~c_factory_base_concrete()	{ --count_concrete; }
	virtual	bool		is_concrete() CONST final override	{ return true; }
};

template< class T>
class c_factory final : public c_factory_base_concrete
{
private:
	virtual c_obj_ui*	new_hidden()
		{
		T*	obj = new T;
//		if( obj)
//			obj->factory_ = factory;
		return (c_obj_ui*)obj;
		}
protected:
public:
	c_factory<T>( C_PCHAR_C cid, c_factory_base* super_factory, C_PCHAR_C human=nullptr, C_PCHAR_C ext=nullptr, C_PCHAR_C props=nullptr )
		: c_factory_base_concrete( cid, super_factory, human, ext, props )
			{};
	virtual	~c_factory<T>() {};
	virtual void delete_hidden( c_obj_ui* obj )
	{
		((T*)obj)->~T();
	}
};

//	c_factory_abstract<class_name> class_name::the_factory()( (cid), &(SUPER::the_factory()), (human), nullptr, str_props )
//we should avoid macro but...
#define FACTORY_ABSTRACT_CREATE_MACRO( class_name, cid, human, str_props )	\
	c_factory_abstract< class_name >&	class_name::the_factory()	\
		{	\
		static	c_factory_abstract< class_name >	class_factory( cid, &(SUPER::the_factory()), human, nullptr, str_props );	\
		return class_factory;	\
		}
//	bool b_trick_##class_name = class_name::the_factory().is_obj_first();
//	was there to force creation of factory but nor need for abstract
#define FACTORY_ABSTRACT_CREATE_PROP_V1( class_name, cid, human, str_props )	FACTORY_ABSTRACT_CREATE_MACRO( class_name, (#cid), (#human), (#str_props) ) 	
#define FACTORY_ABSTRACT_CREATE_V1( class_name, cid, human )					FACTORY_ABSTRACT_CREATE_MACRO( class_name, (#cid), (#human), nullptr )

#define	FACTORY_ABSTRACT_DECLARE( class_name, super_name )	\
public:	\
	typedef class_name SELF;	\
	typedef super_name SUPER;	\
	friend	c_factory_abstract< class_name >;	\
	static	c_factory_abstract< class_name >&	the_factory();	\
public:	\
	class_name(c_factory_base* factory = nullptr);	\
	virtual ~class_name();	\
private:

//we should avoid macro but...
//c_factory<class_name> class_name::the_factory()( (#cid), &(SUPER::the_factory()), (#human), (#ext), (#str_props) )
#define FACTORY_CREATE_MACRO( class_name, cid, human, ext, str_props )	\
	c_factory<class_name>&	class_name::the_factory()	\
		{	\
		static	c_factory<class_name> class_factory( cid, &(SUPER::the_factory()), human, ext, str_props );	\
		return class_factory;	\
		}\
	bool b_trick_##class_name = class_name::the_factory().is_obj_first();


#define FACTORY_INSTANCE_PROP_V1( class_name, cid, human, ext, str_props )	FACTORY_CREATE_MACRO( class_name, (#cid), (#human), (#ext), (#str_props) ) 	
//			class_name::the_factory().dummy();
#define FACTORY_INSTANCE_V1( class_name, cid, human, ext )					FACTORY_CREATE_MACRO( class_name, (#cid), (#human), (#ext), nullptr )
//			class_name::the_factory().dummy();

//#define FACTORY_CREATE_PROP_V1( class_name, cid, human, ext, str_props )	FACTORY_CREATE_MACRO( class_name, (#cid), (#human), (#ext), (#str_props) )
//#define FACTORY_CREATE_V1( class_name, cid, human, ext )					FACTORY_CREATE_MACRO( class_name, (#cid), (#human), (#ext), nullptr )
#define FACTORY_CREATE_PROP_V1( class_name, cid, human, ext, str_props )	FACTORY_INSTANCE_PROP_V1( class_name, cid, human, ext, str_props )
#define FACTORY_CREATE_V1( class_name, cid, human, ext )					FACTORY_INSTANCE_V1( class_name, cid, human, ext )
	

#define	FACTORY_DECLARE( class_name, super_name )	\
public:	\
	typedef class_name SELF;	\
	typedef super_name SUPER;	\
	static	c_factory<class_name>&	the_factory(); \
	friend	c_factory<class_name>;	\
	static	FINLINE	SELF*	create_obj()							{	return (SELF*) the_factory().create_obj(); }		\
	static	FINLINE	SELF*	create_obj( c_obj_ui* father )			{ \
																	SELF* obj = create_obj(); \
																	obj->set_root(father); \
																	return obj;	\
																	}	\
			FINLINE void	delete_using_factory( c_obj_ui* obj )		{	the_factory().delete_hidden( obj ); }		\
	static	FINLINE	bool	is_instance( CONST c_obj_ui* CONST src )	{	return ((c_factory_base*)&(the_factory())) == src->get_factory();	} \
public:	\
	class_name(c_factory_base* factory = nullptr);	\
	virtual ~class_name();	\
private:


#define	EMPTY_DESTRUCTOR( class_name )				class_name::~class_name() {}
#define	CONSTRUCTOR_CREATE( class_name )			class_name::class_name(c_factory_base* factory) : SUPER( &the_factory() )
#define	CONSTRUCTOR_ABSTRACT_CREATE( class_name )	class_name::class_name(c_factory_base* factory) : SUPER( factory )

/*
template<	class T, typename id_type, typename creator >
//	,template<typename, class>	class FactoryErrorPolicy
class Factory
//	: public FactoryErrorPolicy<IdentifierType, AbstractProduct>
{
private:
	typedef map< id_type, creator> assoc_map;
	assoc_map pool_;
public:
	T* create_obj( CONST id_type& id)
		{
		typename AssocMap::const_iterator it = pool_.find(id);
		if( it != pool_.end() )
			{
			return (it->second)();
			}
//		return OnUnknownType(id);
		return nullptr;
		}
};(
*/

