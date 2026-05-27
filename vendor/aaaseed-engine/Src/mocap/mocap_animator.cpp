
// Main header
#include "mocap/mocap_format_bvh.h"

#if AAA_LIB_USE_ASSIMP()
#include "mocap/mocap_animator.h"
#include <assimp/scene.h>
#include "aaa_mem.h"

#ifndef AAA_TQUATERNION_H
#	include "math\TQuaternion.h"
#endif

/** Identity matrix to return a reference to in case of error */
static aiMatrix4x4 IdentityMatrix;


///////////////////////////////////////////////////////////////////////////////////////////////////
//		mocap::Animator class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
mocap::c_animator * mocap::c_animator::create_ptr( const aiScene* scene, uint32_t anim_index )
{
	mocap::c_animator * ptr_return = new mocap::c_animator( scene, anim_index );
	
	ptr_return->init();

	return ptr_return;
}



//=================================================================================================
mocap::c_animator::c_animator( const aiScene * scene, uint32_t animation_index ) 

	: _scene					( scene )
	, _time_last				(  0.0 )
	, _node_root				( nullptr )
	, _animation_index_cur		( -1 )
	, _animation_index_first	( animation_index )
	, _animation_cur			( nullptr )
	, _position_frame_last		( nullptr )
	, _id_cur					( 0 )
{}

//=================================================================================================
mocap::c_animator::~c_animator( void ) 
{
	SAFE_DELETE_ARRAY( _position_frame_last );
	SAFE_DELETE( _node_root );

	_bone_node_by_name.clear();
	_transforms_channel.clear();
	_transforms_bone.clear();
}



//=================================================================================================
void mocap::c_animator::init( void )
{
	// Build the nodes-for-bones table
	for( uint32_t i = 0; i < _scene->mNumMeshes; i++ ) 
	{
		_mesh = _scene->mMeshes[i];

		for( uint32_t j = 0; j < _mesh->mNumBones; j++ ) 
		{
			const aiBone* pCurrentBone = _mesh->mBones[j];
			_bone_node_by_name[ pCurrentBone->mName.data ] = _scene->mRootNode->FindNode( pCurrentBone->mName );
		}
	}

	// Changing the current animation also creates the node tree for this animation
	set_animation_index( _animation_index_first );
}



//=================================================================================================
//Sets the animation to use for playback. 
void mocap::c_animator::set_animation_index( uint32_t animation_index ) 
{
	if( animation_index != _animation_index_cur ) 
	{
		// Release data of previous animation
		SAFE_DELETE( _node_root );
		SAFE_DELETE_ARRAY( _position_frame_last );
		_nodes_by_name.clear();

		// Invalid animation index -> back to first one
		_animation_index_cur = animation_index >= _scene->mNumAnimations ? 0 : animation_index;
		_animation_cur = _scene->mAnimations[_animation_index_cur];

		//create the internal node tree. Do this even in case of invalid animation index
		//so that the transformation matrices are properly set up to mimic the current scene
		INT32 id = 0;
		_node_root = create_node_tree( id, _scene->mRootNode, nullptr );
		//m_pRootNode->mSkeletonId = 0; // //

		_position_frame_last = new Vec3ui[ _animation_cur->mNumChannels ];

		// Compute segments based on Bones.

		// method use in create_node_tree work in certain case fail in other
		// Compute IDs.
		_node_root->_skeleton_id = 0;

		for( uint32_t i = 0; i < _mesh->mNumBones; i++ ) 
		{
			const aiBone* bone = _mesh->mBones[i];

			std::map<const aiNode*, mocap::c_animation_node*>::const_iterator it = _nodes_by_name.find( _bone_node_by_name[bone->mName.data] );
			if( it != _nodes_by_name.end() )
			{
				it->second->_skeleton_id = i+1;
			}
		}

		// Compute Segments.
		for( uint32_t i = 0; i < _mesh->mNumBones; i++ ) 
		{
			const aiBone* bone = _mesh->mBones[i];

			std::map<const aiNode*, mocap::c_animation_node*>::const_iterator it = _nodes_by_name.find( _bone_node_by_name[bone->mName.data] );
			if( it != _nodes_by_name.end() )
			{
				mocap::c_animation_node* anim_node = it->second;
				if( anim_node->_parent )
				{
					INT32 parent_id	=	anim_node->_parent->_skeleton_id;
					INT32 id		=	anim_node->_skeleton_id;
					if( parent_id == id )
					{
						ERR_PRINT_STRING( "Skipping this segment because it have only one point \"%s\" with id : %d.", anim_node->get_name(), id );
					}
					else if ( parent_id==0 )
					{
						ERR_PRINT_STRING( "Skipping this segment because parent \"%s\" id is 0.", anim_node->_parent->get_name() );
					}
					else if ( id==0 )
					{
						ERR_PRINT_STRING( "Skipping this segment because node \"%s\" is 0.", anim_node->get_name() );
					}
					else
					{
						_segment_ids.push_back( parent_id	);
						_segment_ids.push_back( id			);
					}
				}
			}
		}
	}
}



//=================================================================================================
//Recursively creates an internal node structure matching the current scene and animation.
mocap::c_animation_node * mocap::c_animator::create_node_tree( INT32& id, aiNode * node, mocap::c_animation_node * parent ) 
{
	// Begin : Previous
	// Create a node
	mocap::c_animation_node* node_internal = new mocap::c_animation_node( node->mName.data );
	node_internal->_parent = parent;
	_nodes_by_name[node] = node_internal;

	//// Segment IDs
	node_internal->_skeleton_id = ++id;
	//if( pInternalNode->pParent )
	//{
	//	m_vSegmentIndices.push_back( pInternalNode->pParent->mSkeletonId );
	//	m_vSegmentIndices.push_back( pInternalNode->mSkeletonId  );
	//}

	// Copy its transformation
	node_internal->_transform_local = node->mTransformation;
	compute_transform_global( node_internal );

	// Copy its offset
	node_internal->_offset_local = Vec3f( node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4 );
	compute_offset( node_internal );

	// Find the index of the animation track affecting this node, if any
	if( _animation_index_cur < _scene->mNumAnimations ) 
	{
		node_internal->_channel_index = -1;

		bool b_found = false;
		for (uint32_t a = 0; a < _animation_cur->mNumChannels && !b_found; a++) 
		{
			if( node_internal->is_name_same( _animation_cur->mChannels[a]->mNodeName.data ) ) 
			{
				node_internal->_channel_index = a;
				b_found = true;
			}
		}
	}

	// Continue for all child nodes and assign the created internal nodes as our children
	for( uint32_t a = 0; a < node->mNumChildren; a++ ) 
	{
		mocap::c_animation_node * childNode = create_node_tree( id, node->mChildren[a], node_internal );
		node_internal->_children.push_back( childNode );
	}

	return node_internal;
	// End : Previous
}



//=================================================================================================
//Calculates the global transformation matrix for the given internal node.
void mocap::c_animator::compute_transform_global( mocap::c_animation_node * node_internal ) 
{
	//concatenate all parent transforms to get the global transform for this node
	node_internal->_transform_world = node_internal->_transform_local;
	mocap::c_animation_node* parent = node_internal->_parent;
	while (parent)
	{
		node_internal->_transform_world = parent->_transform_local * node_internal->_transform_world;
		parent = parent->_parent;
	}
}

//=================================================================================================
//Calculates the offset for the given internal node.
void mocap::c_animator::compute_offset( mocap::c_animation_node * node_internal )
{
	node_internal->_offset_global = node_internal->_offset_local;
	mocap::c_animation_node* parent = node_internal->_parent;
	while (parent)
	{
		node_internal->_offset_global += parent->_offset_local;
		parent = parent->_parent;
	}
}



//=================================================================================================
//Calculates the bone matrices for the given mesh. 
const std::vector<aiMatrix4x4>& mocap::c_animator::get_bone_matrices( const aiNode* node, uint32_t node_mesh_index ) 
{
	if( node_mesh_index < node->mNumMeshes ) 
	{
		uint32_t uMeshIndex = node->mMeshes[node_mesh_index];

		if( _scene && (uMeshIndex < _scene->mNumMeshes) ) 
		{
			_mesh = _scene->mMeshes[uMeshIndex];

			//maa	sert a rien we initialize after
			//for( uint32_t i = 0; i < _transforms.size(); i++ )
			//{ 
			//	_transforms[i] = aiMatrix4x4();
			//}

			// Resize array and initialize it with identity matrices
			//maa	sert a rien we initialize after
			//		_transforms.resize( _mesh->mNumBones, aiMatrix4x4() );
			_transforms_bone.resize( _mesh->mNumBones );

			
			//// Calculate the mesh's inverse global transform -> do not use it to stay in bones space use it to apply to mesh objects
			//aiMatrix4x4 mGlobalInverseMeshTransform = GetGlobalTransform(pNode);
			//mGlobalInverseMeshTransform.Inverse(); 


			// Bone matrices transform from mesh coordinates in bind pose to mesh coordinates in skinned pose
			// Therefore the formula is offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform
			for( uint32_t i = 0; i < _mesh->mNumBones; i++ ) 
			{
				const aiBone* bone = _mesh->mBones[i];
				const aiMatrix4x4& transform_global_cur = get_transform_global_cur( _bone_node_by_name[bone->mName.data] );

				// Do not apply inverse transform to stay in bones space -> use it ta apply to mesh objects
				//m_vTransforms[i] = /*mGlobalInverseMeshTransform **/ mCurrentGlobalTransform * pBone->mOffsetMatrix;

				_transforms_bone[i] = transform_global_cur * bone->mOffsetMatrix;
			}
		}
	}

	//and return the result
	return _transforms_bone;
}



//=================================================================================================
// Retrieves the most recent global transformation matrix for the given node. 
const aiMatrix4x4 & mocap::c_animator::get_transform_global_cur( const aiNode * node ) const 
{
	std::map<const aiNode*, mocap::c_animation_node*>::const_iterator it = _nodes_by_name.find(node);

	if( it == _nodes_by_name.end() )
	{
		return IdentityMatrix;
	}

	return it->second->_transform_world;
}



//=================================================================================================
// Calculates the node transformations for the scene. 
void mocap::c_animator::update_animation( long time_elapsed, double ticks_per_second ) 
{
	if( (_animation_cur) && (_animation_cur->mDuration > 0.0) )
	{
		double time = ((double) time_elapsed) / 1000.0;

		// Calculate current local transformations
		// Extract ticks per second. Assume default value if not given
		double ticks_per_second_corrected = ticks_per_second; // Must be !=0.0

		// Map into animation's duration
		double time_in_ticks = fmod(time * ticks_per_second_corrected, _animation_cur->mDuration);

		_transforms_channel.resize( _animation_cur->mNumChannels );

		//calculate the transformations for each animation channel
		for( uint32_t i = 0; i < _animation_cur->mNumChannels; i++ ) 
		{
			const aiNodeAnim* channel = _animation_cur->mChannels[i];

			//******** Position *****
			aiVector3D pos( 0.0f, 0.0f, 0.0f );

			if( channel->mNumPositionKeys > 0 ) 
			{
				//Look for present frame number. Search from last position if time is after the last time, else from beginning
				//Should be much quicker than always looking from start for the average use case.
				uint32_t frame = (time_in_ticks >= _time_last) ? _position_frame_last[i].x : 0;

				while( frame < channel->mNumPositionKeys - 1 ) 
				{
					if( time_in_ticks < channel->mPositionKeys[frame + 1].mTime )
					{
						break;
					}
					++frame;
				}

				//interpolate between this frame's value and next frame's value
				uint32_t frame_next			= (frame + 1) % channel->mNumPositionKeys;
				const aiVectorKey& key		= channel->mPositionKeys[frame];
				const aiVectorKey& key_next = channel->mPositionKeys[frame_next];
				double time_diff			= key_next.mTime - key.mTime;

				if( time_diff < 0. )
				{
					time_diff += _animation_cur->mDuration;
				}

				if( time_diff > 0. ) 
				{
					float interpolator_factor = (float) ((time_in_ticks - key.mTime) / time_diff);
					pos = key.mValue + (key_next.mValue - key.mValue) * interpolator_factor;
				}
				else
				{
					pos = key.mValue;
				}

				_position_frame_last[i].x = frame;
			}


			//******** Rotation *********
			aiQuaternion rot( 1.0f, 0.0f, 0.0f, 0.0f );

			if( channel->mNumRotationKeys > 0 ) 
			{
				uint32_t frame = (time_in_ticks >= _time_last) ? _position_frame_last[i].y : 0;

				while( frame < channel->mNumRotationKeys - 1 ) 
				{
					if( time_in_ticks < channel->mRotationKeys[frame + 1].mTime)
					{
						break;
					}
					++frame;
				}

				//interpolate between this frame's value and next frame's value
				uint32_t frame_next			= (frame + 1) % channel->mNumRotationKeys;
				const aiQuatKey& key		= channel->mRotationKeys[ frame ];
				const aiQuatKey& key_next	= channel->mRotationKeys[ frame_next ];
				double time_diff			= key_next.mTime - key.mTime;

				if( time_diff < 0.0 )
				{
					time_diff += _animation_cur->mDuration;
				}

				if( time_diff > 0 ) 
				{
					float interpolator_factor = (float) ((time_in_ticks - key.mTime) / time_diff);
					aiQuaternion::Interpolate(rot, key.mValue, key_next.mValue, interpolator_factor);
				}
				else
				{
					rot = key.mValue;
				}

				_position_frame_last[i].y = frame;
			}


			//******** Scaling **********
			aiVector3D sca;
			bool b_sca = false;

			if( channel->mNumScalingKeys > 0 ) 
			{
				uint32_t frame = (time_in_ticks >= _time_last) ? _position_frame_last[i].z : 0;

				while( frame < channel->mNumScalingKeys - 1 ) 
				{
					if( time_in_ticks < channel->mScalingKeys[frame + 1].mTime )
						break;
					++frame;
				}

				sca = channel->mScalingKeys[frame].mValue;
				b_sca = ( sca.x != 1. ) || ( sca.y != 1. ) || ( sca.z != 1. ) ;
				_position_frame_last[i].z = frame;
			}

			//build a transformation matrix from it
			aiMatrix4x4 & matrix = _transforms_channel[i];

			// Grab rotation matrix from quaternion
			matrix = aiMatrix4x4( rot.GetMatrix() );

			// Apply scaling
			if( b_sca )
			{
				matrix.a1 *= sca.x;	matrix.b1 *= sca.x;	matrix.c1 *= sca.x;
				matrix.a2 *= sca.y;	matrix.b2 *= sca.y;	matrix.c2 *= sca.y;
				matrix.a3 *= sca.z;	matrix.b3 *= sca.z;	matrix.c3 *= sca.z;
			}
			// Apply translation
			matrix.a4 = pos.x;	matrix.b4 = pos.y;	matrix.c4 = pos.z;
		}

		_time_last = time_in_ticks;

		// and update all node transformations with the results
		update_transforms( _node_root, _transforms_channel );
	}
}

//=================================================================================================
//Calculates the node transformations for the scene at given frame.
void mocap::c_animator::update_animation( uint32_t frame_in )
{
	if( (_animation_cur) && (_animation_cur->mDuration > 0.0) ) 
	{
		//if( _transforms_channel.size() != _animation_cur->mNumChannels )
		//{
			_transforms_channel.resize(_animation_cur->mNumChannels);
		//}

		//calculate the transformations for each animation channel
		for( uint32_t i = 0; i < _animation_cur->mNumChannels; i++ ) 
		{
			const aiNodeAnim* channel = _animation_cur->mChannels[i];

			//******** Position *****
			aiVector3D pos(0.0f, 0.0f, 0.0f);

			if( channel->mNumPositionKeys > 0 ) 
			{
				uint32_t frame = (frame_in < channel->mNumPositionKeys) ? frame_in : _position_frame_last[i].x;
				//	this commented code is unused
				// Interpolate between this frame's value and next frame's value
				//uint32_t frame_next			= (frame + 1) % channel->mNumPositionKeys;
				const aiVectorKey& key		= channel->mPositionKeys[ frame ];
				//const aiVectorKey& key_next = channel->mPositionKeys[ frame_next ];	//todo unused ?
				//double time_diff			= key_next.mTime - key.mTime;	//todo unused ?

				//if( time_diff < 0.0 )
				//{
				//	time_diff += _animation_cur->mDuration;
				//}

				pos = key.mValue;
				_position_frame_last[i].x = frame;
			}

			//******** Rotation *********
			aiQuaternion rot(1.0f, 0.0f, 0.0f, 0.0f);

			if( channel->mNumRotationKeys > 0 ) 
			{
				uint32_t frame = (frame_in < channel->mNumRotationKeys) ? frame_in : _position_frame_last[i].y;
				//	this commented code is unused
				// Interpolate between this frame's value and next frame's value
				//uint32_t frame_next			= (frame + 1) % channel->mNumRotationKeys;
				const aiQuatKey& key		= channel->mRotationKeys[ frame ];
				//const aiQuatKey& key_next	= channel->mRotationKeys[ frame_next ];//todo unused ?
				//double time_diff			= key_next.mTime - key.mTime;	//todo unused ?

				//if( time_diff < 0.0 )
				//{
				//	time_diff += _animation_cur->mDuration;
				//}

				rot = key.mValue;
				_position_frame_last[i].y = frame;
			}

			//******** Scaling **********
			aiVector3D sca;
			bool b_sca = false;

			if( channel->mNumScalingKeys > 0 ) 
			{
				uint32_t frame = (frame_in < channel->mNumScalingKeys) ? frame_in : _position_frame_last[i].z;

				sca = channel->mScalingKeys[ frame ].mValue;
				b_sca = ( sca.x != 1. ) || ( sca.y != 1. ) || ( sca.z != 1. ) ;
				_position_frame_last[i].z = frame;
			}

			//build a transformation matrix from it
			aiMatrix4x4 & matrix = _transforms_channel[i];

			// Grab rotation matrix from quaternion
			matrix = aiMatrix4x4( rot.GetMatrix() );

			// Apply scaling
			if( b_sca )
			{
				matrix.a1 *= sca.x;	matrix.b1 *= sca.x;	matrix.c1 *= sca.x;
				matrix.a2 *= sca.y;	matrix.b2 *= sca.y;	matrix.c2 *= sca.y;
				matrix.a3 *= sca.z;	matrix.b3 *= sca.z;	matrix.c3 *= sca.z;
			}

			// Apply translation
			matrix.a4 = pos.x;	matrix.b4 = pos.y;	matrix.c4 = pos.z;
		}

		//maa was strange corrected but still look weird
#if 1
		double time_elapsed = double(frame_in) * (1.0/_animation_cur->mTicksPerSecond);
#else
		long time_elapsed = long( double(frame_in) * (1.0/_animation_cur->mTicksPerSecond) );
#endif
		double time_in_ticks = fmod(time_elapsed * _animation_cur->mTicksPerSecond, _animation_cur->mDuration);
		_time_last = time_in_ticks;

		// and update all node transformations with the results
		update_transforms( _node_root, _transforms_channel );
	}
}



//=================================================================================================
//Recursively updates the internal node transformations from the given matrix array
void mocap::c_animator::update_transforms( mocap::c_animation_node* node, const std::vector<aiMatrix4x4>& transforms )
{
	//update node local transform
	if( node->_channel_index != -1 ) 
	{
		if( node->_channel_index >= transforms.size() )
		{
			return;
		}

		node->_transform_local = transforms[ node->_channel_index ];
	}

	// Concatenate all parent transforms to get the global transform for this node
	compute_transform_global( node );

	// Recursively continue for all children
	for( auto const & elt : node->_children )
	{
		update_transforms( elt, transforms );
	}

}

#endif	//#if AAA_LIB_USE_ASSIMP()