#include <Novice.h>
#include <math.h>
#include <assert.h>
#define _USE_MATH_DEFINES

const char kWindowTitle[] = "GC1C_99_オオハラ_ヒデフミ";

// 2x2の行列を表す
struct Matrix2x2
{
	float m[2][2];
};

// 3x3の行列を表す
struct Matrix3x3
{
	float m[3][3];

};

// 2次元ベクトルを表す
struct Vector2
{
	float x;
	float y;
};

static const int kRowHeight = 20;
static const int kColumnWidth = 64;  // 44

// 行列の表示
void MatrixScreenPrintf(int x, int y, Matrix2x2 matrix) {
	for (int row = 0; row < 2; ++row) {
		for (int column = 0; column < 2; ++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + row * kRowHeight, "%.02f", matrix.m[row][column]);
		}
	}
}

// ベクトルの表示
void VectorScreenPrintf(int x, int y, Vector2 vector) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
}

// 1. 行列の加法
Matrix2x2 Add(Matrix2x2 matrix1, Matrix2x2 matrix2)
{
	Matrix2x2 result = {};

	result.m[0][0] = matrix1.m[0][0] + matrix2.m[0][0];
	result.m[0][1] = matrix1.m[0][1] + matrix2.m[0][1];
	result.m[1][0] = matrix1.m[1][0] + matrix2.m[1][0];
	result.m[1][1] = matrix1.m[1][1] + matrix2.m[1][1];

	return result;
}


// 2. 行列の減法
Matrix2x2 Subtract(Matrix2x2 matrix1, Matrix2x2 matrix2)
{
	Matrix2x2 result = {};

	result.m[0][0] = matrix1.m[0][0] - matrix2.m[0][0];
	result.m[0][1] = matrix1.m[0][1] - matrix2.m[0][1];
	result.m[1][0] = matrix1.m[1][0] - matrix2.m[1][0];
	result.m[1][1] = matrix1.m[1][1] - matrix2.m[1][1];

	return result;
}

// 3. 行列の積
Matrix3x3 Multiply(Matrix3x3 matrix1, Matrix3x3 matrix2)
{
	Matrix3x3 result = {};

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				result.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
			}
		}
	}

	return result;
}

// 4. ベクトルと行列の積
Vector2 Multiply(Vector2 vector, Matrix2x2 matrix)
{
	Vector2 result = {};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1];

	return result;
}

// 回転行列
Matrix3x3 MakeRotateMatrix(float theta)
{
	Matrix3x3 result = {};

	//行列を作る
	result.m[0][0] = cosf(theta);
	result.m[0][1] = sinf(theta);
	result.m[0][2] = 0;

	result.m[1][0] = -sinf(theta);
	result.m[1][1] = cosf(theta);
	result.m[1][2] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;

	return result;
}

//スクリーン座標変換用関数
Vector2 ToScreen(Vector2 world) {
	// 今回のワールド座標系からスクリーン座標系は
	// 原点位置がyに500ずれていて、y軸が反転
	const Vector2 kWorldToScreenTranslate = { 0.0f, 500.f };
	const Vector2 kWorldToScreenScale = { 1.0f, -1.0f };
	return {
	  (world.x * kWorldToScreenScale.x) + kWorldToScreenTranslate.x,
	  (world.y * kWorldToScreenScale.y) + kWorldToScreenTranslate.y };
}

// 平衡移動行列
Matrix3x3 MakeTranslateMatrix(Vector2 translate)
{
	Matrix3x3 result = {};

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;

	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	result.m[2][2] = 1;

	return result;
}

// 平衡移動を行う
Vector2 Transform(Vector2 vector, Matrix3x3 matrix) {
	Vector2 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	return result;
}

// 拡大縮小行列の作成関数
Matrix3x3 MakeScaleMatrix(Vector2 scale)
{
	Matrix3x3 result = {};

	//行列を作る
	result.m[0][0] = scale.x;
	result.m[0][1] = 0;
	result.m[0][2] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;

	return result;
}

Matrix3x3 MakeAffineMatrix(Vector2 scale, float rotate, Vector2 translate) {

	Matrix3x3 result = {};

	//拡大縮小行列を生成する
	Matrix3x3 scaleMatrix = MakeScaleMatrix(scale);

	//回転行列を生成する
	Matrix3x3 rotateMatrix = MakeRotateMatrix(rotate);

	//平行移動行列を生成する
	Matrix3x3 translateMatrix = MakeTranslateMatrix(translate);

	result = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
	// result = Multiply(Multiply(scaleMatrix, translateMatrix), rotateMatrix);

	return result;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int textureHandle = Novice::LoadTexture("white1x1.png");

	// もととなる矩形の定数
	const Vector2 kRectSize = { 200, 100 };

	const Vector2 leftTop =
	{
		-kRectSize.x / 2,
		-kRectSize.y / 2
	};
	const Vector2 rightTop =
	{
		kRectSize.x / 2,
		-kRectSize.y / 2
	};
	const Vector2 leftBottom =
	{
		-kRectSize.x / 2,
		kRectSize.y / 2

	};
	const Vector2 rightBottom =
	{
		kRectSize.x / 2,
		kRectSize.y / 2
	};

	// 中心の座標
	Vector2 translate = { 400, 100 };
	// 拡縮
	//Vector2 scale = { 1, 1 };
	// 角度
	float theta = 0;

	// キー入力で移動する速さ
	const int kSpeed = 4;

	// スケール
	const float kMaxScale = 2.0f;
	const float kMinScale = 0.5f;
	float scale = 1.0f;
	float scaleIncrement = 0.04f;

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

		// 上キーを押したら上に動かす
		if (keys[DIK_UP] != 0) {
			translate.y += kSpeed;
		}
		// 下キーを押したら下に動かす
		if (keys[DIK_DOWN] != 0) {
			translate.y -= kSpeed;
		}
		// 左キーを押したら左に動かす
		if (keys[DIK_LEFT] != 0) {
			translate.x -= kSpeed;
		}
		// 右キーを押したら右に動かす
		if (keys[DIK_RIGHT] != 0) {
			translate.x += kSpeed;
		}

		// 角度
		theta += 0.1f;

		// スケール
		scale += scaleIncrement;
		if (scale <= kMinScale || kMaxScale <= scale) {
			scaleIncrement *= -1.0f;
		}

		/*
		// それぞれの行列を作成する
		Matrix3x3 scaleMatrix = MakeScaleMatrix(Vector2{ scale,scale });
		Matrix3x3 rotateMatrix = MakeRotateMatrix(theta);
		Matrix3x3 translateMatrix = MakeTranslateMatrix(rectCenter);
		// かける順番を間違えないように！
		Matrix3x3 worldMatrix = Multiply(scaleMatrix, rotateMatrix);
		worldMatrix = Multiply(worldMatrix, translateMatrix);
		*/

		// アフィン変換の行列
		Matrix3x3 worldMatrix = MakeAffineMatrix(Vector2{ scale,scale }, theta, translate);

		// 結合した行列で変換を行う
		Vector2 worldLeftTop = Transform(leftTop, worldMatrix);
		Vector2 worldRightTop = Transform(rightTop, worldMatrix);
		Vector2 worldLeftBottom = Transform(leftBottom, worldMatrix);
		Vector2 worldRightBottom = Transform(rightBottom, worldMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// スクリーン座標へ変換
		Vector2 screenLeftTop = ToScreen(worldLeftTop);
		Vector2 screenRightTop = ToScreen(worldRightTop);
		Vector2 screenLeftBottom = ToScreen(worldLeftBottom);
		Vector2 screenRightBottom = ToScreen(worldRightBottom);

		// 描画
		Novice::DrawQuad(
			int(screenLeftTop.x), int(screenLeftTop.y), int(screenRightTop.x), int(screenRightTop.y), int(screenLeftBottom.x),
			int(screenLeftBottom.y), int(screenRightBottom.x), int(screenRightBottom.y), 0, 0, 1, 1, textureHandle,
			WHITE);

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
