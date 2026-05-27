
#ifdef AAA_BDD_BULLET_H
#error "BDD_BULLET_H included more than once."
#endif
#define AAA_BDD_BULLET_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_BULLET_OBJ_H
#	include "bullet_obj.h"
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif

#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

#if 0
	class btCollisionDispatcher;
	class btBroadphaseInterface;
	class btDefaultCollisionConfiguration;
	class btSequentialImpulseConstraintSolver;
	class btMultiBodyConstraintSolver;
	class btDiscreteDynamicsWorld;
	class btRigidBody;
	class btSoftBody;
	class btSphereShape;
	class btCylinderShape;
	class btBoxShape;
	class btTransform;
#else
#	include "Bullet/btBulletDynamicsCommon.h"
#	include "Bullet/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.h"
#	include "Bullet/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#endif


class	c_bdd_bullet final : public c_bdd_multiple 
{
	FACTORY_DECLARE(c_bdd_bullet,c_bdd_multiple);

private:

	INT32										_s_dispatcher;
	INT32										_s_broadphase;
	INT32										_s_solver;
	INT32										_s_collision;
	INT32										_s_dynamics_world;

#if AAA_LIB_USE_BULLET()
	// World configuration
	void										open_world();

	// Remove object and reset world
	void				reset_boxes();
	void				exit_physics();

	btCollisionDispatcher*						_dispatcher;
	btBroadphaseInterface*						_broadphase;
	btSequentialImpulseConstraintSolver*		_solver;
	btMultiBodyConstraintSolver*				_multiBodySolver;
	btDefaultCollisionConfiguration*			_collisionConfiguration;
	btDiscreteDynamicsWorld*					_dynamicsWorld;
#else
	void	err_print_lib_unused( C_PCHAR_C fn_name ) CONST;
#endif	//#if AAA_LIB_USE_BULLET()

	REAL										_gravity[3];
	REAL										_wind[3];
	REAL										_scale;
	REAL										_size_ui[4];

	REAL										_table_size_ui[4]; 
	INT32										_s_ground;
	
	c_delta_t									_delta_t;
	bool										_b_restart_trig_ui;
	
	// C Bullet Objects
	INT32										_objects_nb_allocated;
	INT32										_objects_nb;
	//std::vector<c_bullet_obj*>				_objects;
	std::unordered_map<INT32, c_bullet_obj*>	_objects;
	std::vector<c_bullet_obj*>					_objects_free_sphere;
	std::vector<c_bullet_obj*>					_objects_free_box;
	//std::vector<INT32>						_objects_free;
	
	// Store bullet internal objects created
#if AAA_LIB_USE_BULLET()
	std::vector<btRigidBody*>					_rigid_bodies;
//	std::vector<btSoftBody*>					_soft_bodies;

	std::vector<btSphereShape*>					_sphere_shapes;
	std::vector<btCylinderShape*>				_cylinder_shapes;
	std::vector<btBoxShape*>					_box_shapes;

	std::vector<btTransform*>					_transforms;

	void			draw_obj_one( c_bullet_obj* obj );
	void			my_draw_soft(); // for soft bodies
#endif	//#if AAA_LIB_USE_BULLET()

	bool										_b_draw_by_type[c_bullet_obj::SHAPE_NB];



public:
	FINLINE	c_bullet_obj*	get_obj_by_bid(		INT32 bid )
	{
		const auto it = _objects.find( bid );
		return it==_objects.end() ? nullptr : it->second;
	}


	void				set_gravity(			FP32* gravity );
	// Create bullet internal objects
	// Shapes
	INT32				create_box_shape_id(		btVector3* box_size );
	INT32				create_cylinder_shape_id(	btVector3* cylinder_size );
	INT32				create_sphere_shape_id(		FP32 sphere_size );
	//INT32				create_heightfield_shape_id( void* data, INT32 width, INT32 depth );

	btBoxShape*			get_box_shape(			INT32 shape_id );
	btCylinderShape*	get_cylinder_shape(		INT32 shape_id );
	btSphereShape*		get_sphere_shape(		INT32 shape_id );

	// Transforms
	INT32				create_transform_id(	FP32* pos );

	btTransform*		get_transform(			INT32 trans_id );

	//Bodies
	template <class SHAPE>
	INT32				create_rigid_body_id(	FP32 mass, INT32 transform_index, FP32 CONST * CONST local_inertia, SHAPE* shape );
	btRigidBody*		get_rigid_body(			INT32 id_body);

	
	// C bullet object
	template <class SHAPE>
	INT32				add_rigid_body_bid(		FP32 mass, btTransform* trans, btRigidBody* body, SHAPE* shape );
	INT32				delete_obj_bid(			INT32 bid	);
	
	// Lock motion or rotation on given axises
	void				lock_all_objects(		FP32* axes, FP32* angles ); // Only rigid body for now
	void				lock_move_rotate(		btRigidBody* body, FP32* axes, FP32* angles ); // Only rigid body for now



	virtual	void		param_init_pt();

	virtual void		update();
	virtual	void		draw_single();
	virtual	void		draw_multiple();
	// todo my_draw_mesh


	virtual void		restart();

	virtual	INT32		get_point_nb() final override;
	virtual	bool		get_point(				REAL* dst, INT32 CONST index ) final override;
	
	// test to pick an object when clicking, with a bullet ray cast
	/*
	void				pick(					FP32* from, FP32* to );
	bool				compute_mouse_start(	FP32* from, FP32* to );
	void				print_it(				DOUBLE* mat_proj, DOUBLE* mat_modelview);
	void				get_matrixes(			DOUBLE* mat_proj, DOUBLE* mat_modelview);
	*/
};

template <class SHAPE>
INT32	c_bdd_bullet::create_rigid_body_id( FP32 mass, INT32 transform_index, FP32 CONST * CONST local_inertia, SHAPE* shape )
{
#if AAA_LIB_USE_BULLET()
	//btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool b_dynamic = mass != 0.f;
	btVector3 localInertia( local_inertia[0], local_inertia[1], local_inertia[2] );
	if( b_dynamic )
		shape->calculateLocalInertia( mass, localInertia );

	if( transform_index >= _transforms.size() )
		debug_break( "Transform id out of range !" );
	btDefaultMotionState* myMotionState = new btDefaultMotionState( *_transforms[transform_index] );
	btRigidBody::btRigidBodyConstructionInfo info( mass, myMotionState, shape, localInertia );
	btRigidBody* body = new btRigidBody( info );

	//debug_break( "in create_rigid_body, after cinfo" );
	//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

	body->setUserIndex( -1 );	//todo understand
	_rigid_bodies.push_back( body );
	return INT32(_rigid_bodies.size() - 1);
#else
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}

template <class SHAPE>
INT32	c_bdd_bullet::add_rigid_body_bid( FP32 mass, btTransform* trans, btRigidBody* body, SHAPE* shape )
{
#if AAA_LIB_USE_BULLET()
	if( !_dynamicsWorld )
		return 0;

	c_bullet_obj* obj;	
	if( !_objects_free_sphere.empty() )
	{
		obj = _objects_free_sphere.back();
		_objects_free_sphere.pop_back();
	}
	else
		obj = new c_bullet_obj( mass, trans, body, shape  );

	if( obj )
	{
		INT32	bid = obj->get_bid();
		_objects[bid] = obj;
		_dynamicsWorld->addRigidBody( obj->get_rigid_body() );
		return bid;
	}
	else
	{
		debug_break( "OBJECT ADD ERROR !" );
		return -1;
	}
#else
	return -1;
#endif	//#if AAA_LIB_USE_BULLET()
}


