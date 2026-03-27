#pragma once
#include <KamataEngine.h>

struct UICommand {

	int move = 0;        // -1 上 / +1 下
	int moveLR = 0;      // 左右移動 (-1 左 / +1 右)
	bool decide = false; // 決定
	bool pause = false;  // ポーズ
};