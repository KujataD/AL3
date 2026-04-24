#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include <string>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayer,// プレイヤー
	kEnemy,	// 敵
};

struct MapChipDataUnit {
	MapChipType type; // マップチップの種別
	uint8_t subID;   // 種類ごとのサブID
};

struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};


class MapChipField {
public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// マップチップCSVの文字番号
	enum MapChipCharIndex {
		kChipType = 0, // マップチップタイプ
		kChipSubID = 1 // タイプごとのサブID
	};
public:
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);
	
	// Accessor
	// ------------------------------------------

	// --- get ---
	KujakuEngine::Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const KujakuEngine::Vector3& position);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);

	KujakuEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	float GetBlockWidth() { return kBlockWidth; }
	float GetBlockHeight() { return kBlockHeight; }

	uint32_t GetNumBlockVertical() { return kNumBlockVertical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロック個数
	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};
