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

#ifndef VKTS_TEXTUREOBJECT_HPP_
#define VKTS_TEXTUREOBJECT_HPP_

#include <vkts/vulkan/composition/vkts_composition.hpp>

namespace vkts
{

class TextureObject: public ITextureObject
{

private:

    const IContextObjectSP contextObject;

    std::string name;

    IImageObjectSP imageObject;

    ISamplerSP sampler;

public:

    TextureObject() = delete;
    TextureObject(const IContextObjectSP& contextObject, const std::string& name, const IImageObjectSP& imageObject, const ISamplerSP& sampler);
    TextureObject(const TextureObject& other) = delete;
    TextureObject(TextureObject&& other) = delete;
    virtual ~TextureObject();

    TextureObject& operator =(const TextureObject& other) = delete;
    TextureObject& operator =(TextureObject && other) = delete;

    //
    // ITextureObject
    //

    virtual const IContextObjectSP& getContextObject() const override;

    virtual const std::string& getName() const override;

    virtual const IImageObjectSP& getImageObject() const override;

    virtual const ISamplerSP& getSampler() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_TEXTUREOBJECT_HPP_ */
