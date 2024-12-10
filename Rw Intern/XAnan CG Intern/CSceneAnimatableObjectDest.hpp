#pragma once
#include "matrix.hpp"
#include "color.hpp"
#include "CMaterial2.hpp"
#include "UtlStrongHandle.h"
class CObjectInfo {
public:
    char pad_01[0xB0];
    int nId;
};

#pragma pack(push, 1)
class CSceneObject {
public:
    void* vftable;
    void* heapPtr;
    void* m_pMeshInstanceData;
    void* m_pDesc;
    void* m_pRefData;
private:
    char pad_1[0x8];
public:
    Matrix3x4 m_transform;
private:
    char pad_2[0x26];
public:
    uint16_t m_nObjectTypeFlags;
    uint16_t m_nGameRenderCounter;
    uint8_t m_clr[3];
private:
    char pad_3[0x3];
public:
    void* m_pPVS;
    void* m_pExtraData;
private:
    char pad_4[0x10];
public:
    void* m_pWorld;
    uint32_t m_hEntity;
private:
    char pad_5[0x14];
};
#pragma pack(pop)

class CSceneAnimatableObject : public CSceneObject {
public:
    CUtlVector<Matrix3x4> m_worldSpaceRenderBones;
    CUtlVector<float> m_flexControllerWeights;
    int m_nPerVertexBoneInfluenceCount;
private:
    char pad_6[0xC];
public:
    void* m_pProceduralBoneTransforms;
};

class CMeshData {
public:
    char pad_01[0x18];
    CSceneAnimatableObject* m_pSceneAnimatableObject;
    CMaterial2* pMaterial;
    char pad_02[0x28];
    Color Colors;
    char pad_03[0x4];
    CObjectInfo* ObjectInfo;
    void SetColor(Color NewColor) {
        Colors.Override(NewColor);
    }

    void ForceOverrideMaterial(CStrongHandle<CMaterial2>& NewMaterial) {
        if (!NewMaterial.IsValid()) {
            return;
        }

        pMaterial = NewMaterial;
    }
};