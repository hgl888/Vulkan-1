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

#ifndef VKTS_IBSDFMATERIAL_HPP_
#define VKTS_IBSDFMATERIAL_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class IBSDFMaterial : public ICloneable<IBSDFMaterial>, public IDestroyable
{

public:

    IBSDFMaterial() :
        IDestroyable()
    {
    }

    virtual ~IBSDFMaterial()
    {
    }

    virtual VkBool32 getForwardRendering() const = 0;

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;


    virtual IRenderMaterialSP getRenderMaterial(const uint32_t index) const = 0;

    virtual VkDeviceSize getRenderMaterialSize() const = 0;

    virtual void addRenderMaterial(const IRenderMaterialSP& renderMaterial) = 0;


    virtual IShaderModuleSP getFragmentShader() const = 0;

	virtual void setFragmentShader(const IShaderModuleSP& fragmentShader) = 0;

    virtual VkTsVertexBufferType getAttributes() const = 0;

	virtual void setAttributes(const VkTsVertexBufferType attributes) = 0;

    virtual void addTextureObject(const ITextureObjectSP& textureObject) = 0;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) = 0;

    virtual uint32_t getNumberTextureObjects() const = 0;

    virtual const SmartPointerVector<ITextureObjectSP>& getTextureObjects() const = 0;

    virtual VkBool32 isTransparent() const = 0;

    virtual void setTransparent(const VkBool32 transparent) = 0;

    //

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName) = 0;

    //

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName) = 0;

};

typedef std::shared_ptr<IBSDFMaterial> IBSDFMaterialSP;

} /* namespace vkts */

#endif /* VKTS_IBSDFMATERIAL_HPP_ */
