#include "TimedCall.h"
 
TimedCall::TimedCall(std::function<void(void)> func, float time) {
	func_ = func;
	time_ = time;
}

void TimedCall::Update() {
	if (isFinished_) {
		return;
	}

	time_ -= 1.0f / 60.0f;

	if (time_ <= 0.0f) {
		isFinished_ = true;
		// コールバック関数呼び出し
		func_();
	}
}
