#pragma once

#include <windef.h>

#define WM_MYBUTTON WM_USER + 123

extern PCTSTR szDataFile;

enum Command {
    No = 1,  // 不叫地主
    Score1,  // 1分
    Score2,  // 2分
    Score3,  // 3分
    Discard, // 出牌
    Pass,    // 不出
    Hint     // 提示
};
