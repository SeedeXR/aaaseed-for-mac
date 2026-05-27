
#include "bdd_bullet.h"
#ifndef AAA_BULLET_OBJ_H
#	include "bullet_obj.h"
#endif

#include "infrastructure/layer/layers.h"
#include "draw/seedcam.h"
#include "ui/seed_ui.h"
#include "draw/axe.h"
#include "draw/box.h"
#include "draw/geo/sphere.h"
#include "gol/gol_matrix.h" 

#include "math/rand.h"

static	c_rand_lin	rnd;

//todo solve if no bid is 0 or -1

#if AAA_LIB_USE_BULLET()
#	include "Bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#	include "Bullet/BulletSoftBody/btSoftMultiBodyDynamicsWorld.h"
#	include "Bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#	include "Bullet/BulletSoftBody/btSoftBodyHelpers.h"
//#	include "Bullet/LinearMath/btConvexHull.h"
//#	include "Bullet/BulletSoftBody/btSoftBody.h"
//#	include "Bullet/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.h"
//#	include "Bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
//#	include "Bullet/btBulletCollisionCommon.h"

#	include <lib_use.h>
#	if AAA_WIN64()
#		if AAA_DEBUG()
			AAA_LIB_USE64("Bullet/BulletSoftBody_Debug")
			AAA_LIB_USE64("Bullet/BulletCollision_Debug")
			AAA_LIB_USE64("Bullet/BulletDynamics_Debug")
			AAA_LIB_USE64("Bullet/LinearMath_Debug")
			//AAA_LIB_USE64("Bullet/Bullet3Common")
#		else
			AAA_LIB_USE64("Bullet/BulletSoftBody")
			AAA_LIB_USE64("Bullet/BulletDynamics")
			AAA_LIB_USE64("Bullet/BulletCollision")
			AAA_LIB_USE64("Bullet/LinearMath")
#		endif
#	endif
#endif	//#if AAA_LIB_USE_BULLET()

FACTORY_CREATE_PROP_V1( c_bdd_bullet, bdd_bullet, Bullet, bdd_bullet, sub_menu="Experimental"; );

namespace {
	C_PCHAR_C dispatcher_str[2] =
	{
		"DISPATCHER",
		"DISPATCHERMT"
	};
	C_PCHAR_C broadphase_str[3] =
	{
		"DBVT",
		"32BITAXISSWEEP3",
		"AXISSWEEP3"
	};
	C_PCHAR_C solver_str[2] =
	{
		"SEQUENTIAL",
		"MULTIBODY"
	};
	C_PCHAR_C collision_str[2] =
	{
		"DEFAULT",
		"SOFT"
	};
	C_PCHAR_C dynamics_world_str[4] =
	{
		"RIGID",
		"SOFT",
		"MULTIBODY",
		"MULTIBODY-SOFT"
	};
}

namespace	n_bdd_bullet
{
	CONSTEXPR INT32 BASE_PARAM_NB = 12 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 WORLD_PARAM_NB = 5;
	CONSTEXPR INT32 GROUP_PARAM_NB = 1;
	CONSTEXPR INT32 PARAM_NB_MAX = BASE_PARAM_NB + WORLD_PARAM_NB + GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		restart_trig		)
		PARAM_DEF_GROUP_CLOSED( World objects, WORLD_PARAM_NB		)
			PARAM_DEF_SYMBO_PSTR(	dispatcher,			1, 2,	dispatcher_str )
			PARAM_DEF_SYMBO_PSTR(	broadphase,			1, 2,	broadphase_str)
			PARAM_DEF_SYMBO_PSTR(	solver,				1, 1,	solver_str)
			PARAM_DEF_SYMBO_PSTR(	collision,			1, 2,	collision_str)
			PARAM_DEF_SYMBO_PSTR(	dynamics_world,		1, 2,	dynamics_world_str)

		PARAM_DEF_SCALE_XYZ(		gravity				)
		PARAM_DEF_SCALE_XYZ(		wind				)
		PARAM_DEF_REAL_POS_ONE(		scale				)

		PARAM_DEF_INT32_POS_ONE(	objects_nb_alloc	)
		PARAM_DEF_INT32_LOCKED(		objects_nb			)
		PARAM_DEF_BOOL_ON(			draw_sphere			)
		PARAM_DEF_BOOL_ON(			draw_box			)
	};
}


void	c_bdd_bullet::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(		h, _b_restart_trig_ui		);
	++h;
		param_set_pt(		h, _s_dispatcher		);
		param_set_pt(		h, _s_broadphase		);
		param_set_pt(		h, _s_solver			);
		param_set_pt(		h, _s_collision			);
		param_set_pt(		h, _s_dynamics_world	);

	param_set_pt_3(	h, _gravity	);
	param_set_pt_3(	h, _wind	);
	param_set_pt(	h, _scale	);

	param_set_pt(		h, _objects_nb_allocated	);
	param_set_pt(		h, _objects_nb				);

	param_set_pt(		h, _b_draw_by_type[c_bullet_obj::SHAPE_SPHERE]	);
	param_set_pt(		h, _b_draw_by_type[c_bullet_obj::SHAPE_BOX]		);

	err_param_init_pt(h);
}

#if !AAA_LIB_USE_BULLET()
void	c_bdd_bullet::err_print_lib_unused( C_PCHAR_C fn_name ) CONST
{
	c_obj_ui::err_print_unused( "Bullet Physics Lib", fn_name );
}
#endif	//#if !AAA_LIB_USE_BULLET()

CONSTRUCTOR_CREATE(c_bdd_bullet)
#if AAA_LIB_USE_BULLET()
	,_collisionConfiguration	(nullptr)
	,_dispatcher				(nullptr)
	,_broadphase				(nullptr)
	,_solver					(nullptr)
	,_dynamicsWorld				(nullptr)
#endif	//#if AAA_LIB_USE_BULLET()
	,_objects_nb				(0)
{
	param_init_with( n_bdd_bullet::param, n_bdd_bullet::PARAM_NB_MAX );
	//open_world();
}
EMPTY_DESTRUCTOR(c_bdd_bullet)

#if AAA_LIB_USE_BULLET()
void	c_bdd_bullet::open_world()
{
	//HEAP_IS_CORRUPT();


	switch( _s_collision )
	{
	default: debug_break( "Unknown _s_collision %d", _s_collision );
	case 0:	_collisionConfiguration = new btDefaultCollisionConfiguration();			break;
	case 1:	_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();	break;
	}
	if( !_collisionConfiguration )
	{
		debug_break( "Failed to init btDefaultCollisionConfiguration." );
		return;
	}

	switch( _s_dispatcher )
	{
	default: debug_break( "Unknown _s_dispatcher %d", _s_dispatcher );
	case 0:	_dispatcher = new btCollisionDispatcher(_collisionConfiguration);	break;
	case 1:	_dispatcher = new btCollisionDispatcherMt(_collisionConfiguration);	break;
	}
	if( !_dispatcher )
	{
		debug_break( "Failed to init btCollisionDispatcher." );
		return;
	}

	btVector3 CONST minAABB = btVector3(-1000, -1000, -1000);
	btVector3 CONST maxAABB = btVector3(1000, 1000, 1000);
	switch( _s_broadphase )
	{
	default: debug_break( "Unknown _s_broadphase %d", _s_broadphase );
	case 0:	_broadphase = new btDbvtBroadphase();					break;
	case 1:	_broadphase = new bt32BitAxisSweep3( minAABB, maxAABB);	break;
	case 2:	_broadphase = new btAxisSweep3(		 minAABB, maxAABB);	break;
	}
	if( !_broadphase )
	{
		debug_break( "Failed to init btDbvtBroadphase." );
		return;
	}

	bool b_solver = false;
	switch( _s_solver )
	{
	default: debug_break( "Unknown _s_solver %d", _s_solver );
	case 0:	_solver				= new btSequentialImpulseConstraintSolver();	b_solver = _solver!=nullptr;			break;
	case 1:	_multiBodySolver	= new btMultiBodyConstraintSolver();			b_solver = _multiBodySolver!=nullptr;	break;
	}
	if( !b_solver )
	{
		debug_break( "Failed to init Solver." );
		return;
	}

	switch( _s_dynamics_world )
	{
	default: debug_break( "Unknown _s_dynamics_world %d", _s_dynamics_world );
	case 0:	_dynamicsWorld = new btDiscreteDynamicsWorld( _dispatcher, _broadphase, _solver, _collisionConfiguration);	break;
	case 1:	_dynamicsWorld = new btSoftRigidDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);	break;
	case 2:
		if( _s_solver != 1 )
			err_print("not a multibody constraint solver ?");
		else
			_dynamicsWorld = new btMultiBodyDynamicsWorld(_dispatcher, _broadphase, _multiBodySolver, _collisionConfiguration);
		break;
	case 3:
		if( _s_solver != 1 )
			err_print("not a multibody constraint solver ?");
		else
			_dynamicsWorld = new btSoftMultiBodyDynamicsWorld(_dispatcher, _broadphase, _multiBodySolver, _collisionConfiguration);
		break;
	}
	if( !_dynamicsWorld )
	{
		debug_break( "Failed to init btDiscrecteDynamicsWorld." );
		return;
	}
	_dynamicsWorld->setGravity( btVector3( -_gravity[0], -_gravity[1], -_gravity[2] ) );

	//HEAP_IS_CORRUPT();
}
#endif	//#if AAA_LIB_USE_BULLET()

void c_bdd_bullet::set_gravity( FP32* gravity )
{
#if AAA_LIB_USE_BULLET()
	if( !_dynamicsWorld )
		return ;
	_dynamicsWorld->setGravity( btVector3( -_gravity[0], -_gravity[1], -_gravity[2] ) );
#else
	err_print_lib_unused( __FUNCTION__ );
#endif	//#if AAA_LIB_USE_BULLET()
}

INT32 c_bdd_bullet::create_box_shape_id( btVector3* box_size )
{
#if AAA_LIB_USE_BULLET()
	btBoxShape* box = new btBoxShape( box_size ? (*box_size)*btScalar(.5) : btVector3( .5, .5, .5 ) );
	_box_shapes.push_back( box );
	return INT32( _box_shapes.size() - 1 );
#else
	err_print_lib_unused( __FUNCTION__ );
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}

INT32 c_bdd_bullet::create_cylinder_shape_id( btVector3* cylinder_size )
{
#if AAA_LIB_USE_BULLET()
	btCylinderShape* cylinder = new btCylinderShape( cylinder_size ? (*cylinder_size)*btScalar(.5) : btVector3( .5, .5, .5 ) );
	_cylinder_shapes.push_back( cylinder );
	return INT32( _cylinder_shapes.size() - 1 );
#else
	err_print_lib_unused( __FUNCTION__ );
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}

INT32 c_bdd_bullet::create_sphere_shape_id( FP32 sphere_size )
{
#if AAA_LIB_USE_BULLET()
	btSphereShape* sph = new btSphereShape( btScalar(sphere_size * .5) );
	_sphere_shapes.push_back( sph );
	return INT32( _sphere_shapes.size() - 1 );
#else
	err_print_lib_unused( __FUNCTION__ );
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}


/*INT32 c_bdd_bullet::create_heightfield_shape_id( void* data, INT32 width, INT32 depth )
{
	if( !data )
	{
		debug_break( "Data is null, can't create a btHeightfieldTerrainShape." );
		return -1;
	}
	INT32 height_scale = 1.;
	INT32 up_axis = 1;
	bool flip_quad_edges = false; // to  flip triangles
	INT32 max_height = 8;
	INT32 min_height = -8;

	btCollisionShape*  hfts = new btHeightfieldTerrainShape( width, depth, data, height_scale, min_height, max_height,
																	up_axis, PHY_FLOAT, flip_quad_edges );
	_collisionShapes.push_back( hfts );
	return INT32( _collisionShapes.size() - 1 ); 
}*/

INT32	c_bdd_bullet::create_transform_id( FP32* pos )
{
#if AAA_LIB_USE_BULLET()
	btTransform* transform = new btTransform();
	transform->setIdentity();
	transform->setOrigin( btVector3( pos[0], pos[1], pos[2] ) );
	_transforms.push_back( transform );
	return INT32( _transforms.size() - 1 );
#else
	err_print_lib_unused( __FUNCTION__ );
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}

//void				c_bdd_bullet::create_soft_body() {}

btBoxShape*			c_bdd_bullet::get_box_shape( INT32 shape_id )
{
#if AAA_LIB_USE_BULLET()
	if( shape_id >= _box_shapes.size() )
	{
		debug_break( "Shape id out of range !" );
		return nullptr;
	}
	return _box_shapes[shape_id];
#else
	err_print_lib_unused( __FUNCTION__ );
	return nullptr;
#endif	//#if AAA_LIB_USE_BULLET()
}

btCylinderShape*	c_bdd_bullet::get_cylinder_shape( INT32 shape_id )
{
#if AAA_LIB_USE_BULLET()
	if( shape_id >= _cylinder_shapes.size() )
	{
		debug_break( "Shape id out of range !" );
		return nullptr;
	}
	return _cylinder_shapes[shape_id];
#else
	err_print_lib_unused( __FUNCTION__ );
	return nullptr;
#endif	//#if AAA_LIB_USE_BULLET()
}

btSphereShape*		c_bdd_bullet::get_sphere_shape( INT32 shape_id )
{
#if AAA_LIB_USE_BULLET()
	if ( shape_id >= _sphere_shapes.size() )
	{
		debug_break( "Shape id out of range !" );
		return nullptr;
	}
	return _sphere_shapes[shape_id];
#else
	err_print_lib_unused( __FUNCTION__ );
	return nullptr;
#endif	//#if AAA_LIB_USE_BULLET()
}

btTransform*	c_bdd_bullet::get_transform( INT32 trans_id )
{
#if AAA_LIB_USE_BULLET()
	if( trans_id >= _transforms.size() )
	{
		debug_break( "Transform id out of range !" );
		return nullptr;
	}
	return _transforms[trans_id];
#else
	err_print_lib_unused( __FUNCTION__ );
	return nullptr;
#endif	//#if AAA_LIB_USE_BULLET()
}

btRigidBody*	c_bdd_bullet::get_rigid_body(INT32 id_body)
{
#if AAA_LIB_USE_BULLET()
	if ( id_body >= _rigid_bodies.size() )
	{
		debug_break( "Body id out of range !" );
		return nullptr;
	}
	return _rigid_bodies[id_body];
#else
	err_print_lib_unused( __FUNCTION__ );
	return nullptr;
#endif	//#if AAA_LIB_USE_BULLET()
}


/*void	c_bdd_bullet::viol_template_logic_force_to_instanciate_it()
{
	FP32 pos[3] {0, 0, 0};
	INT32 t_id = create_transform_id( pos );
	INT32 sp_id = create_sphere_shape_id( 1 );
	INT32 bo_id = create_box_shape_id( &btVector3(1, 1, 1) );
	FP32 li[3] {0, 0, 0};
	INT32 b_id = create_rigid_body_id( 0, t_id, li, _sphere_shapes[sp_id] );
	INT32 bid = add_rigid_body_bid( 0, _transforms[t_id], _rigid_bodies[b_id], _sphere_shapes[sp_id] );
	b_id =		create_rigid_body_id( 0, t_id, li, _box_shapes[bo_id] );
	bid =		add_rigid_body_bid( 0, _transforms[t_id], _rigid_bodies[b_id], _box_shapes[bo_id] );
}*/

INT32	c_bdd_bullet::delete_obj_bid( INT32 bid )
{
#if AAA_LIB_USE_BULLET()
	if( !_dynamicsWorld )
		return 0;

	DBG_PRINT_STRING( "deleting bid : %d", bid );	
	auto it = _objects.find( bid );
	if( it!=_objects.end() )
	{
		c_bullet_obj* obj = it->second;
		//btCollisionObject* col_obj = dynamic_cast<btCollisionObject*>(obj->get_rigid_body());
		_dynamicsWorld->removeRigidBody( obj->get_rigid_body() );
		//_objects_free.push_back(obj);
		_objects.erase( it );
		//delete obj;
		switch( obj->get_shape_type() )
		{
		case c_bullet_obj::SHAPE_SPHERE:	_objects_free_sphere.push_back(obj);	break;
		case c_bullet_obj::SHAPE_BOX:		_objects_free_box.push_back(obj);		break;
		default:
			break;
		}

		return bid;
	}
	else
	{
		debug_break( "No bid in map" );
		return 0;
	}
#else
	err_print_lib_unused( __FUNCTION__ );
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}

void	c_bdd_bullet::lock_all_objects( FP32* axes, FP32* angles ) // Only rigid body for now
{
	for( auto const & elt : _objects )
	{
		lock_move_rotate( elt.second->get_rigid_body(), axes, angles );
	}
}

void	c_bdd_bullet::lock_move_rotate( btRigidBody* body, FP32* axes, FP32* angles ) // Only rigid body for now
{
	btVector3 btaxes(axes[0], axes[1], axes[2]);
	btVector3 btangles(angles[0], angles[1], angles[2]);
	if ( body )
	{
		body->setLinearFactor( btaxes );
		body->setAngularFactor( btangles );
	}
}

void	c_bdd_bullet::restart()
{
	_b_restart_trig_ui = true;
}

#if AAA_LIB_USE_BULLET()
void	c_bdd_bullet::reset_boxes()
{
	exit_physics();
	DBG_PRINT_STRING("after exit_physics");

	open_world();
	DBG_PRINT_STRING("after open_world");
	if (_objects_nb_allocated > 100000 || _objects_nb_allocated < 0 || _objects_nb > _objects_nb_allocated)
	{
		PRINT_STRING( "obj nb alloc was to high : %d, we set it to 1000\n", _objects_nb_allocated);
		_objects_nb_allocated = 1000;
	}

	DBG_HEAP_IS_CORRUPT();
}

void	c_bdd_bullet::exit_physics()
{

//	removePickingConstraint();
	//cleanup in the reverse order of creation/initialization
	//remove the rigid bodies from the dynamics world and delete them
	//HEAP_IS_CORRUPT();
	if( !_dynamicsWorld )
		return;

	DBG_HEAP_IS_CORRUPT();
	for( INT32 i = _dynamicsWorld->getNumConstraints() - 1; i >= 0; i-- )
		_dynamicsWorld->removeConstraint( _dynamicsWorld->getConstraint(i) );

/*	for (i = m_dynamicsWorld->getNumMultiBodyConstraints() - 1; i >= 0; i--)
	{
		btMultiBodyConstraint* mbc = m_dynamicsWorld->getMultiBodyConstraint(i);
		m_dynamicsWorld->removeMultiBodyConstraint(mbc);
		delete mbc;
	}
	for (i = m_dynamicsWorld->getNumMultibodies() - 1; i >= 0; i--)
	{
		btMultiBody* mb = m_dynamicsWorld->getMultiBody(i);
		m_dynamicsWorld->removeMultiBody(mb);
		delete mb;
	}*/
	for( INT32 i = _dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = _dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast( obj );
		if( body && body->getMotionState() )
			delete body->getMotionState();
		_dynamicsWorld->removeCollisionObject( obj );
		SAFE_DELETE( obj );
	}
	DBG_PRINT_STRING( "before _rigidBodies delete" );
		_rigid_bodies.clear();
	DBG_PRINT_STRING( "after _rigidBodies delete" );
	for( auto& trans : _transforms )
		SAFE_DELETE( trans );

	_transforms.clear();
	//delete collision shapes
	for( auto& shape : _box_shapes )
		SAFE_DELETE( shape );

	_box_shapes.clear();
	for( auto& shape : _sphere_shapes )
		SAFE_DELETE( shape );

	_sphere_shapes.clear();
	// delete c_bullet_obj
	for( auto& elt : _objects )
		SAFE_DELETE( elt.second );
	_objects.clear();

	for( auto& elt : _objects_free_sphere )
		SAFE_DELETE( elt );
	_objects_free_sphere.clear();

	for( auto& elt : _objects_free_box )
		SAFE_DELETE( elt );
	_objects_free_box.clear();

	SAFE_DELETE( _dynamicsWorld );
	SAFE_DELETE( _solver );
	SAFE_DELETE( _broadphase );
	SAFE_DELETE( _dispatcher );
	SAFE_DELETE( _collisionConfiguration );

	DBG_HEAP_IS_CORRUPT();
}
#endif	//#if AAA_LIB_USE_BULLET()

void	c_bdd_bullet::update( )
{
#if AAA_LIB_USE_BULLET()
	//HEAP_IS_CORRUPT();
	if( _delta_t.update() || _b_restart_trig_ui )
	{
		reset_boxes();
		_b_restart_trig_ui = false;
	}
	_delta_t.update();

	if( !_dynamicsWorld )
	{
		debug_break( "No dynamics world !!!" );
		return;
	}
	_dynamicsWorld->stepSimulation( btScalar(_delta_t.get_dt()) );

	_objects_nb = (INT32)_objects.size();
	for( auto& elt : _objects )
		elt.second->update();
/*
	// TODO : update graphic objects separately from physic objects, so we only move dynamic objects
	auto collision_array = _dynamicsWorld->getCollisionObjectArray();
	
	for( INT32 i=_dynamicsWorld->getNumCollisionObjects()-1 ; i>=0 ; i-- )
	{
		btCollisionObject* obj = collision_array[i];
		btRigidBody* body = btRigidBody::upcast( obj ); 
		if ( body && body->getMotionState() )
			body->getMotionState()->getWorldTransform( trans );
		else
			trans = obj->getWorldTransform();
		}
	}
*/
#else
	err_print_lib_unused( __FUNCTION__ );
#endif	//#if AAA_LIB_USE_BULLET()
}

#if AAA_LIB_USE_BULLET()
void	c_bdd_bullet::draw_obj_one( c_bullet_obj* obj )
{
	GOL::matrix::push();
		GOL::matrix::scale( _scale );
		GOL::matrix::mul_matrix( (glm::mat4 CONST *) obj->get_mat() );
		switch( obj->get_shape_type() )
		{
		case c_bullet_obj::SHAPE_SPHERE:	draw_sphere( obj->get_size()[0] );	break;
		case c_bullet_obj::SHAPE_CYLINDER:	draw_box( obj->get_size() );		break;
		case c_bullet_obj::SHAPE_BOX:		draw_box( obj->get_size() );		break;
		default:	debug_break( "UNKNOWN shape type" );		break;
		}
	GOL::matrix::pop();
}
#endif	//#if AAA_LIB_USE_BULLET()

void	c_bdd_bullet::draw_single()
{
#if AAA_LIB_USE_BULLET()
	if( !_dynamicsWorld )
	{
		debug_break( "No dynamics world !!!" );
		return;
	}

	// Draw objects
	for( auto& elt : _objects )
	{
		c_bullet_obj* obj = elt.second;
		if( _b_draw_by_type[ obj->get_shape_type() ] )
			draw_obj_one( obj );		
	}
#else
	err_print_lib_unused( __FUNCTION__ );
#endif	//#if AAA_LIB_USE_BULLET()
}

void	c_bdd_bullet::draw_multiple()
{
#if AAA_LIB_USE_BULLET()
	if( !_dynamicsWorld )
	{
		debug_break( "No dynamics world !!!" );
		return;
	}
	if( !c_multiple::cur ) return;

	REAL CONST * CONST size_mul = c_multiple::cur->get_size();

	INT32 nb =  _dynamicsWorld->getNumCollisionObjects() - 1;
	c_multiple::cur->set_nb( nb );
	c_multiple::cur->init_index_w();
	REAL pos[3];

	INT32 i = -1;
	if( c_multiple::cur->is_align_normal() )
	{
		for( auto& elt : _objects )
		{
			c_bullet_obj* obj = elt.second;
			c_multiple::cur->set_index(++i);
			//c_tex_anim::cur->bind_this( i );
			if( _b_draw_by_type[ obj->get_shape_type() ] )
			{
				scale_v3( pos, obj->get_mat(), _scale );
				c_multiple::cur->draw_one_at_nor( pos, size_mul, obj->get_axe());
			}
		}
	}
	else
	{
		for( auto& elt : _objects )
		{
			c_bullet_obj* obj = elt.second;
			c_multiple::cur->set_index(++i);
			//c_tex_anim::cur->bind_this( i );
			if( _b_draw_by_type[ obj->get_shape_type() ] )
			{
				scale_v3( pos, obj->get_mat(), _scale );
				c_multiple::cur->align_then_draw(pos, size_mul);
			}
		}
	}
#else
	err_print_lib_unused( __FUNCTION__ );
#endif	//#if AAA_LIB_USE_BULLET()
}

INT32	c_bdd_bullet::get_point_nb()
{
#if AAA_LIB_USE_BULLET()
	return _objects_nb;
#else
	err_print_lib_unused( __FUNCTION__ );
	return 0;
#endif	//#if AAA_LIB_USE_BULLET()
}

bool	c_bdd_bullet::get_point( REAL* CONST dst, INT32 CONST index )
{
#if AAA_LIB_USE_BULLET()
	scale_v3( dst, _objects[ index + 1 ]->get_mat(), _scale );
	return true;
#else
	return false;
#endif	//#if AAA_LIB_USE_BULLET()
}

#if AAA_LIB_USE_BULLET()
void	c_bdd_bullet::my_draw_soft()
{
	/*
	/// Draw Softbodies

	// Get a ref to the array of softbodies   //
	btSoftBodyArray& softbodies(world->getSoftBodyArray());

	// For each soft bodies               //
	for (int i = 0; i < softbodies.size(); ++i)
	{
		btSoftBody* softbody(softbodies[i]);

		// Each soft body contain an array of vertices (nodes/particles_mass)   //
		btSoftBody::tNodeArray& nodes(softbody->m_nodes);

		// And edges (links/distances constraints)                        //
		btSoftBody::tLinkArray& links(softbody->m_links);

		// And finally, faces (triangles)                                 //
		btSoftBody::tFaceArray& faces(softbody->m_faces);

		// Then, you can draw vertices...      //
		// Node::m_x => position            //
		// Node::m_n => normal (if meaningful)   //
		for (int j = 0; j < nodes.size(); ++j)
		{
								//			mygfx->DrawPoint(nodes[j].m_x);
		}

		// Or edges (for ropes)               //
		// Link::m_n[2] => pointers to nodes   //
		for (int j = 0; j < links.size(); ++j)
		{
			btSoftBody::Node* node_0 = links[j].m_n[0];
			btSoftBody::Node* node_1 = links[j].m_n[1];
								//			mygfx->DrawLine(node_0->m_x, node_1->m_x);

			// Or if you need indices...      //
			const int indices[] = { int(node_0 - &nodes[0]),
									int(node_1 - &nodes[0]) };
		}

		// And even faces                  //
		// Face::m_n[3] -> pointers to nodes   //
		for (int j = 0; j < faces.size(); ++j)
		{
			btSoftBody::Node* node_0 = faces[j].m_n[0];
			btSoftBody::Node* node_1 = faces[j].m_n[1];
			btSoftBody::Node* node_2 = faces[j].m_n[2];
								//			mygfx->DrawTriangle(node_0->m_x, node_1->m_x, node_2->m_x);

			// Or if you need indices...      //
			const int indices[] = { int(node_0 - &nodes[0]),
									int(node_1 - &nodes[0]),
									int(node_2 - &nodes[0]) };
		}
	}
	*/
}
#endif	//#if AAA_LIB_USE_BULLET()

/*
// Ray test : first hit
void	c_bdd_bullet::pick(FP32* from, FP32* to)
{
	if( !_dynamicsWorld )
	{
		debug_break( "No dynamics world !!!");
		return;
	}
	
	//	DBG_PRINT_STRING("to[0] :% f, to[1] : % f, to[2] : % f\n", to[0], to[1], to[2]);
	//	DBG_PRINT_STRING("from[0] :% f, from[1] : % f, from[2] : % f\n", from[0], from[1], from[2]);
	//
	compute_mouse_start(from, to);
	
//		DBG_PRINT_STRING("after cam to world\n");
//		DBG_PRINT_STRING("to[0] :% f, to[1] : % f, to[2] : % f\n", to[0], to[1], to[2]);
//		DBG_PRINT_STRING("from[0] :% f, from[1] : % f, from[2] : % f\n", from[0], from[1], from[2]);
//
	btVector3 btfrom(from[0], from[1], from[2]);
	btVector3 btto(to[0], to[1], to[2]);
//	compute_my_ray_plz(from, to);

	btCollisionWorld::ClosestRayResultCallback closestResults( btfrom, btto );
	//closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	_dynamicsWorld->rayTest( btfrom, btto, closestResults );
	
		FP32 mat[16];
		INT32 t_id = create_transform_id( to );
		_transforms[t_id]->getOpenGLMatrix( mat );
		
		print_it((DOUBLE*)(mat), (DOUBLE*)(mat));

		GOL::matrix::push();
			GOL::scale( _scale );
			GOL::mul_matrix( mat );
			draw_cube( 15 );
		GOL::matrix::pop();


	if (closestResults.hasHit())
	{
		btVector3 p = btfrom.lerp( btto, closestResults.m_closestHitFraction );

		btCollisionObject* coll_obj = const_cast<btCollisionObject*>( closestResults.m_collisionObject );
		btRigidBody* pBody = btRigidBody::upcast( coll_obj );

		DBG_PRINT_STRING("hit pbody : %p\n", pBody);

		if (pBody->getInvMass() != 0)
		{
			printf("mass : %f", pBody->getInvMass());
			printf("HIT :)");
			btVector3 move_it(0, 10, 0);
			pBody->translate(move_it);
			//pBody->applyCentralImpulse(move_it);
			//m_dynamicsWorld->getDebugDrawer()->drawLine(p, p + closestResults.m_hitNormalWorld, blue);
		}
	}
}
struct st_ui_store
{
	REAL	tra[3];
	REAL	rot[3];
	REAL	ortho_size;
};
void	c_bdd_bullet::get_matrixes(DOUBLE* mat_proj, DOUBLE* mat_modelview)
{
	auto cam = c_seedcam::get_ui(); //c_layers::get_layers_camera_used();

	if (cam)
	{
		cam->update();
		cam->do_projection_and_modelview( AXE_NONE, false );
		cam->update_bdd_to_camera();
		GOL::get_matrix_projection_double(mat_proj);
		GOL::get_matrix_modelview_double(mat_modelview);
		print_it(mat_proj, mat_modelview);
		st_ui_store* cam_pos = new st_ui_store;
		cam->store_pos(cam_pos);
		printf("cam_pos c : \n");
		printf("tra_x : %f, tra_y : %f, tra_z : %f\n", cam_pos->tra[0], cam_pos->tra[1], cam_pos->tra[2]);
		printf("rot_x : %f, rot_y : %f, rot_z : %f\n", cam_pos->rot[0], cam_pos->rot[1], cam_pos->rot[2]);
		printf("ortho size : %f\n", cam_pos->ortho_size);
	}
}

void	c_bdd_bullet::print_it(DOUBLE* mat_proj, DOUBLE* mat_modelview)
{
	DBG_PRINT_STRING("mp[0] : %f, mp[1] : %f, mp[2] : %f, mp[3] : %f, \n\
				mp[4] : %f, mp[5] : %f, mp[6] : %f, mp[7] : %f, \n\
				mp[8] : %f, mp[9] : %f, mp[10] : %f, mp[11] : %f, \n\
				mp[12] : %f, mp[13] : %f, mp[14] : %f, mp[15] : %f, \n\
				mv[0] : %f, mv[1] : %f, mv[2] : %f, mv[3] : %f, \n\
				mv[4] : %f, mv[5] : %f, mv[6] : %f, mv[7] : %f, \n\
				mv[8] : %f, mv[9] : %f, mv[10] : %f, mv[11] : %f, \n\
				mv[12] : %f, mv[13] : %f, mv[14] : %f, mv[15] : %f, \n",
		mat_proj[0], mat_proj[1], mat_proj[2], mat_proj[3],
		mat_proj[4], mat_proj[5], mat_proj[6], mat_proj[7],
		mat_proj[8], mat_proj[9], mat_proj[10], mat_proj[11],
		mat_proj[12], mat_proj[13], mat_proj[14], mat_proj[16],
		mat_modelview[0], mat_modelview[1], mat_modelview[2], mat_modelview[3],
		mat_modelview[4], mat_modelview[5], mat_modelview[6], mat_modelview[7],
		mat_modelview[8], mat_modelview[9], mat_modelview[10], mat_modelview[11],
		mat_modelview[12], mat_modelview[13], mat_modelview[14], mat_modelview[16]);
}

// see c_bdd_curve_3d::compute_mouse_start()
bool	c_bdd_bullet::compute_mouse_start(FP32* from, FP32* to)
{
//	REAL pt[3];

//	auto cam = c_layers::get_layers_camera_used();
	auto cam = c_layers::get_cur()->get_camera(1);
	if (!cam)
	{
		err_print( "layers got no camera ?" );
		auto	cam = c_seedcam::get_ui();
	}
	if (!cam)
	{
		err_print( "ui got no camera ?!" );
		return false;
	}

	cam->update();
	cam->do_projection_and_modelview( AXE_NONE, false );
	cam->update_bdd_to_camera();

	DBG_PRINT_STRING("before cam to world\n");
	DBG_PRINT_STRING("to[0] :% f, to[1] : % f, to[2] : % f\n", to[0], to[1], to[2]);
	DBG_PRINT_STRING("from[0] :% f, from[1] : % f, from[2] : % f\n", from[0], from[1], from[2]);
	cam->coor_camera_to_world(from);
	cam->coor_camera_to_world(to);
	DBG_PRINT_STRING("after cam to world\n");
	DBG_PRINT_STRING("to[0] :% f, to[1] : % f, to[2] : % f\n", to[0], to[1], to[2]);
	DBG_PRINT_STRING("from[0] :% f, from[1] : % f, from[2] : % f\n", from[0], from[1], from[2]);
	//draw_sphere_at(1.5, to);

//
//		double	mat_proj[16];
//todomatrix
//		GOL::get_matrix_projection_double( mat_proj );
//
//		double	mat_modelview[16];
//todomatrix
//		GOL::get_matrix_modelview_double( mat_modelview );
//
//		int		viewport[4];
//		GOL::get_integer( GL_VIEWPORT, viewport );
//
//		INT32	win_x, win_y;
//		c_mouse::get_cur()->get_pos_window_pixel( win_x, win_y );
//		
//		double	mouse_x, mouse_y, zdepth, x, y, z;
//		gluProject( *pt , *(pt+1), *(pt+2), mat_modelview, mat_proj, viewport, &mouse_x, &mouse_y, &zdepth );
//
//		draw_sphere_at(.5, pt);
//
//		gluUnProject( win_x, viewport[3]-win_y, zdepth, mat_modelview, mat_proj, viewport, &x, &y, &z );
//		return true;
	}

	return false;
}
// end of ray test
*/
