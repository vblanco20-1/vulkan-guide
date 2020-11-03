
#include "json.hpp"
#include "lz4.h"
#include <material_asset.h>
assets::MaterialInfo assets::read_material_info(AssetFile* file)
{
	assets::MaterialInfo info;

	nlohmann::json texture_metadata = nlohmann::json::parse(file->json);
	info.baseEffect = texture_metadata["baseEffect"];


	for (auto& [key, value] : texture_metadata["textures"].items())
	{

		info.textures[key] = value;
	}

	return info;
}

assets::AssetFile assets::pack_material(MaterialInfo* info)
{
	nlohmann::json texture_metadata;
	texture_metadata["baseEffect"] = info->baseEffect;
	texture_metadata["textures"] = info->textures;
	

	//core file header
	AssetFile file;
	file.type[0] = 'M';
	file.type[1] = 'A';
	file.type[2] = 'T';
	file.type[3] = 'X';
	file.version = 1;


	std::string stringified = texture_metadata.dump();
	file.json = stringified;


	return file;
}
