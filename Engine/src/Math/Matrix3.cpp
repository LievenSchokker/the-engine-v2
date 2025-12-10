#include "Math/Matrix3.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Matrix3::Matrix3()
{
	m.fill(0.0f);
	m[0] = 1.0f;  // m00
	m[4] = 1.0f;  // m11
	m[8] = 1.0f;  // m22
}

Matrix3::Matrix3(float m00, float m01, float m02, float m10, float m11,
				 float m12, float m20, float m21, float m22)
{
	m[0] = m00;
	m[1] = m01;
	m[2] = m02;
	m[3] = m10;
	m[4] = m11;
	m[5] = m12;
	m[6] = m20;
	m[7] = m21;
	m[8] = m22;
}

Matrix3 Matrix3::identity()
{
	return Matrix3();
}

Matrix3 Matrix3::translation(const Vector2& translation)
{
	Matrix3 result = identity();
	result.m[2] = translation.x;  // m02
	result.m[5] = translation.y;  // m12
	return result;
}

Matrix3 Matrix3::rotation(double angleDegrees)
{
	const double angleRadians = angleDegrees * M_PI / 180.0;
	const float cosA = static_cast<float>(std::cos(angleRadians));
	const float sinA = static_cast<float>(std::sin(angleRadians));

	Matrix3 result = identity();
	result.m[0] = cosA;	  // m00
	result.m[1] = -sinA;  // m01
	result.m[3] = sinA;	  // m10
	result.m[4] = cosA;	  // m11
	return result;
}

Matrix3 Matrix3::scale(const Vector2& scale)
{
	Matrix3 result = identity();
	result.m[0] = scale.x;	// m00
	result.m[4] = scale.y;	// m11
	return result;
}

Matrix3 Matrix3::transform(const Vector2& position, double rotationDegrees,
						   const Vector2& scale)
{
	const double angleRadians = rotationDegrees * M_PI / 180.0;
	const float cosA = static_cast<float>(std::cos(angleRadians));
	const float sinA = static_cast<float>(std::sin(angleRadians));

	Matrix3 result;
	// Scale and rotation combined
	result.m[0] = scale.x * cosA;	// m00
	result.m[1] = -scale.y * sinA;	// m01
	result.m[2] = position.x;		// m02 (translation x)
	result.m[3] = scale.x * sinA;	// m10
	result.m[4] = scale.y * cosA;	// m11
	result.m[5] = position.y;		// m12 (translation y)
	result.m[6] = 0.0f;				// m20
	result.m[7] = 0.0f;				// m21
	result.m[8] = 1.0f;				// m22
	return result;
}

Matrix3 Matrix3::operator*(const Matrix3& other) const
{
	Matrix3 result;
	for ( int row = 0; row < 3; ++row )
	{
		for ( int col = 0; col < 3; ++col )
		{
			float sum = 0.0f;
			for ( int k = 0; k < 3; ++k )
			{
				sum += get(row, k) * other.get(k, col);
			}
			result.set(row, col, sum);
		}
	}
	return result;
}

Vector2 Matrix3::operator*(const Vector2& v) const
{
	// Treat vector as homogeneous point [x, y, 1]
	const float x = m[0] * v.x + m[1] * v.y + m[2];
	const float y = m[3] * v.x + m[4] * v.y + m[5];
	return Vector2(x, y);
}

float Matrix3::get(int row, int col) const
{
	return m[row * 3 + col];
}

void Matrix3::set(int row, int col, float value)
{
	m[row * 3 + col] = value;
}

Vector2 Matrix3::extractTranslation() const
{
	return Vector2(m[2], m[5]);	 // m02, m12
}

double Matrix3::extractRotation() const
{
	// Extract rotation from the upper-left 2x2 matrix
	// For a rotation matrix: [cos -sin] [sin cos]
	const float m00 = m[0];
	const float m10 = m[3];
	const float angleRadians = std::atan2(m10, m00);
	return angleRadians * 180.0 / M_PI;
}

Vector2 Matrix3::extractScale() const
{
	// Extract scale from the upper-left 2x2 matrix
	// Scale is the magnitude of the first two column vectors
	const float scaleX = std::sqrt(m[0] * m[0] + m[3] * m[3]);
	const float scaleY = std::sqrt(m[1] * m[1] + m[4] * m[4]);
	return Vector2(scaleX, scaleY);
}

Matrix3 Matrix3::inverse() const
{
	// For a 2D transformation matrix, we can use a simplified inverse
	// [a c tx]^-1   [a' c' -a'*tx - c'*ty]
	// [b d ty]    = [b' d' -b'*tx - d'*ty]
	// [0 0 1 ]      [0  0   1              ]
	// where [a' c'] = [a c]^-1
	//       [b' d']   [b d]

	const float a = m[0], b = m[3], c = m[1], d = m[4];
	const float tx = m[2], ty = m[5];

	// Determinant of the 2x2 rotation/scale matrix
	const float det = a * d - b * c;

	if ( std::abs(det) < 1e-6f )
	{
		// Singular matrix, return identity
		return identity();
	}

	const float invDet = 1.0f / det;
	const float aInv = d * invDet;
	const float bInv = -b * invDet;
	const float cInv = -c * invDet;
	const float dInv = a * invDet;

	Matrix3 result;
	result.m[0] = aInv;
	result.m[1] = cInv;
	result.m[2] = -(aInv * tx + cInv * ty);
	result.m[3] = bInv;
	result.m[4] = dInv;
	result.m[5] = -(bInv * tx + dInv * ty);
	result.m[6] = 0.0f;
	result.m[7] = 0.0f;
	result.m[8] = 1.0f;

	return result;
}

Matrix3 Matrix3::transpose() const
{
	Matrix3 result;
	for ( int row = 0; row < 3; ++row )
	{
		for ( int col = 0; col < 3; ++col )
		{
			result.set(row, col, get(col, row));
		}
	}
	return result;
}
