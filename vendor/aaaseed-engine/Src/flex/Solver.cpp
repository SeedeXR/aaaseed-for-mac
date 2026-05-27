#include "flex_sdk.h"


#if AAA_USE_FLEX()

#include "Solver.h"
#include "aaa_mem.h"
#include "gol/gol_shader.h"
#include "aaa_util.h"
#include "Buffer_Gl.h"
#include "gl/ubo.h"
#include "Collision_Shapes.h"
#include "particle_host.h"
#include "triangle_host.h"
#include "spring_host.h"
#include "rigid_host.h"
#include "Buffer_Cuda_Host.h"
#include "err.h"
#include <string>

namespace {
	INT32 error_count = 0;
}

void reportdbg( NvFlexErrorSeverity type, const char* msg, const char* file, INT32 line )
{
	std::string header;
	switch( type )
	{
	case NvFlexErrorSeverity::eNvFlexLogError:		header = "Error";		break;
	case NvFlexErrorSeverity::eNvFlexLogInfo:		header = "Information";	break;
	case NvFlexErrorSeverity::eNvFlexLogWarning:	header = "Warning";		break;
	case NvFlexErrorSeverity::eNvFlexLogDebug:		header = "Debug";		break;
	}

	//todo done quickly at minima
	ERR_PRINT_STRING( "FLEX %s(%d): %s", header.c_str(), error_count, msg );
	ERR_PRINT_STRING( "\tin %s at line %d", file, line );
	
	++error_count;
}

c_flex_solver::c_flex_solver( NvFlexLibrary* library )
	:_library(library)
	,_creation_info{}
	,_solver(nullptr)
	,_solver_parameters{}
	,_position_mass(nullptr)
	,_position_mass_init(nullptr)
	,_position_mass_cpu(nullptr)
	,_rigid_position(nullptr)
	,_rigid_rotation(nullptr)
	,_rigid_position_cpu(nullptr)
	,_rigid_rotation_cpu(nullptr)
	,_velocity(nullptr)
	,_phase(nullptr)
	,_active(nullptr)
	,_position_smoothed(nullptr)
	,_density(nullptr)
	,_aniso_q1(nullptr)
	,_aniso_q2(nullptr)
	,_aniso_q3(nullptr)
	,_custom_buffer_gl(nullptr)
	,_particle_rigid_index(nullptr)
	,_ubo_gl(nullptr)
	,_ubo_custom_draw_gl(nullptr)
	,_indices_free_buffer_gl(nullptr)
	,_free_indices_buffer_accum_gl(nullptr)
	,_collision_shapes(nullptr)
	,_timers{}
	,_solver_data{}
	,_b_data_valid(false)
	,_rigid_nb(0)
	,_spring_indices(nullptr)
	,_triangle_indices(nullptr)
	,_spring_nb(0)
	,_triangle_nb(0)
	,_normals(0)
	,_acc(0)
	,_rigid_size_tag(0)
	,_texcoords(nullptr)
{
}

c_flex_solver* c_flex_solver::create()
{
	NvFlexInitDesc init_desc;

	//dx related, set to defaults
	init_desc.computeContext = nullptr; 
	init_desc.deviceIndex = 0; 
	init_desc.enableExtensions = false;
	init_desc.renderContext = nullptr;
	init_desc.renderDevice = nullptr;

	init_desc.computeType = NvFlexComputeType::eNvFlexCUDA;
	auto library = NvFlexInit( NV_FLEX_VERSION, &reportdbg, &init_desc );
	if( library )
		return new c_flex_solver( library );
	return nullptr;
}

void c_flex_solver::init_solver( st_flex_solver_creation_info creation_info )
{
	release_solver();

	_creation_info = creation_info;

	UINT32 nb_max = creation_info._particles_nb_max;

	_solver_data._particles_nb_max = nb_max;
	_solver_data._frame_index = 0;

// this are all the init params of the solver
	NvFlexSolverDesc solver_desc;
	solver_desc.featureMode = NvFlexFeatureMode::eNvFlexFeatureModeDefault; //we want fluids only in our case, todo: allow both options
	solver_desc.maxContactsPerParticle = creation_info._particle_contact_nb_max;
	solver_desc.maxDiffuseParticles = 0; //no diffuse particles
	solver_desc.maxParticles = nb_max;
	solver_desc.maxNeighborsPerParticle = creation_info._particle_neighbour_nb_max;

	_solver = NvFlexCreateSolver( _library, &solver_desc );

// POSITION AND VELOCITY
	_position_mass = new c_flex_buffer_gl();
	_position_mass->reset(		_library, nb_max, 4 * sizeof(FP32), nullptr );	//stride : 16 on position

	_position_mass_init = new c_flex_buffer_gl();
	_position_mass_init->reset(	_library, nb_max, 4 * sizeof(FP32), nullptr );	//stride : 16 on position

	_position_mass_cpu = new c_flex_buffer_cuda_host();
	_position_mass_cpu->reset(	_library, nb_max, 4 * sizeof(FP32), nullptr );	//stride : 16 on position

	_velocity = new c_flex_buffer_gl();
	_velocity->reset(			_library, nb_max, 3 * sizeof(FP32), nullptr );	//stride : 12 on velocity



// COLLISION SHAPES
	_collision_shapes = new c_flex_collision_shapes();
	_collision_shapes->reset(	_library, creation_info._collision_shapes_nb_max	);

// ACCELERATION
	//4 float size (normally we only have 2 float in case of df field, but we could have vec3 and keep one extra for extra data slot
	_acc							= gl::ssbo::make( nb_max * 4 * sizeof(FP32), false, nullptr );
	

// CUSTOM
	//4 floats size for custom gl buffer
	_custom_buffer_gl				= gl::ssbo::make( nb_max * 4 * sizeof(FP32), false, nullptr );
	_ubo_gl							= gl::ubo::make( sizeof(GOL::st_flex_solver), true, nullptr, "flex ubo" );
	//256 bytes for a custom draw ubo
	_ubo_custom_draw_gl				= gl::ubo::make( 256, true, nullptr, "flex ubo" );
// INDICES
	_indices_free_buffer_gl			= gl::ssbo::make( nb_max * sizeof(UINT32), false, nullptr );
	_free_indices_buffer_accum_gl	= gl::ssbo::make( sizeof(UINT32), true, nullptr );

//PHASE
	{
		_phase = new c_flex_buffer_gl();
		_phase->reset( _library, nb_max, sizeof(UINT32) ); //phase : 4 bytes
	}

//ACTIVE
	{
		_active = new c_flex_buffer_cuda_host();
		_active->reset( _library, nb_max, sizeof(UINT32) ); //active indices : 4 bytes
	}

// SMOOTH POSITION use for rendering
	if( creation_info._b_smooth_position_use )
	{
		_position_smoothed = new c_flex_buffer_gl();
		_position_smoothed->reset(	_library, nb_max, 4*sizeof(FP32) ); //stride : 16 on position
	}

// DENSITY
	if( creation_info._b_density_use )
	{
		_density = new c_flex_buffer_gl();
		_density->reset( _library, nb_max, sizeof(FP32) ); 
	}

// ANISOTROPY
	if( creation_info._b_anisotropy_use )
	{
		//3 gl buffers for anisotropy (4x float each)
		_aniso_q1 = new c_flex_buffer_gl();
		_aniso_q1->reset( _library, nb_max, 4*sizeof(FP32) ); 

		_aniso_q2 = new c_flex_buffer_gl();
		_aniso_q2->reset( _library, nb_max, 4*sizeof(FP32) ); 

		_aniso_q3 = new c_flex_buffer_gl();
		_aniso_q3->reset( _library, nb_max, 4*sizeof(FP32) ); 
	}

	_b_data_valid = true;
	_rigid_nb = 0;
	_spring_nb = 0;
	_triangle_nb = 0;
}

bool c_flex_solver::validate_hosts(
		c_particle_host * particle_host , 
		c_spring_host * spring_host , 
		c_triangle_host * triangle_host ,
		c_rigid_host * rigid_host )
{
	bool b_valid = true;
	if( particle_host && particle_host->is_overflow() )
	{
		ERR_PRINT_STRING("particle overflow (needed)   :    %i.", particle_host->get_nb_needed());
		ERR_PRINT_STRING("particle overflow (capacity) :    %i.", particle_host->get_nb_max());
		b_valid = false;
	}
	if( spring_host && spring_host->is_overflow() )
	{
		ERR_PRINT_STRING("spring overflow   (needed)   :    %i.", spring_host->get_nb_needed());
		ERR_PRINT_STRING("spring overflow   (capacity) :    %i.", spring_host->get_nb_max());
		b_valid = false;
	}
	if( triangle_host && triangle_host->is_overflow() )
	{
		ERR_PRINT_STRING("triangle overflow (needed)   :    %i.", triangle_host->get_nb_needed());
		ERR_PRINT_STRING("triangle overflow (capacity) :    %i.", triangle_host->get_nb_max());
		b_valid = false;
	}
	if( rigid_host && rigid_host->is_overflow() )
	{
		ERR_PRINT_STRING("rigid body overflow (needed):     %i.", rigid_host->get_nb_needed());
		ERR_PRINT_STRING("rigid body overflow (capacity):   %i.", rigid_host->get_nb_max());
		b_valid = false;
	}
	if( rigid_host && rigid_host->is_indices_overflow() )
	{
		ERR_PRINT_STRING("rigid body indices overflow (needed):     %i.", rigid_host->get_indices_nb_needed());
		ERR_PRINT_STRING("rigid body indices overflow (capacity):   %i.", rigid_host->get_indices_nb_max());
		b_valid = false;
	}

	//in case of overflow, just return, since most other bounds check should be solved by updating capacity
	if( !b_valid )
		return false;

	//this validates bounds check on springs
	if( spring_host && !spring_host->validate( particle_host ) )
		b_valid = false;

	//triangles
	if( triangle_host && !triangle_host->validate( particle_host ) )
		b_valid = false;

	//rigid
	if( rigid_host && !rigid_host->validate( particle_host ) )
		b_valid = false;

	return b_valid;
}

void c_flex_solver::reset_from_host( 
	st_flex_solver_creation_info creation_info, 
	c_particle_host * particle_host , 
	c_spring_host * spring_host , 
	c_triangle_host * triangle_host , 
	c_rigid_host * rigid_host , 
	bool b_build_rest_length ,
	bool b_build_rest_positions )
{
	if( !validate_hosts( particle_host, spring_host, triangle_host, rigid_host ))
	{
		_b_data_valid = false;
		return;
	}

	_b_data_valid = true;

	if( spring_host && b_build_rest_length )
	{
		spring_host->build_rest_length( particle_host );
	}

	if( rigid_host && b_build_rest_positions )
	{
		rigid_host->build_local_rest_positions( particle_host );
	}

	init_solver( creation_info );

	UINT32 nb_max = creation_info._particles_nb_max;

	//set default data from host
	_solver_data._active_nb = creation_info._init_nb;
	if( creation_info._init_nb > 0 )
	{
		particle_host->send_to_solver( _solver );

		//copy particles in gl buffers too
		NvFlexGetParticles(		_solver, _position_mass->get_flex_buffer()			,nullptr );
		NvFlexGetParticles(		_solver, _position_mass_init->get_flex_buffer_gl()	,nullptr );

		NvFlexGetVelocities(	_solver, _velocity->get_flex_buffer()				,nullptr );
		NvFlexGetPhases(		_solver, _phase->get_flex_buffer()					,nullptr );

		//also get them in gl buffer, if we want to integrate
		NvFlexGetParticles(		_solver, _position_mass->get_flex_buffer_gl()		,nullptr );
		NvFlexGetVelocities(	_solver, _velocity->get_flex_buffer_gl()			,nullptr );
		NvFlexGetPhases(		_solver, _phase->get_flex_buffer_gl()				,nullptr );

		_texcoords = gl::ssbo::make( nb_max * 2 * sizeof(FP32), false, (const char*)particle_host->get_texcoord_buffer()->get_data() );

	}

	if( spring_host )
	{
		spring_host->send_to_solver( _solver );
		_spring_nb = spring_host->get_nb();
		if( _spring_nb > 0 )
		{
			_spring_indices	= gl::ssbo::make( _spring_nb * 2 * sizeof(UINT32), false, (const char*)spring_host->get_indices_data() );
		}
	}
		
	if( triangle_host )
	{
		triangle_host->send_to_solver( _solver );
		_triangle_nb = triangle_host->get_nb();

		if( _triangle_nb > 0 )
		{
			_triangle_indices	= gl::ssbo::make( _triangle_nb * 3 * sizeof(UINT32), false, (const char*)triangle_host->get_indices_data() );
			_normals			= new c_flex_buffer_gl();
			_normals->reset( _library, _creation_info._particles_nb_max, 4*sizeof(FP32) );
		}
	}
		
	if( rigid_host )
	{
		rigid_host->send_to_solver( _solver );

		_rigid_nb = rigid_host->get_rigid_nb();
		if( _rigid_nb > 0 )
		{
			//buffers for draw
			_rigid_position = new c_flex_buffer_gl();
			_rigid_position->reset(			_library, _rigid_nb, 3 * sizeof(FP32), nullptr );	//stride : 12 on rigid position
	
			_rigid_rotation = new c_flex_buffer_gl();
			_rigid_rotation->reset(			_library, _rigid_nb, 4 * sizeof(FP32), nullptr );	//stride : 16 on rigid rotation

			//buffers for readback
			_rigid_position_cpu = new c_flex_buffer_cuda_host();
			_rigid_position_cpu->reset(		_library, _rigid_nb, 3 * sizeof(FP32), nullptr );	//stride : 12 on rigid position
	
			_rigid_rotation_cpu = new c_flex_buffer_cuda_host();
			_rigid_rotation_cpu->reset(		_library, _rigid_nb, 4 * sizeof(FP32), nullptr );	//stride : 16 on rigid rotation

			_rigid_size_tag	= gl::ssbo::make( _rigid_nb * sizeof(UINT32) * 16, false, (const char*)rigid_host->get_size_tag_data() );

			_particle_rigid_index = gl::ssbo::make( _creation_info._particles_nb_max * sizeof(INT32), false, (const char*)particle_host->get_particle_to_rigid_data() );
		}
	}
}

void c_flex_solver::reset( st_flex_solver_creation_info creation_info )
{
	init_solver( creation_info );

	UINT32 nb_max = creation_info._particles_nb_max;

//PHASE
	if( creation_info._b_phase_use )
	{
		UINT32* p_phases = (UINT32*)MALLOC( nb_max * sizeof(UINT32) );

		//todo deal with
		UINT32 collide_flags = 0;
		switch( creation_info._particles_interaction_mode )
		{
		case e_flex_particle_phase_type::SOLID:
			collide_flags = eNvFlexPhaseSelfCollide;
			break;
		case e_flex_particle_phase_type::FLUID:
			collide_flags = eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid;
			break;
		}

		UINT32 phase = NvFlexMakePhaseWithChannels( 0, collide_flags, eNvFlexPhaseShapeChannelMask );

		for( UINT32 i = 0; i < nb_max; ++i )
			p_phases[i] = phase;

		_phase->get_ssbo()->write( p_phases, sizeof(UINT32) * nb_max, 0, true );
		_phase->copy_gl_to_flex();
		NvFlexSetPhases( _solver, _phase->get_flex_buffer(), nullptr );

		IF_FREE_AND_NULL( p_phases );  
	}

//ACTIVE
	{
		//if we use default active indices, just set index counter directly
		if( creation_info._b_indices_use )
		{
			UINT32* p_indices = (UINT32*)_active->get_data();
			for( UINT32 i = 0; i < nb_max; ++i )
				p_indices[i] = i;

			_active->copy( _active->get_size() );
		}

		NvFlexSetActive( _solver, _active->get_flex_buffer(), nullptr );
	}

		//set default active and position if relevant
	_solver_data._active_nb = creation_info._init_nb;

	if( creation_info._init_nb > 0 )
	{
		if( creation_info._init_position_mass )
		{
			_position_mass->get_ssbo()->write( creation_info._init_position_mass, 4 * sizeof(FP32) * creation_info._init_nb, 0, true );
			_position_mass->copy_gl_to_flex();

			//copy initial position, no need to send to flex
			_position_mass_init->get_ssbo()->write( creation_info._init_position_mass, 4 * sizeof(FP32) * creation_info._init_nb, 0, true );
		}

		if( creation_info._init_velocity )
		{
			_velocity->get_ssbo()->write( creation_info._init_velocity, 3 * sizeof(FP32) * creation_info._init_nb, 0, true );
			_velocity->copy_gl_to_flex();
		}
	}
}

void c_flex_solver::set_simulation_parameters( NvFlexParams CONST * params )
{
	_solver_parameters = *params;
}

void c_flex_solver::set_active_nb( UINT32 active_nb )
{
	if( _solver )
	{
		//clamp
		_solver_data._active_nb =  MIN( active_nb, _solver_data._particles_nb_max );
	}
}

void c_flex_solver::apply_emit_offset( UINT32 emit_nb )
{	
	if( _solver )
	{
		set_active_nb( _solver_data._active_nb + emit_nb );
		//wrap
		_solver_data._emit_offset = (_solver_data._emit_offset + emit_nb) % _solver_data._particles_nb_max;
	}
}

void c_flex_solver::copy_gl_buffers_to_flex( bool b_phase )
{
	if( _solver )
	{
		_position_mass->copy_gl_to_flex();
		_velocity->copy_gl_to_flex();

		if( b_phase )
		{
			_phase->copy_gl_to_flex();
			NvFlexSetPhases( _solver, _phase->get_flex_buffer(),	nullptr );
		}
	}
}

void c_flex_solver::update( st_flex_solver_options CONST * options )
{
	if( _solver )
	{
		NvFlexCopyDesc copy_desc ={};
		copy_desc.elementCount = get_active_nb();

		NvFlexSetParticles(  _solver, _position_mass->get_flex_buffer() , &copy_desc );
		NvFlexSetVelocities( _solver, _velocity->get_flex_buffer()		, &copy_desc );
		
	//assign collision parameters
		NvFlexSetParams(		_solver, &_solver_parameters);
		NvFlexSetActiveCount(	_solver, (INT32)_solver_data._active_nb );

	//colliders
		//todo do we need to do it every frame ?
		_collision_shapes->attach(_solver);

	//update
		NvFlexUpdateSolver( _solver, _solver_data._dt , options->_substep_nb, options->_b_timers );
		if( options->_b_timers )
			NvFlexGetTimers( _solver, &_timers );


	//get back position/velocity, active and phase is not needed as it is not modified
		if( options->_b_particle_cuda_read )
			NvFlexGetParticles(  _solver, _position_mass->get_flex_buffer() , &copy_desc );

		if( options->_b_velocity_cuda_read )
			NvFlexGetVelocities( _solver, _velocity->get_flex_buffer()		, &copy_desc );

		//get particles into our gl buffer as well (we need velocity as we will integrate the force field into it)
		NvFlexGetParticles(  _solver, _position_mass->get_flex_buffer_gl()	, &copy_desc );
		NvFlexGetVelocities( _solver, _velocity->get_flex_buffer_gl()		, &copy_desc );

		if( _position_smoothed && options->_b_smoothed_position_compute )
			NvFlexGetSmoothParticles( _solver, _position_smoothed->get_flex_buffer_gl(), &copy_desc );

		if( _density && options->_b_density_read )
			NvFlexGetDensities( _solver, _density->get_flex_buffer_gl(), &copy_desc );

		if( _aniso_q1 && options->_compute_anisotropy )
			NvFlexGetAnisotropy( 
				_solver, 
				_aniso_q1->get_flex_buffer(), 
				_aniso_q2->get_flex_buffer(),
				_aniso_q3->get_flex_buffer(), 
				nullptr );

		if( _normals )
			NvFlexGetNormals(	_solver, _normals->get_flex_buffer_gl()	, &copy_desc );

		if( _rigid_position )
			NvFlexGetRigids( _solver, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, _rigid_rotation->get_flex_buffer_gl(), _rigid_position->get_flex_buffer_gl() );

		_solver_data._frame_index++;
	}
}

void c_flex_solver::set_active_collision_shapes( st_flex_collision_shape* const shapes, UINT32 CONST nb )
{
	_collision_shapes->set_active_collision_shapes( shapes, nb );
}

void c_flex_solver::bind_buffers_gl_draw( bool b_prefer_smoothed )
{
	if( b_prefer_smoothed && _position_smoothed )
		_position_smoothed->bind_gl(0);
	else
		_position_mass->bind_gl(0);
	
	_custom_buffer_gl->bind(1);

	if( _density )
		_density->bind_gl(2);

	_velocity->bind_gl(3);

	if( _spring_indices )
		_spring_indices->bind(4);
	if( _triangle_indices )
		_triangle_indices->bind(5);
	if( _normals )
		_normals->bind_gl(6);

	_acc->bind(7);

	//normally they always created in pair
	if( _rigid_position )
		_rigid_position->bind_gl(8);
	if( _rigid_rotation )
		_rigid_rotation->bind_gl(9);	
	if( _rigid_size_tag )
		_rigid_size_tag->bind(10);
	if( _texcoords )
		_texcoords->bind(11);
}

void c_flex_solver::unbind_buffers_gl_draw( bool b_prefer_smoothed )
{
	if( _texcoords )
		_texcoords->unbind(11);
	if( _rigid_size_tag )
		_rigid_size_tag->unbind(10);
	if( _rigid_rotation )
		_rigid_rotation->unbind_gl(9);
	if( _rigid_position )
		_rigid_position->unbind_gl(8);

	_acc->unbind(7);

	if( _normals )
		_normals->unbind_gl(6);
	if( _triangle_indices )
		_triangle_indices->unbind(5);
	if( _spring_indices )
		_spring_indices->unbind(4);

	_velocity->unbind_gl(3);

	if( _density )
		_density->unbind_gl(2);

	_custom_buffer_gl->unbind(1);

	if( b_prefer_smoothed && _position_smoothed )
		_position_smoothed->unbind_gl(0);
	else
		_position_mass->unbind_gl(0);
}

void c_flex_solver::bind_ubo_gl_compute()
{
	_ubo_gl->bind_and_write( 0, &_solver_data );
}

void c_flex_solver::bind_buffers_gl_compute( bool b_density )
{
	_position_mass->bind_gl(0);
	_velocity->bind_gl(1);
	_phase->bind_gl(2);
	_custom_buffer_gl->bind(3);
	_indices_free_buffer_gl->bind(4);
	_free_indices_buffer_accum_gl->bind(5);

	if( b_density && _density )
		_density->bind_gl(6);

	_position_mass_init->bind_gl(7);
	_acc->bind(8);

	if( _rigid_position )
		_rigid_position->bind_gl(9);
	if( _rigid_rotation )
		_rigid_rotation->bind_gl(10);
	if( _rigid_size_tag )
		_rigid_size_tag->bind(11);
	if( _particle_rigid_index )
		_particle_rigid_index->bind(12);

	bind_ubo_gl_compute();
}

void c_flex_solver::unbind_buffers_gl_compute( bool b_density )
{
	_ubo_gl->unbind(0);

	if( _particle_rigid_index )
		_particle_rigid_index->unbind(12);
	if( _rigid_size_tag )
		_rigid_size_tag->unbind(11);
	if( _rigid_rotation )
		_rigid_rotation->unbind_gl(10);
	if( _rigid_position )
		_rigid_position->unbind_gl(9);

	_acc->unbind(8);
	_position_mass_init->unbind_gl(7);

	if( b_density && _density )
		_density->unbind_gl(6);

	_free_indices_buffer_accum_gl->unbind(5);
	_indices_free_buffer_gl->unbind(4);
	_custom_buffer_gl->unbind(3);
	_phase->unbind_gl(2);
	_velocity->unbind_gl(1);
	_position_mass->unbind_gl(0);
}

void c_flex_solver::clear_indices_accum()
{
	if( _free_indices_buffer_accum_gl )
	{ 
		UINT32 accum_reset = 0;
		_free_indices_buffer_accum_gl->write( &accum_reset, sizeof(UINT32), 0, true );
	}
}

UINT32 c_flex_solver::read_indices_counter()
{
	if( _free_indices_buffer_accum_gl )
	{ 
		UINT32 nb;
		_free_indices_buffer_accum_gl->read( &nb, 4, 0 );
		return nb;
	}
	return 0;
}

void c_flex_solver::dispatch_gl_compute()
{
	GOL::dispatch_compute_1d( _solver_data._active_nb, 512 );
	GOL::barrier_memory( GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );
}

void c_flex_solver::read_position( NvFlexBuffer * buffer )
{
	if( _solver )
		NvFlexGetParticles( _solver, buffer, nullptr );
}

void c_flex_solver::read_velocity( NvFlexBuffer * buffer )
{
	if( _solver )
		NvFlexGetVelocities( _solver, buffer, nullptr );
}

void c_flex_solver::write_position( NvFlexBuffer * buffer )
{
	if( _solver )
		NvFlexSetParticles( _solver, buffer, nullptr );
}

void c_flex_solver::readback_positions()
{
	if( _solver )
	{ 
		NvFlexCopyDesc copy_desc ={};
		copy_desc.elementCount = get_active_nb();

		NvFlexGetParticles( _solver, _position_mass_cpu->get_flex_buffer(), &copy_desc );
		UINT32 size = get_active_nb() * 4 * sizeof(FP32);
		_position_mass_cpu->readback_data( size );
	}
}

void c_flex_solver::readback_rigids()
{
	if( _solver && _rigid_position_cpu )
	{ 

		NvFlexGetRigids( _solver, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, _rigid_rotation_cpu->get_flex_buffer(), _rigid_position_cpu->get_flex_buffer() );

		_rigid_position_cpu->readback_data();
		_rigid_rotation_cpu->readback_data();
	}
}

void* c_flex_solver::get_position_data()
{
	if( _solver )
		return _position_mass_cpu->get_data();
	
	return nullptr;
	
}

void* c_flex_solver::get_rigid_position_data()
{
	if( _solver && _rigid_position_cpu)
		return _rigid_position_cpu->get_data();
	
	return nullptr;
}

void* c_flex_solver::get_rigid_rotation_data()
{
	if( _solver && _rigid_rotation_cpu)
		return _rigid_position_cpu->get_data();
	
	return nullptr;
}

void c_flex_solver::release_solver()
{
	SAFE_DELETE( _position_mass			);
	SAFE_DELETE( _position_mass_init	);
	SAFE_DELETE( _position_mass_cpu		);
	SAFE_DELETE( _rigid_position		);
	SAFE_DELETE( _rigid_rotation		);
	SAFE_DELETE( _rigid_position_cpu	);
	SAFE_DELETE( _rigid_rotation_cpu	);
	SAFE_DELETE( _velocity				);
	SAFE_DELETE( _phase					);
	SAFE_DELETE( _active				);

	SAFE_DELETE( _collision_shapes		);

	SAFE_DELETE( _position_smoothed		);
	SAFE_DELETE( _density				);
	SAFE_DELETE( _aniso_q1				);
	SAFE_DELETE( _aniso_q2				);
	SAFE_DELETE( _aniso_q3				);

	SAFE_DELETE( _spring_indices		);
	SAFE_DELETE( _triangle_indices		);
	SAFE_DELETE( _normals				);

	if( _solver )
	{
		NvFlexDestroySolver(_solver);
		_solver = nullptr;
	}

	gl::ssbo::release_and_null( _texcoords );
	gl::ssbo::release_and_null(	_acc );
	gl::ssbo::release_and_null(	_custom_buffer_gl );
	gl::ssbo::release_and_null(	_indices_free_buffer_gl );
	gl::ssbo::release_and_null(	_free_indices_buffer_accum_gl );
	gl::ubo::release_and_null(	_ubo_gl );
	gl::ubo::release_and_null(	_ubo_custom_draw_gl );
}

c_flex_solver::~c_flex_solver()
{
	release_solver();

	if( _library )
	{
		NvFlexShutdown(_library);
		_library = nullptr;
	}
}

#endif //#if AAA_USE_FLEX()