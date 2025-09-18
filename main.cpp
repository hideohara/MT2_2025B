#include <Novice.h>
#include <math.h>
//#define _USE_MATH_DEFINES

const char kWindowTitle[] = "GC1C_99_オオハラ_ヒデフミ";

// 2x2の行列を表す
struct Matrix2x2
{
	float m[2][2];
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
Matrix2x2 Multiply(Matrix2x2 matrix1, Matrix2x2 matrix2)
{
	Matrix2x2 result = {};

	result.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0];
	result.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1];
	result.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0];
	result.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1];

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
Matrix2x2 MakeRotateMatrix(float theta)
{
	Matrix2x2 result = {};

	//行列を作る
	result.m[0][0] = cosf(theta);
	result.m[0][1] = sinf(theta);
	result.m[1][0] = -sinf(theta);
	result.m[1][1] = cosf(theta);
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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	int textureHandle = Novice::LoadTexture("white1x1.png");

	// もととなる矩形の定数
	const Vector2 kRectCenter = { 400, 100 };
	const Vector2 kRectSize = { 200, 100 };

	const Vector2 kLeftTop =
	{
		-kRectSize.x / 2,
		-kRectSize.y / 2
	};
	const Vector2 kRightTop =
	{
		kRectSize.x / 2,
		-kRectSize.y / 2
	};
	const Vector2 kLeftBottom =
	{
		-kRectSize.x / 2,
		kRectSize.y / 2

	};
	const Vector2 kRightBottom =
	{
		kRectSize.x / 2,
		kRectSize.y / 2
	};
	float theta = 0.0f;

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

		//theta += float(M_PI) / 120.0f;
		theta += 0.1f;

		// 回転行列を作成
		Matrix2x2 rotateMatrix = MakeRotateMatrix(theta);
		// 各頂点に回転行列を掛ける
		Vector2 leftTop = Multiply(kLeftTop, rotateMatrix);
		Vector2 rightTop = Multiply(kRightTop, rotateMatrix);
		Vector2 leftBottom = Multiply(kLeftBottom, rotateMatrix);
		Vector2 rightBottom = Multiply(kRightBottom, rotateMatrix);

		//各頂点にkRectCenterを加算(xにはkRectCenter.xを、yにはkRectCenter.yを加算)
		leftTop.x += kRectCenter.x;
		leftTop.y += kRectCenter.y;
		rightTop.x += kRectCenter.x;
		rightTop.y += kRectCenter.y;
		leftBottom.x += kRectCenter.x;
		leftBottom.y += kRectCenter.y;
		rightBottom.x += kRectCenter.x;
		rightBottom.y += kRectCenter.y;


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// スクリーン座標へ変換
		Vector2 screenLeftTop = ToScreen(leftTop);
		Vector2 screenRightTop = ToScreen(rightTop);
		Vector2 screenLeftBottom = ToScreen(leftBottom);
		Vector2 screenRightBottom = ToScreen(rightBottom);

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
