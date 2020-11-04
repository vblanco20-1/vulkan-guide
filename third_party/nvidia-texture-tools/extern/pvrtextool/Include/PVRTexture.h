/******************************************************************************

 @File         PVRTexture.h

 @Title        

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited. All Rights Reserved. Strictly Confidential.

 @Platform     

 @Description  

******************************************************************************/
#ifndef _PVRTEXTURE_H
#define _PVRTEXTURE_H

#include "PVRTextureDefines.h"
#include "PVRTextureHeader.h"
//ACS: removed unneccesary includes:
//#include "PVRTString.h"

namespace pvrtexture
{
	class PVR_DLL CPVRTexture : public CPVRTextureHeader
	{		
	public:
	/*******************************************************************************
	* Construction methods for a texture.
	*******************************************************************************/
		/*!***********************************************************************
		 @Function		CPVRTexture
		 @Return		CPVRTexture A new texture.
		 @Description	Creates a new empty texture
		*************************************************************************/
		CPVRTexture();
		/*!***********************************************************************
		 @Function		CPVRTexture
		 @Input			sHeader
		 @Input			pData
		 @Return		CPVRTexture A new texture.
		 @Description	Creates a new texture based on a texture header, 
						pre-allocating the correct amount of memory. If data is
						supplied, it will be copied into memory.
		*************************************************************************/
		CPVRTexture(const CPVRTextureHeader& sHeader, const void* pData=NULL);

		/*!***********************************************************************
		 @Function		CPVRTexture
		 @Input			szFilePath
		 @Return		CPVRTexture A new texture.
		 @Description	Creates a new texture from a filepath.
		*************************************************************************/
		CPVRTexture(const char* szFilePath);

		/*!***********************************************************************
		 @Function		CPVRTexture
		 @Input			pTexture
		 @Return		CPVRTexture A new texture.
		 @Description	Creates a new texture from a pointer that includes a header
						structure, meta data and texture data as laid out in a file.
						This functionality is primarily for user defined file loading.
						Header may be any version of pvr.
		*************************************************************************/
		CPVRTexture( const void* pTexture );

		/*!***********************************************************************
		 @Function		CPVRTexture
		 @Input			texture
		 @Return		CPVRTexture A new texture
		 @Description	Creates a new texture as a copy of another.
		*************************************************************************/
		CPVRTexture(const CPVRTexture& texture);

		/*!***********************************************************************
		 @Function		~CPVRTexture
		 @Description	Deconstructor for CPVRTextures.
		*************************************************************************/
		~CPVRTexture();

		/*!***********************************************************************
		 @Function		operator=
		 @Input			rhs
		 @Return		CPVRTexture& This texture.
		 @Description	Will copy the contents and information of another texture into this one.
		*************************************************************************/
		CPVRTexture& operator=(const CPVRTexture& rhs);
		
	/*******************************************************************************
	* Texture accessor functions - others are inherited from CPVRTextureHeader.
	*******************************************************************************/
		/*!***********************************************************************
		 @Function		getDataPtr
		 @Input			uiMIPLevel
		 @Input			uiArrayMember
		 @Input			uiFaceNumber
		 @Return		void* Pointer to a location in the texture.
		 @Description	Returns a pointer into the texture's data. 
						It is possible to specify an offset to specific array members, 
						faces and MIP Map levels.
		*************************************************************************/
		void* getDataPtr(uint32 uiMIPLevel = 0, uint32 uiArrayMember = 0, uint32 uiFaceNumber = 0) const;

		/*!***********************************************************************
		 @Function		getHeader
		 @Return		const CPVRTextureHeader& Returns the header only for this texture.
		 @Description	Gets the header for this texture, allowing you to create a new
						texture based on this one with some changes. Useful for passing
						information about a texture without passing all of its data.
		*************************************************************************/
		const CPVRTextureHeader& getHeader() const;

	/*******************************************************************************
	* File io.
	*******************************************************************************/

		/*!***********************************************************************
		 @Function		setPaddedMetaData
		 @Input			uiPadding
		 @Description	When writing the texture out to a PVR file, it is often
						desirable to pad the meta data so that the start of the
						texture data aligns to a given boundary.
						This function pads to a boundary value equal to "uiPadding".
						For example setting uiPadding=8 will align the start of the
						texture data to an 8 byte boundary.
						Note - this should be called immediately before saving as
						the value is worked out based on the current meta data size.
		*************************************************************************/
		void addPaddingMetaData( uint32 uiPadding );

		/*!***********************************************************************
		 @Function		saveFile
		 @Input			filepath
		 @Return		bool Whether the method succeeds or not.
		 @Description	Writes out to a file, given a filename and path. 
						File type will be determined by the extension present in the string. 
						If no extension is present, PVR format will be selected. 
						Unsupported formats will result in failure.
		*************************************************************************/
		bool saveFile(const CPVRTString& filepath) const;
	
		/*!***********************************************************************
		 @Function		saveFileLegacyPVR
		 @Input			filepath
		 @Input			eApi
		 @Return		bool Whether the method succeeds or not.
		 @Description	Writes out to a file, stripping any extensions specified
						and appending .pvr. This function is for legacy support only
						and saves out to PVR Version 2 file. The target api must be
						specified in order to save to this format.
		*************************************************************************/
		bool saveFileLegacyPVR(const CPVRTString& filepath, ELegacyApi eApi) const;

	private:
		size_t	m_stDataSize;		// Size of the texture data.
		uint8*	m_pTextureData;		// Pointer to texture data.

	/*******************************************************************************
	* Private IO functions
	*******************************************************************************/
		/*!***********************************************************************
		 @Function		loadPVRFile
		 @Input			pTextureFile
		 @Description	Loads a PVR file.
		*************************************************************************/
		bool privateLoadPVRFile(FILE* pTextureFile);

		/*!***********************************************************************
		 @Function		privateSavePVRFile
		 @Input			pTextureFile
		 @Description	Saves a PVR File.
		*************************************************************************/
		bool privateSavePVRFile(FILE* pTextureFile) const;

		/*!***********************************************************************
		 @Function		loadKTXFile
		 @Input			pTextureFile
		 @Description	Loads a KTX file.
		*************************************************************************/
		bool privateLoadKTXFile(FILE* pTextureFile);

		/*!***********************************************************************
		 @Function		privateSaveKTXFile
		 @Input			pTextureFile
		 @Description	Saves a KTX File.
		*************************************************************************/
		bool privateSaveKTXFile(FILE* pTextureFile) const;

		/*!***********************************************************************
		 @Function		loadDDSFile
		 @Input			pTextureFile
		 @Description	Loads a DDS file.
		 *************************************************************************/
		bool privateLoadDDSFile(FILE* pTextureFile);
		bool privateLoadDDS10File(FILE* pTextureFile);

		/*!***********************************************************************
		 @Function		privateSaveDDSFile
		 @Input			pTextureFile
		 @Description	Saves a DDS File.
		*************************************************************************/
		bool privateSaveDDSFile(FILE* pTextureFile) const;

		//Legacy IO
		/*!***********************************************************************
		 @Function		privateSavePVRFile
		 @Input			pTextureFile
		 @Input			filename
		 @Description	Saves a .h File.
		*************************************************************************/
		bool privateSaveCHeaderFile(FILE* pTextureFile, CPVRTString filename) const;

		/*!***********************************************************************
		 @Function		privateSaveLegacyPVRFile
		 @Input			pTextureFile
		 @Input			eApi
		 @Description	Saves a legacy PVR File - Uses version 2 file format.
		*************************************************************************/
		bool privateSaveLegacyPVRFile(FILE* pTextureFile, ELegacyApi eApi) const;
	};
};

#endif //_PVRTEXTURE_H

