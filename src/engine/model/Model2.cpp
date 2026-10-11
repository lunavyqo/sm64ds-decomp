//cpp
/* Model material show/hide and polygon mode. Separate from Model.cpp:
   ModelBase::ApplyOpacity begins at 0x02016a9c, and func_02016aac (real C)
   walls this run off from the home TU at 0x02016b78. Written back-to-front:
   mwccarm emits .text in reverse source order under default deferred codegen. */
#include "Model.h"

/* local extern: no header declares it */
extern "C" void func_02046234(ModelComponents *data, int mode);

// @symbol _ZN5Model14SetPolygonModeEi
void Model::SetPolygonMode(int mode)
{
    func_02046234(&data, mode);
}

// @symbol _ZN5Model12HideMaterialEii
void Model::HideMaterial(int boneID, int listIdx)
{
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags |= 0x80000000;
}

// @symbol _ZN5Model12ShowMaterialEii
void Model::ShowMaterial(int boneID, int listIdx)
{
    BMD_File *file = data.modelFile;
    BMD_Material *mats = data.materials;
    u8 *ids = file->bones[boneID].materialIds;
    mats[ids[listIdx]].flags &= 0x7fffffff;
}
