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

#ifndef VKTS_CAMERA_HPP_
#define VKTS_CAMERA_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

class Camera : public ICamera
{

private:

    std::string name;

    enum CameraType cameraType;

    float zNear;
    float zFar;

    float aspect;
    float fovy;

    glm::vec4 ortho;

    glm::mat4 view;

    glm::mat4 projection;

    void updateProjectionMatrix();

public:

    Camera();
    Camera(const Camera& other);
    Camera(Camera&& other) = delete;
    virtual ~Camera();

    Camera& operator =(const Camera& other) = delete;
    Camera& operator =(Camera && other) = delete;

    //
    // ICamera
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;

    virtual enum CameraType getCameraType() const override;

    virtual void setCameraType(const enum CameraType lightType) override;

    virtual float getZNear() const override;

    virtual void setZNear(const float znear) override;

    virtual float getZFar() const override;

    virtual void setZFar(const float zfar) override;


    virtual float getAspect() const override;

    virtual void setAspect(const float aspect) override;

    virtual float getFovY() const override;

    virtual void setFovY(const float fovy) override;


    virtual float getLeft() const override;

    virtual void setLeft(const float left) override;

    virtual float getRight() const override;

    virtual void setRight(const float right) override;

    virtual float getBottom() const override;

    virtual void setBottom(const float bottom) override;

    virtual float getTop() const override;

    virtual void setTop(const float top) override;


    virtual const glm::mat4& getViewMatrix() const override;

    virtual void updateViewMatrix(const glm::mat4& transform) override;

    virtual const glm::mat4& getProjectionMatrix() const override;

    //
    // ICloneable
    //

    virtual ICameraSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_CAMERA_HPP_ */
