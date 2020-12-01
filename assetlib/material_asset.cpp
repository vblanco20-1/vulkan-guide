
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

	for (auto& [key, value] : texture_metadata["customProperties"].items())
	{

		info.customProperties[key] = value;
	}

	info.transparency = TransparencyMode::Opaque;

	auto it = texture_metadata.find("transparency");
	if (it != texture_metadata.end())
	{
		std::string val = (*it);
		if (val.compare("transparent") == 0) {
			info.transparency == TransparencyMode::Transparent;
		}
		if (val.compare("masked") == 0) {
			info.transparency == TransparencyMode::Masked;
		}
	}

	return info;
}

assets::AssetFile assets::pack_material(MaterialInfo* info)
{
	nlohmann::json texture_metadata;
	texture_metadata["baseEffect"] = info->baseEffect;
	texture_metadata["textures"] = info->textures;
	texture_metadata["customProperties"] = info->customProperties;

	switch (info->transparency)
	{	
	case TransparencyMode::Transparent:
		texture_metadata["transparency"] = "transparent";
		break;
	case TransparencyMode::Masked:
		texture_metadata["transparency"] = "masked";
		break;
	}

	

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
