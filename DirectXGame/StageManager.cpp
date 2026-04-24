#include "StageManager.h"
#include <fstream>
#include <sstream>

void StageManager::LoadStageDataFile() {
	// ステージデータファイルのパス
	const std::string filePath = "Resources/stageDatas.csv";

	// ifstreamでステージデータファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "ステージデータファイルが存在しません");

	// ファイルの内容を格納するstringstreamの宣言
	std::stringstream stageDatasCsv;
	// ファイルの内容をstringstreamにコピーする
	stageDatasCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// ステージデータを最終行まで1行ずつ読み込む
	// 1行分の内容を格納するstringの宣言
	std::string line;

	while (std::getline(stageDatasCsv, line)) {
		// 1行分の内容を格納するstringstreamを宣言して、stringから変換
		std::istringstream lineStream(line);

		// ステージデータを格納する構造体
		StageData stageData;
		// カンマ区切りの一つ分を格納するstringの宣言
		std::string word;

		// カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');

		// ステージ名を格納する
		stageData.name = word;

		// カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');

		// 整数に変換して制限時間を格納する
		stageData.timeLimit = std::stoi(word);

		// ステージデータテーブルに格納する
		stageDatas_.push_back(stageData);
	}
}

void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	// stageData_の中からnameが一致する要素を検索
	auto result = std::find_if(stageDatas_.begin(), stageDatas_.end(), [&name](const StageData& stageData) { return stageData.name == name; });
	
	// ステージ名が一致したら現在のステージ番号を設定する
	if (result != stageDatas_.end()) {
		currentStageIndex_ = static_cast<int32_t>(std::distance(stageDatas_.begin(), result));
		// 目的を達したので関数を抜ける
		return;
	}

	assert(false && "指定されたステージ名は存在しません");
}
