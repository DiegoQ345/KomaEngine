#pragma once

#include <glm/mat4x4.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include <cstddef>
#include <glm/mat4x4.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace KomaEngine
{
    inline constexpr std::size_t kInvalidModelNodeIndex = static_cast<std::size_t>(-1);

    struct Bone
    {
        std::string name;
        /// Stable index into the flattened/visited ModelNode list for the imported hierarchy (set during traversal).
        /// kInvalidModelNodeIndex if no associated ModelNode could be resolved.
        std::size_t modelNodeIndex = kInvalidModelNodeIndex;
        /// Index of parent bone in Skeleton::bones; -1 if none (or parent is not a bone node).
        int parentBoneIndex = -1;
        /// Local transform of the corresponding aiNode (bind), same convention as ModelNode::LocalTransform (ToGLM).
        glm::mat4 localBindTransform{1.0f};
    };

    /// Per-mesh skinning binding data (no vertex weights stored in this task).
    struct BoneBinding
    {
        /// Index into Skeleton::bones.
        std::size_t skeletonBoneIndex = 0;
        /// aiMesh::mBones[i]->mOffsetMatrix converted via the same ToGLM as other matrices.
        /// Assimp offset (inverse bind in the usual skinning convention): transforms from mesh/vertex space
        /// to the bone space for that mesh context; must be paired with the corresponding mesh binding.
        glm::mat4 inverseBindMatrix{1.0f};
    };

    /// Single bone influence on a mesh vertex (imported from aiBone::mWeights).
    struct SkinVertexInfluence
    {
        /// Index into Skeleton::bones (global for the model skeleton).
        std::size_t boneIndex = 0;
        float weight = 0.0f;
    };

    struct Skeleton
    {
        std::vector<Bone> bones;
        std::unordered_map<std::string, std::size_t> nameToIndex;
    };
}
