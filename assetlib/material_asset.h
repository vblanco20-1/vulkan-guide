#pragma once
#include <asset_loader.h>


namespace assets {


	struct MaterialInfo {
		std::string baseEffect;
		std::unordered_map<std::string, std::string> textures; //name -> path
	};

	MaterialInfo read_material_info(AssetFile* file);

	AssetFile pack_material(MaterialInfo* info);
}