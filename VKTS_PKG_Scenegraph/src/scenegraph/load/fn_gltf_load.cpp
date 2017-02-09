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

#include <vkts/scenegraph/vkts_scenegraph.hpp>

#include "GltfVisitor.hpp"

#define VKTS_GLTF_FORWARD_FRAGMENT_SHADER_NAME "shader/SPIR/V/glTF_forward.frag.spv"

namespace vkts
{

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const std::string& factorName, const float factor[4], const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string textureObjectName;
	std::string imageObjectName;
	std::string imageDataName;

	if (texture && texture->source && texture->source->imageData.get())
	{
		textureObjectName = texture->name;

		imageObjectName = texture->source->name;

		imageDataName = texture->source->imageData->getName();
	}
	else
	{
		textureObjectName = "INTERNAL_WHITE";

		imageObjectName = "INTERNAL_WHITE";

		imageDataName = "INTERNAL_WHITE";
	}

	textureObjectName += factorName;

	imageObjectName += factorName;

	imageDataName += factorName;

	//

	VkBool32 mipmap = texture ? VK_TRUE : VK_FALSE;

	//

	ITextureObjectSP textureObject = sceneManager->useTextureObject(textureObjectName);

	if (textureObject.get())
	{
		return textureObject;
	}

	//

	IImageObjectSP imageObject = sceneManager->useImageObject(imageObjectName);

	if (imageObject.get())
	{
		textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName, mipmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

		if (!textureObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addTextureObject(textureObject);

		//

		textureObject = sceneManager->useTextureObject(textureObjectName);

		return textureObject;
	}

	//

	IImageDataSP imageData = sceneManager->useImageData(imageDataName);

	if (!imageData.get())
	{
		if (texture && texture->source && texture->source->imageData.get())
		{
			imageData = imageDataConvert(imageData, imageData->getFormat(), imageDataName, imageDataType, imageDataType, glm::vec4(factor[0], factor[1], factor[2], factor[3]));
		}
		else
		{
			if (imageDataType == VKTS_NORMAL_DATA)
			{
				imageData = imageDataCreate(imageDataName, 1, 1, 1, glm::vec4(0.0f, factor[1], 0.0f, 0.0f), VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);
			}
			else
			{
				imageData = imageDataCreate(imageDataName, 1, 1, 1, glm::vec4(factor[0], factor[1], factor[2], factor[3]), VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM);
			}

		}

		if (!imageData.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addImageData(imageData);

		//

		imageData = sceneManager->useImageData(imageDataName);
	}

	if (imageData.get())
	{
		imageObject = createImageObject(sceneManager->getAssetManager(), imageObjectName, imageData, VK_FALSE);

		if (!imageObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addImageObject(imageObject);

		imageObject = sceneManager->useImageObject(imageObjectName);

		//

		textureObject = createTextureObject(sceneManager->getAssetManager(), textureObjectName, mipmap, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

		if (!textureObject.get())
		{
			return ITextureObjectSP();
		}

		sceneManager->addTextureObject(textureObject);

		//

		textureObject = sceneManager->useTextureObject(textureObjectName);

		return textureObject;
	}

	return ITextureObjectSP();
}

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const float factor[4], const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string factorName = "_" + std::to_string(factor[0]) + "_" + std::to_string(factor[1]) + "_" + std::to_string(factor[2]) + "_" + std::to_string(factor[3]);

	return gltfProcessTextureObject(texture, factorName, factor, imageDataType, sceneManager);
}

static ITextureObjectSP gltfProcessTextureObject(const GltfTexture* texture, const float factor, const enum VkTsImageDataType imageDataType, const ISceneManagerSP& sceneManager)
{
	std::string factorName = "_" + std::to_string(factor);

	const float tempFactor[4] = {factor, factor, factor, factor};

	return gltfProcessTextureObject(texture, factorName, tempFactor, imageDataType, sceneManager);
}

static VkBool32 gltfProcessSubMesh(ISubMeshSP& subMesh, const GltfVisitor& visitor, const GltfPrimitive& gltfPrimitive, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	if (!gltfPrimitive.position)
	{
		return VK_FALSE;
	}

	subMesh->setNumberVertices((int32_t)gltfPrimitive.position->count);

	uint32_t totalSize = 0;
	uint32_t strideInBytes = 0;

    VkTsVertexBufferType vertexBufferType = 0;

    //

    if (!visitor.isFloat(gltfPrimitive.position->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.position->type) == 3 || visitor.getComponentsPerType(gltfPrimitive.position->type) == 4))
    {
    	return VK_FALSE;
    }

    subMesh->setVertexOffset(strideInBytes);
    strideInBytes += 4 * sizeof(float);

    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_VERTEX;

	if (gltfPrimitive.normal)
	{
        if (gltfPrimitive.normal->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Normal has different size");

            return VK_FALSE;
        }

        if (!visitor.isFloat(gltfPrimitive.normal->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.normal->type) == 3))
        {
        	return VK_FALSE;
        }

        subMesh->setNormalOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_NORMAL;
	}

    if (gltfPrimitive.normal && (gltfPrimitive.binormal || gltfPrimitive.tangent))
    {
    	if (gltfPrimitive.binormal)
    	{
			if (gltfPrimitive.binormal->count != gltfPrimitive.position->count)
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bitangent has different size");

				return VK_FALSE;
			}

			if (!visitor.isFloat(gltfPrimitive.binormal->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.binormal->type) == 3))
			{
				return VK_FALSE;
			}
    	}

        subMesh->setBitangentOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        //

        if (gltfPrimitive.tangent)
        {
			if (gltfPrimitive.tangent->count != gltfPrimitive.position->count)
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Tangent has different size");

				return VK_FALSE;
			}

			if (!visitor.isFloat(gltfPrimitive.tangent->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.tangent->type) == 3))
			{
				return VK_FALSE;
			}
        }

        subMesh->setTangentOffset(strideInBytes);
        strideInBytes += 3 * sizeof(float);

        totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

        //

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TANGENTS;
    }

	if (gltfPrimitive.texCoord)
	{
        if (gltfPrimitive.texCoord->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Texture coordinate has different size");

            return VK_FALSE;
        }

        if (!visitor.isFloat(gltfPrimitive.texCoord->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.texCoord->type) == 2))
        {
        	return VK_FALSE;
        }

        subMesh->setTexcoordOffset(strideInBytes);
        strideInBytes += 2 * sizeof(float);

        totalSize += 2 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TEXCOORD;
	}

    if (gltfPrimitive.joint && gltfPrimitive.weight)
    {
        if (gltfPrimitive.joint->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Joint has different size");

            return VK_FALSE;
        }

        if (!visitor.isFloat(gltfPrimitive.joint->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.joint->type) == 4))
        {
        	return VK_FALSE;
        }

        subMesh->setBoneIndices0Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //

        subMesh->setBoneIndices1Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //
        //

        if (gltfPrimitive.weight->count != gltfPrimitive.position->count)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Weight has different size");

            return VK_FALSE;
        }

        if (!visitor.isFloat(gltfPrimitive.weight->componentType) || !(visitor.getComponentsPerType(gltfPrimitive.weight->type) == 4))
        {
        	return VK_FALSE;
        }

        subMesh->setBoneWeights0Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //

        subMesh->setBoneWeights1Offset(strideInBytes);
        strideInBytes += 4 * sizeof(float);

        totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

        //
        //

        subMesh->setNumberBonesOffset(strideInBytes);
        strideInBytes += 1 * sizeof(float);

        totalSize += 1 * sizeof(float) * subMesh->getNumberVertices();

        vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
    }

    subMesh->setStrideInBytes(strideInBytes);

    if (totalSize > 0)
    {
        auto vertexBinaryBuffer = binaryBufferCreate((uint32_t)totalSize);

        if (!vertexBinaryBuffer.get() || vertexBinaryBuffer->getSize() != (uint32_t)totalSize)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex binary buffer");

            return VK_FALSE;
        }

        for (int32_t currentVertexElement = 0; currentVertexElement < subMesh->getNumberVertices(); currentVertexElement++)
        {
        	const float* currentFloatData = nullptr;

            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
            {
            	float vertex[4] = {0.0f, 0.0f, 0.0f, 1.0f};

            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.position, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

            	for (uint32_t i = 0; i < visitor.getComponentsPerType(gltfPrimitive.position->type); i++)
            	{
            		vertex[i] = currentFloatData[i];
            	}

                vertexBinaryBuffer->write((const void*)vertex, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.normal, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
            {
            	float binormal[3];

            	if (gltfPrimitive.binormal)
            	{
					currentFloatData = visitor.getFloatPointer(*gltfPrimitive.binormal, currentVertexElement);

					if (!currentFloatData)
					{
						return VK_FALSE;
					}

					for (uint32_t i = 0; i < 3; i++)
					{
						binormal[i] = currentFloatData[i];
					}
            	}
            	else if (gltfPrimitive.normal && gltfPrimitive.tangent)
            	{
					auto* normal = visitor.getFloatPointer(*gltfPrimitive.normal, currentVertexElement);
					auto* tangent = visitor.getFloatPointer(*gltfPrimitive.tangent, currentVertexElement);

					if (!normal || !tangent)
					{
						return VK_FALSE;
					}

					auto temp = glm::cross(glm::vec3(normal[0], normal[1], normal[2]), glm::vec3(tangent[0], tangent[1], tangent[2]));

					binormal[0] = temp.x;
					binormal[1] = temp.y;
					binormal[2] = temp.z;
            	}
            	else
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)binormal, 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
            {
            	float tangent[3];

            	if (gltfPrimitive.tangent)
            	{
					currentFloatData = visitor.getFloatPointer(*gltfPrimitive.tangent, currentVertexElement);

					if (!currentFloatData)
					{
						return VK_FALSE;
					}

					for (uint32_t i = 0; i < 3; i++)
					{
						tangent[i] = currentFloatData[i];
					}
            	}
            	else if (gltfPrimitive.normal && gltfPrimitive.binormal)
            	{
					auto* normal = visitor.getFloatPointer(*gltfPrimitive.normal, currentVertexElement);
					auto* binormal = visitor.getFloatPointer(*gltfPrimitive.binormal, currentVertexElement);

					if (!normal || !binormal)
					{
						return VK_FALSE;
					}

					auto temp = glm::cross(glm::vec3(binormal[0], binormal[1], binormal[2]), glm::vec3(normal[0], normal[1], normal[2]));

					tangent[0] = temp.x;
					tangent[1] = temp.y;
					tangent[2] = temp.z;
            	}
            	else
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)tangent, 1, 3 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.texCoord, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 2 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.joint, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
            {
            	float boneIndices1[4] = {-1.0f, -1.0f, -1.0f, -1.0f};

            	// Do not change, as data not given.

                vertexBinaryBuffer->write((const void*)boneIndices1, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
            {
            	currentFloatData = visitor.getFloatPointer(*gltfPrimitive.weight, currentVertexElement);

            	if (!currentFloatData)
            	{
            		return VK_FALSE;
            	}

                vertexBinaryBuffer->write((const void*)currentFloatData, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
            {
            	float boneWeights1[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            	// Do not change, as data not given.

                vertexBinaryBuffer->write((const void*)boneWeights1, 1, 4 * sizeof(float));
            }
            if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
            {
            	float numberBones[1] = {4};

            	// Do not change, as data not given.

                vertexBinaryBuffer->write((const void*)numberBones, 1, 1 * sizeof(float));
            }
        }

        //

        auto vertexBuffer = createVertexBufferObject(sceneManager->getAssetManager(), vertexBinaryBuffer);

        if (!vertexBuffer.get())
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex buffer");

            return VK_FALSE;
        }

        //

        subMesh->setVertexBuffer(vertexBuffer, vertexBufferType, Aabb((const float*)vertexBinaryBuffer->getData(), subMesh->getNumberVertices(), subMesh->getStrideInBytes()));
    }
    else
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh incomplete");

        return VK_FALSE;
    }

	//
	// Indices
	//

	if (gltfPrimitive.indices)
	{
		if (visitor.getComponentsPerType(gltfPrimitive.indices->type) != 1)
		{
			return VK_FALSE;
		}

		subMesh->setNumberIndices((int32_t)gltfPrimitive.indices->count);
	}
	else
	{
		subMesh->setNumberIndices((int32_t)gltfPrimitive.position->count);
	}

	uint32_t size = sizeof(int32_t) * subMesh->getNumberIndices();

    auto indicesBinaryBuffer = binaryBufferCreate(size);

    if (!indicesBinaryBuffer.get() || indicesBinaryBuffer->getSize() != size)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices binary buffer");

        return VK_FALSE;
    }

    for (uint32_t i = 0; i < (uint32_t)subMesh->getNumberIndices(); i++)
    {
    	int32_t index = (int32_t)i;

    	if (gltfPrimitive.indices)
    	{
    		const void* currentVoidPointer = visitor.getBufferPointer(*gltfPrimitive.indices, i);

    		if (!currentVoidPointer)
    		{
    			return VK_FALSE;
    		}

    		switch (gltfPrimitive.indices->componentType)
    		{
    			case 5120:
    				index = (int32_t)*((const int8_t*)currentVoidPointer);
    				break;
    			case 5121:
    				index = (int32_t)*((const uint8_t*)currentVoidPointer);
    				break;
    			case 5122:
    				index = (int32_t)*((const int16_t*)currentVoidPointer);
    				break;
    			case 5123:
    				index = (int32_t)*((const uint16_t*)currentVoidPointer);
    				break;
    			case 5125:
    				index = (int32_t)*((const uint32_t*)currentVoidPointer);
    				break;
    			case 5126:
    				index = (int32_t)*((const float*)currentVoidPointer);
    				break;
    			default:
    				return VK_FALSE;
    		}
    	}

    	indicesBinaryBuffer->write((const void*)&index, 1, sizeof(int32_t));
    }

    //

    auto indexVertexBuffer = createIndexBufferObject(sceneManager->getAssetManager(), indicesBinaryBuffer);

    if (!indexVertexBuffer.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices vertex buffer");

        return VK_FALSE;
    }

    //

    subMesh->setIndexBuffer(indexVertexBuffer);

	switch (gltfPrimitive.mode)
	{
		case 0:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
			break;
		case 1:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
			break;
		case 2:
			return VK_FALSE;
			break;
		case 3:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
			break;
		case 4:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			break;
		case 5:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
			break;
		case 6:
			subMesh->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
			break;
		default:
			return VK_FALSE;
	}

	// Optional

	if (gltfPrimitive.material)
	{
		auto bsdfMaterial = sceneManager->useBSDFMaterial(gltfPrimitive.material->name);

		if (!bsdfMaterial.get())
		{
			bsdfMaterial = sceneFactory->createBSDFMaterial(sceneManager, VK_TRUE);

			if (!bsdfMaterial.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->setName(gltfPrimitive.material->name);

			//
			// Base color
			//

			ITextureObjectSP baseColor = gltfProcessTextureObject(gltfPrimitive.material->baseColorTexture, gltfPrimitive.material->baseColorFactor, VKTS_LDR_COLOR_DATA, sceneManager);

			if (!baseColor.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(baseColor);

			//
			// Metallic
			//

			ITextureObjectSP metallic = gltfProcessTextureObject(gltfPrimitive.material->metallicTexture, gltfPrimitive.material->metallicFactor, VKTS_NON_COLOR_DATA, sceneManager);

			if (!metallic.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(metallic);

			//
			// Roughness
			//

			ITextureObjectSP roughness = gltfProcessTextureObject(gltfPrimitive.material->roughnessTexture, gltfPrimitive.material->roughnessFactor, VKTS_NON_COLOR_DATA, sceneManager);

			if (!roughness.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(roughness);

			//
			// Normal
			//

			ITextureObjectSP normal = gltfProcessTextureObject(gltfPrimitive.material->normalTexture, gltfPrimitive.material->normalFactor, VKTS_NORMAL_DATA, sceneManager);

			if (!normal.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(normal);

			//
			// Ambient occlusion
			//

			ITextureObjectSP ambientOcclusion = gltfProcessTextureObject(gltfPrimitive.material->aoTexture, gltfPrimitive.material->aoFactor, VKTS_NON_COLOR_DATA, sceneManager);

			if (!ambientOcclusion.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(ambientOcclusion);

			//
			// Emissive
			//

			ITextureObjectSP emissive = gltfProcessTextureObject(gltfPrimitive.material->emissiveTexture, gltfPrimitive.material->emissiveFactor, VKTS_LDR_COLOR_DATA, sceneManager);

			if (!emissive.get())
			{
				return VK_FALSE;
			}

			bsdfMaterial->addTextureObject(emissive);

			//
			// Shader
			//

			const char* fragmentShader = VKTS_GLTF_FORWARD_FRAGMENT_SHADER_NAME;

            auto shaderModule = sceneManager->useFragmentShaderModule(fragmentShader);

            if (!shaderModule.get())
            {
                std::string finalFilename = visitor.getDirectory() + std::string(fragmentShader);

                auto shaderBinary = fileLoadBinary(finalFilename.c_str());

				if (!shaderBinary.get())
				{
					shaderBinary = fileLoadBinary(fragmentShader);

					if (!shaderBinary.get())
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", fragmentShader, VKTS_MAX_TOKEN_CHARS);

						return VK_FALSE;
					}
				}

				//

				shaderModule = createShaderModule(sceneManager->getAssetManager(), fragmentShader, shaderBinary);

				if (!shaderModule.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

					return VK_FALSE;
				}

				sceneManager->addFragmentShaderModule(shaderModule);
            }

            bsdfMaterial->setFragmentShader(shaderModule);

			//
			// Attribute
			//

			bsdfMaterial->setAttributes(subMesh->getVertexBufferType());
		}

		subMesh->setBSDFMaterial(bsdfMaterial);

    	if (!sceneFactory->getSceneRenderFactory()->prepareBSDFMaterial(sceneManager, subMesh))
    	{
    		return VK_FALSE;
    	}
	}

	return VK_TRUE;
}

static VkBool32 gltfProcessNode(INodeSP& node, const GltfVisitor& visitor, const GltfNode& gltfNode, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	node->setNodeRotationMode(VKTS_EULER_XYZ);

	//

	if (gltfNode.useMatrix)
	{
		// Process matrix.

		glm::mat4 matrix;

		for (uint32_t i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] = gltfNode.matrix[i];
		}

		node->setTranslate(decomposeTranslate(matrix));
		node->setRotate(decomposeRotateRzRyRx(matrix));
		node->setScale(decomposeScale(matrix));
	}
	else
	{
		// Process translation, rotation and scale.

		if (gltfNode.useTranslation)
		{
			node->setTranslate(glm::vec3(gltfNode.translation[0], gltfNode.translation[1], gltfNode.translation[2]));
		}

		if (gltfNode.useRotation)
		{
			Quat quat(gltfNode.rotation[0], gltfNode.rotation[1], gltfNode.rotation[2], gltfNode.rotation[3]);

			node->setRotate(decomposeRotateRzRyRx(quat.mat4()));
		}

		if (gltfNode.useScale)
		{
			node->setScale(glm::vec3(gltfNode.scale[0], gltfNode.scale[1], gltfNode.scale[2]));
		}
	}

    if (gltfNode.skin)
    {
    	// Process skin, so this is an armature node.

    	if (!sceneFactory->getSceneRenderFactory()->prepareJointsUniformBuffer(sceneManager, node, (int32_t)gltfNode.skin->jointNodes.size()))
    	{
            return VK_FALSE;
    	}

		glm::mat4 matrix;

		for (uint32_t i = 0; i < 16; i++)
		{
			matrix[i / 4][i % 4] = gltfNode.skin->bindShapeMatrix[i];
		}

		node->setTranslate(decomposeTranslate(matrix));
		node->setRotate(decomposeRotateRzRyRx(matrix));
		node->setScale(decomposeScale(matrix));
    }

    if (gltfNode.jointName != "")
    {
    	// Process jointName, so this is a joint node.

    	if (!node->getParentNode().get())
    	{
    		return VK_FALSE;
    	}

    	auto currentParentNode = node->getParentNode();

    	while (currentParentNode.get())
    	{
    		if (currentParentNode->isArmature())
    		{
    			break;
    		}

    		currentParentNode = currentParentNode->getParentNode();
    	}

    	if (!currentParentNode.get())
    	{
    		return VK_FALSE;
    	}

    	//

    	const auto& armatureGltfNode = visitor.getAllGltfNodes()[currentParentNode->getName()];

    	if (!armatureGltfNode.skin)
    	{
    		return VK_FALSE;
    	}

    	auto jointIndex = armatureGltfNode.skin->jointNames.index(node->getName());

    	if (jointIndex == armatureGltfNode.skin->jointNames.size())
    	{
    		return VK_FALSE;
    	}

    	node->setJointIndex((int32_t)jointIndex);

    	//

    	// Not using inverse bind matrix, as calculated by the engine.
    }

    // Process meshes.
    for (uint32_t i = 0; i < gltfNode.meshes.size(); i++)
    {
        auto mesh = sceneFactory->createMesh(sceneManager);

        if (!mesh.get())
        {
            return VK_FALSE;
        }

        mesh->setName(gltfNode.meshes[i]->name);

        sceneManager->addMesh(mesh);

        //

        for (uint32_t k = 0; k < gltfNode.meshes[i]->primitives.size(); k++)
        {
            auto subMesh = sceneFactory->createSubMesh(sceneManager);

            if (!subMesh.get())
            {
                return VK_FALSE;
            }

            subMesh->setName(gltfNode.meshes[i]->name + "_" + gltfNode.meshes[i]->primitives[k].name);

            sceneManager->addSubMesh(subMesh);

            //

            if (!gltfProcessSubMesh(subMesh, visitor, gltfNode.meshes[i]->primitives[k], sceneManager, sceneFactory))
            {
            	return VK_FALSE;
            }

            //

            auto currentSubMesh = sceneManager->useSubMesh(subMesh->getName());

            mesh->addSubMesh(currentSubMesh);
        }

        //

        auto currentMesh = sceneManager->useMesh(mesh->getName());

        if (!currentMesh.get())
        {
            return VK_FALSE;
        }

        node->addMesh(currentMesh);
    }

	// Process children.
    for (uint32_t i = 0; i < gltfNode.childrenPointer.size(); i++)
    {
        auto childNode = sceneFactory->createNode(sceneManager);

        if (!childNode.get())
        {
            return VK_FALSE;
        }

        childNode->setName(gltfNode.children[i]);

        childNode->setParentNode(node);

        //

        node->addChildNode(childNode);

        //

        if (!gltfProcessNode(childNode, visitor, *(gltfNode.childrenPointer[i]), sceneManager, sceneFactory))
        {
        	return VK_FALSE;
        }
    }

	// Process skeletons.
    for (uint32_t i = 0; i < gltfNode.skeletonsPointer.size(); i++)
    {
        auto childNode = sceneFactory->createNode(sceneManager);

        if (!childNode.get())
        {
            return VK_FALSE;
        }

        childNode->setName(gltfNode.skeletons[i]);

        childNode->setParentNode(node);

        //

        node->addChildNode(childNode);

        //

        if (!gltfProcessNode(childNode, visitor, *(gltfNode.skeletonsPointer[i]), sceneManager, sceneFactory))
        {
        	return VK_FALSE;
        }
    }

    //
    // Process animations.
    //

    IAnimationSP animation;

    for (uint32_t animationIndex = 0; animationIndex < visitor.getAllGltfAnimations().values().size(); animationIndex++)
    {
    	const auto& gltfAnimation = visitor.getAllGltfAnimations().values()[animationIndex];

    	for (uint32_t channelIndex = 0; channelIndex < gltfAnimation.channels.size(); channelIndex++)
    	{
    		if (gltfAnimation.channels[channelIndex].targetNode == &gltfNode)
    		{
    			const auto& gltfChannel = gltfAnimation.channels[channelIndex];

    			if (!gltfChannel.sampler)
    			{
    				return VK_FALSE;
    			}

    			if (!animation.get())
    			{
    	            animation = sceneFactory->createAnimation(sceneManager);

    	            if (!animation.get())
    	            {
    	                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not created: '%s'", visitor.getAllGltfAnimations().keys()[animationIndex].c_str(), VKTS_MAX_TOKEN_CHARS);

    	                return VK_FALSE;
    	            }

    	            animation->setName(visitor.getAllGltfAnimations().values()[animationIndex].name);

    	            sceneManager->addAnimation(animation);
    			}

                //

                auto* timeAccessor = gltfChannel.sampler->input;
                auto* targetAccessor = gltfChannel.sampler->output;

                if (!timeAccessor || !targetAccessor)
                {
                	return VK_FALSE;
                }

                if (timeAccessor->minFloat.size() == 0 || timeAccessor->maxFloat.size() == 0)
                {
                	return VK_FALSE;
                }

				if (timeAccessor->count != targetAccessor->count)
				{
                	return VK_FALSE;
				}

				if (!visitor.isFloat(timeAccessor->componentType) || !visitor.isFloat(targetAccessor->componentType))
				{
                	return VK_FALSE;
				}

                //

                if (1 != visitor.getComponentsPerType(timeAccessor->type))
                {
                	return VK_FALSE;
                }


                VkTsTargetTransform targetTransform;

                uint32_t targetTransformElementCount = 0;

                if (gltfChannel.targetPath == "translation")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_TRANSLATE;

                	targetTransformElementCount = 3;
                }
                else if (gltfChannel.targetPath == "rotation")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE;

                	targetTransformElementCount = 4;
                }
                else if (gltfChannel.targetPath == "scale")
                {
                	targetTransform = VKTS_TARGET_TRANSFORM_SCALE;

                	targetTransformElementCount = 3;
                }
                else
                {
                	return VK_FALSE;
                }

                if (targetTransformElementCount != visitor.getComponentsPerType(targetAccessor->type))
                {
                	return VK_FALSE;
                }

                //

                VkTsInterpolator interpolator;

                if (gltfChannel.sampler->interpolation == "STEP")
                {
                	interpolator = VKTS_INTERPOLATOR_CONSTANT;
                }
                else if (gltfChannel.sampler->interpolation == "LINEAR")
                {
                	interpolator = VKTS_INTERPOLATOR_LINEAR;
                }
                else
                {
                	return VK_FALSE;
                }

                //

                animation->setStart(glm::min(animation->getStart(), timeAccessor->minFloat[0]));
                animation->setStop(glm::max(animation->getStop(), timeAccessor->maxFloat[0]));

    			//
    			// Process channel.
    			//

                for (uint32_t targetTransformElementIndex = 0; targetTransformElementIndex < targetTransformElementCount; targetTransformElementIndex++)
                {
					auto channel = sceneFactory->createChannel(sceneManager);

					std::string channelName = gltfAnimation.name + "_" + gltfChannel.name + "_" + gltfChannel.targetPath + "_" + std::to_string(targetTransformElementIndex);

					if (!channel.get())
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not created: '%s'", channelName.c_str(), VKTS_MAX_TOKEN_CHARS);

						return VK_FALSE;
					}

					channel->setName(channelName);

					channel->setTargetTransform(targetTransform);

					channel->setTargetTransformElement((VkTsTargetTransformElement)targetTransformElementIndex);

					//

					sceneManager->addChannel(channel);

					//

					for (uint32_t entryIndex = 0; entryIndex < gltfChannel.sampler->input->count; entryIndex++)
					{
						const float* key = visitor.getFloatPointer(*gltfChannel.sampler->input, entryIndex);
						const float* value = visitor.getFloatPointer(*gltfChannel.sampler->output, entryIndex);

						if (!key || !value)
						{
							return VK_FALSE;
						}

						channel->addEntry(*key, value[targetTransformElementIndex], glm::vec4(*key - 0.1f, value[targetTransformElementIndex], *key + 0.1f, *value), interpolator);
					}
                }
    		}
    	}
    }

	return VK_TRUE;
}

static VkBool32 gltfProcessObject(IObjectSP& object, const GltfVisitor& visitor, const GltfScene& gltfScene, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	// Process root node.

    for (uint32_t i = 0; i < gltfScene.nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene.nodes[i];

    	if (gltfNode->name == object->getName())
    	{
            auto node = sceneFactory->createNode(sceneManager);

            if (!node.get())
            {
                return VK_FALSE;
            }

            node->setName(gltfNode->name);

            //

            if (!gltfProcessNode(node, visitor, *gltfNode, sceneManager, sceneFactory))
            {
            	return VK_FALSE;
            }

            //

            object->setRootNode(node);
    	}
    }

	return VK_TRUE;
}

ISceneSP VKTS_APIENTRY gltfLoad(const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, const VkBool32 freeHostMemory)
{
    if (!filename || !sceneManager.get() || !sceneFactory.get())
    {
        return ISceneSP();
    }

	auto textFile = fileLoadText(filename);

	if (!textFile.get())
	{
		return ISceneSP();
	}

	auto json = jsonDecode(textFile->getString());

	if (!json.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Parsing JSON failed");

		return ISceneSP();
	}

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fileGetDirectory(directory, filename);

	GltfVisitor visitor(directory);

	json->visit(visitor);

	if (visitor.getState() != GltfState_End)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Processing glTF failed");

		return ISceneSP();
	}

	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Processing glTF succeeded");

	//
	// Scene.
	//

	const GltfScene* gltfScene = visitor.getDefaultScene();

	if (!gltfScene && visitor.getAllGltfScenes().size() > 0)
	{
		gltfScene = &(visitor.getAllGltfScenes().valueAt(0));
	}

	if (!gltfScene)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No glTF scene found");

		return ISceneSP();
	}

    auto scene = sceneFactory->createScene(sceneManager);

    if (!scene.get())
    {
        return ISceneSP();
    }

    scene->setName(gltfScene->name);

    //
    // Objects: Create out of every root node an object, that it can be moved around independently.
    //

    for (uint32_t i = 0; i < gltfScene->nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene->nodes[i];

    	VkBool32 isRoot = VK_TRUE;

        for (uint32_t k = 0; k < gltfScene->nodes.size(); k++)
        {
        	if (k == i)
        	{
        		continue;
        	}

        	auto* testGltfNode = gltfScene->nodes[k];

            for (uint32_t m = 0; m < testGltfNode->childrenPointer.size(); m++)
            {
            	if (gltfNode == testGltfNode->childrenPointer[m])
            	{
            		isRoot = VK_FALSE;

            		break;
            	}
            }

            if (isRoot)
            {
                for (uint32_t m = 0; m < testGltfNode->skeletonsPointer.size(); m++)
                {
                	if (gltfNode == testGltfNode->skeletonsPointer[m])
                	{
                		isRoot = VK_FALSE;

                		break;
                	}
                }
            }

            if (!isRoot)
            {
            	break;
            }
        }

        if (!isRoot)
        {
        	continue;
        }

    	//

        auto object = sceneFactory->createObject(sceneManager);

        if (!object.get())
        {
        	return ISceneSP();
        }

        object->setName(gltfNode->name);

        sceneManager->addObject(object);

        //

        if (!gltfProcessObject(object, visitor, *gltfScene, sceneManager, sceneFactory))
        {
        	return ISceneSP();
        }

        //

        auto currentObject = sceneManager->useObject(gltfNode->name);

        if (!currentObject.get())
        {
            return ISceneSP();
        }

        scene->addObject(currentObject);
    }

    //
    // Free host memory if wanted.
    //
    if (freeHostMemory)
    {
        for (uint32_t i = 0; i < sceneManager->getAllImageDatas().values().size(); i++)
        {
        	sceneManager->getAllImageDatas().values()[i]->freeHostMemory();
        }
    }

    return scene;
}

}
