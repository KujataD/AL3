#pragma once
#include <numbers>
#include <string>
#include <vector>
#include <cassert>

struct StageData {
	std::string name;  // ステージ名 (Field csv file name)
	int32_t timeLimit; // 制限時間 (秒)
};

/// <summary>
/// ステージ管理
/// </summary>
class StageManager {
public:
	/// <summary>
	/// ステージデータファイルの読み込み
	/// </summary>
	void LoadStageDataFile();

	// Accessor
	// ------------------------------------------

	// --- set ---

	void SetCurrentStageIndex(int32_t index) {
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()));
		currentStageIndex_ = index;
	}

	/// <summary>
	/// ステージ名指定で現在ステージ番号設定
	/// </summary>
	/// <param name="name">ステージ名</param>
	void SetCurrentStageIndexByName(const std::string& name);

	// --- get ---

	/// <summary>
	/// ステージデータの取得
	/// </summary>
	/// <param name="index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetStageData(int32_t index) const {
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()));
		return stageDatas_[index];
	}

	/// <summary>
	/// 現在ステージのステージデータ取得
	/// </summary>
	/// <param name="index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetCurrentStageData() const { return GetStageData(currentStageIndex_); }

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

private:
	// 全ステージデータ
	std::vector<StageData> stageDatas_;

	// 現在のステージ番号
	int32_t currentStageIndex_ = 0;
};
