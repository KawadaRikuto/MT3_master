#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include<imgui.h>


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

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
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

		// 0で割るのを防ぐ
		if (std::abs(pivot) < 1e-6f) return MakeIdentity4x4();


		for (int j = 0; j < 4; ++j) {
			a.m[i][j] /= pivot;
			result.m[i][j] /= pivot;
		}

		// 他の行の成分を0にする
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

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m.m[j][i];
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


static const int kColumnWidth = 60;
static const int kRowHeight = 20;

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y + kRowHeight, "%s", label);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Novice::ScreenPrintf(
				x + j * kColumnWidth,
				y + kRowHeight * 2 + i * kRowHeight,
				"%6.02f", matrix.m[i][j]
			);
		}
	}
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	Matrix4x4 rotateXMatrix = MakeRotationXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotationYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotationZMatrix(rotate.z);

	
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	Matrix4x4 translationMatrix = MakeTranslationMatrix(translate);

	Matrix4x4 result = Multiply(scaleMatrix, Multiply(rotateMatrix, translationMatrix));

	return result;
}

// 1. 透視投影行列
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

// 2. 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;
	return result;
}

// 3. ビューポート変換行列
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


Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		// Z方向に伸びる線
		Vector3 start = { x, 0.0f, -kGridHalfWidth };
		Vector3 end = { x, 0.0f, kGridHalfWidth };

		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, 0x000000FF);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		// X方向に伸びる線
		Vector3 start = { -kGridHalfWidth, 0.0f, z };
		Vector3 end = { kGridHalfWidth, 0.0f, z };

		Vector3 screenStart = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		Novice::DrawLine((int)screenStart.x, (int)screenStart.y, (int)screenEnd.x, (int)screenEnd.y, 0x000000FF);
	}

}


void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(M_PI) / kSubdivision;
	const float kLatEvery = (float(M_PI) * 2.0f) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			// 現在の点 a, 次の経度の点 b, 次の緯度の点 c を求めて線で結ぶ
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

// 正射影ベクトルを計算する関数
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 v2Norm = Normalize(v2);
	float dot = Dot(v1, v2Norm);
	return Multiply(dot, v2Norm);
}

// 線分上の最近接点を計算する関数
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 v = Subtract(point, segment.origin);
	float lenSq = LengthSquared(segment.diff); // diffの長さの2乗

	if (lenSq < 1e-6f) return segment.origin; // 微小な線分対策

	// 射影係数 t を計算し、線分の範囲（0.0 〜 1.0）に収める
	float t = Dot(v, segment.diff) / lenSq;
	if (t < 0.0f) {
		t = 0.0f;
	} else if (t > 1.0f) {
		t = 1.0f;
	}

	// 始点に、クランプされた差分ベクトルを足して最近接点とする
	return Add(segment.origin, Multiply(t, segment.diff));
}

const char kWindowTitle[] = "LE2B_07_カワダ_リクト";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/*Vector3 v1{ 1.0f, 3.0f, -5.0f };
	Vector3 v2{ 4.0f, -1.0f, 2.0f };
	float k = 4.0f;*/

	/*Matrix4x4 m1 = { { 3.2f, 0.7f, 9.6f, 4.4f,
					 5.5f, 1.3f, 7.8f, 2.1f,
					 6.9f, 8.0f, 2.6f, 1.0f,
					 0.5f, 7.2f, 5.1f, 3.3f } };

	Matrix4x4 m2 = { { 4.1f, 6.5f, 3.3f, 2.2f,
					 8.8f, 0.6f, 9.9f, 7.7f,
					 1.1f, 5.5f, 6.6f, 0.0f,
					 3.3f, 9.9f, 8.8f, 2.2f } };


	Vector3 translate{ 4.1f, 2.6f, 0.8f };
	Vector3 scale{ 1.5f, 5.2f, 7.3f };
	Vector3 point{ 2.3f, 3.8f, 1.4f };
	Matrix4x4 transformMatrix = {{ 1.0f, 2.0f, 3.0f, 4.0f,
									3.0f, 1.0f, 1.0f, 2.0f,
									1.0f, 4.0f, 2.0f, 3.0f,
									2.0f, 2.0f, 1.0f, 3.0f }};
	

	Vector3 rotate{ 0.4f, 1.43f, -0.8f };*/


	/*Vector3 scale{ 1.2f, 0.79f, -2.1f };
	Vector3 rotate{ 0.4f, 1.43f, -0.8f };
	Vector3 translate{ 2.7f, -4.15f, 1.57f };*/
	
	/*Matrix4x4 orthographicMatrix = MakeOrthographicMatrix(-160.0f, 160.0f, 200.0f, 300.0f, 0.0f, 1000.0f);
	Matrix4x4 perspectiveFovMatrix = MakePerspectiveFovMatrix(0.63f, 1.33f, 0.1f, 1000.0f);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(100.0f, 200.0f, 600.0f, 300.0f, 0.0f, 1.0f);*/


	//// 三角形のローカル座標
	//Vector3 kLocalVertices[3] = {
	//	{ 0.0f,  1.0f, 0.0f}, // 上
	//	{ 1.0f, -1.0f, 0.0f}, // 右下
	//	{-1.0f, -1.0f, 0.0f}, // 左下
	//};

	//// 三角形のトランスフォーム用変数
	//Vector3 rotate{ 0.0f, 0.0f, 0.0f };
	//Vector3 translate{ 0.0f, 0.0f, 0.0f };

	//// カメラ設定
	//Vector3 cameraPosition{ 0.0f, 0.0f, -10.0f }; // 少し手前に配置

	//// クロス積の確認用データ
	//Vector3 v1_cross{ 1.2f, -3.9f, 2.5f };
	//Vector3 v2_cross{ 2.8f, 0.4f, -1.3f };

	/*Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Sphere sphere = { {0,0,0}, 1.0f };*/

	Segment segment{ {-2.0f, -1.0f, 0.0f}, {3.0f, 2.0f, 2.0f} };
	Vector3 point{ -1.5f, 0.6f, 0.6f };

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

		// 各種計算の実行
		/*Vector3 resultAdd = Add(v1, v2);
		Vector3 resultSubtract = Subtract(v1, v2);
		Vector3 resultMultiply = Multiply(k, v1);
		float resultDot = Dot(v1, v2);
		float resultLength = Length(v1);
		Vector3 resultNormalize = Normalize(v2);*/


		/*Matrix4x4 resultAdd = Add(m1, m2);
		Matrix4x4 resultMultiply = Multiply(m1, m2);
		Matrix4x4 resultSubtract = Subtract(m1, m2);
		Matrix4x4 inverseM1 = Inverse(m1);
		Matrix4x4 inverseM2 = Inverse(m2);
		Matrix4x4 transposeM1 = Transpose(m1);
		Matrix4x4 transposeM2 = Transpose(m2);
		Matrix4x4 identity = MakeIdentity4x4();


		Matrix4x4 translationMatrix = MakeTranslationMatrix(translate);
		Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
		Vector3 transformed = Transform(point, transformMatrix);


		Matrix4x4 rotationXMatrix = MakeRotationXMatrix(rotate.x);
		Matrix4x4 rotationYMatrix = MakeRotationYMatrix(rotate.y);
		Matrix4x4 rotationZMatrix = MakeRotationZMatrix(rotate.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotationXMatrix, Multiply(rotationYMatrix, rotationZMatrix));*/


		/*Matrix4x4 worldMatrix = MakeAffineMatrix(scale, rotate, translate);*/


		//// 1. キー入力で移動 (W,Sで前後、A,Dで左右)
		//if (keys[DIK_W]) translate.z += 0.1f;
		//if (keys[DIK_S]) translate.z -= 0.1f;
		//if (keys[DIK_A]) translate.x -= 0.1f;
		//if (keys[DIK_D]) translate.x += 0.1f;

		//// 2. 自動でY軸回転
		//rotate.y += 0.05f;

		//// 3. 各種行列の計算
		//Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		//Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, cameraPosition);
		//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280 / 720, 0.1f, 100.0f);
		//Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280, 720, 0.0f, 1.0f);

		//// 行列の合成 (World -> View -> Projection)
		//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		//// 4. 座標変換
		//Vector3 screenVertices[3];
		//for (int i = 0; i < 3; ++i) {
		//	Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
		//	screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		//}

#ifdef USE_IMGUI

		ImGui::Begin("window");
		/*ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("SphereRadius", &sphere.radius, 0.01f);*/
		
		ImGui::DragFloat3("Point", &point.x, 0.01f);
		ImGui::DragFloat3("Segment Origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment Diff", &segment.diff.x, 0.01f);
		
		ImGui::End();

#endif

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

		Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
		Vector3 closestPoint = ClosestPoint(point, segment);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		/*VectorScreenPrintf(0, 0, resultAdd, " : Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, " : Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, " : Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f : Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f : Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, " : Normalize");*/


		/*MatrixScreenPrintf(0, 0, resultAdd, "Add");
		MatrixScreenPrintf(0, kRowHeight * 5, resultSubtract, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, resultMultiply, "Multiply");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, inverseM1, "Inverse M1");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4, inverseM2, "Inverse M2");
		MatrixScreenPrintf(kColumnWidth * 5, 0, transposeM1, "Transpose M1");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5, transposeM2, "Transpose M2");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2, identity, "Identity");*/


		/*VectorScreenPrintf(0, 0, transformed, "transformed");
		MatrixScreenPrintf(0, 0, translationMatrix, "transformMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, scaleMatrix, "scaleMatrix");*/
		

		/*MatrixScreenPrintf(0, 0, rotationXMatrix, "rotationXMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, rotationYMatrix, "rotationYMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, rotationZMatrix, "rotationZMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, rotateXYZMatrix, "rotateXYZMatrix");*/

		/*MatrixScreenPrintf(0, 0, worldMatrix, "worldMatrix");*/

		/*MatrixScreenPrintf(0, 0, orthographicMatrix, "orthographicMatrix");
		MatrixScreenPrintf(0, kRowHeight * 5, perspectiveFovMatrix, "perspectiveFovMatrix");
		MatrixScreenPrintf(0, kRowHeight * 10, viewportMatrix, "viewportMatrix");*/

		//// クロス積の表示
		//Vector3 crossResult = Cross(v1_cross, v2_cross);
		//VectorScreenPrintf(0, 0, crossResult, "Cross");

		//// 三角形の描画
		//Novice::DrawTriangle(
		//	(int)screenVertices[0].x, (int)screenVertices[0].y,
		//	(int)screenVertices[1].x, (int)screenVertices[1].y,
		//	(int)screenVertices[2].x, (int)screenVertices[2].y,
		//	RED, kFillModeSolid
		//);

		// グリッドを描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		//// 球体を描画
		//DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);

		
		Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, WHITE);

		
		Sphere pointSphere{ point, 0.01f };
		Sphere closestPointSphere{ closestPoint, 0.01f };

		
		DrawSphere(pointSphere, viewProjectionMatrix, viewportMatrix, RED);
		DrawSphere(closestPointSphere, viewProjectionMatrix, viewportMatrix, BLACK);

		
		ImGui::Begin("Result");
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
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
