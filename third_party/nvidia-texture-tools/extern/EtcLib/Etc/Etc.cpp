/*
 * Copyright 2015 The Etc2Comp Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EtcConfig.h"
#include "Etc.h"

#include <string.h>

namespace Etc
{
	// ----------------------------------------------------------------------------------------------------
	// C-style inteface to the encoder
	//
	void Encode(float *a_pafSourceRGBA,
				unsigned int a_uiSourceWidth, 
				unsigned int a_uiSourceHeight,
				Image::Format a_format,
				ErrorMetric a_eErrMetric,
				float a_fEffort,
				unsigned int a_uiJobs,
				unsigned int a_uiMaxJobs,
				unsigned char **a_ppaucEncodingBits,
				unsigned int *a_puiEncodingBitsBytes,
				unsigned int *a_puiExtendedWidth,
				unsigned int *a_puiExtendedHeight, 
				int *a_piEncodingTime_ms, bool a_bVerboseOutput)
	{

		Image image(a_pafSourceRGBA, a_uiSourceWidth,
					a_uiSourceHeight,
					a_eErrMetric);
		image.m_bVerboseOutput = a_bVerboseOutput;
		image.Encode(a_format, a_eErrMetric, a_fEffort, a_uiJobs, a_uiMaxJobs);

		*a_ppaucEncodingBits = image.GetEncodingBits();
		*a_puiEncodingBitsBytes = image.GetEncodingBitsBytes();
		*a_puiExtendedWidth = image.GetExtendedWidth();
		*a_puiExtendedHeight = image.GetExtendedHeight();
		*a_piEncodingTime_ms = image.GetEncodingTimeMs();
	}

	// ----------------------------------------------------------------------------------------------------
	//

}
