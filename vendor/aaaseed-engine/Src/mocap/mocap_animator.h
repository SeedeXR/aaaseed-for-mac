
#ifdef AAA_MOCAP_ANIMATOR_H
#error "MOCAP_ANIMATOR_H included more than once."
#endif
#define AAA_MOCAP_ANIMATOR_H 1


#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _STRING_
#	include <string>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _MAP_
#	include <map>
#endif

#ifndef AI_VECTOR3D_H_INC
#	include "assimp\vector3.h"		//	assimp should do it in matrix4x4.h
#endif
#ifndef AI_MATRIX4X4_H_INC
#	include "assimp\matrix4x4.h"
#endif

#ifndef AAA_TVECTOR_H
#	include "math/TVector.h"
#endif


#define MAXBONESPERMESH 128

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiAnimation;

namespace mocap
{
	//---------------------------------------------------------------------------------
	// Tree class to match the scene's node structure, but holding additional data.
	class c_animation_node 
	{	
		std::string						_name;
	public:
		c_animation_node *				_parent;
		std::vector<c_animation_node*> _children;

		//most recently calculated local transform
		aiMatrix4x4						_transform_local; 
		//same, but in world space
		aiMatrix4x4						_transform_world;

		// Most recently calculated offset
		Vec3f							_offset_global;
		Vec3f							_offset_local;

		// Index in the current animation's channel array.
		uint32_t						_channel_index;  

		// Indices in the current skeleton
		int32_t							_skeleton_id;
			

	public:
		// Construction from a given name
		c_animation_node( const std::string & p_name="" ) 
			: _name				( p_name )  
			, _channel_index	( -1 )
			, _parent			( nullptr )
			, _offset_global	( Vec3f::zero() )
			, _offset_local		( Vec3f::zero() )
			, _skeleton_id		( -1 )
		{}

		C_PCHAR_C	get_name()								{	return _name.data();	}
		bool		is_name_same( const std::string& str )	{	return str == _name;	}

		~c_animation_node( void ) 
		{
			for( std::vector<c_animation_node *>::iterator it = _children.begin(); it != _children.end(); it++ ) 
			{
				delete *it;
				*it = nullptr;
			}
			_children.clear();
		}
	};

	//---------------------------------------------------------------------------------
	/** Calculates the animated node transformations for a given scene and timestamp. 
	*
	*  Create an instance for a aiScene you want to animate and set the current animation 
	*  to play. You can then have the instance calculate the current pose for all nodes 
	*  by calling Calculate() for a given timestamp. After this you can retrieve the 
	*  present transformation for a given node by calling GetLocalTransform() or
	*  GetGlobalTransform().
	*/
	class c_animator
	{

	public:
		/** Name to node map to quickly find nodes by their name */
		std::map<const aiNode*, mocap::c_animation_node*> _nodes_by_name;

		/** Name to node map to quickly find nodes for given bones by their name */
		std::map<const char*, const aiNode*> _bone_node_by_name;

		/** At which frame the last evaluation happened for each channel. 
		* Useful to quickly find the corresponding frame for slightly increased time stamps
		*/
		double						_time_last;
		Vec3ui*						_position_frame_last;

		/** Array to return transformations results inside. */
		std::vector<aiMatrix4x4>	_transforms_channel;
		std::vector<aiMatrix4x4>	_transforms_bone;

		/** Segments indices */
		int32_t						_id_cur;
		int32_t _id_parent;
		std::vector<int32_t>		_segment_ids;


		const aiScene *				_scene;
		const aiMesh *				_mesh;
		mocap::c_animation_node *	_node_root;
		uint32_t					_animation_index_cur;
		uint32_t					_animation_index_first;
		const aiAnimation *			_animation_cur;

	protected:
		//----------------------------------------------------------------------------
		/** Constructor for a given scene.
		*
		* The object keeps a reference to the scene during its lifetime, but 
		* ownership stays at the caller. 
		* @param scene The scene to animate.
		* @param anim_index Index of the animation to play.
		*/
		c_animator(const aiScene * scene, uint32_t anim_index);


		//----------------------------------------------------------------------------
		/** Allocate and init class members. */
		void init( void );

	public:
		//----------------------------------------------------------------------------
		/** Create a new Animator pointer based on ASSIMP scene \a scene and animation index \a anim_index.	*/
		static c_animator * create_ptr( const aiScene * scene, uint32_t anim_index );


		//----------------------------------------------------------------------------
		/** Class destructor */
		virtual ~c_animator( void );


		//----------------------------------------------------------------------------
		/** Sets the animation to use for playback. This also recreates the internal 
		* mapping structures, which might take a few cycles.
		* @param anim_index Index of the animation in the scene's animation array 
		*/
		void set_animation_index(uint32_t anim_index );


		//----------------------------------------------------------------------------
		/** Calculates the node transformations for the scene. Call this to get 
		* up-to-date results before calling one of the getters.
		* Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
		* array of transformation matrices afterwards by calling GetTransformations().
		* @param time_elapsed Elapsed time since animation start in ms.
		*/
		void update_animation(long time_elapsed, double ticks_per_second );


		//----------------------------------------------------------------------------
		/** Calculates the node transformations for the scene. Call this to get 
		* up-to-date results before calling one of the getters.
		* Evaluates the animation tracks for a given frame. The calculated pose can be retrieved as a
		* array of transformation matrices afterwards by calling GetTransformations().
		* @param frame target animation frame.
		*/
		void update_animation( uint32_t frame );


		//----------------------------------------------------------------------------
		/** Calculates the bone matrices for the given mesh.
		*
		* Each bone matrix transforms from mesh space in bind pose to mesh space in 
		* skinned pose, it does not contain the mesh's world matrix. Thus the usual
		* matrix chain for using in the vertex shader is
		* @code
		* projMatrix * viewMatrix * worldMatrix * boneMatrix
		* @endcode
		* @param pNode The node carrying the mesh.
		* @param pMeshIndex Index of the mesh in the node's mesh array. The NODE's
		*   mesh array, not  the scene's mesh array! Leave out to use the first mesh
		*   of the node, which is usually also the only one.
		* @return A reference to a vector of bone matrices. Stays stable till the
		*   next call to GetBoneMatrices();
		*/
		const std::vector<aiMatrix4x4>& get_bone_matrices(const aiNode * node, uint32_t mesh_index = 0);


		// ----------------------------------------------------------------------------
		/** Retrieves the most recent global transformation matrix for the given node.
		*
		* The returned matrix is in world space, which is the same coordinate space 
		* as the transformation of the scene's root node. If the node is not animated, 
		* the node's original transformation is returned so that you can safely use or 
		* assign it to the node itsself. If there is no node with the given name, the 
		* identity matrix is returned. All transformations are updated whenever 
		* Calculate() is called.
		* @param pNodeName Name of the node
		* @return A reference to the node's most recently calculated global 
		*   transformation matrix.
		*/
		const aiMatrix4x4 & get_transform_global_cur(const aiNode * node) const;


	protected:
		//----------------------------------------------------------------------------
		/** Recursively creates an internal node structure matching the 
		*  current scene and animation. 
		*/
		mocap::c_animation_node * create_node_tree( INT32 & id, aiNode * node, mocap::c_animation_node * parent );


		//----------------------------------------------------------------------------
		/** Calculates the global transformation matrix for the given internal node */
		void compute_transform_global( mocap::c_animation_node * node_internal );


		//----------------------------------------------------------------------------
		/** Calculates the offset for the given internal node */
		void compute_offset( mocap::c_animation_node * node_internal );


		//----------------------------------------------------------------------------
		/** Recursively updates the internal node transformations from the given matrix array. */
		void update_transforms( mocap::c_animation_node * mode, const std::vector<aiMatrix4x4> & transforms );
	};

} // namespace mocap

