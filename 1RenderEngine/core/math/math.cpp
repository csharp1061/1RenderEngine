#include "math.h"
#include "matrix4.h"

namespace OEngine {
	Math::AngleUnit Math::k_AngleUnit;

	Math::Math() { k_AngleUnit = AngleUnit::AU_DEGREE; }

	bool Math::realEqual(float a, float b, float tolerance)
	{
		if (fabs(a - b) <= tolerance)
			return true;
		else
			return false;
	}

	float Math::clamp(float v, float min, float max)
	{
		return (v < min) ? min : ((v > max) ? max : v);
	}

	float Math::getMaxElement(float a, float b, float c)
	{
		return std::max(a, std::max(b, c));
	}

	float Math::degreesToRadians(float degree) { return degree * Math_fDeg2Rad; }
	
	float Math::radiansToDegrees(float radian) { return radian * Math_fRad2Deg; }

	float Math::angleUnitsToRadians(float units) {
		if (k_AngleUnit == AngleUnit::AU_DEGREE)
			return units * Math_fDeg2Rad;
		else
			return units;
	}

	float Math::radiansToAngleUnits(float radian)
	{
		if (k_AngleUnit == AngleUnit::AU_DEGREE)
			return radian * Math_fRad2Deg;
		else
			return radian;
	}

	float Math::angleUnitsToDegrees(float units)
	{
		if (k_AngleUnit == AngleUnit::AU_RADIAN)
			return units * Math_fRad2Deg;
		else
			return units;
	}

	float Math::degreesToAngleUnits(float degree)
	{
		if (k_AngleUnit == AngleUnit::AU_RADIAN)
			return degree * Math_fDeg2Rad;
		else
			return degree;
	}

	Radian Math::acos(float value)
	{
		if (-1.0 < value)
		{
			if (value < 1.0)
				return Radian(acos(value));
			else
				return Radian(0.0);
		}
		else
		{
			return Radian(Math_PI);
		}
	}

	Radian Math::asin(float value)
	{
		if (-1.0 < value)
		{
			if (value < 1.0)
				return Radian(asin(value));
			else
				return Radian(Math_HALF_PI);
		}
		else
		{
			return Radian(-Math_HALF_PI);
		}
	}

	Matrix4x4 Math::makeViewMatrix(const Vector3& position, const Quaternion& orientation, const Matrix4x4* reflect_matrix)
	{
		Matrix4x4 viewMatrix;

		Matrix3x3 rot;
		orientation.toRotationMatrix(rot);

		Matrix3x3 rotT = rot.tranpose();
		Vector3 trans = -rotT * position;

		viewMatrix = Matrix4x4::IDENTITY;
		viewMatrix.setMatrix3x3(rotT);    // rotation
		
		// translation
		viewMatrix[0][3] = trans.x;		
		viewMatrix[1][3] = trans.y;
		viewMatrix[2][3] = trans.z;

		// deal with reflection
		if (reflect_matrix)
			viewMatrix = viewMatrix * (*reflect_matrix);

		return viewMatrix;
	}
	
	/*
	*   将模型坐标转换至观察空间/摄像空间的变换矩阵
	* 
	*				  [ sx, sy, sz, 0 ]	   [ 1, 0, 0, -Px]    s: 左 or 右方向
	*				  | ux, uy, uz, 0 ] *  | 0, 1, 0, -Py|    u: 上方
	*		LookAt =  | fx, fy, fz, 0 |	   | 0, 0, 1, -Pz|    f: 面向方向 从摄像机朝向物体坐标
	*				  [  0,  0,  0, 1 ]	   [ 0, 0, 0,  1 ]
	* 
	*/
	Matrix4x4 Math::makeLookAtMatrix(const Vector3& eye_position, const Vector3& target_position, const Vector3& up_dir)
	{
		const Vector3& up = up_dir.normalizedCopy();

		Vector3 f = (target_position - eye_position).normalizedCopy();
		Vector3 s = f.crossProduct(up).normalizedCopy();
		Vector3 u = s.crossProduct(f);

		Matrix4x4 view_mat = Matrix4x4::IDENTITY;

		view_mat[0][0] = s.x;
		view_mat[0][1] = s.y;
		view_mat[0][2] = s.z;
		view_mat[0][3] = -s.dotProduct(eye_position);
		view_mat[1][0] = u.x;
		view_mat[1][1] = u.y;
		view_mat[1][2] = u.z;
		view_mat[1][3] = -u.dotProduct(eye_position);
		view_mat[2][0] = -f.x;
		view_mat[2][1] = -f.y;
		view_mat[2][2] = -f.z;
		view_mat[2][3] = f.dotProduct(eye_position);
		return view_mat;
	}

	// ---------- projection 部分 ----------
	/*
	*  公式推导 TODO
	*/

	Matrix4x4 Math::makePerspectiveMatrix(Radian fovy, float aspect, float znear, float zfar)
	{
		float tan_half_fovy = Math::tan(fovy / 2.f);
		float t = fabs(znear) * tan_half_fovy;
		float r = aspect * t;

		Matrix4x4 m = Matrix4x4::IDENTITY;
		m[0][0] = znear / r;
		m[1][1] = znear / t;
		m[2][2] = (znear + zfar) / (znear - zfar);
		m[2][3] = 2 * znear * zfar / (zfar - znear);
		m[3][2] = 1;
		m[3][3] = 0;
		return m;
	}

	Matrix4x4 Math::makeOrthographicProjectionMatrix(const float left,
		const float right,
		const float bottom,
		const float top,
		const float znear,
		const float zfar)
	{
		float inv_width = 1.0f / (right - left);
		float inv_height = 1.0f / (top - bottom);
		float inv_distance = 1.0f / (zfar - znear);

		float A = 2 * inv_width;
		float B = 2 * inv_height;
		float C = -(right + left) * inv_width;
		float D = -(top + bottom) * inv_height;
		float q = -2 * inv_distance;
		float qn = qn = -(zfar + znear) * inv_distance;

		// NB: This creates 'uniform' orthographic projection matrix,
		// which depth range [-1,1], right-handed rules
		//
		// [ A   0   0   C  ]
		// [ 0   B   0   D  ]
		// [ 0   0   q   qn ]
		// [ 0   0   0   1  ]
		//
		// A = 2 * / (right - left)
		// B = 2 * / (top - bottom)
		// C = - (right + left) / (right - left)
		// D = - (top + bottom) / (top - bottom)
		// q = - 2 / (far - near)
		// qn = - (far + near) / (far - near)

		Matrix4x4 proj_matrix = Matrix4x4::ZERO;
		proj_matrix[0][0] = A;
		proj_matrix[0][3] = C;
		proj_matrix[1][1] = B;
		proj_matrix[1][3] = D;
		proj_matrix[2][2] = q;
		proj_matrix[2][3] = qn;
		proj_matrix[3][3] = 1;

		return proj_matrix;
	}

	/*
	* ----------------------视锥剔除-------------------------------
	*/
	void Math::getFrustumPlanes(std::vector<Vector4>& planes, Matrix4x4 viewPersM)
	{
		// left
		planes[0].x = viewPersM[0][3] + viewPersM[0][0];
		planes[0].y = viewPersM[1][3] + viewPersM[1][0];
		planes[0].z = viewPersM[2][3] + viewPersM[2][0];
		planes[0].w = viewPersM[3][3] + viewPersM[3][0];

		// right
		planes[1].x = viewPersM[0][3] - viewPersM[0][0];
		planes[1].y = viewPersM[1][3] - viewPersM[1][0];
		planes[1].z = viewPersM[2][3] - viewPersM[2][0];
		planes[1].w = viewPersM[3][3] - viewPersM[3][0];

		// bottom
		planes[2].x = viewPersM[0][3] + viewPersM[0][1];
		planes[2].y = viewPersM[1][3] + viewPersM[1][1];
		planes[2].z = viewPersM[2][3] + viewPersM[2][1];
		planes[2].w = viewPersM[3][3] + viewPersM[3][1];

		// top
		planes[3].x = viewPersM[0][3] - viewPersM[0][1];
		planes[3].y = viewPersM[1][3] - viewPersM[1][1];
		planes[3].z = viewPersM[2][3] - viewPersM[2][1];
		planes[3].w = viewPersM[3][3] - viewPersM[3][1];

		// near
		planes[4].x = viewPersM[0][3] + viewPersM[0][2];
		planes[4].y = viewPersM[1][3] + viewPersM[1][2];
		planes[4].z = viewPersM[2][3] + viewPersM[2][2];
		planes[4].w = viewPersM[3][3] + viewPersM[3][2];

		// Far
		planes[5].x = viewPersM[0][3] - viewPersM[0][2];
		planes[5].y = viewPersM[1][3] - viewPersM[1][2];
		planes[5].z = viewPersM[2][3] - viewPersM[2][2];
		planes[5].w = viewPersM[3][3] - viewPersM[3][2];
	}

	bool Math::point2Plane(const Vector3& point, const Vector4& plane)
	{
		return (point.x * plane.x + point.y * plane.y + point.z * plane.z + plane.w) >= 0;
	} 
} // OEngine