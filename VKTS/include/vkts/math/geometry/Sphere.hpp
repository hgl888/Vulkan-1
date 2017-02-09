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

#ifndef VKTS_SPHERE_HPP_
#define VKTS_SPHERE_HPP_

#include <vkts/math/vkts_math.hpp>

namespace vkts
{

class Plane;

class Sphere
{

private:

    glm::vec4 c;

    float r;

public:

    Sphere();
    Sphere(const float x, const float y, const float z, const float radius);
    Sphere(const glm::vec4& center, const float radius);
    ~Sphere();

    const glm::vec4& getCenter() const;

    float getRadius() const;

    float distance(const glm::vec4& point) const;

    float distance(const Plane& plane) const;

    float distance(const Sphere& sphere) const;

    Sphere operator +(const Sphere & other) const;

    Sphere & operator +=(const Sphere & other);

};

Sphere operator *(const glm::mat4& transform, const Sphere& s);

} /* namespace vkts */

#endif /* VKTS_SPHERE_HPP_ */
