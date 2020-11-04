// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "cmdline.h"

#include "nvimage/Image.h"
#include "nvimage/ImageIO.h"
#include "nvimage/DirectDrawSurface.h"

#include "nvmath/Color.h"

#include "nvcore/Array.inl"
#include "nvcore/StrLib.h"
#include "nvcore/StdStream.h"

// @@ Add decent error messages.
// @@ Add option to resize images.
// @@ Add support for reading DDS files with 2D images and possibly mipmaps.

int main(int argc, char *argv[])
{
	MyAssertHandler assertHandler;
	MyMessageHandler messageHandler;

	bool assembleCubeMap = true;
	bool assembleVolume = false;
	bool assembleTextureArray = false;
	bool bgra = false;
	bool forceNoAlpha = false;
	
	nv::Array<nv::Path> files;
	nv::Path output = "output.dds";
	
	// Parse arguments.
	for (int i = 1; i < argc; i++)
	{
		// Input options.
		if (strcmp("-cube", argv[i]) == 0)
		{
			assembleCubeMap = true;
			assembleVolume = false;
			assembleTextureArray = false;
		}
		else if (strcmp("-volume", argv[i]) == 0)
		{
			assembleCubeMap = false;
			assembleVolume = true;
			assembleTextureArray = false;
		}
		else if (strcmp("-array", argv[i]) == 0)
		{
			assembleCubeMap = false;
			assembleVolume = false;
			assembleTextureArray = true;
		}
		else if (strcmp("-o", argv[i]) == 0)
		{
			i++;
			if (i < argc && argv[i][0] != '-')
			{
				output = argv[i];
			}
		}
		else if (strcmp("-bgra", argv[i]) == 0)
		{
			bgra = true;
		}
		else if (strcmp("-noalpha", argv[i]) == 0)
		{
			forceNoAlpha = true;
		}
		else if (argv[i][0] != '-')
		{
			files.append(argv[i]);
		}
		else
		{
			printf("Warning: unrecognized option \"%s\"\n", argv[i]);
		}
	}
	
	if (files.count() == 0)
	{
		printf("NVIDIA Texture Tools - Copyright NVIDIA Corporation 2007\n\n");
		printf("usage: nvassemble [-cube|-volume|-array] [-bgra] [-noalpha] 'file0' 'file1' ...\n\n");
		return 1;
	}
	
	if (nv::strCaseDiff(output.extension(), ".dds") != 0)
	{
		//output.stripExtension();
		output.append(".dds");
	}

	if (assembleCubeMap && files.count() != 6)
	{
		printf("*** error, 6 files expected, but got %d\n", files.count());
		return 1;
	}

	if (forceNoAlpha && assembleTextureArray)
	{
		printf("*** error, -noalpha is incompatible with texture arrays\n");
		return 1;
	}
	
	// Load all files.
	nv::Array<nv::Image> images;
	
	uint w = 0, h = 0;
	bool hasAlpha = false;
	
	const uint imageCount = files.count();
	images.resize(imageCount);

	for (uint i = 0; i < imageCount; i++)
	{
		if (!images[i].load(files[i].str()))
		{
			printf("*** error loading file\n");
			return 1;
		}
		
		if (i == 0)
		{
			w = images[i].width;
			h = images[i].height;
		}
		else if (images[i].width != w || images[i].height != h)
		{
			printf("*** error, size of image '%s' does not match\n", files[i].str());
			return 1;
		}
		
		if (images[i].format == nv::Image::Format_ARGB)
		{
			hasAlpha = true;
		}
	}
	
	nv::StdOutputStream stream(output.str());
	if (stream.isError()) {
		printf("Error opening '%s' for writting\n", output.str());
		return 1;
	}
	
	// Output DDS header.
	nv::DDSHeader header;
	header.setWidth(w);
	header.setHeight(h);

	if (assembleCubeMap)
	{
		header.setTextureCube();
	}
	else if (assembleVolume)
	{
		header.setTexture3D();
		header.setDepth(imageCount);
	}
	else if (assembleTextureArray)
	{
		header.setTextureArray(imageCount);
	}

	if (!assembleTextureArray) {
		uint bpp = forceNoAlpha ? 24 : 32;

		uint pitch = w * bpp / 8;
		if (pitch % 4 != 0) {
			printf("Warning: bytes per scanline is not divisable by 4\n");
		}
		header.setPitch(pitch);

		uint alphaMask = (!hasAlpha || forceNoAlpha) ? 0 : 0xFF000000;

		if (bgra) {
			header.setPixelFormat(bpp, 0xFF0000, 0xFF00, 0xFF, alphaMask);
		}
		else {
			header.setPixelFormat(bpp, 0xFF, 0xFF00, 0xFF0000, alphaMask);
		}
	} else {
		if (bgra) {
			header.setDX10Format(hasAlpha ? nv::DXGI_FORMAT_B8G8R8A8_UNORM : nv::DXGI_FORMAT_B8G8R8X8_UNORM);
		}
		else {
			header.setDX10Format(nv::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
	}

	stream << header;

	// Output images.
	for (uint i = 0; i < imageCount; i++)
	{
		const uint pixelCount = w * h;
		for (uint p = 0; p < pixelCount; p++)
		{
			nv::Color32 c = images[i].pixel(p);
			uint8 r = c.r;
			uint8 g = c.g;
			uint8 b = c.b;
			uint8 a = c.a;
			if (bgra) {
				stream << b << g << r;
			}
			else {
				stream << r << g << b; 
			}
			if (!forceNoAlpha) {
				stream << a;
			}
		}
	}

	return 0;
}

