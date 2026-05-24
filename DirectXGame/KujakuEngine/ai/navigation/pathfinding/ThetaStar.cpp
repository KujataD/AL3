#include "ThetaStar.h"
#include <algorithm>
#include <cassert>
#include <queue>
#include <unordered_set>

namespace KujakuEngine {

// aはbより後回しにすべきかどうか
// 例) a>bの時、小さいbを優先すべきなので、後回しにすべき
struct CompareFCost {
	bool operator()(SearchNode* a, SearchNode* b) const { return a->f > b->f; }
};

void ThetaStar::Init(const Grid* grid) {
	assert(grid);
	grid_ = grid;
}

std::vector<GridIndex> ThetaStar::FindPath(GridIndex start, GridIndex goal) {
	// 返り値
	std::vector<GridIndex> path;

	// 次に調べる候補ノード。優先度付きキューを用いて、自動でコストが低い順になる。
	std::priority_queue<SearchNode*, std::vector<SearchNode*>, CompareFCost> openList;
	// オープンリストに入っている場所のリスト
	std::vector<std::vector<bool>> inOpen(grid_->height, std::vector<bool>(grid_->width, false));

	// 記録してきたすべてのノード
	std::vector<SearchNode*> allPathNodes;

	// すでに調べた場所
	std::vector<std::vector<bool>> closed(grid_->height, std::vector<bool>(grid_->width, false));

	// --- スタートノード ---
	SearchNode* startPathNode = new SearchNode(start.x, start.y);
	startPathNode->g = 0.0f;
	startPathNode->h = NavigationUtil::Heuristic(start, goal);
	startPathNode->f = startPathNode->h;
	startPathNode->parent = nullptr;

	openList.push(startPathNode); // push() で追加
	allPathNodes.push_back(startPathNode);

	// 調べていないノードがなくなるまでループ
	while (!openList.empty()) {

		SearchNode* currentPathNode = openList.top(); // 先頭取得
		openList.pop();                               // 先頭削除

		// --- ゴール判定 ---
		if (currentPathNode->x == static_cast<int32_t>(goal.x) && currentPathNode->y == static_cast<int32_t>(goal.y)) {

			// 親をたどってpath配列に追加していく
			SearchNode* pathNode = currentPathNode;
			while (pathNode) {
				path.push_back({(uint32_t)pathNode->x, (uint32_t)pathNode->y});
				pathNode = pathNode->parent;
			}

			// ゴール→スタートの順なので順序反転する
			std::reverse(path.begin(), path.end());

			// メモリ解放
			for (SearchNode* n : allPathNodes) {
				delete n;
			}

			return path;
		}

		closed[currentPathNode->y][currentPathNode->x] = true;

		// --- 隣接しているノードを調べる ---

		// 上下左右の組み合わせ
		const int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
		const int dz[8] = {0, 0, 1, -1, 1, 1, -1, -1};

		// 8方向アルゴリズムの場合は4
		int indexNum = 8;

		// 4方向アルゴリズムの場合は4
		if (heuristicAlgorithm_ == HeuristicAlgorithm::kManhattan) {
			indexNum = 4;
		}

		for (int i = 0; i < indexNum; i++) {

			// 近接マス
			uint32_t neighborX = currentPathNode->x + dx[i];
			uint32_t neighborZ = currentPathNode->y + dz[i];

			// 範囲チェック
			if (neighborX < 0 || neighborX >= grid_->width) {
				continue;
			}
			if (neighborZ < 0 || neighborZ >= grid_->height) {
				continue;
			}

			// 通行チェック
			if (!IsWalkable(neighborX, neighborZ)) {
				continue;
			}

			// すでに調べていたらスキップ
			if (closed[neighborZ][neighborX]) {
				continue;
			}

			// 隣接ノードを追加する前にチェック
			if (inOpen[neighborZ][neighborX]) {
				continue; // 重複追加を防ぐ
			}

			bool isDiagonal = (i >= 4); // 4番目以降が斜め
			// 斜め移動する際、壁にめり込まないようにする。
			if (isDiagonal) {
				// 右上に進む場合、右が壁でないかどうか
				bool xWalkable = IsWalkable(currentPathNode->x + dx[i], currentPathNode->y);
				// 右上に進む場合、上が壁でないかどうか
				bool zWalkable = IsWalkable(currentPathNode->x, currentPathNode->y + dz[i]);
				if (!xWalkable || !zWalkable) {
					continue;
				}
			}

			// 近接ノード
			SearchNode* neighbor = new SearchNode(neighborX, neighborZ);
			float stepCost;

			// 斜め移動の時のコストはroot2
			if (isDiagonal) {
				stepCost = std::numbers::sqrt2_v<float>;
			} else {
				stepCost = 1.0f;
			}

			// 斜め移動
			if (currentPathNode->parent && HasLineOfSight(currentPathNode->parent, neighbor)) {

				// 親をスキップ
				neighbor->parent = currentPathNode->parent;

				float newG = currentPathNode->parent->g + Distance(currentPathNode->parent, neighbor);

				neighbor->g = newG;

			} else {

				// 通常A*
				neighbor->parent = currentPathNode;

				neighbor->g = currentPathNode->g + stepCost;
			}
			neighbor->h = NavigationUtil::Heuristic({(uint32_t)neighborX, (uint32_t)neighborZ}, goal);
			neighbor->f = neighbor->g + neighbor->h;

			openList.push(neighbor); // push() で追加
			allPathNodes.push_back(neighbor);
		}
	}

	// 見つからなかった場合
	for (SearchNode* n : allPathNodes) {
		delete n;
	}

	return {};
}
bool ThetaStar::HasLineOfSight(const SearchNode* startNode, const SearchNode* endNode) {
	int currentX = static_cast<int>(startNode->x);
	int currentY = static_cast<int>(startNode->y);
	int targetX = static_cast<int>(endNode->x);
	int targetY = static_cast<int>(endNode->y);

	// スタートからゴールまでの差分
	int deltaX = abs(targetX - currentX);
	int deltaY = abs(targetY - currentY);

	// 進行方向
	int stepX = (currentX < targetX) ? 1 : -1;
	int stepY = (currentY < targetY) ? 1 : -1;

	// 誤差値
	int error = deltaX - deltaY;

	while (true) {
		if (!IsWalkable(currentX, currentY)) {
			return false;
		}

		if (currentX == targetX && currentY == targetY) {
			break;
		}

		// 次マスへ進む
		int doubledError = error * 2;
		if (doubledError > -deltaY) {
			error -= deltaY;
			currentX += stepX;
		}

		if (doubledError < deltaX) {
			error += deltaX;
			currentY += stepY;
		}
	}

	return true;
}

float ThetaStar::Distance(const SearchNode* a, const SearchNode* b) {
	float dx = float(a->x - b->x);
	float dz = float(a->y - b->y);
	return std::sqrt(dx * dx + dz * dz);
}

} // namespace KujakuEngine