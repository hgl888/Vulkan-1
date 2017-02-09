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

#ifndef VKTS_SMARTPOINTERVECTOR_HPP_
#define VKTS_SMARTPOINTERVECTOR_HPP_

#include <vkts/core/vkts_core.hpp>

namespace vkts
{

template<class V>
class SmartPointerVector
{

protected:

    V* allData;

    uint32_t topElement;
    uint32_t allDataCount;

    uint32_t getAllocSize()
    {
    	uint32_t tempAllDataCount = allDataCount * 2;

    	return tempAllDataCount > 0 ? tempAllDataCount : 1;
    }

public:

    SmartPointerVector() :
        SmartPointerVector(0)
    {

    }

    SmartPointerVector(const uint32_t allDataCount) :
        allData(nullptr), topElement(0), allDataCount(0)
    {
        allData = new V[allDataCount];

        if (!allData)
        {
        	throw std::bad_alloc();
        }

        for (uint32_t i = 0; i < allDataCount; i++)
        {
            allData[i] = nullptr;
        }

        this->topElement = allDataCount;
        this->allDataCount = allDataCount;
    }

    SmartPointerVector(const SmartPointerVector& other) :
        allData(nullptr), topElement(0), allDataCount(0)
    {
        allData = new V[other.allDataCount];

        if (!allData)
        {
        	throw std::bad_alloc();
        }

        for (uint32_t i = 0; i < other.allDataCount; i++)
        {
            allData[i] = nullptr;
            allData[i] = other.allData[i];
        }

        this->topElement = other.topElement;
        this->allDataCount = other.allDataCount;
    }

    SmartPointerVector(SmartPointerVector&& other) :
        allData(other.allData), topElement(other.topElement), allDataCount(other.allDataCount)
    {
        other.allData = nullptr;
        other.topElement = 0;
        other.allDataCount = 0;
    }

    SmartPointerVector& operator= (const SmartPointerVector& other)
    {
    	clear();

        if (allData)
        {
            delete[] allData;

            allData = nullptr;
        }

        topElement = 0;
        allDataCount = 0;

    	//

        allData = new V[other.allDataCount];

        if (!allData)
        {
        	throw std::bad_alloc();
        }

        for (uint32_t i = 0; i < other.allDataCount; i++)
        {
            allData[i] = nullptr;
            allData[i] = other.allData[i];
        }

        this->topElement = other.topElement;
        this->allDataCount = other.allDataCount;

        //

    	return *this;
    }

    SmartPointerVector& operator= (SmartPointerVector&& other)
    {
    	clear();

        if (allData)
        {
            delete[] allData;

            allData = nullptr;
        }

        topElement = 0;
        allDataCount = 0;

    	//

        allData = other.allData;
        topElement = other.topElement;
        allDataCount = other.allDataCount;

        other.allData = nullptr;
        other.topElement = 0;
        other.allDataCount = 0;

        //

    	return *this;
    }

    ~SmartPointerVector()
    {
        clear();

        if (allData)
        {
            delete[] allData;

            allData = nullptr;
        }

        topElement = 0;
        allDataCount = 0;
    }

    void clear()
    {
        for (uint32_t i = 0; i < topElement; i++)
        {
            allData[i].reset();
        }

        topElement = 0;
    }

    void append(const V& value)
    {
        if (topElement >= allDataCount)
        {
            V* newAllData = new V[getAllocSize()];

            if (!newAllData)
            {
            	throw std::bad_alloc();
            }

            for (uint32_t i = 0; i < getAllocSize(); i++)
            {
                newAllData[i] = nullptr;

                if (i < allDataCount)
                {
                    newAllData[i] = allData[i];
                    allData[i].reset();
                }
            }

            delete[] allData;

            allData = newAllData;

            allDataCount = getAllocSize();
        }

        allData[topElement] = value;
        topElement++;
    }

    VkBool32 insert(const uint32_t index, const V& value)
    {
        if (index > topElement)
        {
            return VK_FALSE;
        }

        if (index == topElement)
        {
            append(value);

            return VK_TRUE;
        }

        if (topElement + 1 >= allDataCount)
        {
            V* newAllData = new V[getAllocSize()];

            if (!newAllData)
            {
            	throw std::bad_alloc();
            }

            for (uint32_t i = 0; i < getAllocSize(); i++)
            {
                newAllData[i] = nullptr;

                if (i < allDataCount)
                {
                    newAllData[i] = allData[i];
                    allData[i].reset();
                }
            }

            delete[] allData;

            allData = newAllData;

            allDataCount = getAllocSize();
        }

        for (uint32_t copyIndex = topElement; copyIndex >= index + 1; copyIndex--)
        {
            allData[copyIndex] = allData[copyIndex - 1];
        }

        allData[index] = value;
        topElement++;

        return VK_TRUE;
    }

    VkBool32 remove(const V& value)
    {
        for (uint32_t i = 0; i < topElement; i++)
        {
            if (allData[i] == value)
            {
                allData[i].reset();

                allData[i] = nullptr;

                if (i == topElement - 1)
                {
                    topElement--;

                    return VK_TRUE;
                }

                for (uint32_t copyIndex = i; copyIndex < topElement - 1; copyIndex++)
                {
                    allData[copyIndex] = allData[copyIndex + 1];
                    allData[copyIndex + 1].reset();
                }

                topElement--;

                return VK_TRUE;
            }
        }

        return VK_FALSE;
    }

    VkBool32 removeAt(const uint32_t index)
    {
        if (index >= topElement)
        {
            return VK_FALSE;
        }

        return remove(allData[index]);
    }

    VkBool32 contains(const V& value) const
    {
        for (uint32_t i = 0; i < topElement; i++)
        {
            if (value == allData[i])
            {
                return VK_TRUE;
            }
        }

        return VK_FALSE;
    }

    V& operator[](const uint32_t index)
    {
    	if (index > topElement)
    	{
    		throw std::out_of_range(std::to_string(index) + " > " + std::to_string(topElement));
    	}
    	else if (index == topElement)
    	{
    		// Allow to append at the end.
    		V v;

    		append(v);
    	}

        return allData[index];
    }

    const V& operator[](const uint32_t index) const
    {
    	if (index >= topElement)
    	{
    		throw std::out_of_range(std::to_string(index) + " >= " + std::to_string(topElement));
    	}

    	return allData[index];
    }

    const V* data() const
    {
        return allData;
    }

    uint32_t size() const
    {
        return topElement;
    }

    uint32_t index(const V& value) const
    {
        for (uint32_t i = 0; i < topElement; i++)
        {
            if (value == allData[i])
            {
                return i;
            }
        }

        return topElement;
    }

};

}

#endif /* VKTS_SMARTPOINTERVECTOR_HPP_ */
