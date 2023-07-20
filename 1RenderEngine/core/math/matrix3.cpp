#include "matrix3.h"

namespace OEngine
{
	const Matrix3x3 Matrix3x3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);
	const Matrix3x3 Matrix3x3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

	void Matrix3x3::setColumn(size_t col_index, const Vector3& col)
	{
		m_mat[0][col_index] = col.x;
		m_mat[1][col_index] = col.y;
		m_mat[2][col_index] = col.z;
	}

	void Matrix3x3::fromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
	{
		setColumn(0, x_axis);
		setColumn(1, y_axis);
		setColumn(2, z_axis);
	}

	void Matrix3x3::calculateQDUDecomposition(Matrix3x3& out_Q, Vector3& out_D, Vector3& out_U) const
	{
		// QR decomposition using Gram-Schmidt orthogonalization (the QR algorithm) https://www.math.ucla.edu/~yanovsky/Teaching/Math151B/handouts/GramSchmidt.pdf
		float inv_length = m_mat[0][0] * m_mat[0][0] + m_mat[1][0] * m_mat[1][0] + m_mat[2][0] * m_mat[2][0];
		if (!Math::realEqual(inv_length, 0))
			inv_length = Math::invSqrt(inv_length);

		out_Q[0][0] = m_mat[0][0] * inv_length;
		out_Q[1][0] = m_mat[1][0] * inv_length;
		out_Q[2][0] = m_mat[2][0] * inv_length;

		float dot = out_Q[0][0] * m_mat[0][1] + out_Q[1][0] * m_mat[1][1] + out_Q[2][0] * m_mat[2][1];
		out_Q[0][1] = m_mat[0][1] - dot * out_Q[0][0];
		out_Q[1][1] = m_mat[1][1] - dot * out_Q[1][0];
		out_Q[2][1] = m_mat[2][1] - dot * out_Q[2][0];
		inv_length = out_Q[0][1] * out_Q[0][1] + out_Q[1][1] * out_Q[1][1] + out_Q[2][1] * out_Q[2][1];
		if (!Math::realEqual(inv_length, 0))
			inv_length = Math::invSqrt(inv_length);
		out_Q[0][1] *= inv_length;
		out_Q[1][1] *= inv_length;
		out_Q[2][1] *= inv_length;

		dot = out_Q[0][0] * m_mat[0][2] + out_Q[1][0] * m_mat[1][2] + out_Q[2][0] * m_mat[2][2];
		out_Q[0][2] = m_mat[0][2] - dot * out_Q[0][0];
		out_Q[1][2] = m_mat[1][2] - dot * out_Q[1][0];
		out_Q[2][2] = m_mat[2][2] - dot * out_Q[2][0];
		dot = out_Q[0][1] * m_mat[0][2] + out_Q[1][1] * m_mat[1][2] + out_Q[2][1] * m_mat[2][2];
		out_Q[0][2] -= dot * out_Q[0][1];
		out_Q[1][2] -= dot * out_Q[1][1];
		out_Q[2][2] -= dot * out_Q[2][1];
		inv_length = out_Q[0][2] * out_Q[0][2] + out_Q[1][2] * out_Q[1][2] + out_Q[2][2] * out_Q[2][2];
		if (!Math::realEqual(inv_length, 0))
			inv_length = Math::invSqrt(inv_length);
		out_Q[0][2] *= inv_length;
		out_Q[1][2] *= inv_length;
		out_Q[2][2] *= inv_length;

		float det = out_Q[0][0] * out_Q[1][1] * out_Q[2][2] + out_Q[0][1] * out_Q[1][2] * out_Q[2][0] +
			out_Q[0][2] * out_Q[1][0] * out_Q[2][1] - out_Q[0][2] * out_Q[1][1] * out_Q[2][0] -
			out_Q[0][1] * out_Q[1][0] * out_Q[2][2] - out_Q[0][0] * out_Q[1][2] * out_Q[2][1];

		if (det < 0.0)
		{
			for (size_t row_index = 0; row_index < 3; row_index++)
				for (size_t rol_index = 0; rol_index < 3; rol_index++)
					out_Q[row_index][rol_index] = -out_Q[row_index][rol_index];
		}

		// QR -> R 上三角矩阵
		Matrix3x3 R;
		R[0][0] = m_mat[0][0] * out_Q[0][0] + m_mat[1][0] * out_Q[1][0] + m_mat[2][0] * out_Q[2][0];
		R[0][1] = m_mat[0][1] * out_Q[0][0] + m_mat[1][1] * out_Q[1][0] + m_mat[2][1] * out_Q[2][0];
		R[0][2] = m_mat[0][2] * out_Q[0][0] + m_mat[1][2] * out_Q[1][0] + m_mat[2][2] * out_Q[2][0];
		R[1][1] = m_mat[0][1] * out_Q[0][1] + m_mat[1][1] * out_Q[1][1] + m_mat[2][1] * out_Q[2][1];
		R[1][2] = m_mat[0][2] * out_Q[0][1] + m_mat[1][2] * out_Q[1][1] + m_mat[2][2] * out_Q[2][1];
		R[2][2] = m_mat[0][2] * out_Q[0][2] + m_mat[1][2] * out_Q[1][2] + m_mat[2][2] * out_Q[2][2];

		// R 中取出 scaling 和 shear 的部分
		// scaling
		out_D[0] = R[0][0];
		out_D[1] = R[1][1];
		out_D[2] = R[2][2];

		// the shear
		float inv_d0 = 1.0f / out_D[0];
		out_U = R[0][1] * inv_d0;
		out_U = R[0][2] * inv_d0;
		out_U = R[1][2] / out_D[1];
	}

	// TODO
	void Matrix3x3::toAngleAxis(Vector3& axis, Radian& angel) const
	{
		// The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
		// I is the identity and
		//
		//       +-        -+
		//   P = |  0 -z +y |
		//       | +z  0 -x |
		//       | -y +x  0 |
		//       +-        -+
		//
		float trace = m_mat[0][0] + m_mat[1][1] + m_mat[2][2];
		float cos_v = 0.5f * (trace - 1.0f);
		angel = Math::acos(cos_v); // in [0,PI]

		if (angel > Radian(0.0))
		{
			if (angel < Radian(Math_PI))
			{
				axis.x = m_mat[2][1] - m_mat[1][2];
				axis.y = m_mat[0][2] - m_mat[2][0];
				axis.z = m_mat[1][0] - m_mat[0][1];
				axis.normalise();
			}
			else
			{
				// angle is PI
				float half_inv;
				if (m_mat[0][0] >= m_mat[1][1])
				{
					// r00 >= r11
					if (m_mat[0][0] >= m_mat[2][2])
					{
						// r00 is maximum diagonal term
						axis.x = 0.5f * Math::sqrt(m_mat[0][0] - m_mat[1][1] - m_mat[2][2] + 1.0f);
						half_inv = 0.5f / axis.x;
						axis.y = half_inv * m_mat[0][1];
						axis.z = half_inv * m_mat[0][2];
					}
					else
					{
						// r22 is maximum diagonal term
						axis.z = 0.5f * Math::sqrt(m_mat[2][2] - m_mat[0][0] - m_mat[1][1] + 1.0f);
						half_inv = 0.5f / axis.z;
						axis.x = half_inv * m_mat[0][2];
						axis.y = half_inv * m_mat[1][2];
					}
				}
				else
				{
					// r11 > r00
					if (m_mat[1][1] >= m_mat[2][2])
					{
						// r11 is maximum diagonal term
						axis.y = 0.5f * Math::sqrt(m_mat[1][1] - m_mat[0][0] - m_mat[2][2] + 1.0f);
						half_inv = 0.5f / axis.y;
						axis.x = half_inv * m_mat[0][1];
						axis.z = half_inv * m_mat[1][2];
					}
					else
					{
						// r22 is maximum diagonal term
						axis.z = 0.5f * Math::sqrt(m_mat[2][2] - m_mat[0][0] - m_mat[1][1] + 1.0f);
						half_inv = 0.5f / axis.z;
						axis.x = half_inv * m_mat[0][2];
						axis.y = half_inv * m_mat[1][2];
					}
				}
			}
		}
		else
		{
			// The angle is 0 and the matrix is the identity.  Any axis will
			// work, so just use the x-axis.
			axis.x = 1.0;
			axis.y = 0.0;
			axis.z = 0.0;
		}
	}

	// 这是所熟知的按照任意轴旋转角度theta的旋转矩阵
	 // 推导过程见 《3D数学基础：图形与游戏开发》第8章 R(n, theta)
	void Matrix3x3::fromAngleAxis(const Vector3& axis, const Radian& angle)
	{
		float cos_v = Math::cos(angle);
		float sin_v = Math::sin(angle);
		float one_minus_cos = 1.0f - cos_v;
		float x2 = axis.x * axis.x;
		float y2 = axis.y * axis.y;
		float z2 = axis.z * axis.z;
		float xym = axis.x * axis.y * one_minus_cos;
		float xzm = axis.x * axis.z * one_minus_cos;
		float yzm = axis.y * axis.z * one_minus_cos;
		float x_sin_v = sin_v * axis.x;
		float y_sin_v = sin_v * axis.y;
		float z_sin_v = sin_v * axis.z;

		m_mat[0][0] = x2 * one_minus_cos + cos_v;
		m_mat[0][1] = xym - z_sin_v;
		m_mat[0][2] = xzm + y_sin_v;
		m_mat[1][0] = xym + z_sin_v;
		m_mat[1][1] = y2 * one_minus_cos + cos_v;
		m_mat[1][2] = yzm - x_sin_v;
		m_mat[2][0] = xzm - y_sin_v;
		m_mat[2][1] = yzm + x_sin_v;
		m_mat[2][2] = z2 * one_minus_cos + cos_v;
	}
} // OEngine