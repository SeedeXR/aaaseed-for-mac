#ifdef AAA_FLEX_RIGID_HOST_H
#error "FLEX/RIGID_HOST_H included more than once."
#endif
#define AAA_FLEX_RIGID_HOST_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

class c_particle_host;
class c_flex_buffer_cuda_host;
struct NvFlexLibrary;
struct NvFlexSolver;

struct st_flex_rigid_point_desc
{
	UINT32					_index;
	FP32					_position[3]; //note : if _auto_build_locals is true, this will be eventually computed
};

struct st_flex_rigid_desc
{
	FP32						_position[3];
	FP32						_orientation[4];
	FP32						_stiffness;
	bool						_auto_build_locals;
	UINT32						_points_nb;
	st_flex_rigid_point_desc*	_pt_points;
	INT32						_tag;
};

struct st_size_tag
{
	FP32 _size[3];
	INT32   _tag;
	INT32   _first_idx;
	FP32 _dum1[3];
	FP32 _dum2[4];
	FP32 _dum3[4];
};

class c_rigid_host final : public c_obj
{
private:
	UINT32						_rigid_nb_max;
	UINT32						_rigid_indices_nb_max;
	UINT32						_rigid_nb_cur;
	UINT32						_rigid_indices_nb_cur;
	UINT32						_rigid_nb_needed;
	UINT32						_rigid_indices_nb_needed;

	//note: offset buffer is _rigid_nb_max + 1 (first element must be zero)
	c_flex_buffer_cuda_host*	_flex_offsets_buffer;
	c_flex_buffer_cuda_host*	_flex_indices_buffer;
	c_flex_buffer_cuda_host*	_flex_rest_position_buffer;
	
	//note: tried on some grid/sphere shapes, seems it was not so needed, and passing null is fine
	//c_flex_buffer_cuda_host*	_flex_rest_normal_buffer;

	c_flex_buffer_cuda_host*	_flex_stiffness_buffer;
	c_flex_buffer_cuda_host*	_flex_translation_buffer;
	c_flex_buffer_cuda_host*	_flex_rotation_buffer;
	
	//add an arbitrary tag per rigid
	st_size_tag*				_size_tags;

public:
	c_rigid_host();
	~c_rigid_host();

	void build_local_rest_positions( c_particle_host * particle_host);

	void reset();
	void release();
	void set_size( NvFlexLibrary * library, UINT32 CONST rigid_nb_max, UINT32 CONST rigid_indices_nb_max );
	void append( st_flex_rigid_desc CONST * rigid_desc );

	bool validate ( c_particle_host * particle_host );

	void send_to_solver( NvFlexSolver * solver );
	
	FINLINE UINT32 get_rigid_nb()					CONST	{	return _rigid_nb_cur;						}
	FINLINE UINT32 get_nb_needed()					CONST	{	return _rigid_nb_needed;					}
	FINLINE UINT32 get_nb_max()						CONST	{	return _rigid_nb_max;						}
	FINLINE UINT32 is_overflow()					CONST	{	return _rigid_nb_needed >= _rigid_nb_max;	}

	FINLINE UINT32 get_indices_nb_needed()			CONST	{	return _rigid_indices_nb_needed;							}
	FINLINE UINT32 get_indices_nb_max()				CONST	{	return _rigid_indices_nb_max;								}
	FINLINE UINT32 is_indices_overflow()			CONST	{	return _rigid_indices_nb_needed >= _rigid_indices_nb_max;	}

	FINLINE void* get_size_tag_data()				CONST	{	return _size_tags;										}

};
#endif//#if AAA_USE_FLEX()
