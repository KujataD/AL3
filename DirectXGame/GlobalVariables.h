#pragma once
#include <KujakuEngine.h>
#include <map>
#include <string>
#include <variant>

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	// 構造体宣言
	// ------------------------------------------

	// 項目
	using Item = std::variant<int32_t, float, KujakuEngine::Vector3>;

	// グループ
	using Group = std::map<std::string, Item>;

	// ------------------------------------------

	// 各機能
	// ------------------------------------------

	void Update();

	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);

	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);

	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const KujakuEngine::Vector3& value);

private:
	// シングルトン化
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	const GlobalVariables& operator=(const GlobalVariables&) = delete;

private:
	// 全データ
	std::map<std::string, Group> datas_;
};
