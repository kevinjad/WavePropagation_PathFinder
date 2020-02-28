#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class pathFinder_wp : public olc::PixelGameEngine {
public:  // constructors and destructors
	pathFinder_wp() {
		sAppName = "path finding using wave propagation";
	}
	~pathFinder_wp() {

	}
private:
	int nMapHeight;
	int nMapWidth;
	int nCellSize;
	int nBorderWidth;

	int StartX;
	int StartY;
	int EndX;
	int EndY;

	int* flowfieldZ;
	bool* bObstacle;
	
	

public: // override functions

	bool OnUserCreate() override {
		nBorderWidth = 4;
		nCellSize = 64;
		nMapHeight = ScreenHeight() / nCellSize;
		nMapWidth = ScreenWidth() / nCellSize;
		StartX = 3;
		StartY = 4;
		EndX = 6;
		EndY = 10;
		bObstacle = new bool[nMapHeight * nMapWidth]{ false };
		flowfieldZ = new int[nMapHeight * nMapWidth]{ 0 };
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		auto p = [&](int x, int y) {return y * nMapWidth + x; };
		Clear(olc::BLACK);

		//mouse monitoring
		int nSelectedCellX = GetMouseX() / nCellSize;
		int nSelectedCellY = GetMouseY() / nCellSize;

		if (GetMouse(0).bReleased) {
			bObstacle[p(nSelectedCellX, nSelectedCellY)] = !bObstacle[p(nSelectedCellX, nSelectedCellY)];
		}
		if (GetMouse(1).bReleased) {
			StartX = nSelectedCellX;
			StartY = nSelectedCellY;
		}


		for (int i = 0; i < nMapWidth; i++) {
			for (int j = 0; j < nMapHeight; j++) {
				if (i == 0 || j == 0 || i == (nMapWidth - 1) || j == (nMapHeight - 1) || bObstacle[p(i,j)]) {
					flowfieldZ[p(i, j)] = -1;
				}
				else {
					flowfieldZ[p(i, j)] = 0;
				}
			}
		}

		std::list<std::tuple<int, int, int>> nodes;
		nodes.push_back({ EndX,EndY,1 });

		while (!nodes.empty()) {
			std::list<std::tuple<int, int, int>> new_nodes;
			for (auto& n : nodes) {
				int x = std::get<0>(n);
				int y = std::get<1>(n);
				int d = std::get<2>(n);

				flowfieldZ[p(x, y)] = d;

				if ((x + 1) < nMapWidth && flowfieldZ[p(x + 1, y)] == 0) {
					new_nodes.push_back({ x + 1,y,d + 1 });
				}
				if ((x - 1) < nMapWidth && flowfieldZ[p(x - 1, y)] == 0) {
					new_nodes.push_back({ x - 1,y,d + 1 });
				}
				if ((y + 1) < nMapHeight && flowfieldZ[p(x, y+1)] == 0) {
					new_nodes.push_back({ x,y+1,d + 1 });
				}
				if ((y - 1) < nMapHeight && flowfieldZ[p(x, y-1)] == 0) {
					new_nodes.push_back({ x ,y-1,d + 1 });
				}
			}
			new_nodes.sort([&](const std::tuple<int, int, int>& r, const std::tuple<int, int, int>& s) {
				return p(std::get<0>(r), std::get<1>(r)) < p(std::get<0>(s), std::get<1>(s)); });

			new_nodes.unique([&](const std::tuple<int, int, int>& r1, const std::tuple<int, int, int>& s1) {
				return p(std::get<0>(r1), std::get<1>(r1)) == p(std::get<0>(s1), std::get<1>(s1)); });

				nodes.clear();
				nodes.insert(nodes.begin(), new_nodes.begin(), new_nodes.end());
		}

		std::list<std::pair<int, int>> finalPath;
		//finding path
		finalPath.push_back({ StartX,StartY });
		bool flag = true;
		if (flowfieldZ[p(StartX, StartY)] != 0) {
			while (finalPath.back().first != EndX || finalPath.back().second != EndY) {
				std::pair<int, int> t = finalPath.back();
				if (flowfieldZ[p(t.first - 1, t.second)] +1 == flowfieldZ[p(t.first, t.second)]) {
					finalPath.push_back({ t.first - 1,t.second });
				}
				else if (flowfieldZ[p(t.first + 1, t.second)] +1 == flowfieldZ[p(t.first, t.second)]) {
					finalPath.push_back({ t.first + 1,t.second });
				}
				else if (flowfieldZ[p(t.first, t.second -1)] +1 == flowfieldZ[p(t.first, t.second)]) {
					finalPath.push_back({ t.first,t.second-1 });
				}
				else if (flowfieldZ[p(t.first, t.second + 1)] +1 == flowfieldZ[p(t.first, t.second)]) {
					finalPath.push_back({ t.first,t.second + 1 });
				}
			}
		}

		//Drawing

		for (int i = 0; i < nMapWidth; i++) {
			for (int j = 0; j < nMapHeight; j++) {
				olc::Pixel colour = olc::BLUE;
				if (bObstacle[p(i, j)]) colour = olc::GREY;

				if (i == StartX && j == StartY) {
					colour = olc::BLACK;
				}

				if (i == EndX && j == EndY) {
					colour = olc::RED;
				}
				FillRect(i * nCellSize, j * nCellSize, nCellSize - nBorderWidth, nCellSize - nBorderWidth, colour);
				//DrawString(i * nCellSize, j * nCellSize, std::to_string(flowfieldZ[p(i, j)]), olc::WHITE);
			}
		}

		for (auto& pathCor : finalPath) {
			int i = pathCor.first;
			int j = pathCor.second;
			//FillRect(i * nCellSize, j * nCellSize, nCellSize - nBorderWidth, nCellSize - nBorderWidth, olc::CYAN);
			FillCircle((i * nCellSize) + (nCellSize - nBorderWidth) / 2, (j * nCellSize) + (nCellSize - nBorderWidth) / 2, (nCellSize - nBorderWidth) / 4, olc::YELLOW);
		}
		return true;
	}

};

int main() {

	pathFinder_wp demo;
	if (demo.Construct(1080, 1080, 2, 2))
		demo.Start();

	return 0;
}