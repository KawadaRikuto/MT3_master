#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui.h>
#include <algorithm>

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff;   //!< 終点への差分ベクトル
};

struct Plane {
	Vector3 normal;
	float distance;
};

// 三角形構造体の定義
struct Triangle {
	Vector3 vertices[3]; //!< 頂点
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size; //!< 【定義】中心から面までの半分の長さ（Extents）
};

struct Ray {
	Vector3 origin;
	Vector3 direction;
};

struct Line {
	Vector3 point;
	Vector3 direction;
};

struct Spring {
	Vector3 anchor;
	float naturalLength;
	float stiffness;
	float dampingCoefficient;
};

struct Ball {
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

struct Pendulum {
	Vector3 anchor;
	float length;
	float angle;
	float angularVelocity;
	float angularAcceleration;
};

// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

// スカラー倍
Vector3 Multiply(float scalar, const Vector3& v) {
	return { scalar * v.x, scalar * v.y, scalar * v.z };
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

// 外積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

// 長さ
float Length(const Vector3& v) {
	return sqrtf(Dot(v, v));
}

float LengthSquared(const Vector3& v) {
	return Dot(v, v);
}

// 正規化
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len != 0) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return { 0, 0, 0 };
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 a = m;
	Matrix4x4 result = MakeIdentity4x4();

	for (int i = 0; i < 4; ++i) {
		float pivot = a.m[i][i];

		if (std::abs(pivot) < 1e-6f) return MakeIdentity4x4();

		for (int j = 0; j < 4; ++j) {
			a.m[i][j] /= pivot;
			result.m[i][j] /= pivot;
		}

		for (int k = 0; k < 4; ++k) {
			if (i != k) {
				float factor = a.m[k][i];
				for (int j = 0; j < 4; ++j) {
					a.m[k][j] -= a.m[i][j] * factor;
					result.m[k][j] -= result.m[i][j] * factor;
				}
			}
		}
	}
	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslationMatrix(const Vector3& translation) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[3][0] = translation.x;
	result.m[3][1] = translation.y;
	result.m[3][2] = translation.z;
	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

// X軸回転行列
Matrix4x4 MakeRotationXMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[1][1] = cosA;
	result.m[1][2] = sinA;
	result.m[2][1] = -sinA;
	result.m[2][2] = cosA;
	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotationYMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][2] = -sinA;
	result.m[2][0] = sinA;
	result.m[2][2] = cosA;
	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotationZMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][1] = sinA;
	result.m[1][0] = -sinA;
	result.m[1][1] = cosA;
	return result;
}

// アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotationXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotationYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotationZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 translationMatrix = MakeTranslationMatrix(translate);

	return Multiply(scaleMatrix, Multiply(rotateMatrix, translationMatrix));
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float cot = 1.0f / std::tan(fovY / 2.0f);
	result.m[0][0] = cot / aspectRatio;
	result.m[1][1] = cot;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;
	return result;
}

// Matrix4x4同士の加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

// Matrix4x4同士の減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

// グリッド描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f, kGridHalfWidth };

		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, 0xAAAAAAFF);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };

		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, 0xAAAAAAFF);
	}
}

// 線分の描画関数
void DrawSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 start = segment.origin;
	Vector3 end = Add(segment.origin, segment.diff);

	Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
	Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, color);
}

// 三角形の描画関数
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenVertices[3];
	for (int i = 0; i < 3; ++i) {
		screenVertices[i] = Transform(Transform(triangle.vertices[i], viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawTriangle(
		(int)screenVertices[0].x, (int)screenVertices[0].y,
		(int)screenVertices[1].x, (int)screenVertices[1].y,
		(int)screenVertices[2].x, (int)screenVertices[2].y,
		color, kFillModeWireFrame
	);
}

// 三角形と線分の衝突判定関数
bool IsCollision(const Triangle& triangle, const Segment& segment) {
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	Vector3 normal = Normalize(Cross(v01, v12));
	float distance = Dot(triangle.vertices[0], normal);

	float dot = Dot(normal, segment.diff);

	if (std::abs(dot) < 1e-6f) {
		return false;
	}

	float t = (distance - Dot(segment.origin, normal)) / dot;

	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Vector3 p = Add(segment.origin, Multiply(t, segment.diff));

	Vector3 v1p = Subtract(p, triangle.vertices[0]);
	Vector3 v2p = Subtract(p, triangle.vertices[1]);
	Vector3 v0p = Subtract(p, triangle.vertices[2]);

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		return true;
	}

	return false;
}


bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) return false;
	if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) return false;
	if (aabb1.max.z < aabb2.min.z || aabb1.min.z > aabb2.max.z) return false;
	return true;
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8] = {
		{ aabb.min.x, aabb.min.y, aabb.min.z },
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z }
	};
	int indices[12][2] = {
		{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
		{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
	};
	for (int i = 0; i < 12; ++i) {
		Vector3 start = vertices[indices[i][0]];
		Vector3 end = vertices[indices[i][1]];
		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, color);
	}
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	float closestX = std::clamp(sphere.center.x, aabb.min.x, aabb.max.x);
	float closestY = std::clamp(sphere.center.y, aabb.min.y, aabb.max.y);
	float closestZ = std::clamp(sphere.center.z, aabb.min.z, aabb.max.z);

	float distanceSquared = (closestX - sphere.center.x) * (closestX - sphere.center.x) +
		(closestY - sphere.center.y) * (closestY - sphere.center.y) +
		(closestZ - sphere.center.z) * (closestZ - sphere.center.z);

	return distanceSquared < (sphere.radius * sphere.radius);
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(M_PI) / kSubdivision;
	const float kLatEvery = (float(M_PI) * 2.0f) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			auto GetPoint = [&](float lat, float lon) {
				Vector3 p;
				p.x = sphere.radius * std::cos(lat) * std::cos(lon) + sphere.center.x;
				p.y = sphere.radius * std::sin(lat) + sphere.center.y;
				p.z = sphere.radius * std::cos(lat) * std::sin(lon) + sphere.center.z;
				return p;
				};

			Vector3 a = GetPoint(lat, lon);
			Vector3 b = GetPoint(lat, lon + kLonEvery);
			Vector3 c = GetPoint(lat + kLatEvery, lon);

			Vector3 screenA = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 screenB = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 screenC = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			Novice::DrawLine((int)screenA.x, (int)screenA.y, (int)screenB.x, (int)screenB.y, color);
			Novice::DrawLine((int)screenA.x, (int)screenA.y, (int)screenC.x, (int)screenC.y, color);
		}
	}
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	float tMin = 0.0f;
	float tMax = 1.0f;

	float origins[3] = { segment.origin.x, segment.origin.y, segment.origin.z };
	float diffs[3] = { segment.diff.x, segment.diff.y, segment.diff.z };
	float mins[3] = { aabb.min.x, aabb.min.y, aabb.min.z };
	float maxs[3] = { aabb.max.x, aabb.max.y, aabb.max.z };

	for (int i = 0; i < 3; ++i) {
		if (std::abs(diffs[i]) < 1e-6f) {
			if (origins[i] < mins[i] || origins[i] > maxs[i]) {
				return false;
			}
		} else {
			float t1 = (mins[i] - origins[i]) / diffs[i];
			float t2 = (maxs[i] - origins[i]) / diffs[i];
			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;

			if (tMin > tMax) return false;
		}
	}
	return true;
}

bool IsCollision(const OBB& obb, const Sphere& sphere) {
	Vector3 closestPoint = obb.center;

	float extentArray[3] = { obb.size.x, obb.size.y, obb.size.z };

	for (int i = 0; i < 3; ++i) {
		float distance = Dot(Subtract(sphere.center, obb.center), obb.orientations[i]);
		distance = std::clamp(distance, -extentArray[i], extentArray[i]);
		closestPoint = Add(closestPoint, Multiply(distance, obb.orientations[i]));
	}
	float distanceSquared = LengthSquared(Subtract(closestPoint, sphere.center));
	return distanceSquared < (sphere.radius * sphere.radius);
}

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 vertices[8];

	for (int i = 0; i < 8; ++i) {
		Vector3 corner = {
			((i & 1) ? obb.size.x : -obb.size.x),
			((i & 2) ? obb.size.y : -obb.size.y),
			((i & 4) ? obb.size.z : -obb.size.z)
		};
		vertices[i] = Add(obb.center, Add(Multiply(corner.x, obb.orientations[0]), Add(Multiply(corner.y, obb.orientations[1]), Multiply(corner.z, obb.orientations[2]))));
	}
	int indices[12][2] = {
		{ 0, 1 }, { 0, 2 }, { 2, 3 }, { 3, 1 },
		{ 4, 5 }, { 4, 6 }, { 6, 7 }, { 7, 5 },
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
	};
	for (int i = 0; i < 12; ++i) {
		Vector3 start = vertices[indices[i][0]];
		Vector3 end = vertices[indices[i][1]];
		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, color);
	}
}


bool IsCollision(const Segment& segment, const OBB& obb) {
	// 線分をOBBのローカル空間（AABB）に変換して判定
	Vector3 localOrigin = Subtract(segment.origin, obb.center);
	Vector3 localSegmentOrigin = {
		Dot(localOrigin, obb.orientations[0]),
		Dot(localOrigin, obb.orientations[1]),
		Dot(localOrigin, obb.orientations[2])
	};
	Vector3 localSegmentDiff = {
		Dot(segment.diff, obb.orientations[0]),
		Dot(segment.diff, obb.orientations[1]),
		Dot(segment.diff, obb.orientations[2])
	};

	AABB localAABB = {
		{ -obb.size.x, -obb.size.y, -obb.size.z },
		{  obb.size.x,  obb.size.y,  obb.size.z }
	};
	Segment localSegment = { localSegmentOrigin, localSegmentDiff };
	return IsCollision(localAABB, localSegment);
}


bool IsCollision(const Ray& ray, const OBB& obb) {
	// RayをOBBのローカル空間（AABB）に変換
	Vector3 localOrigin = Subtract(ray.origin, obb.center);
	Vector3 localRayOrigin = {
		Dot(localOrigin, obb.orientations[0]),
		Dot(localOrigin, obb.orientations[1]),
		Dot(localOrigin, obb.orientations[2])
	};
	Vector3 localRayDir = {
		Dot(ray.direction, obb.orientations[0]),
		Dot(ray.direction, obb.orientations[1]),
		Dot(ray.direction, obb.orientations[2])
	};

	// OBBのローカル空間での範囲（AABB相当）
	float extents[3] = { obb.size.x, obb.size.y, obb.size.z };
	float origins[3] = { localRayOrigin.x, localRayOrigin.y, localRayOrigin.z };
	float dirs[3] = { localRayDir.x, localRayDir.y, localRayDir.z };

	float tMin = 0.0f;           // Rayは前方に無限（t >= 0）
	float tMax = 1e+30f;         // 正の無限大のシミュレート

	for (int i = 0; i < 3; ++i) {
		if (std::abs(dirs[i]) < 1e-6f) {
			// 軸に平行な場合、範囲外なら交差しない
			if (origins[i] < -extents[i] || origins[i] > extents[i]) {
				return false;
			}
		} else {
			float t1 = (-extents[i] - origins[i]) / dirs[i];
			float t2 = (extents[i] - origins[i]) / dirs[i];
			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;

			if (tMin > tMax) return false;
		}
	}
	return true;
}

bool IsCollision(const Line& line, const OBB& obb) {
	// LineをOBBのローカル空間（AABB）に変換
	Vector3 localOrigin = Subtract(line.point, obb.center);
	Vector3 localLineOrigin = {
		Dot(localOrigin, obb.orientations[0]),
		Dot(localOrigin, obb.orientations[1]),
		Dot(localOrigin, obb.orientations[2])
	};
	Vector3 localLineDir = {
		Dot(line.direction, obb.orientations[0]),
		Dot(line.direction, obb.orientations[1]),
		Dot(line.direction, obb.orientations[2])
	};

	// OBBのローカル空間での範囲（AABB相当）
	float extents[3] = { obb.size.x, obb.size.y, obb.size.z };
	float origins[3] = { localLineOrigin.x, localLineOrigin.y, localLineOrigin.z };
	float dirs[3] = { localLineDir.x, localLineDir.y, localLineDir.z };

	float tMin = -1e+30f;        // Lineは両方向に無限（負の無限大）
	float tMax = 1e+30f;         // 正の無限大

	for (int i = 0; i < 3; ++i) {
		if (std::abs(dirs[i]) < 1e-6f) {
			// 軸に平行な場合、範囲外なら交差しない
			if (origins[i] < -extents[i] || origins[i] > extents[i]) {
				return false;
			}
		} else {
			float t1 = (-extents[i] - origins[i]) / dirs[i];
			float t2 = (extents[i] - origins[i]) / dirs[i];
			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;

			if (tMin > tMax) return false;
		}
	}
	return true;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return Add(v1, v2);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return Subtract(v1, v2);
}

Vector3 operator*(float s, const Vector3& v) {
	return Multiply(s, v);
}

Vector3 operator*(const Vector3& v, float s) {
	return s * v;
}

// Vector3型の/（スカラー除算）演算子オーバーロードを追加
Vector3 operator/(const Vector3& v, float s) {
	return Multiply(1.0f / s, v);
}

Matrix4x4 operator+( const Matrix4x4& m1, const Matrix4x4& m2 ) {
	return Add(m1, m2);
}

Matrix4x4 operator-( const Matrix4x4& m1, const Matrix4x4& m2 ) {
	return Subtract(m1, m2);
}

Matrix4x4 operator*( const Matrix4x4& m1, const Matrix4x4& m2 ) {
	return Multiply(m1, m2);
}

// Vector3型の+=演算子オーバーロードを追加
Vector3& operator+=(Vector3& lhs, const Vector3& rhs) {
	lhs = lhs + rhs;
	return lhs;
}

Vector3& operator-=(Vector3& lhs, const Vector3& rhs) {
	lhs = lhs - rhs;
	return lhs;
}

Vector3& operator*=(Vector3& lhs, float s) {
	lhs = lhs * s;
	return lhs;
}

Vector3& operator/=(Vector3& lhs, float s) {
	lhs = lhs / s;
	return lhs;
}

const char kWindowTitle[] = "LE2B_07_カワダ_リクト";

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const float deltaTime = 1.0f / 60.0f;

	Pendulum pendulum;
	pendulum.anchor = { 0.0f, 1.0f, 0.0f };
	pendulum.length = 0.8f;
	pendulum.angle = 0.7f;
	pendulum.angularVelocity = 0.0f;
	pendulum.angularAcceleration = 0.0f;

	
	Vector3 p;

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		if (keys[DIK_W]) cameraTranslate.z += 0.05f;
		if (keys[DIK_S]) cameraTranslate.z -= 0.05f;
		if (keys[DIK_A]) cameraTranslate.x -= 0.05f;
		if (keys[DIK_D]) cameraTranslate.x += 0.05f;

		pendulum.angularAcceleration = -(9.8f / pendulum.length) * std::sin(pendulum.angle);
		pendulum.angularVelocity += pendulum.angularAcceleration * deltaTime;
		pendulum.angle += pendulum.angularVelocity * deltaTime;

		p.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
		p.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
		p.z = pendulum.anchor.z;
		

		ImGui::Begin("Window");



		ImGui::End();

		
		// カメラ行列・ビューポート変換等の計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		// SegmentとOBBの衝突判定
		//bool colliding = IsCollision(segment, obb);

		// 描画処理
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 衝突していたら赤(RED)、していなければ白(WHITE)
		//uint32_t color = colliding ? RED : WHITE;

		// 描画
		DrawSphere({ p, 0.1f }, viewProjectionMatrix, viewportMatrix, 0xFF0000FF);
		DrawSegment({ pendulum.anchor, Subtract(p, pendulum.anchor) }, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);
		
		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}