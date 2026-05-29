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
	// 1. 三角形から平面の法線を求める
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);

	// 法線は v01 と v12 の外積を正規化したもの
	Vector3 normal = Normalize(Cross(v01, v12));
	// 平面の原点からの距離 distance = Dot(頂点, 法線)
	float distance = Dot(triangle.vertices[0], normal);

	// 2. 線分と平面の衝突（交点）判定を行う
	float dot = Dot(normal, segment.diff);

	// 平行な場合は衝突しない
	if (std::abs(dot) < 1e-6f) {
		return false;
	}

	// 交点までの媒介変数 t を求める
	float t = (distance - Dot(segment.origin, normal)) / dot;

	// t が線分の範囲内（0〜1）にない場合は平面と交差していない
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 交点 p を計算
	Vector3 p = Add(segment.origin, Multiply(t, segment.diff));

	// 交点 p が三角形の内側にあるか外積を使って判定
	Vector3 v1p = Subtract(p, triangle.vertices[0]);
	Vector3 v2p = Subtract(p, triangle.vertices[1]);
	Vector3 v0p = Subtract(p, triangle.vertices[2]);

	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を取る
	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		return true; // 衝突
	}

	return false;
}

bool IsCollision(const AABB& aabb1, const AABB& aabbb2) {
	// AABB同士の衝突判定
	if (aabb1.max.x < aabbb2.min.x || aabb1.min.x > aabbb2.max.x) return false;
	if (aabb1.max.y < aabbb2.min.y || aabb1.min.y > aabbb2.max.y) return false;
	if (aabb1.max.z < aabbb2.min.z || aabb1.min.z > aabbb2.max.z) return false;
	return true; // 衝突
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
		{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, // 前面
		{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, // 背面
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }  // 側面
	};
	for (int i = 0; i < 12; ++i) {
		Vector3 start = vertices[indices[i][0]];
		Vector3 end = vertices[indices[i][1]];
		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, color);
	}
}

const char kWindowTitle[] = "LE2B_07_カワダ_リクト";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	

	AABB aabb1 = {
		{ -0.5f, -0.5f, -0.5f }, // min
		{ 0.0f, 0.0f, 0.0f }     // max
	};

	AABB aabb2 = {
		{ -0.2f, -0.2f, -0.2f }, // min
		{ 1.0f, 1.0f, 1.0f }     // max
	};

	// 実装イメージの画面に合わせるためのカメラアングル
	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// カメラの簡易キー操作
		if (keys[DIK_W]) cameraTranslate.z += 0.05f;
		if (keys[DIK_S]) cameraTranslate.z -= 0.05f;
		if (keys[DIK_A]) cameraTranslate.x -= 0.05f;
		if (keys[DIK_D]) cameraTranslate.x += 0.05f;
		if (keys[DIK_UP]) cameraRotate.x += 0.01f;
		if (keys[DIK_DOWN]) cameraRotate.x -= 0.01f;

		// ImGuiを使用したパラメータのリアルタイム変更機能
		ImGui::Begin("Window");

		ImGui::DragFloat3("AABB1.Min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("AABB1.Max", &aabb1.max.x, 0.01f);
		ImGui::DragFloat3("AABB2.Min", &aabb2.min.x, 0.01f);
		ImGui::DragFloat3("AABB2.Max", &aabb2.max.x, 0.01f);


		ImGui::End();

		// 1. カメラのワールド行列を作成
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		// 2. ビュー行列はカメラの逆行列
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		// 3. 投影行列（パース）
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		// 4. 合成行列 (View * Projection)
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		// 5. ビューポート行列
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		bool colliding = IsCollision(aabb1, aabb2);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// グリッドを描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		uint32_t aabbColor = colliding ? RED : WHITE;
		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, aabbColor);
		DrawAABB(aabb2, viewProjectionMatrix, viewportMatrix, aabbColor);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}