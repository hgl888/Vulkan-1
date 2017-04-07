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

static float signum(const float f)
{
	return f >= 0.0f ? 1.0f : -1.0f;
}

glm::mat4 VKTS_APIENTRY translateMat4(const float x, const float y, const float z)
{
    glm::mat4 matrix(1.0f);

    matrix[3][0] = x;
    matrix[3][1] = y;
    matrix[3][2] = z;

    return matrix;
}

glm::mat3 VKTS_APIENTRY translateMat3(const float x, const float y)
{
    glm::mat3 matrix(1.0f);

    matrix[2][0] = x;
    matrix[2][1] = y;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateAxisMat4(const float angle, const float x, const float y, const float z)
{
    glm::mat4 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    glm::vec3 v(x, y, z);

    v = glm::normalize(v);

    matrix[0][0] = v.x * v.x * (1 - c) + c;
    matrix[0][1] = v.x * v.y * (1 - c) + v.z * s;
    matrix[0][2] = v.x * v.z * (1 - c) - v.y * s;

    matrix[1][0] = v.x * v.y * (1 - c) - v.z * s;
    matrix[1][1] = v.y * v.y * (1 - c) + c;
    matrix[1][2] = v.y * v.z * (1 - c) + v.x * s;

    matrix[2][0] = v.x * v.z * (1 - c) + v.y * s;
    matrix[2][1] = v.y * v.z * (1 - c) - v.x * s;
    matrix[2][2] = v.z * v.z * (1 - c) + c;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateAxisMat3(const float angle, const float x, const float y, const float z)
{
    glm::mat3 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    glm::vec3 v(x, y, z);

    v = glm::normalize(v);

    matrix[0][0] = v.x * v.x * (1 - c) + c;
    matrix[0][1] = v.x * v.y * (1 - c) + v.z * s;
    matrix[0][2] = v.x * v.z * (1 - c) - v.y * s;
    matrix[1][0] = v.x * v.y * (1 - c) - v.z * s;
    matrix[1][1] = v.y * v.y * (1 - c) + c;
    matrix[1][2] = v.y * v.z * (1 - c) + v.x * s;
    matrix[2][0] = v.x * v.z * (1 - c) + v.y * s;
    matrix[2][1] = v.y * v.z * (1 - c) - v.x * s;
    matrix[2][2] = v.z * v.z * (1 - c) + c;

    return matrix;
}

glm::mat2 VKTS_APIENTRY rotateAxisMat2(const float angle)
{
    glm::mat2 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[0][0] = c;
    matrix[0][1] = s;

    matrix[1][0] = -s;
    matrix[1][1] = c;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRxMat4(const float angle)
{
    glm::mat4 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[1][1] = c;
    matrix[1][2] = s;

    matrix[2][1] = -s;
    matrix[2][2] = c;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRxMat3(const float angle)
{
    glm::mat3 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[1][1] = c;
    matrix[1][2] = s;

    matrix[2][1] = -s;
    matrix[2][2] = c;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRyMat4(const float angle)
{
    glm::mat4 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[0][0] = c;
    matrix[0][2] = -s;

    matrix[2][0] = s;
    matrix[2][2] = c;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRyMat3(const float angle)
{
    glm::mat3 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[0][0] = c;
    matrix[0][2] = -s;

    matrix[2][0] = s;
    matrix[2][2] = c;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRzMat4(const float angle)
{
    glm::mat4 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[0][0] = c;
    matrix[0][1] = s;

    matrix[1][0] = -s;
    matrix[1][1] = c;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRzMat3(const float angle)
{
    glm::mat3 matrix(1.0f);

    float s = sinf(glm::radians(angle));
    float c = cosf(glm::radians(angle));

    matrix[0][0] = c;
    matrix[0][1] = s;

    matrix[1][0] = -s;
    matrix[1][1] = c;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRzRxRyMat4(const float anglez, const float anglex, const float angley)
{
    glm::mat4 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz - sx * sy * sz;
    matrix[0][1] = cz * sx * sy + cy * sz;
    matrix[0][2] = -cx * sy;

    matrix[1][0] = -cx * sz;
    matrix[1][1] = cx * cz;
    matrix[1][2] = sx;

    matrix[2][0] = cz * sy + cy * sx * sz;
    matrix[2][1] = -cy * cz * sx + sy * sz;
    matrix[2][2] = cx * cy;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRzRyRxMat4(const float anglez, const float angley, const float anglex)
{
    glm::mat4 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz;
    matrix[0][1] = cy * sz;
    matrix[0][2] = -sy;

    matrix[1][0] = -cx * sz + cz * sx * sy;
    matrix[1][1] = cx * cz + sx * sy * sz;
    matrix[1][2] = cy * sx;

    matrix[2][0] = sz * sx + cx * cz * sy;
    matrix[2][1] = -cz * sx + cx * sy * sz;
    matrix[2][2] = cx * cy;

    return matrix;
}

glm::mat4 VKTS_APIENTRY rotateRyRzRxMat4(const float angley, const float anglez, const float anglex)
{
    glm::mat4 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz;
    matrix[0][1] = sz;
    matrix[0][2] = -cz * sy;

    matrix[1][0] = sx * sy - cx * cy * sz;
    matrix[1][1] = cx * cz;
    matrix[1][2] = cy * sx + cx * sy * sz;

    matrix[2][0] = cx * sy + cy * sx * sz;
    matrix[2][1] = -cz * sx;
    matrix[2][2] = cx * cy - sx * sy * sz;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRzRxRyMat3(const float anglez, const float anglex, const float angley)
{
    glm::mat3 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz - sx * sy * sz;
    matrix[0][1] = cz * sx * sy + cy * sz;
    matrix[0][2] = -cx * sy;

    matrix[1][0] = -cx * sz;
    matrix[1][1] = cx * cz;
    matrix[1][2] = sx;

    matrix[2][0] = cz * sy + cy * sx * sz;
    matrix[2][1] = -cy * cz * sx + sy * sz;
    matrix[2][2] = cx * cy;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRzRyRxMat3(const float anglez, const float angley, const float anglex)
{
    glm::mat3 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz;
    matrix[0][1] = cy * sz;
    matrix[0][2] = -sy;

    matrix[1][0] = -cx * sz + cz * sx * sy;
    matrix[1][1] = cx * cz + sx * sy * sz;
    matrix[1][2] = cy * sx;

    matrix[2][0] = sz * sx + cx * cz * sy;
    matrix[2][1] = -cz * sx + cx * sy * sz;
    matrix[2][2] = cx * cy;

    return matrix;
}

glm::mat3 VKTS_APIENTRY rotateRyRzRxMat3(const float angley, const float anglez, const float anglex)
{
    glm::mat3 matrix(1.0f);

    float rx = glm::radians(anglex);
    float ry = glm::radians(angley);
    float rz = glm::radians(anglez);

    float sx = sinf(rx);
    float cx = cosf(rx);
    float sy = sinf(ry);
    float cy = cosf(ry);
    float sz = sinf(rz);
    float cz = cosf(rz);

    matrix[0][0] = cy * cz;
    matrix[0][1] = sz;
    matrix[0][2] = -cz * sy;

    matrix[1][0] = sx * sy - cx * cy * sz;
    matrix[1][1] = cx * cz;
    matrix[1][2] = cy * sx + cx * sy * sz;

    matrix[2][0] = cx * sy + cy * sx * sz;
    matrix[2][1] = -cz * sx;
    matrix[2][2] = cx * cy - sx * sy * sz;

    return matrix;
}

glm::mat4 VKTS_APIENTRY scaleMat4(const float x, const float y, const float z)
{
    glm::mat4 matrix(1.0f);

    matrix[0][0] = x;
    matrix[1][1] = y;
    matrix[2][2] = z;

    return matrix;
}

glm::mat3 VKTS_APIENTRY scaleMat3(const float x, const float y, const float z)
{
    glm::mat3 matrix(1.0f);

    matrix[0][0] = x;
    matrix[1][1] = y;
    matrix[2][2] = z;

    return matrix;
}

glm::mat2 VKTS_APIENTRY scaleMat2(const float x, const float y)
{
    glm::mat2 matrix(1.0f);

    matrix[0][0] = x;
    matrix[1][1] = y;

    return matrix;
}

glm::mat4 VKTS_APIENTRY shearMat4(const float shxy, const float shxz, const float shyx, const float shyz, const float shzx, const float shzy)
{
    glm::mat4 matrix(1.0f);

    matrix[1][0] = shxy;
    matrix[2][0] = shxz;

    matrix[0][1] = shyx;
    matrix[2][1] = shyz;

    matrix[0][2] = shzx;
    matrix[1][2] = shzy;

    return matrix;
}

glm::mat3 VKTS_APIENTRY shearMat3(const float shxy, const float shxz, const float shyx, const float shyz, const float shzx, const float shzy)
{
    glm::mat3 matrix(1.0f);

    matrix[1][0] = shxy;
    matrix[2][0] = shxz;

    matrix[0][1] = shyx;
    matrix[2][1] = shyz;

    matrix[0][2] = shzx;
    matrix[1][2] = shzy;

    return matrix;
}

glm::mat2 VKTS_APIENTRY shearMat2(const float shx, const float shy)
{
    glm::mat2 matrix(1.0f);

    matrix[1][0] = shx;

    matrix[0][1] = shy;

    return matrix;
}

//

glm::vec3 VKTS_APIENTRY decomposeTranslate(const glm::mat4& matrix)
{
	return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

glm::vec2 VKTS_APIENTRY decomposeTranslate(const glm::mat3& matrix)
{
	return glm::vec2(matrix[2][0], matrix[2][1]);
}

glm::vec3 VKTS_APIENTRY decomposeRotateRzRxRy(const glm::mat4& matrix)
{
	return decomposeRotateRzRxRy(glm::mat3(matrix));
}

glm::vec3 VKTS_APIENTRY decomposeRotateRzRxRy(const glm::mat3& matrix)
{
	glm::vec3 rotation;

	glm::vec3 scale = decomposeScale(matrix);

	rotation[0] = glm::degrees(asinf(matrix[1][2] / scale[1]));
	rotation[1] = glm::degrees(atan2f(-matrix[0][2] / scale[0], matrix[2][2] / scale[2]));
	rotation[2] = glm::degrees(atan2f(-matrix[1][0] / scale[1], matrix[1][1] / scale[1]));

	// Bring between [-180.0;180.0[
	if ((rotation[1] <= -90.0f || rotation[1] > 90.0f) && (rotation[2] <= -90.0f || rotation[2] > 90.0f))
	{
		if (rotation[0] >= 0.0f)
		{
			rotation[0] = 180.0f - rotation[0];
		}
		else
		{
			rotation[0] = -180.0f - rotation[0];
		}

		rotation[1] -= 180.0f;
		if (rotation[1] < -180.0f)
		{
			rotation[1] += 360.0f;
		}

		rotation[2] -= 180.0f;
		if (rotation[2] < -180.0f)
		{
			rotation[2] += 360.0f;
		}
	}

	return rotation;
}

glm::vec3 VKTS_APIENTRY decomposeRotateRzRyRx(const glm::mat4& matrix)
{
	return decomposeRotateRzRyRx(glm::mat3(matrix));
}

glm::vec3 VKTS_APIENTRY decomposeRotateRzRyRx(const glm::mat3& matrix)
{
	glm::vec3 rotation;

	glm::vec3 scale = decomposeScale(matrix);

	rotation[0] = glm::degrees(atan2f(matrix[1][2] / scale[1], matrix[2][2] / scale[2]));
	rotation[1] = glm::degrees(asinf(-matrix[0][2] / scale[0]));
	rotation[2] = glm::degrees(atan2f(matrix[0][1] / scale[0], matrix[0][0] / scale[0]));

	// Bring between [-180.0;180.0[
	if ((rotation[0] <= -90.0f || rotation[0] > 90.0f) && (rotation[2] <= -90.0f || rotation[2] > 90.0f))
	{
		rotation[0] -= 180.0f;
		if (rotation[0] < -180.0f)
		{
			rotation[0] += 360.0f;
		}

		if (rotation[1] >= 0.0f)
		{
			rotation[1] = 180.0f - rotation[1];
		}
		else
		{
			rotation[1] = -180.0f - rotation[1];
		}

		rotation[2] -= 180.0f;
		if (rotation[2] < -180.0f)
		{
			rotation[2] += 360.0f;
		}
	}

	return rotation;
}

glm::vec3 VKTS_APIENTRY decomposeRotateRyRzRx(const glm::mat4& matrix)
{
	return decomposeRotateRzRyRx(glm::mat3(matrix));
}

glm::vec3 VKTS_APIENTRY decomposeRotateRyRzRx(const glm::mat3& matrix)
{
	glm::vec3 rotation;

	glm::vec3 scale = decomposeScale(matrix);

	rotation[0] = glm::degrees(atan2f(-matrix[2][1] / scale[2], matrix[1][1] / scale[1]));
	rotation[1] = glm::degrees(atan2f(-matrix[0][2] / scale[0], matrix[0][0] / scale[0]));
	rotation[2] = glm::degrees(asinf(matrix[0][1] / scale[0]));

	// Bring between [-180.0;180.0[
	if ((rotation[0] <= -90.0f || rotation[0] > 90.0f) && (rotation[2] <= -90.0f || rotation[2] > 90.0f))
	{
		rotation[0] -= 180.0f;
		if (rotation[0] < -180.0f)
		{
			rotation[0] += 360.0f;
		}

		if (rotation[1] >= 0.0f)
		{
			rotation[1] = 180.0f - rotation[1];
		}
		else
		{
			rotation[1] = -180.0f - rotation[1];
		}

		rotation[2] -= 180.0f;
		if (rotation[2] < -180.0f)
		{
			rotation[2] += 360.0f;
		}
	}

	return rotation;
}

float VKTS_APIENTRY decomposeRotate(const glm::mat3& matrix)
{
	glm::vec2 scale = decomposeScale(glm::mat2(matrix));

	if (scale.x != 0.0f)
	{
		return glm::degrees(acosf(matrix[0][0] / scale.x));
	}

	return 0.0f;
}

glm::vec3 VKTS_APIENTRY decomposeScale(const glm::mat4& matrix)
{
	return decomposeScale(glm::mat3(matrix));
}

glm::vec3 VKTS_APIENTRY decomposeScale(const glm::mat3& matrix)
{
	glm::vec3 scale;

	float sign = signum(glm::determinant(matrix));

	for (int32_t i = 0; i < 3; i++)
	{
		scale[i] = sqrtf(matrix[i][0] * matrix[i][0] + matrix[i][1] * matrix[i][1] + matrix[i][2] * matrix[i][2]);

		if (sign < 0.0f)
		{
			scale[i] *= signum(matrix[i][i]);
		}
	}

	return scale;
}

glm::vec2 VKTS_APIENTRY decomposeScale(const glm::mat2& matrix)
{
	glm::vec2 scale;

	float sign = signum(glm::determinant(matrix));

	for (int32_t i = 0; i < 2; i++)
	{
		scale[i] = sqrtf(matrix[i][0] * matrix[i][0] + matrix[i][1] * matrix[i][1]);

		if (sign < 0.0f)
		{
			scale[i] *= signum(matrix[i][i]);
		}
	}

	return scale;
}

}
