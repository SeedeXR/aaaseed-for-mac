
#ifdef AAA_BULLET_OBJ_H
#error "BULLET_OBJ_H included more than once."
#endif
#define AAA_BULLET_OBJ_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#define AAA_LIB_USE_BULLET() 1

#ifndef AAA_TQUATERNION_H
#	include "math/TQuaternion.h"
#endif

class btQuaternion;
class btRigidBody;
class btSoftBody;

class btSphereShape;
class btCylinderShape;
class btBoxShape;

class btTransform;

class btQuaternion;
class btVector3;


//body_types : 1 = rigid, 2 = soft


class	c_bullet_obj
{
public:
	enum BODY_TYPE : INT32
	{
		RIGIDBODY		= 0,
		SOFTBODY,
		BODY_NB
	};

	enum SHAPE_TYPE : INT32
	{
		SHAPE_SPHERE		= 0,
		SHAPE_BOX,
		SHAPE_CYLINDER,
		SHAPE_NB
	};

private:
		INT32				_bid;
		SHAPE_TYPE			_shape_type;
		INT32				_body_type;

		btRigidBody*		_rbody;
		btSoftBody*			_sbody;

		btSphereShape*		_sph_shape;
		btCylinderShape*	_cyl_shape;
		btBoxShape*			_box_shape;

		btTransform*		_trans;

		FP32				_mass;
		
		Quaternion<FP32>	_quat;
		FP32				_ogl_mat[16];
		FP32				_axis[3];
		FP32				_angle;
		FP32				_position[3];
		FP32				_size[3];
//		FP32				_euler_rotation[3];
	public:

		c_bullet_obj( FP32 mass, btTransform* trans,	btRigidBody* body, btSphereShape* sphere );
		c_bullet_obj( FP32 mass, btTransform* trans,	btRigidBody* body, btCylinderShape* cylinder );
		c_bullet_obj( FP32 mass, btTransform* trans,	btRigidBody* body, btBoxShape*    box	);
		virtual ~c_bullet_obj();

		void						update();
		FINLINE FP32 CONST * 		get_mat() CONST				{ return _ogl_mat; }
		FINLINE FP32 CONST * 		get_axe() CONST				{ return _axis; }

		FINLINE	INT32				get_bid()	CONST			{ return _bid; }

		
		void						set_mass( FP32 mass );
		void						set_pos(FP32* pos);
		FINLINE	FP32 CONST *		get_pos()	CONST			{ return _position; }
		void						set_size(FP32* size);
		FINLINE	FP32 CONST *		get_size()	CONST			{ return _size; }
		void						set_rot_z(FP32 rot);
		FP32						get_rot_z() CONST;
		void						set_damping( FP32 lin, FP32 ang ) CONST;
		void						set_dynamic( bool b_dynamic ) CONST;
		void						set_restitution( FP32 restitution ) CONST;

		FINLINE btRigidBody*		get_rigid_body()		CONST		{ return _rbody; }
		FINLINE btSoftBody*			get_soft_body()			CONST		{ return _sbody; }

		FINLINE btSphereShape*		get_sphere_shape()		CONST	{ return _sph_shape; }
		FINLINE btCylinderShape*	get_cylinder_shape()	CONST	{ return _cyl_shape; }
		FINLINE btBoxShape*			get_box_shape()			CONST	{ return _box_shape; }

		FINLINE	SHAPE_TYPE			get_shape_type()		CONST	{ return _shape_type; }
};

