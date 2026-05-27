
// Main header
#include "mocap/mocap_format_bvh.h"

#if AAA_LIB_USE_ASSIMP()
// External header
#ifndef _INC_STDIO
#	include <stdio.h>
#endif
#ifndef _INC_STRING
#	include <string.h>
#endif
#ifndef _INC_MATH
#	include <math.h>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

// Mocap header
#ifndef AAA_MOCAP_ANIMATOR_H
#	include "mocap_animator.h"
#endif
// Assimp headers
#ifndef INCLUDED_AI_ASSIMP_HPP
#	include <assimp/Importer.hpp>
#endif
#ifndef __AI_SCENE_H_INC__
#	include <assimp/scene.h>
#endif

// AAA header
#ifndef AAA_AAA_MEM_H
#	include "aaa_mem.h"
#endif
// Math header
#ifndef	AAA_MATRIX_H
#	include "math/matrix.h"
#endif
#ifndef AAA_TVECTOR_H
#	include "math/TVector.h"
#endif
#ifndef	AAA_V_BASE_H
#	include "math/v_base.h"
#endif


#	include <lib_use.h>
#	if AAA_WIN64()
#		if AAA_DEBUG()
			AAA_LIB_USE64("assimp-vc140-mtD")
#		else
			AAA_LIB_USE64("assimp-vc140-mt")
#		endif
#	else
#		if AAA_DEBUG()
			AAA_LIB_USE32( "assimpD" )
#		else
			AAA_LIB_USE32( "assimp" )
#		endif
#	endif	//#if AAA_WIN64()


///////////////////////////////////////////////////////////////////////////////////////////////////
//		mocap::BVHFormat class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
mocap::c_bvh_data * mocap::c_bvh_data::create_method( void )
{
	mocap::c_bvh_data * p = new mocap::c_bvh_data();

	p->init_members();

	return p;
}

//=================================================================================================
mocap::c_bvh_data::c_bvh_data( void )
	// Members init
	: _b_file_loaded	( false )
						
	, _bones_nb			( 0 )
	, _frame_nb			( nullptr )
	, _frame_by_sec		( nullptr )
	, _anim_nb			( 0 )
						
//	, _importer			( nullptr )
	, _scene			( nullptr )
	, _animator			( nullptr )
	, _bones_weight		( nullptr )
{}

//=================================================================================================
mocap::c_bvh_data::~c_bvh_data( void )
{
	dealloc();
}

//=================================================================================================
void mocap::c_bvh_data::init_members( void )
{
	//// Assimp base importer
	//m_pImporter = new Assimp::Importer();
}

//=================================================================================================
void mocap::c_bvh_data::dealloc( void )
{
	// Frames amount
	IF_FREE_AND_NULL( _frame_nb );
	// Data rate
	IF_FREE_AND_NULL( _frame_by_sec );
	// Bones weights
	IF_FREE_AND_NULL( _bones_weight );
	// Animator
	SAFE_DELETE( _animator );

	_scene         = nullptr;	//todo check if we need to release
//	// Assimp importer
//	SAFE_DELETE( _importer );
	// Misc
	_anim_nb       = 0;
	_bones_nb      = 0;
	_b_file_loaded = false;
}

//=================================================================================================
bool mocap::c_bvh_data::import_from_assimp( const std::string & file_path )
{
	// Return value
	bool b_return = true;

	// Test dealloc needed
	if( _b_file_loaded )
		dealloc();

	// Assimp base importer
	Assimp::Importer* importer = new Assimp::Importer();
	// Set verbose mode
//#if AAA_DEBUG()
	//todo put under control of a param
	importer->SetExtraVerbose( false );
//#endif

	// Load Scene
	_scene = importer->ReadFile( file_path, 0 );
	if( _scene )
	{
		// Test meshes amount (must be 1)
		if( (_scene->mNumMeshes > 1) && (_scene->mNumMeshes < 1) )
		{
			ERR_PRINT_STRING( "mocap::BVHFormat::importFromAssimp() BVH files contains %i skeletons", _scene->mNumMeshes );
			b_return = false;
		}

		if( b_return )
		{
			if( _scene->HasAnimations() ) 
			{
				// Create animator
				_animator = mocap::c_animator::create_ptr( _scene, 0 );

				// Grab basic datas
				aiNode * node = _scene->mRootNode;
				const aiMesh* mesh_cur = _scene->mMeshes[ node->mMeshes[0] ];

				// Stock bones amount
				_bones_nb = mesh_cur->mNumBones;

				// Bones weights alloc
				_bones_weight = (REAL*) MALLOC( sizeof(REAL) * _bones_nb );
				// Retrieve bones original positions
				const aiBone * bone = nullptr;
				for( INT32 j=0; j<_bones_nb; j++ ) 
				{
					bone = mesh_cur->mBones[ j ];
					INT32 id = bone->mWeights[0].mVertexId;

					_bones_weight[ j ] = mesh_cur->mBones[ j ]->mWeights[0].mWeight;
				}

				// Retrieve animation data
				INT32			anim_nb		= _scene->mNumAnimations;
				aiAnimation**	anim_array	= _scene->mAnimations;

				// Basic datas
				_anim_nb		= anim_nb;
				_frame_nb		= (INT32*)	MALLOC( sizeof(size_t)*_anim_nb );
				_frame_by_sec	= (REAL*)	MALLOC( sizeof(REAL)*_anim_nb );

				// Run threw animations
				for( INT32 i=0; i<anim_nb; i++ ) 
				{
					aiAnimation* anim = anim_array[ i ];
					// Node animation
					aiNodeAnim** anim_node_array = anim->mChannels;
					//_anim_node_nb = anim->mNumChannels;	// if we use it we should deal with a different 

					_frame_nb[ i ]	= anim_node_array[ 0 ]->mNumPositionKeys;	// Be careful it appears it may not be the same value for each node
																				//hack is it why we use 0 instead of i as index ?
					_frame_by_sec[ i ]	= REAL( anim->mTicksPerSecond );
				}
			}
			// No animation found
			else
				b_return = false;
		}
	}
	// Scene failed to load
	else
	{
		ERR_PRINT_STRING( "mocap::BVHFormat::importFromAssimp() scene failed to load with error : %s", importer->GetErrorString() );
		b_return = false;
	}
	SAFE_DELETE( importer );
	return b_return;
}

//=================================================================================================
bool mocap::c_bvh_data::import_data( const char* file_path )
{
	_b_file_loaded = import_from_assimp( std::string(file_path) );
	return _b_file_loaded;
}

//=================================================================================================
AAA_ERR	mocap::c_bvh_data::retrieve_data( REAL* data, int32_t real_by_node, const INT32 & animation_index )
{
	AAA_ERR ret = ERR_ANY;

	if( _b_file_loaded )
	{
		INT32 anim_nb = _anim_nb;
		float	offset[3];	
		if( animation_index < anim_nb )
		{
			int32_t	frame_nb = get_frame_nb( animation_index );
			int32_t	node_nb	 = get_node_nb();
		
			// Processor like preinc -> Denver style
			REAL* dst = data - 1;	

			int32_t	stride = real_by_node - 3;

			// Performing frame walk through before nodes walk through is important !!! 
			// bdd_mocap needs datas in this order
			for( int32_t frame=0; frame<frame_nb; frame++ )
			{
				// Compute frame
				//long elapsedTime = frame * (1.0f/m_pDatarate[0]) * 1000;
				//m_pAnimator->UpdateAnimation( elapsedTime, m_pDatarate[0] );
				_animator->update_animation( frame );

				// Compute bones matrices
				const std::vector<aiMatrix4x4>& vBoneMatrices = _animator->get_bone_matrices( _scene->mRootNode, 0 );

				// Walk through nodes
				for( int32_t j=0; j<node_nb; j++ )
				{
					// Grab node offset
					aiBone * bone = _animator->_mesh->mBones[ j ];
					
					find_offset_by_name( std::string(bone->mName.data), frame, offset );

					// Grab bone matrix
					aiMatrix4x4 mat = vBoneMatrices[ j ];

					// Matrix order -> OK
					float x = mat.a1*offset[ 0 ] + mat.a2*offset[ 1 ] + mat.a3*offset[ 2 ] + mat.a4;
					float y = mat.b1*offset[ 0 ] + mat.b2*offset[ 1 ] + mat.b3*offset[ 2 ] + mat.b4;
					float z = mat.c1*offset[ 0 ] + mat.c2*offset[ 1 ] + mat.c3*offset[ 2 ] + mat.c4;
					float w = mat.d1*offset[ 0 ] + mat.d2*offset[ 1 ] + mat.d3*offset[ 2 ] + mat.d4;

					// Send datas
					*++dst = (x * _bones_weight[ j ]) / w ;
					*++dst = (y * _bones_weight[ j ]) / w ;
					*++dst = (z * _bones_weight[ j ]) / w ;

					dst += stride;
				}
			}

#	if AAA_DEBUG()
		//todo put under control of a param
		//	HEAP_IS_CORRUPT();
			ret = AAA_OK;
#	endif
		}
	}
	return ret;
}

//=================================================================================================
void mocap::c_bvh_data::find_offset_by_name( const std::string & name, int32_t frame, float * dst )
{
	clear_v3( dst );

	bool b_found = false;
	std::map<const aiNode*, c_animation_node*>::const_iterator it = _animator->_nodes_by_name.begin();

	for( ; it!=_animator->_nodes_by_name.end() && !b_found; it++ ) 
	{
		if( it->second->is_name_same( name ) )
		{
			Vec3f& off = it->second->_offset_global;
			set_v3( dst, off.x, off.y, off.z );
			b_found = true;
		}
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////
//=================================================================================================
INT32 mocap::c_bvh_data::get_segment_nb( void )
{
	return INT32( _animator->_segment_ids.size()/2 );
}

//=================================================================================================
//hack This is dirty, it should be done another way
INT32* mocap::c_bvh_data::get_segment_point_index( void )
{
	const INT32 nb = get_segment_nb() * 2;

	INT32* array = (INT32*) MALLOC( sizeof(INT32) * nb );
	for( INT32 i=0; i<nb; i++ )
		array[i] = _animator->_segment_ids[i];

	return array;
}
void mocap::c_bvh_data::free_segment_point_index( INT32* array )
{
	SAFE_DELETE( array );
}


//=================================================================================================
int32_t mocap::c_bvh_data::get_node_nb( void )
{
	return _b_file_loaded ? _bones_nb : 0 ;
}

#endif	//#if AAA_LIB_USE_ASSIMP()
