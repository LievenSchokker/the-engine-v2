#pragma once

#include "Math/Vector2.h"

#include <array>

/**
 * @brief A 3x3 matrix for 2D transformations using homogeneous coordinates.
 *
 * Matrix layout (row-major):
 * [m00 m01 m02]
 * [m10 m11 m12]
 * [m20 m21 m22]
 *
 * For 2D transformations:
 * [scaleX*cos(θ) -scaleY*sin(θ) tx]
 * [scaleX*sin(θ)  scaleY*cos(θ) ty]
 * [0              0               1 ]
 */
class Matrix3
{
   public:
	/**
	 * @brief Constructs an identity matrix.
	 */
	Matrix3();

	/**
	 * @brief Constructs a matrix from 9 values (row-major order).
	 */
	Matrix3(float m00, float m01, float m02, float m10, float m11, float m12,
			float m20, float m21, float m22);

	/**
	 * @brief Creates an identity matrix.
	 */
	static Matrix3 identity();

	/**
	 * @brief Creates a translation matrix.
	 * @param translation Translation vector
	 */
	static Matrix3 translation(const Vector2& translation);

	/**
	 * @brief Creates a rotation matrix.
	 * @param angleDegrees Rotation angle in degrees
	 */
	static Matrix3 rotation(double angleDegrees);

	/**
	 * @brief Creates a scale matrix.
	 * @param scale Scale vector
	 */
	static Matrix3 scale(const Vector2& scale);

	/**
	 * @brief Creates a transformation matrix from position, rotation, and
	 * scale.
	 * @param position Position vector
	 * @param rotationDegrees Rotation angle in degrees
	 * @param scale Scale vector
	 */
	static Matrix3 transform(const Vector2& position, double rotationDegrees,
							 const Vector2& scale);

	/**
	 * @brief Multiplies this matrix by another matrix.
	 * @param other The matrix to multiply with
	 * @return The result of the multiplication
	 */
	Matrix3 operator*(const Matrix3& other) const;

	/**
	 * @brief Multiplies this matrix by a vector (treating it as a point).
	 * @param v The vector to transform
	 * @return The transformed vector
	 */
	Vector2 operator*(const Vector2& v) const;

	/**
	 * @brief Gets the element at the specified row and column.
	 * @param row Row index (0-2)
	 * @param col Column index (0-2)
	 * @return The element value
	 */
	float get(int row, int col) const;

	/**
	 * @brief Sets the element at the specified row and column.
	 * @param row Row index (0-2)
	 * @param col Column index (0-2)
	 * @param value The value to set
	 */
	void set(int row, int col, float value);

	/**
	 * @brief Extracts the translation (position) from the matrix.
	 * @return The translation vector
	 */
	Vector2 extractTranslation() const;

	/**
	 * @brief Extracts the rotation angle from the matrix (in degrees).
	 * @return The rotation angle in degrees
	 */
	double extractRotation() const;

	/**
	 * @brief Extracts the scale from the matrix.
	 * @return The scale vector
	 */
	Vector2 extractScale() const;

	/**
	 * @brief Gets the inverse of this matrix.
	 * @return The inverse matrix
	 */
	Matrix3 inverse() const;

	/**
	 * @brief Gets the transpose of this matrix.
	 * @return The transposed matrix
	 */
	Matrix3 transpose() const;

   private:
	// Row-major storage: [row0, row1, row2] where each row is [m00, m01, m02],
	// [m10, m11, m12], [m20, m21, m22]
	std::array<float, 9> m;
};
