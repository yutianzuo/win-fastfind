�������е�UI�⣬�����Ѿ����Ѿ��޸ĺ��ˣ�jpg�������ˣ���Ϊ��ʱ�ò�������

��Ҫ��stdafx.cpp��������´���������

#ifdef _DEBUG
#pragma comment(lib, "./UIBase/debuglib/libpng.lib")
#pragma comment(lib, "./UIBase/debuglib/zlib.lib")
#else
#pragma comment(lib, "./UIBase/releaselib/zlib.lib")
#pragma comment(lib, "./UIBase/releaselib/libpng.lib")
#endif