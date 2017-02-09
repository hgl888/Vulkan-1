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

#ifndef VKTS_CHAR_HPP_
#define VKTS_CHAR_HPP_

#include <vkts/gui/vkts_gui.hpp>

namespace vkts
{

class Char: public IChar
{

private:

	int32_t id;

	float x;
	float y;

	float width;
	float height;

	float xoffset;
	float yoffset;

	float xadvance;

	std::map<int32_t, float> allKernings;

public:

    Char();
    Char(const Char& other);
    Char(Char&& other) = delete;
    ~Char();

    Char& operator =(const Char& other);
    Char& operator =(Char && other) = delete;

	virtual int32_t getId() const override;
	void setId(const int32_t id);

	virtual float getX() const override;
	void setX(const float x);

	virtual float getY() const override;
	void setY(const float y);

	virtual float getWidth() const override;
	void setWidth(const float width);

	virtual float getHeight() const override;
	void setHeight(const float height);

	virtual float getXoffset() const override;
	void setXoffset(const float xoffset);

	virtual float getYoffset() const override;
	void setYoffset(const float yoffset);

	virtual float getXadvance() const override;
	void setXadvance(const float xadvance);

	virtual float getKerning(const int32_t characterId) const override;
	void setKerning(const int32_t characterId, const float amount);

};

} /* namespace vkts */

#endif /* VKTS_CHAR_HPP_ */
