#include "mesh_loader.h"

// At the moment we only support static meshes
// Therefore...
//	WaveFront OBJ
//	Manta MDL
std::vector<std::string> MeshLoader::get_extensions() {
    return {"obj", "mmdl"};
}

Asset *MeshLoader::load_from_path(const std::string &path) {
    
}