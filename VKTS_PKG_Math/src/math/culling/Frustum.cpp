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

// Note:  Top and Bottom "switched", as everything is upside down.
//		  For Near, d is 0.0, as NDC for from 0.0 to 1.0.
// Order: Left, Right, Top, Bottom, Near, Far
const Plane Frustum::sidesNDC[6] = {Plane(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f), Plane(glm::vec3(-1.0f, 0.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, -1.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f), Plane(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f), Plane(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f)};

Frustum::Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) :
	sidesWorld()
{
	toWorldSpace(projectionMatrix, viewMatrix);
}

Frustum::~Frustum()
{
}

void Frustum::toWorldSpace(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
{
	glm::mat4 transposedViewProjectionMatrix = glm::transpose(projectionMatrix * viewMatrix);

	for (int32_t i = 0; i < 6; i++)
	{
		sidesWorld[i] = transposedViewProjectionMatrix * sidesNDC[i];
	}
}

VkBool32 Frustum::isVisible(const glm::vec4& pointWorld) const
{
	for (auto& currentSide : sidesWorld)
	{
		if (currentSide.distance(pointWorld) < 0.0f)
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

VkBool32 Frustum::isVisible(const Sphere& sphereWorld) const
{
	for (auto& currentSide : sidesWorld)
	{
		float distance = currentSide.distance(sphereWorld.getCenter());

		if (distance + sphereWorld.getRadius() < 0.0f)
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

VkBool32 Frustum::isVisible(const Aabb& aabbWorld) const
{
	return isVisible(aabbWorld.getObb());
}

VkBool32 Frustum::isVisible(const Obb& obbWorld) const
{
	for (auto& currentSide : sidesWorld)
	{
		VkBool32 outsidePlane = VK_TRUE;

		for (uint32_t i = 0; i < 8; i++)
		{
			if (currentSide.distance(obbWorld.getCorner(i)) >= 0.0f)
			{
				outsidePlane = VK_FALSE;

				break;
			}
		}

		if (outsidePlane)
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

} /* namespace vkts */
