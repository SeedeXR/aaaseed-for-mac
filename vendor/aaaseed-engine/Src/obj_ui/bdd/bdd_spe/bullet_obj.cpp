#include "bullet_obj.h"
#include "Bullet/btBulletDynamicsCommon.h"
#include "Bullet/btBulletCollisionCommon.h"
#include "math/v.h"

namespace {
	UINT32	_bid_generator = 0;
	INT32	generate_bid()		{	return ++_bid_generator;	}
}

c_bullet_obj::c_bullet_obj( FP32 mass, btTransform* trans, btRigidBody* body, btSphereShape* sphere ):
	_body_type	( RIGIDBODY		),
	_shape_type	( SHAPE_SPHERE	),
	_sbody		( nullptr		),
	_box_shape	( nullptr		),
	_sph_shape	( sphere		),
	_cyl_shape	( nullptr		),
	_mass		( mass			),
	_rbody		( body			),

	_trans		( trans			)
{
	_bid = generate_bid();
	trans->getOpenGLMatrix(_ogl_mat);

	btQuaternion rot = trans->getRotation();
	_quat = Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
	Vec3f dir;
	_quat.getAxisAngle( &dir, &_angle );
	_axis[0] = dir[0];
	_axis[1] = dir[1];
	_axis[2] = dir[2];

	FP32* pos = trans->getOrigin().m_floats;
	cpy_v3( _position, pos );
	_size[0] = (FP32)( sphere->getRadius()*2. );
}

c_bullet_obj::c_bullet_obj( FP32 mass, btTransform* trans, btRigidBody* body, btCylinderShape* cylinder ):
	_body_type	( RIGIDBODY			),
	_shape_type	( SHAPE_CYLINDER	),
	_sbody		( nullptr			),
	_box_shape	( nullptr			),
	_sph_shape	( nullptr			),
	_cyl_shape	( cylinder			),
	_mass		( mass				),
	_rbody		( body				),
	_trans		( trans				)
{
	_bid = generate_bid();
	trans->getOpenGLMatrix(_ogl_mat);

	btQuaternion rot = trans->getRotation();
	_quat = Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
	Vec3f dir;
	_quat.getAxisAngle( &dir, &_angle );
	_axis[0] = dir[0];
	_axis[1] = dir[1];
	_axis[2] = dir[2];

	FP32* pos = trans->getOrigin().m_floats;
	cpy_v3( _position, pos );
	btVector3 half_ext = cylinder->getHalfExtentsWithoutMargin();
	half_ext *= 2.;
	scale_v3( _size, half_ext.m_floats, 2. );
	//_size[0] = (FP32)( half_ext.m_floats[0]*2. );
	//_size[1] = (FP32)( half_ext.m_floats[1]*2. );
	//_size[2] = (FP32)( half_ext.m_floats[2]*2. );
}

c_bullet_obj::c_bullet_obj( FP32 mass, btTransform* trans, btRigidBody* body, btBoxShape* box ):
	_body_type	( RIGIDBODY	),
	_shape_type	( SHAPE_BOX	),
	_sbody		( nullptr	),
	_box_shape	( box		),
	_sph_shape	( nullptr	),
	_cyl_shape	( nullptr	),
	_mass		( mass		),
	_rbody		( body		),
	_trans		( trans		)
{
	_bid = generate_bid();
	trans->getOpenGLMatrix(_ogl_mat);

	btQuaternion rot = trans->getRotation();
	_quat = Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
	Vec3f dir;
	_quat.getAxisAngle( &dir, &_angle );
	_axis[0] = dir[0];
	_axis[1] = dir[1];
	_axis[2] = dir[2];

	FP32* pos = trans->getOrigin().m_floats;
	cpy_v3( _position, pos );
	btVector3 half_ext = box->getHalfExtentsWithoutMargin();
	scale_v3( _size, half_ext.m_floats, 2. );
	//_size[0] = (FP32)( half_ext.m_floats[0]*2. );
	//_size[1] = (FP32)( half_ext.m_floats[1]*2. );
	//_size[2] = (FP32)( half_ext.m_floats[2]*2. );
}

c_bullet_obj::~c_bullet_obj()
{
}

void	c_bullet_obj::update()
{
	if( _body_type == RIGIDBODY )
	{
		*_trans = _rbody->getWorldTransform();
		//_rbody->getMotionState()->getWorldTransform(*Fset)mass_trans);
#if AAA_LIB_USE_BULLET()
		_rbody->setActivationState( 1 );
#endif	//#if AAA_LIB_USE_BULLET()
	}
	_trans->getOpenGLMatrix(_ogl_mat);

	btQuaternion rot = _trans->getRotation();
	_quat = Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ());
	Vec3f dir;
	float rad;
	_quat.getAxisAngle( &dir, &rad );
	_axis[0] = dir[0];
	_axis[1] = dir[1];
	_axis[2] = dir[2];

	FP32* pos = _trans->getOrigin().m_floats;
	cpy_v3( _position, pos );

	switch( _shape_type )
	{
	case SHAPE_SPHERE:
		_size[0] = (FP32)( _sph_shape->getRadius()*2. );
		break;
	case SHAPE_CYLINDER:
		_size[0] = (FP32)( _cyl_shape->getRadius()*2. );
		break;
	default:
	case SHAPE_BOX:
		{
			btVector3 half_ext = _box_shape->getHalfExtentsWithoutMargin();
			scale_v3( _size, half_ext.m_floats, 2. );
			//_size[0] = (FP32)( half_ext.m_floats[0]*2. );
			//_size[1] = (FP32)( half_ext.m_floats[1]*2. );
			//_size[2] = (FP32)( half_ext.m_floats[2]*2. );
		}
		break;
	}
}

void	c_bullet_obj::set_mass( FP32 mass )
{
#if AAA_LIB_USE_BULLET()
	if( _mass == 0 )
		set_dynamic( true );
	btVector3 inertia;
	if( _body_type == RIGIDBODY )
	{
		_rbody->getCollisionShape()->calculateLocalInertia( mass, inertia );
		_rbody->setMassProps( mass, inertia );
	}
	else if( _body_type == SOFTBODY )
	{
	//	_sbody->getCollisionShape()->calculateLocalInertia( mass, inertia );
	//	_sbody->setMassProps( mass, inertia );
	}
	if( _mass == 0 )
		set_dynamic( false );
#endif	//#if AAA_LIB_USE_BULLET()
}

void	c_bullet_obj::set_pos( FP32* pos)
{
	btVector3 o = btVector3( pos[0], pos[1], pos[2] );
	switch( _shape_type )
	{
	case SHAPE_SPHERE:
		*_trans = _rbody->getWorldTransform();
		_trans->setOrigin(o);
//		_rbody->setCenterOfMassTransform(*_trans);
		_rbody->setWorldTransform(*_trans);
		_rbody->getMotionState()->setWorldTransform(*_trans);
		break;
	default:
	case SHAPE_BOX:
		break;
	}
	cpy_v3( _position, pos );
}

void	c_bullet_obj::set_size( FP32* size )
{
	scale_v3( _size, .5 );
	switch( _shape_type )
	{
	case SHAPE_SPHERE:
		_sph_shape->setUnscaledRadius(_size[0]);
		break;
	default:
	case SHAPE_BOX:
		break;
	}
}

void	c_bullet_obj::set_rot_z(FP32 rot)
{
	// ACHTUNG DAS IST NICHT GUT FUR DEIN KARMA
	_quat[2] = rot;
}

FP32	c_bullet_obj::get_rot_z() CONST
{
	// ACHTUNG DAS IST NICHT GUT FUR DEIN KARMA
	return _quat[2];
}

void	c_bullet_obj::set_damping( FP32 lin, FP32 ang ) CONST
{
#if AAA_LIB_USE_BULLET()
	_rbody->setDamping( lin, ang );
#endif	//#if AAA_LIB_USE_BULLET()
}

void	c_bullet_obj::set_dynamic( bool b_dynamic ) CONST
{
#if AAA_LIB_USE_BULLET()
	if( b_dynamic )
		_rbody->setCollisionFlags( _rbody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
	else
		_rbody->setCollisionFlags( _rbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
#endif	//#if AAA_LIB_USE_BULLET()
}

void	c_bullet_obj::set_restitution( FP32 restitution ) CONST
{
#if AAA_LIB_USE_BULLET()
	_rbody->setRestitution( restitution );
#endif	//#if AAA_LIB_USE_BULLET()
}