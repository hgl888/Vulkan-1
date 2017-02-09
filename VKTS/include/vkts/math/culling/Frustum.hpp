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

#ifndef VKTS_FRUSTUM_HPP_
#define VKTS_FRUSTUM_HPP_

#include <vkts/math/vkts_math.hpp>

namespace vkts
{

class Frustum
{

private:

	static const Plane sidesNDC[6];

	Plane sidesWorld[6];

public:

	Frustum() = delete;
	Frustum(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
    virtual ~Frustum();

    void toWorldSpace(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

    VkBool32 isVisible(const glm::vec4& pointWorld) const;

    VkBool32 isVisible(const Sphere& sphereWorld) const;

    VkBool32 isVisible(const Aabb& aabbWorld) const;

    VkBool32 isVisible(const Obb& obbWorld) const;

};

} /* namespace vkts */

#endif /* VKTS_FRUSTUM_HPP_ */
