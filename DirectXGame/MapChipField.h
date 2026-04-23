#pragma once
#include "KujakuEngine/KujakuEngine.h"
#include <string>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

class MapChipField {
public:
public:
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	KujakuEngine::Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const KujakuEngine::Vector3& position);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

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
