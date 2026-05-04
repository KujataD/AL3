#pragma once
#include <functional>

/// <summary>
/// 時限発動
/// </summary>
class TimedCall {
public:
	// コンストラクタ
	TimedCall(std::function<void(void)> func, float time);
	//更新
	void Update();
	//完了ならtrueを返す
	bool IsFinished() { return isFinished_; }

private:
	// コールバック
	std::function<void(void)> func_;
	// 残り時間
	float time_;
	// 完了フラグ
	bool isFinished_ = false; 
};
