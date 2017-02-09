/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <vkts/math/vkts_math.hpp>

namespace vkts
{

const glm::vec4 Obb::cornersUnit[8] = {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, -1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f), glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), };

Obb::Obb() :
	Obb(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
{
}

Obb::Obb(const glm::vec3& translate, const glm::vec3& rotate, const glm::vec3& scale) :
	corners()
{
	glm::mat4 transform = translateMat4(translate.x, translate.y, translate.z) * rotateRzRyRxMat4(rotate.z, rotate.y, rotate.x) * scaleMat4(scale.x, scale.y, scale.z);

	for (int32_t i = 0; i < 8; i++)
	{
		corners[i] = transform * cornersUnit[i];
	}
}

Obb::Obb(const glm::vec4& min, const glm::vec4& max) :
	corners{glm::vec4(max.x, max.y, max.z, 1.0f), glm::vec4(min.x, max.y, max.z, 1.0f),
			glm::vec4(min.x, min.y, max.z, 1.0f), glm::vec4(max.x, min.y, max.z, 1.0f),
			glm::vec4(max.x, max.y, min.z, 1.0f), glm::vec4(min.x, max.y, min.z, 1.0f),
			glm::vec4(min.x, min.y, min.z, 1.0f), glm::vec4(max.x, min.y, min.z, 1.0f)}
{
}

Obb::Obb(const glm::vec4 corners[8]) :
	corners{corners[0], corners[1], corners[2], corners[3], corners[4], corners[5], corners[6], corners[7]}
{
}

Obb::~Obb()
{
}

const glm::vec4& Obb::getCorner(const uint32_t i) const
{
	// No check by purpose.
	return corners[i];
}

Obb operator *(const glm::mat4& transform, const Obb& box)
{
	glm::vec4 transformedCorners[8];

	for (int32_t i = 0; i < 8; i++)
	{
		transformedCorners[i] = transform* box.getCorner(i);
	}

	return Obb(transformedCorners);
}

} /* namespace vkts */
