独立运行的UI库，代码已经都已经修改好了，jpg被屏蔽了（因为暂时用不到）。

需要在stdafx.cpp中添加如下代码来运行

#ifdef _DEBUG
#pragma comment(lib, "./UIBase/debuglib/libpng.lib")
#pragma comment(lib, "./UIBase/debuglib/zlib.lib")
#else
#pragma comment(lib, "./UIBase/releaselib/zlib.lib")
#pragma comment(lib, "./UIBase/releaselib/libpng.lib")
#endif