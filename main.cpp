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
	Vector3 origin;
	Vector3 diff;
};

struct Plane {
	Vector3 normal;
	float distance;
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

// 【スライド3枚目】法線と垂直なベクトルを1つ適当に求める関数
Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y, vector.x, 0.0f };
	}
	return { 0.0f, -vector.z, vector.y };
}

// 【スライド4枚目】平面の描画関数
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.distance, plane.normal); // 1. 中心点を決める
	Vector3 perpendiculars[4];

	perpendiculars[0] = Normalize(Perpendicular(plane.normal)); // 2. 法線と垂直なベクトルを1つ求める
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z }; // 3. 2の逆ベクトル
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]); // 4. 2と法線とのクロス積
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z }; // 5. 4の逆ベクトル

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]); // スライド疑似コードの拡大率「2.0f」
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	// 4頂点を結んで矩形を描画する
	Novice::DrawLine((int)points[0].x, (int)points[0].y, (int)points[2].x, (int)points[2].y, color);
	Novice::DrawLine((int)points[2].x, (int)points[2].y, (int)points[1].x, (int)points[1].y, color);
	Novice::DrawLine((int)points[1].x, (int)points[1].y, (int)points[3].x, (int)points[3].y, color);
	Novice::DrawLine((int)points[3].x, (int)points[3].y, (int)points[0].x, (int)points[0].y, color);
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

// 球体描画
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

// 【スライド2枚目】球と平面の衝突判定関数
bool IsCollision(const Sphere& sphere, const Plane& plane) {
	// 平面方程式 ax + by + cz = d を用いた距離の計算
	float distance = Dot(plane.normal, sphere.center) - plane.distance;
	return std::abs(distance) <= sphere.radius;
}

const char kWindowTitle[] = "LE2B_07_カワダ_リクト";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// 課題要件：球の定義（初期位置と半径）
	Sphere sphere = { {0.0f, 1.0f, 0.0f}, 0.5f };

	// 課題要件：平面の定義（法線と原点からの距離）
	Plane plane = { {0.0f, 1.0f, 0.0f}, 0.0f };

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

		// カメラの簡易キー操作 (デバッグ効率化)
		if (keys[DIK_W]) cameraTranslate.z += 0.05f;
		if (keys[DIK_S]) cameraTranslate.z -= 0.05f;
		if (keys[DIK_A]) cameraTranslate.x -= 0.05f;
		if (keys[DIK_D]) cameraTranslate.x += 0.05f;
		if (keys[DIK_UP]) cameraRotate.x += 0.01f;
		if (keys[DIK_DOWN]) cameraRotate.x -= 0.01f;

		// ImGuiを使用したパラメータのリアルタイム変更機能
		ImGui::Begin("Collision Controller");
		ImGui::Text("Camera Control: W/A/S/D or UP/DOWN keys");
		ImGui::Separator();

		// 球体のコントロール
		ImGui::DragFloat3("Sphere Center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere Radius", &sphere.radius, 0.01f, 0.01f, 5.0f);
		ImGui::Separator();

		// 平面のコントロール（スライド2枚目の要件）
		if (ImGui::DragFloat3("Plane.Normal", &plane.normal.x, 0.01f)) {
			// 法線を編集したら必ずNormalizeをかける
			plane.normal = Normalize(plane.normal);
		}
		ImGui::DragFloat("Plane.Distance", &plane.distance, 0.01f);

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

		// 球と平面の衝突判定の実行
		bool colliding = IsCollision(sphere, plane);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// グリッドを描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 平面の描画 (スライド4枚目のアルゴリズムで矩形を描画、色はグリッドと分けて緑系にしています)
		DrawPlane(plane, viewProjectionMatrix, viewportMatrix, 0x00FF00FF);

		// 球体の描画 (衝突時は赤色、非衝突時は白色)
		uint32_t sphereColor = colliding ? RED : WHITE;
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, sphereColor);

		// 判定結果を画面にも表示
		ImGui::Begin("Result");
		if (colliding) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "COLLISION!");
		} else {
			ImGui::Text("No Collision");
		}
		ImGui::End();

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