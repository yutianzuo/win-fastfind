/********************************************************************
	created:	2010/01/04
	created:	04:01:2010   16:58
	file base:	somctrl
	file ext:	h
	author:		wanlixin
	
	purpose:	SOM 界面库的常用控件
*********************************************************************/
#pragma once
#include "png.h"


static void PNGAPI user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PBYTE* ppPointer = (PBYTE*)png_get_io_ptr(png_ptr);
	PBYTE pPointer = *ppPointer;

	memcpy(data, pPointer, length);
	*ppPointer	= pPointer + length;
}

static HBITMAP png_Decode(PBYTE pngdata, DWORD pnglen)
// STDMETHODIMP_(HBITMAP) CLibPng::Decode(PBYTE pngdata, DWORD pnglen)
{
	png_struct* png_ptr	= NULL;
	png_info* info_ptr	= NULL;

	HBITMAP hBmpRet		= NULL;
	do 
	{
		if (!png_check_sig(pngdata, 8))
			return NULL;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING ,(void *)NULL, NULL, NULL);
		if (png_ptr == NULL)
			return NULL;

		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) 
			return NULL;;

		if (setjmp(png_ptr->jmpbuf)) 
		{
			if (hBmpRet)
			{
				DeleteObject(hBmpRet);
				hBmpRet	= NULL;
			}
			break;
		}

		PBYTE pReadPointer = pngdata;
		PBYTE* ppPngData = &pReadPointer;

		png_set_read_fn(png_ptr, ppPngData, user_read_data);
		png_read_info(png_ptr, info_ptr);

		if (info_ptr->bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (info_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (info_ptr->bit_depth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY || info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

		png_read_update_info(png_ptr, info_ptr);

		BITMAPINFO bmi = {0};

		bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth		= info_ptr->width;
		bmi.bmiHeader.biHeight		= -((long)info_ptr->height);
		bmi.bmiHeader.biPlanes		= 1;
		bmi.bmiHeader.biBitCount	= 8 * info_ptr->channels;
		bmi.bmiHeader.biCompression	= 0;

		PBYTE pDibBits	= NULL;
		hBmpRet	= CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDibBits, NULL, 0);
		if (hBmpRet == NULL || pDibBits == NULL)
			break;

		int alpha_present = info_ptr->color_type & PNG_COLOR_MASK_ALPHA;

		if (info_ptr->color_type & PNG_COLOR_MASK_COLOR)
			png_set_bgr(png_ptr);

		int number_passes;

#ifdef PNG_READ_INTERLACING_SUPPORTED
		number_passes = png_set_interlace_handling(png_ptr);
#else
		if (png_ptr->interlaced)
			number_passes	= 1;
#endif

		int rowbytes = info_ptr->width * info_ptr->channels;
		ATLASSERT(rowbytes == png_get_rowbytes(png_ptr, info_ptr));

		rowbytes = (rowbytes + 3) & (~3);

		for (int j = 0; j < number_passes; j++)
		{
			for (png_uint_32 i = 0; i < png_ptr->height; i++)
			{
				png_read_row(png_ptr, (png_bytep)(pDibBits + i * rowbytes), png_bytep_NULL);
			}
		}

		if (info_ptr->channels == 4)
		{
			for (png_uint_32 i = 0; i < png_ptr->height; i++)
			{
				PBYTE pDest = pDibBits + i * rowbytes;
				for (png_uint_32 k = 0; k < png_ptr->width; k ++)
				{
					pDest[0]	= pDest[0] * pDest[3] / 0xff;
					pDest[1]	= pDest[1] * pDest[3] / 0xff;
					pDest[2]	= pDest[2] * pDest[3] / 0xff;
					pDest += 4;
				}
			}
		}

		png_read_end(png_ptr, info_ptr);

	} while (false);

	if (png_ptr && info_ptr)
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	else if (png_ptr)
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

	return hBmpRet;
}

//////////////////////////////////////////////////////////////////////////


class CSomBitmap
{
public:
	enum SOMBITMAP_TYPE
	{
		SOMBITMAP_ICON,
		SOMBITMAP_BMP,
#ifdef SOM_SUPPORT_PNG_JPG
		SOMBITMAP_JPG,
		SOMBITMAP_PNG
#endif //SOM_SUPPORT_PNG_JPG
	};
	CSomBitmap(HICON hIcon, int size)
		: m_lpBmpFile(NULL)
		, m_blocks(1)
		, m_eType(SOMBITMAP_ICON)
		, m_bTransparent(TRUE)
	{
		ATLASSERT(hIcon);
		m_hIcon	= hIcon;
		m_BmpSize.cx	= size;
		m_BmpSize.cy	= size;
	}
	CSomBitmap(HBITMAP hBitmap)
		: m_lpBmpFile(NULL)
		, m_blocks(1)
		, m_eType(SOMBITMAP_BMP)
	{
		ATLASSERT(hBitmap);
		m_hBitmap	= hBitmap;

		if (m_hBitmap)
		{
			BITMAP bm;
			GetObject(m_hBitmap, sizeof(BITMAP), &bm);
			m_BmpSize.cx	= bm.bmWidth / m_blocks;
			m_BmpSize.cy	= bm.bmHeight;
			m_bTransparent	= (bm.bmBitsPixel == 32);

			ATLASSERT(m_BmpSize.cx * m_blocks == bm.bmWidth);
		}
		else
		{
			m_BmpSize.cx	= 0;
			m_BmpSize.cy	= 0;
			m_bTransparent	= FALSE;
		}
	}	
#ifdef SOM_SUPPORT_PNG_JPG
	CSomBitmap(PBYTE pData, DWORD dwLen, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: m_lpBmpFile(NULL)
		, m_hBitmap(NULL)
		, m_blocks(blocks)
		, m_bTransparent(FALSE)
		, m_eType(type)
	{
		LoadBitmap(pData, dwLen);
	}
#endif //SOM_SUPPORT_PNG_JPG
	CSomBitmap(DWORD dwResourceID, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: m_BmpResourceID(dwResourceID)
		, m_hBitmap(NULL)
		, m_blocks(blocks)
		, m_bTransparent(FALSE)
		, m_eType(type)
	{
	}
	CSomBitmap(LPCWSTR lpBmpFile, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: m_lpBmpFile(lpBmpFile)
		, m_hBitmap(NULL)
		, m_blocks(blocks)
		, m_bTransparent(FALSE)
		, m_eType(type)
	{
	}
	~CSomBitmap()
	{
		if (m_hBitmap)
			DeleteObject(m_hBitmap);
	}
	HBITMAP Detach()
	{
		HBITMAP hOld = m_hBitmap;
		m_hBitmap	= NULL;
		return hOld;
	}
	BOOL IsTransparent()
	{
		LoadBitmap();

		return m_bTransparent;
	}
	BOOL IsNull()
	{
		LoadBitmap();
		if (m_eType == SOMBITMAP_ICON)
			return m_hIcon == NULL;
		return m_hBitmap == NULL;
	}
	operator HBITMAP()
	{ 
		ATLASSERT(m_eType != SOMBITMAP_ICON);

		LoadBitmap();

		return m_hBitmap; 
	}
	int GetBlockNums()
	{
		return m_blocks;
	}
	void BitBlt(HDC hDC, int x, int y, int block_index = 0)
	{
		if (m_eType == SOMBITMAP_ICON)
		{
			DrawIconEx(hDC, x, y, m_hIcon, m_BmpSize.cx, m_BmpSize.cy, 0, NULL, DI_NORMAL);
			return;
		}

		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		SomBitBlt(hDC, x, y, m_BmpSize.cx, m_BmpSize.cy, SomTemporaryDC, m_BmpSize.cx * block_index, 0);

		SelectObject(SomTemporaryDC, hOld);
	}
	void BitBlt(HDC hDC, int x, int y, int cx, int cy, int x1, int y1, int block_index = 0)
	{
		ATLASSERT(m_eType != SOMBITMAP_ICON);

		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		SomBitBlt(hDC, x, y, cx, cy, SomTemporaryDC, m_BmpSize.cx * block_index + x1, y1);

		SelectObject(SomTemporaryDC, hOld);
	}
	// 水平方向拉伸
	virtual void StretchBlt(HDC hDC, int x, int y, int cx, int block_index = 0)
	{
		ATLASSERT(m_eType != SOMBITMAP_ICON);

		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		SomStretchBlt(hDC, x, y, cx, m_BmpSize.cy, SomTemporaryDC, m_BmpSize.cx * block_index, 0, m_BmpSize.cx, m_BmpSize.cy);

		SelectObject(SomTemporaryDC, hOld);
	}
	virtual void StretchBlt(HDC hDC, int x, int y, int cx, int cy, int block_index)
	{
		ATLASSERT(m_eType != SOMBITMAP_ICON);

		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		SomStretchBlt(hDC, x, y, cx, cy, SomTemporaryDC, m_BmpSize.cx * block_index, 0, m_BmpSize.cx, m_BmpSize.cy);

		SelectObject(SomTemporaryDC, hOld);
	}
	void StretchBlt(HDC hDC, int x, int y, int cx, int cy, int x1, int y1, int cx1, int cy1, int block_index = 0)
	{
		ATLASSERT(m_eType != SOMBITMAP_ICON);

		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		SomStretchBlt(hDC, x, y, cx, cy, SomTemporaryDC, m_BmpSize.cx * block_index + x1, y1, cx1, cy1);

		SelectObject(SomTemporaryDC, hOld);
	}
	int GetWidth()
	{
		LoadBitmap();

		return m_BmpSize.cx;
	}
	int GetHeight()
	{
		LoadBitmap();

		return m_BmpSize.cy;
	}

protected:
	BOOL SomBitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1)
	{
		if (m_bTransparent)
		{
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
			return ::AlphaBlend(hdc, x, y, cx, cy, hdcSrc, x1, y1, cx, cy, bf);
		}
		else
			return ::BitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, SRCCOPY);
	}
	BOOL SomStretchBlt(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc)
	{
		if (m_bTransparent)
		{
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA};
			return ::AlphaBlend(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, bf);
		}
		else
		{
			::SetStretchBltMode(hdcDest, HALFTONE);
			return ::StretchBlt(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
		}
	}

	union
	{
		LPCWSTR m_lpBmpFile;
		DWORD	m_BmpResourceID;
	};
	union
	{
		HBITMAP m_hBitmap;
		HICON m_hIcon;
	};
	SIZE	m_BmpSize;
	WORD	m_blocks;
	BYTE	m_bTransparent;
	BYTE	m_eType;

#ifdef SOM_SUPPORT_PNG_JPG
	void LoadBitmap(PBYTE pData, DWORD len)
	{
		if (m_hBitmap)
			return;

		if (m_eType == SOMBITMAP_PNG)
		{
			//CSomPtr<ILibPng, true> spLibPng;
			//m_hBitmap	= spLibPng->Decode(pData, len);
			m_hBitmap = png_Decode(pData, len);
		}
// 		else if (m_eType == SOMBITMAP_JPG)
// 		{
// 			CSomPtr<IJpeg, true> spJpeg;
// 			m_hBitmap	= spJpeg->Decode(pData, len);
// 		}

		ATLASSERT(m_hBitmap);

		if (m_hBitmap == NULL)
		{
			m_blocks		= 0;
			m_BmpSize.cx	= 0;
			m_BmpSize.cy	= 0;
			m_bTransparent	= FALSE;
		}
		else
		{
			BITMAP bm;
			GetObject(m_hBitmap, sizeof(BITMAP), &bm);
			m_BmpSize.cx	= bm.bmWidth / m_blocks;
			m_BmpSize.cy	= bm.bmHeight;
			m_bTransparent	= (bm.bmBitsPixel == 32);

			ATLASSERT(m_BmpSize.cx * m_blocks == bm.bmWidth);
		}

	}
#endif //SOM_SUPPORT_PNG_JPG

	void LoadBitmap()
	{
		if (m_hBitmap)
			return;

		if (m_eType == SOMBITMAP_BMP)
		{
			if ((DWORD)m_lpBmpFile < 0xffff)
				m_hBitmap	= (HBITMAP)LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(m_BmpResourceID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			else
				m_hBitmap	= (HBITMAP)LoadImage(NULL, m_lpBmpFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		}
		else
		{
#ifdef SOM_SUPPORT_PNG_JPG
			if ((DWORD)m_lpBmpFile < 0xffff)
			{
				CResource resBmp;
				if (m_eType == SOMBITMAP_PNG && resBmp.Load(L"PNG", m_BmpResourceID))
				{
// 					CSomPtr<ILibPng, true> spLibPng;
// 					m_hBitmap	= spLibPng->Decode((PBYTE)resBmp.Lock(), resBmp.GetSize());
					m_hBitmap = png_Decode((PBYTE)resBmp.Lock(), resBmp.GetSize());
				}
// 				else if (m_eType == SOMBITMAP_JPG &&  resBmp.Load(L"JPG", m_BmpResourceID))
// 				{
// 					CSomPtr<IJpeg, true> spJpeg;
// 					m_hBitmap	= spJpeg->Decode((PBYTE)resBmp.Lock(), resBmp.GetSize());
// 				}
			}
			else
			{
				FILE* f = NULL;
				if (_wfopen_s(&f, m_lpBmpFile, L"rb") == 0 && f)
				{
					fseek(f, 0, SEEK_END);
					int flen = ftell(f);
					fseek(f, 0, SEEK_SET);
					PBYTE p = new BYTE[flen];
					if (p)
					{
						fread(p, 1, flen, f);
						if (m_eType == SOMBITMAP_PNG)
						{
// 							CSomPtr<ILibPng, true> spLibPng;
// 							m_hBitmap	= spLibPng->Decode(p, flen);
							m_hBitmap = png_Decode(p, flen);
						}
// 						else if (m_eType == SOMBITMAP_JPG)
// 						{
// 							CSomPtr<IJpeg, true> spJpeg;
// 							m_hBitmap	= spJpeg->Decode(p, flen);
// 						}

						delete[] p;
					}
					fclose(f);
				}
			}
#endif //SOM_SUPPORT_PNG_JPG
		}

		ATLASSERT(m_hBitmap);

		if (m_hBitmap == NULL)
		{
			m_blocks		= 0;
			m_BmpSize.cx	= 0;
			m_BmpSize.cy	= 0;
			m_bTransparent	= FALSE;
		}
		else
		{
			BITMAP bm;
			GetObject(m_hBitmap, sizeof(BITMAP), &bm);
			m_BmpSize.cx	= bm.bmWidth / m_blocks;
			m_BmpSize.cy	= bm.bmHeight;
			m_bTransparent	= (bm.bmBitsPixel == 32);

			ATLASSERT(m_BmpSize.cx * m_blocks == bm.bmWidth);
		}

	}
};

// 水平方向分 n 张小图，每张小图分 3 部分，第1，3部分不拉伸，2部分拉伸拷贝
class CSomBitmapHorizen3Part : public CSomBitmap
{
public:
	// margin 2，3 指定每部分的起始偏移，第 1 部分从 0 开始，所以不用指定，blocks 指定水平方向一共分多少张同样大小的小图
	CSomBitmapHorizen3Part(DWORD dwResourceID, int margin2, int margin3, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: CSomBitmap(dwResourceID, blocks, type)
		, m_margin2(margin2)
		, m_margin3(margin3)
	{

	}
	// block_index 指定拷贝哪一张小图
	virtual void StretchBlt(HDC hDC, int x, int y, int cx, int block_index = 0)
	{
		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		int xbmp = m_BmpSize.cx * block_index;
		int margin = cx - m_margin2 + m_margin3 - m_BmpSize.cx;

		int xdest = x;
		SomBitBlt(hDC, xdest, y, m_margin2, m_BmpSize.cy, SomTemporaryDC, xbmp, 0);
		xdest += m_margin2;
		if (margin)
		{
			SomStretchBlt(hDC, xdest, y, margin, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin2, 0, m_margin3 - m_margin2, m_BmpSize.cy);
			xdest += margin;
		}
		SomBitBlt(hDC, xdest, y, m_BmpSize.cx - m_margin3, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin3, 0);

		SelectObject(SomTemporaryDC, hOld);
	}
protected:
	int m_margin2;
	int m_margin3;
};

// 水平方向分 n 张小图，每张小图分5部分，第1，3，5部分不拉伸，2，4部分拉伸拷贝
class CSomBitmapHorizen5Part : public CSomBitmap
{
public:
	// margin 2，3，4，5 指定每部分的起始偏移，第 1 部分从 0 开始，所以不用指定，blocks 指定水平方向一共分多少张同样大小的小图
	CSomBitmapHorizen5Part(DWORD dwResourceID, int margin2, int margin3, int margin4, int margin5, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: CSomBitmap(dwResourceID, blocks, type)
		, m_margin2(margin2)
		, m_margin3(margin3)
		, m_margin4(margin4)
		, m_margin5(margin5)
	{
	}
	// block_index 指定拷贝哪一块
	virtual void StretchBlt(HDC hDC, int x, int y, int cx, int block_index = 0)
	{
		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);

		int xbmp = m_BmpSize.cx * block_index;
		int margin = cx - m_margin2 + m_margin3 - m_margin4 + m_margin5 - m_BmpSize.cx;

		if (margin > 0)
		{
			SetStretchBltMode(hDC, HALFTONE);
			int xdest = x;
			int width2 = margin / 2;
			int width4 = margin - width2;
			SomBitBlt(hDC, xdest, y, m_margin2, m_BmpSize.cy, SomTemporaryDC, xbmp, 0);
			xdest += m_margin2;
			if (width2)
			{
				SomStretchBlt(hDC, xdest, y, width2, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin2, 0, m_margin3 - m_margin2, m_BmpSize.cy);
				xdest += width2;
			}
			SomBitBlt(hDC, xdest, y, m_margin4 - m_margin3, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin3, 0);
			xdest += (m_margin4 - m_margin3);
			if (width4)
			{
				SomStretchBlt(hDC, xdest, y, width4, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin4, 0, m_margin5 - m_margin4, m_BmpSize.cy);
				xdest += width4;
			}
			SomBitBlt(hDC, xdest, y, m_BmpSize.cx - m_margin5, m_BmpSize.cy, SomTemporaryDC, xbmp + m_margin5, 0);
		}
		SelectObject(SomTemporaryDC, hOld);
	}
protected:
	int m_margin2;
	int m_margin3;
	int m_margin4;
	int m_margin5;
};

// 分 n 张小图，每张小图分 9 部分，成一个九宫格的形式拉伸以适应各种大小
class CSomBitmap9Part : public CSomBitmap
{
public:
	// margin 2，3 指定每部分的起始偏移，第 1 部分从 0 开始，所以不用指定，horizon 表示水平方向，vertical　表示垂直方向，blocks 指定水平方向一共分多少张同样大小的小图
	CSomBitmap9Part(DWORD dwResourceID, int horizon_margin2, int horizon_margin3, int vertical_margin2, int vertical_margin3, int blocks = 1, SOMBITMAP_TYPE type = SOMBITMAP_BMP)
		: CSomBitmap(dwResourceID, blocks, type)
		, m_horizon_margin2(horizon_margin2)
		, m_horizon_margin3(horizon_margin3)
		, m_vertical_margin2(vertical_margin2)
		, m_vertical_margin3(vertical_margin3)
	{

	}
	// block_index 指定拷贝哪一张小图
	virtual void StretchBlt(HDC hDC, int x, int y, int cx, int cy, int block_index)
	{
		LoadBitmap();

		HBITMAP hOld = (HBITMAP)SelectObject(SomTemporaryDC, m_hBitmap);
		SetStretchBltMode(hDC, HALFTONE);

		int xbmp = m_BmpSize.cx * block_index;
		int horizon_margin = cx - m_horizon_margin2 + m_horizon_margin3 - m_BmpSize.cx;
		int vertical_margin = cy - m_vertical_margin2 + m_vertical_margin3 - m_BmpSize.cy;

		int xdest = x;
		int ydest = y;
		SomBitBlt(hDC, xdest, ydest, m_horizon_margin2, m_vertical_margin2, SomTemporaryDC, xbmp, 0);
		xdest += m_horizon_margin2;
		if (horizon_margin)
		{
			SomStretchBlt(hDC, xdest, ydest, horizon_margin, m_vertical_margin2, SomTemporaryDC, xbmp + m_horizon_margin2, 0, m_horizon_margin3 - m_horizon_margin2, m_vertical_margin2);
			xdest += horizon_margin;
		}
		SomBitBlt(hDC, xdest, ydest, m_BmpSize.cx - m_horizon_margin3, m_vertical_margin2, SomTemporaryDC, xbmp + m_horizon_margin3, 0);
		ydest += m_vertical_margin2;

		if (vertical_margin)
		{
			xdest = x;
			SomStretchBlt(hDC, xdest, ydest, m_horizon_margin2, vertical_margin, SomTemporaryDC, xbmp, m_vertical_margin2, m_horizon_margin2, m_vertical_margin3 - m_vertical_margin2);
			xdest += m_horizon_margin2;
			if (horizon_margin)
			{
				SomStretchBlt(hDC, xdest, ydest, horizon_margin, vertical_margin, SomTemporaryDC, xbmp + m_horizon_margin2, m_vertical_margin2, m_horizon_margin3 - m_horizon_margin2, m_vertical_margin3 - m_vertical_margin2);
				xdest += horizon_margin;
			}
			SomStretchBlt(hDC, xdest, ydest, m_BmpSize.cx - m_horizon_margin3, vertical_margin, SomTemporaryDC, xbmp + m_horizon_margin3, m_vertical_margin2, m_BmpSize.cx - m_horizon_margin3, m_vertical_margin3 - m_vertical_margin2);
			ydest += vertical_margin;
		}

		xdest = x;
		SomBitBlt(hDC, xdest, ydest, m_horizon_margin2, m_BmpSize.cy - m_vertical_margin3, SomTemporaryDC, xbmp, m_vertical_margin3);
		xdest += m_horizon_margin2;
		if (horizon_margin)
		{
			SomStretchBlt(hDC, xdest, ydest, horizon_margin, m_BmpSize.cy - m_vertical_margin3, SomTemporaryDC, xbmp + m_horizon_margin2, m_vertical_margin3, m_horizon_margin3 - m_horizon_margin2, m_BmpSize.cy - m_vertical_margin3);
			xdest += horizon_margin;
		}
		SomBitBlt(hDC, xdest, ydest, m_BmpSize.cx - m_horizon_margin3, m_BmpSize.cy - m_vertical_margin3, SomTemporaryDC, xbmp + m_horizon_margin3, m_vertical_margin3);

		SelectObject(SomTemporaryDC, hOld);
	}
protected:
	int m_horizon_margin2;
	int m_horizon_margin3;
	int m_vertical_margin2;
	int m_vertical_margin3;
};

class CSomSkinManager;
__declspec(selectany) CSomSkinManager* g_pSomSkinManager = NULL;

class CSomSkinManager
{
public:
	CSomSkinManager()
	{
		g_pSomSkinManager	= this;

		CLogFont lf((HFONT)GetStockObject(DEFAULT_GUI_FONT));

		DWORD dwVersion = GetVersion();
		DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
		DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
		if (dwVersion < 0x80000000)
		{
			if (dwWindowsMajorVersion == 5)
			{
				StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), L"Tahoma");
			}
			else if (dwWindowsMajorVersion == 6)
			{
				StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), L"微软雅黑");
			}
			else
			{
				StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), L"Tahoma");
			}
		}
		else
			StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), L"Tahoma");

		lf.lfHeight	= -11;
//		lf.SetHeight(8, SomMemoryDC);
		m_Font1.CreateFontIndirect(&lf);

		lf.SetBold();
		m_Font2.CreateFontIndirect(&lf);

		lf.lfWeight	= FW_NORMAL;
		lf.lfHeight	= -12;
//		lf.SetHeight(9, SomTemporaryDC);
		m_Font3.CreateFontIndirect(&lf);

		lf.SetBold();
		m_Font4.CreateFontIndirect(&lf);

		lf.lfWeight	= FW_NORMAL;
		lf.lfHeight	= -13;
//		lf.SetHeight(10, SomTemporaryDC);
		m_Font5.CreateFontIndirect(&lf);

		lf.SetBold();
		m_Font6.CreateFontIndirect(&lf);

		lf.lfWeight	= FW_NORMAL;
		lf.lfHeight	= -16;
//		lf.SetHeight(12, SomTemporaryDC);
		m_Font7.CreateFontIndirect(&lf);

		lf.SetBold();
		m_Font8.CreateFontIndirect(&lf);
	}

	DWORD GetTextExtent(HFONT hFont, LPCWSTR lpString, int c, LPSIZE lpSize)
	{
		HFONT hOld = SomTemporaryDC.SelectFont(hFont);
		BOOL bRet = SomTemporaryDC.GetTextExtent(lpString, c, lpSize);
		SomTemporaryDC.SelectFont(hOld);

		return bRet ? lpSize->cx : 0;
	}
	void TextOut(HDC hDC, int x, int y, LPCWSTR lpString, int nCount, HFONT hFont, COLORREF color)
	{
		HGDIOBJ hOldFont = SelectObject(hDC, hFont);
		COLORREF oldColor = ::GetTextColor(hDC);
		::SetTextColor(hDC, color);
		::TextOut(hDC, x, y, lpString, nCount);
		::SetTextColor(hDC, oldColor);
		SelectObject(hDC, hOldFont);			
	}
	void DrawText(HDC hDC, LPCWSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, HFONT hFont, COLORREF color, int max_paint_width)
	{
		RECT rcText = *lpRect;
		if (max_paint_width != -1)
		{
			if (max_paint_width < rcText.left)
				return;
			else if (max_paint_width < rcText.right)
				rcText.right	= max_paint_width;
		}
		HGDIOBJ hOldFont = SelectObject(hDC, hFont);
		COLORREF color_old = ::GetTextColor(hDC); //add by yu
		::SetTextColor(hDC, color);
		if (rcText.right == lpRect->right)
			::TextOut(hDC, rcText.left, rcText.top, lpString, nCount);
		else
			::DrawText(hDC, lpString, nCount, &rcText, uFormat);
		::SetTextColor(hDC, color_old);//add by yu
		SelectObject(hDC, hOldFont);			
	}
	void Rectangle(HDC hDC, int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, HPEN hPen)
	{
		HGDIOBJ hOldPen = SelectObject(hDC, hPen);

		MoveToEx(hDC, nLeftRect, nTopRect, NULL);
		LineTo(hDC, nRightRect - 1, nTopRect);
		LineTo(hDC, nRightRect - 1, nBottomRect - 1);
		LineTo(hDC, nLeftRect, nBottomRect - 1);
		LineTo(hDC, nLeftRect, nTopRect);

		SelectObject(hDC, hOldPen);
	}

	// 获得字符串长度
	// 2010.05.14 张龙评审通过
	int GetTextLine(HFONT hFont, LPCWSTR lpText, int len, int nWidth)
	{
		ATLASSERT(nWidth > 0);
		SIZE size;
		int line = 0;
		int pos = 0;
		int i = 0;
		for (; i<len; i++)
		{
			g_pSomSkinManager->GetTextExtent(hFont, lpText, i-pos+1, &size);
			if (size.cx > nWidth)
			{
				lpText += i-pos;
				if (*lpText)
				{
					line++;
				}
				pos = i;
			}
		}
		if (i == len && *lpText)
		{
			line++;
		}
		return line;
	}

	CFont m_Font1;
	CFont m_Font2;
	CFont m_Font3;
	CFont m_Font4;
	CFont m_Font5;
	CFont m_Font6;
	CFont m_Font7;
	CFont m_Font8;
};

template<class TBase>
class CSomWndTipImpl : public TBase
{
public:
	void SetInfoTip(LPCWSTR lpText)
	{
		m_strTextTip	= lpText;
	}
protected:
	void OnGetInfoTip(UINT_PTR uTipId, LPNMTTDISPINFO lpNMTTDISPINFO)
	{
		ATLASSERT(m_strTextTip.GetLength());

		lpNMTTDISPINFO->lpszText	= m_strTextTip.GetBuffer();
	}
	void OnMouseEnter(LPPOINT pos)
	{
		if (!m_strTextTip.IsEmpty())
			m_pWndManager->SomAddTooltip(this, NULL, NULL);

		TBase::OnMouseEnter(pos);
	}
	void OnMouseLeave()
	{
		m_pWndManager->SomDelTooltip(this);
		TBase::OnMouseLeave();
	}
	CString m_strTextTip;
};

//支持折行，字体颜色
class CSomStatic : public CSomWnd
{
public:
	CSomStatic()
	{
		m_iLineWidth = -1;
		m_iLineHeight = 20;
		m_bLink = false;
		m_bDown = false;
		m_bHover = false;
		m_LinkNormalColor = 0x984E00;
		m_LinkOverColor = 0xDA7003;
		SetFont(g_pSomSkinManager->m_Font1);
		SetTextColor(0);
	}
	~CSomStatic()
	{

	}
	void SetLink(LPCWSTR lpLink)
	{
		m_sLink = lpLink;
		m_bLink = true;
	}
	void SetFont(HFONT hFont)
	{
		m_DefaultFont	= hFont;
	}
	void SetTextColor(COLORREF color)
	{
		if (m_DefaultColor != color)
		{
			m_DefaultColor = color;
			RedrawWindow();
		}
	}
	LPCWSTR GetWindowText()
	{
		return m_WindowText;
	}
	void SetWindowText(LPCWSTR lpText)
	{
		m_WindowText = lpText;
		CalculateRect();
		if (m_pWndManager)
			RedrawWindow();
	}
	void SetLineWidth(int iLineWidth)
	{
		m_iLineWidth = iLineWidth;
	}
	void SetLineHeight(int iLineHeight)
	{
		m_iLineHeight = iLineHeight;
	}
protected:
	typedef struct TEXTDATA
	{
		int iLine;
		RECT rect;
		CString	sText;
	}TextData,*LPTextData;
	void CalculateRect()
	{
		m_TextDataArray.RemoveAll();
		SIZE size;
		LPCWSTR lpText = m_WindowText.GetBuffer();
		int len = m_WindowText.GetLength();
		int line = 0;
		int pos = 0;
		int i = 0;
		for (; i<len; i++)
		{
			g_pSomSkinManager->GetTextExtent(m_DefaultFont, lpText, i-pos+1, &size);
			if (size.cx > m_iLineWidth)
			{
				TextData data;
				RECT rect = {0, m_iLineHeight*line, m_iLineWidth, m_iLineHeight*(line+1)};
				data.rect = rect;
				data.sText.Append(lpText, i-pos);
				data.iLine = line;
				lpText += i-pos;
				if (*lpText)
				{
					line++;
				}
				pos = i;
				m_TextDataArray.Add(data);
			}
		}
		if (i == len && *lpText)
		{
			TextData data;
			data.iLine = line;
			RECT rect = {0, m_iLineHeight*line, m_iLineWidth, m_iLineHeight*(line+1)};
			data.rect = rect;
			data.sText = lpText;
			lpText += i;
			len -= i;
			m_TextDataArray.Add(data);
		}
		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			g_pSomSkinManager->GetTextExtent(m_DefaultFont, m_WindowText.GetBuffer(), m_WindowText.GetLength(), &size);
			SizeWindow(size.cx>=m_iLineWidth?m_iLineWidth:size.cx, m_iLineHeight*(line+1));
		}
	}
	void OnCreate()
	{
		m_dwStyle |= SOMWND_TRANSPARENT;

		CalculateRect();
	}
	void OnCaptureChanged()
	{
		if (!m_bLink)
		{
			return;
		}
		if (m_bDown)
		{
			m_bDown	= false;
			RedrawWindow();

			if (m_bHover)
			{
				ATLASSERT(m_pWndManager != NULL);
				m_pWndManager->SomSendCommandMessage(this, BN_CLICKED, m_nID);
			}
		}
	}
	BOOL OnLButtonDown(LPPOINT pos)
	{
		if (!m_bLink)
		{
			return FALSE;
		}
		if (!m_bDown)
		{
			m_bDown	= true;
			RedrawWindow();

			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomSetCapture(this);
		}

		return TRUE;
	}
	BOOL OnLButtonDblClk(LPPOINT pos)
	{
		return OnLButtonDown(pos);
	}
	BOOL OnLButtonUp(LPPOINT pos)
	{
		if (!m_bLink)
		{
			return FALSE;
		}
		if (m_bDown)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}

		return TRUE;
	}
	void OnMouseEnter(LPPOINT pos)
	{
		if (!m_bLink)
		{
			return;
		}
		if (!m_bHover)
		{
			m_bHover	= true;
			RedrawWindow();
		}
	}
	void OnMouseLeave()
	{
		if (!m_bLink)
		{
			return;
		}
		if (m_bHover)
		{
			m_bHover	= false;
			RedrawWindow();
		}
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		SetBkMode(hDC, TRANSPARENT);
		RECT rcClip;
		GetClipBox(hDC, &rcClip);
		RECT rcTemp;
		if (IntersectRect(&rcTemp, &m_rcWindow, &rcClip))
		{
			HGDIOBJ hOldFont = NULL;
			COLORREF color;
			if (m_bLink)
			{
				if (m_bDown || m_bHover)
				{
					hOldFont = SelectObject(hDC, m_DefaultFont);
					color = ::SetTextColor(hDC, m_LinkOverColor);
				}
				else
				{
					hOldFont = SelectObject(hDC, m_DefaultFont);
					color = ::SetTextColor(hDC, m_LinkNormalColor);
				}
			}
			else
			{
				hOldFont = SelectObject(hDC, m_DefaultFont);
				color = ::SetTextColor(hDC, m_DefaultColor);
			}
			for (int i = 0; i < m_TextDataArray.GetSize(); i++)
			{
				::DrawText(hDC, m_TextDataArray[i].sText, m_TextDataArray[i].sText.GetLength(), &m_TextDataArray[i].rect, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL | DT_VCENTER);
			}
			::SetTextColor(hDC, color);
			SelectObject(hDC, hOldFont);
		}
	}

protected:
	bool m_bHover;
	bool m_bDown;
	CString m_WindowText;
	CFontHandle m_DefaultFont;
	COLORREF m_DefaultColor;
	COLORREF m_LinkNormalColor;
	COLORREF m_LinkOverColor;
	int m_iLineWidth;
	int m_iLineHeight;
	bool m_bLink;
	CString m_sLink;
	CSimpleArray<TextData> m_TextDataArray;
};

class CSomTextWnd : public CSomWnd
{
public:
	CSomTextWnd()
	{
		m_iMaxWidth	= -1;
		m_iHover	= -1;
		m_iDown		= -1;
		m_LinkNormalColor = 0x984E00;
		m_LinkOverColor = 0xDA7003;

		SetFont(g_pSomSkinManager->m_Font3);
		SetTextColor(0);
	}
	~CSomTextWnd()
	{

	}
	void SetFont(HFONT hFont)
	{
		m_DefaultFont	= hFont;
		CalculateRect();
	}
	void SetTextColor(COLORREF color)
	{
		if (m_DefaultColor != color)
		{
			m_DefaultColor = color;
			RedrawWindow();
		}
	}
	//2009-4-14 by ireton
	void SetLinkColor(COLORREF colorNormal, COLORREF colorOver)
	{
		m_LinkNormalColor = colorNormal;
		m_LinkOverColor = colorOver;
	}
	LPCWSTR GetWindowText()
	{
		return m_WindowText;
	}
	void SetMaxWidth(int iMaxWidth)
	{
		m_iMaxWidth	= iMaxWidth;
	}
	void SetWindowText(LPCWSTR lpText)
	{
		if (lpText == NULL)
			lpText	= L"";

		if (m_WindowText.Compare(lpText) == 0)
			return;

		if (m_iDown != -1)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}
		m_iHover	= -1;
		m_iDown		= -1;
		m_TextDataArray.RemoveAll();

		m_WindowText	= lpText;

		CSimpleArray<HFONT> FontStack;
		CSimpleArray<COLORREF> ColorStack;
		CSimpleArray<COLORREF> HyperLinkColorStack;
		CSimpleArray<CString> HyperLinkStack;

		FontStack.Add(NULL);
		ColorStack.Add(-1);
		HyperLinkColorStack.Add(-1);
		HyperLinkStack.Add(L"");

		CString outtext;
		for (LPCWSTR p = lpText; *p; p++)
		{
			if (*p == L'{' && *(p + 1) == L'&' && *(p + 2) == L'#')
			{
				LPCWSTR p2 = wcschr(p, L'}');
				if (p2)
				{
					if (outtext.GetLength())
					{
						TEXTDATA textdata;
						textdata.m_Text		= outtext;
						textdata.hyperlink	= HyperLinkStack[HyperLinkStack.GetSize() - 1];
						if (textdata.hyperlink.GetLength())
							textdata.color	= HyperLinkColorStack[HyperLinkColorStack.GetSize() - 1];
						else
							textdata.color	= ColorStack[ColorStack.GetSize() - 1];
						textdata.hFont		= FontStack[FontStack.GetSize() - 1];
						m_TextDataArray.Add(textdata);

						outtext.Empty();
					}

					p	+= 3;
					if (_wcsnicmp(p, L"a=", 2) == 0)
					{
						p += 2;
						CString strHyper;
						StringCchCopyN(strHyper.GetBuffer(p2 - p + 1), p2 - p + 1, p, p2 - p);
						strHyper.ReleaseBuffer(p2 - p);
						HyperLinkStack.Add(strHyper);
					}
					else if (_wcsnicmp(p, L"/a", 2) == 0)
					{
						if (HyperLinkStack.GetSize() > 1)
							HyperLinkStack.RemoveAt(HyperLinkStack.GetSize() - 1);
						else
							ATLASSERT(FALSE);
					}
					else if (_wcsnicmp(p, L"acolor=", 7) == 0)
					{
						p += 7;
						CString strColor;
						StringCchCopyN(strColor.GetBuffer(p2 - p + 1), p2 - p + 1, p, p2 - p);
						strColor.ReleaseBuffer(p2 - p);

						LPCWSTR pColor = strColor;
						LPWSTR ppStop;
						if (pColor[0] == L'#')
							HyperLinkColorStack.Add(wcstol(&pColor[1], &ppStop, 16));
						else if (pColor[0] == L'0' && (pColor[1] == L'x' || pColor[1] == L'X'))
							HyperLinkColorStack.Add(wcstol(&pColor[2], &ppStop, 16));
						else
							HyperLinkColorStack.Add(wcstol(pColor, &ppStop, 10));
					}
					else if (_wcsnicmp(p, L"/acolor", 7) == 0)
					{
						if (HyperLinkColorStack.GetSize() > 1)
							HyperLinkColorStack.RemoveAt(HyperLinkColorStack.GetSize() - 1);
						else
							ATLASSERT(FALSE);
					}
					else if (_wcsnicmp(p, L"font=", 5) == 0)
					{
						p += 5;
						CString strFont;
						StringCchCopyN(strFont.GetBuffer(p2 - p + 1), p2 - p + 1, p, p2 - p);
						strFont.ReleaseBuffer(p2 - p);
						int iFontIndex = _wtol(strFont);
						switch (iFontIndex)
						{
						case 1:
							FontStack.Add(g_pSomSkinManager->m_Font1);
							break;
						case 2:
							FontStack.Add(g_pSomSkinManager->m_Font2);
							break;
						case 3:
							FontStack.Add(g_pSomSkinManager->m_Font3);
							break;
						case 4:
							FontStack.Add(g_pSomSkinManager->m_Font4);
							break;
						case 5:
							FontStack.Add(g_pSomSkinManager->m_Font5);
							break;
						case 6:
							FontStack.Add(g_pSomSkinManager->m_Font6);
							break;
						case 7:
							FontStack.Add(g_pSomSkinManager->m_Font7);
							break;
						case 8:
							FontStack.Add(g_pSomSkinManager->m_Font8);
							break;
						default:
							ATLASSERT(FALSE);
						}
					}
					else if (_wcsnicmp(p, L"/font", 5) == 0)
					{
						if (FontStack.GetSize() > 1)
							FontStack.RemoveAt(FontStack.GetSize() - 1);
						else
							ATLASSERT(FALSE);
					}
					else if (_wcsnicmp(p, L"color=", 6) == 0)
					{
						p += 6;
						CString strColor;
						StringCchCopyN(strColor.GetBuffer(p2 - p + 1), p2 - p + 1, p, p2 - p);
						strColor.ReleaseBuffer(p2 - p);

						LPCWSTR pColor = strColor;
						LPWSTR ppStop;
						if (pColor[0] == L'#')
							ColorStack.Add(wcstol(&pColor[1], &ppStop, 16));
						else if (pColor[0] == L'0' && (pColor[1] == L'x' || pColor[1] == L'X'))
							ColorStack.Add(wcstol(&pColor[2], &ppStop, 16));
						else
							ColorStack.Add(wcstol(pColor, &ppStop, 10));
					}
					else if (_wcsnicmp(p, L"/color", 6) == 0)
					{
						if (ColorStack.GetSize() > 1)
							ColorStack.RemoveAt(ColorStack.GetSize() - 1);
						else
							ATLASSERT(FALSE);
					}
					else
						ATLASSERT(FALSE);

					p = p2;
				}
				else
					outtext += *p;
			}
			else
				outtext += *p;
		}
		if (outtext.GetLength())
		{
			TEXTDATA textdata;
			textdata.m_Text		= outtext;
			textdata.hyperlink	= HyperLinkStack[HyperLinkStack.GetSize() - 1];
			if (textdata.hyperlink.GetLength())
				textdata.color	= HyperLinkColorStack[HyperLinkColorStack.GetSize() - 1];
			else
				textdata.color	= ColorStack[ColorStack.GetSize() - 1];
			textdata.hFont		= FontStack[FontStack.GetSize() - 1];
			m_TextDataArray.Add(textdata);

			outtext.Empty();
		}

		CalculateRect();

		if (m_pWndManager)
			RedrawWindow();
	}
protected:
	typedef struct _TEXTDATA
	{
		CString m_Text;
		RECT rect;
		CFontHandle hFont;
		COLORREF color;
		CString hyperlink;
	} TEXTDATA, *PTEXTDATA;

	void CalculateRect()
	{
		int maxHeight = 0;
		for (int i = 0; i < m_TextDataArray.GetSize(); i++)
		{
			TEXTDATA& textdata = m_TextDataArray[i];

			SIZE size;
			g_pSomSkinManager->GetTextExtent(textdata.hFont ? textdata.hFont : m_DefaultFont, textdata.m_Text, textdata.m_Text.GetLength(), &size);
			textdata.rect.right		= size.cx;
			textdata.rect.bottom	= size.cy;
			maxHeight	= max(maxHeight, size.cy);
		}
		int cx = 0;
		for (int i = 0; i < m_TextDataArray.GetSize(); i++)
		{
			TEXTDATA& textdata = m_TextDataArray[i];

			RECT& rect = textdata.rect;

			rect.left	= cx;
			rect.top	= maxHeight - rect.bottom;
			rect.right	+= cx;
			rect.bottom	+= rect.top;
			cx			= rect.right;
		}

		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			if (m_iMaxWidth != -1 && cx > m_iMaxWidth)
				cx	= m_iMaxWidth;

			SizeWindow(cx, maxHeight);
		}
	}
	void OnCreate()
	{
		m_dwStyle |= SOMWND_TRANSPARENT;
		m_iHover	= -1;
		m_iDown		= -1;

		CalculateRect();
	}
	BOOL OnSetCursor(LPPOINT pos) 
	{
		for (int i = 0; i < m_TextDataArray.GetSize(); i++)
		{
			TEXTDATA& textdata = m_TextDataArray[i];
			if (textdata.hyperlink.GetLength())
			{
				if (PtInRect(&textdata.rect, *pos))
				{
					SetCursor(LoadCursor(NULL, IDC_HAND));
					return TRUE;
				}
			}
		}
		return FALSE; 
	}
	void OnCaptureChanged()
	{
		if (m_iDown != -1)
		{
			int iOldDown = m_iDown;
			m_iDown	= -1;
			RedrawWindow(&m_TextDataArray[iOldDown].rect, LPVOID(iOldDown + 1));

			if (m_iHover != -1)
			{
				ATLASSERT(m_TextDataArray[m_iHover].hyperlink.GetLength() > 0);
				ATLASSERT(m_pWndManager != NULL);

				LPCWSTR lpLink = m_TextDataArray[m_iHover].hyperlink;
				if (_wcsnicmp(lpLink, L"button:", 7) == 0)
				{
					lpLink	+= 7;
					LPWSTR ppStop;
					DWORD dwID = 0;
					if (lpLink[0] == L'0' && (lpLink[1] == L'x' || lpLink[1] == L'X'))
						dwID	= wcstol(&lpLink[2], &ppStop, 16);
					else
						dwID	= wcstol(lpLink, &ppStop, 10);
					m_pWndManager->SomSendCommandMessage(this, BN_CLICKED, (WORD)dwID);
				}
				else
					::ShellExecute(0, L"open", lpLink, 0, 0, SW_SHOWNORMAL);
			}
		}
	}
	BOOL OnLButtonDown(LPPOINT pos) 
	{ 
		for (int i = 0; i < m_TextDataArray.GetSize(); i++)
		{
			TEXTDATA& textdata = m_TextDataArray[i];
			if (PtInRect(&textdata.rect, *pos))
			{
				int iOldDown = m_iDown;
				if (textdata.hyperlink.GetLength())
				{
					if (m_iDown != i)
					{
						m_iDown	= i;
						RedrawWindow(&textdata.rect, LPVOID(i + 1));

						ATLASSERT(m_pWndManager != NULL);
						m_pWndManager->SomSetCapture(this);
					}
				}
				else
					m_iDown = -1;

				if (iOldDown != -1)
					RedrawWindow(&m_TextDataArray[iOldDown].rect, LPVOID(iOldDown + 1));
				break;
			}
		}
		return m_iDown != -1;
	}
	BOOL OnLButtonUp(LPPOINT pos) 
	{ 
		if (m_iDown != -1)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}

		return m_iDown != -1;
	}
	BOOL OnLButtonDblClk(LPPOINT pos) 
	{ 
		return OnLButtonDown(pos);
	}
	BOOL OnMouseMove(LPPOINT pos)
	{
		for (int i = 0; i < m_TextDataArray.GetSize(); i++)
		{
			TEXTDATA& textdata = m_TextDataArray[i];
			if (PtInRect(&textdata.rect, *pos))
			{
				int iOldHover = m_iHover;
				if (textdata.hyperlink.GetLength())
				{
					if (m_iHover != i)
					{
						m_iHover	= i;
						RedrawWindow(&textdata.rect, LPVOID(i + 1));
					}
				}
				else
					m_iHover = -1;

				if (iOldHover != -1)
					RedrawWindow(&m_TextDataArray[iOldHover].rect, LPVOID(iOldHover + 1));
				break;
			}
		}
		return FALSE; 
	}
	void OnMouseLeave()
	{
		if (m_iHover != -1)
		{
			int iOldHover = m_iHover;
			m_iHover	= -1;
			RedrawWindow(&m_TextDataArray[iOldHover].rect, LPVOID(iOldHover + 1));
		}
	}
	void OnDrawItem(HDC hDC, int max_paint_width, int iItem)
	{
		TEXTDATA& textdata = m_TextDataArray[iItem];
		COLORREF color;
		if (m_iHover == iItem)
			color	= m_LinkOverColor/*0xDA7003*/;
		else if (textdata.hyperlink.GetLength())
			color	= ((int)textdata.color) < 0 ? m_LinkNormalColor/*0x984E00*/ : (textdata.color & 0xffffff);
		else
			color	= ((int)textdata.color) < 0 ? m_DefaultColor : (textdata.color & 0xffffff);

		if (max_paint_width == -1)
			max_paint_width	= m_iMaxWidth;

		UINT   uFormat = DT_LEFT | DT_SINGLELINE | DT_NOPREFIX ;
		uFormat |= DT_PATH_ELLIPSIS ;

		g_pSomSkinManager->DrawText(hDC, textdata.m_Text, textdata.m_Text.GetLength(), &textdata.rect, uFormat, textdata.hFont ? textdata.hFont : m_DefaultFont, color, max_paint_width);
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		SetBkMode(hDC, TRANSPARENT);
		COLORREF oldColor = ::GetTextColor(hDC);
		if (lpRedrawParam != NULL)
			OnDrawItem(hDC, max_paint_width, (int)lpRedrawParam - 1);
		else
		{
			RECT rcClip;
			GetClipBox(hDC, &rcClip);
			for (int i = 0; i < m_TextDataArray.GetSize(); i++)
			{
				TEXTDATA& textdata = m_TextDataArray[i];
				RECT rcTemp;
				if (IntersectRect(&rcTemp, &textdata.rect, &rcClip))
					OnDrawItem(hDC, max_paint_width, i);
			}
		}
		::SetTextColor(hDC, oldColor);
	}

	CString m_WindowText;
	CSimpleArray<TEXTDATA> m_TextDataArray;
	CFontHandle m_DefaultFont;
	COLORREF m_DefaultColor;
	//2009-4-14 by ireton
	COLORREF m_LinkNormalColor;
	COLORREF m_LinkOverColor;
	int m_iMaxWidth;

	int m_iHover;
	int m_iDown;
};

// 2010-03-26 新加一个不过滤双击消息的CSomInstallTextWnd
// 管炜评审通过
class CSomInstallTextWnd : public CSomTextWnd
{
public:
	BOOL OnLButtonDblClk(LPPOINT pos){return TRUE;}
};

typedef CSomWndTipImpl<CSomTextWnd> CSomTextWndWithTip;

class CSomImageWnd : public CSomWnd
{
public:
	CSomImageWnd()
	{
		m_pBitmap		= NULL;
		m_block_index	= 0;
	}
	void LoadBitmap(CSomBitmap* lpBmp, int block_index = 0)
	{
		m_block_index	= block_index;
		m_pBitmap	= lpBmp;

		UpdateWnd();

		RedrawWindow();
	}
	void OnCreate()
	{
		UpdateWnd();
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		if (m_pBitmap)
			m_pBitmap->BitBlt(hDC, 0, 0, m_block_index);
	}
protected:
	CSomBitmap* m_pBitmap;
	int m_block_index;

	void UpdateWnd()
	{
		if (m_pBitmap == NULL)
			return;

		if (m_dwStyle & SOMWND_AUTOSIZE)
			SizeWindow(m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		if (m_pBitmap->IsTransparent())
			m_dwStyle |= SOMWND_TRANSPARENT;
		else
			m_dwStyle &= ~SOMWND_TRANSPARENT;
	}
};

typedef CSomWndTipImpl<CSomImageWnd> CSomImageWndWithTip;

class CSomButton4 : public CSomWnd
{
public:
	CSomButton4()
	{
		m_pBackground	= NULL;
		m_pIcon			= 0;
		m_font = g_pSomSkinManager->m_Font3;
		m_color = 0;
		m_x_margin = 0;
		m_y_margin = 0;
	}
	void LoadBitmap(CSomBitmap* lpBmp)
	{
		m_pBackground	= lpBmp;
		UpdateSize();
	}
	void SetWindowText(LPCWSTR lpText)
	{
		m_WindowText	= lpText;
		UpdateSize();
	}

	void SetXMargin(int x)
	{
		m_x_margin = x;
	}

	void SetYMargin(int y)
	{
		m_y_margin = y;
	}

	void SetFont(HFONT hfont)
	{		
		m_font = hfont;
	}

	void SetColor(COLORREF color)
	{
		m_color = color;
	}

	void SetIcon(CSomBitmap* lpIcon, int block_index = 0)
	{
		m_icon_block_index	= block_index;
		m_pIcon				= lpIcon;
		UpdateSize();
	}
protected:
	void UpdateSize()
	{
		m_offset_x	= 0;
		int width = 0;
		if (m_pIcon)
			width	+= m_pIcon->GetWidth();

		if (m_WindowText.GetLength())
		{
			SIZE size;
			g_pSomSkinManager->GetTextExtent(m_font, m_WindowText, m_WindowText.GetLength(), &size);
			width	+= size.cx;
		}
		if (m_pIcon && m_WindowText.GetLength())
			width	+= 5;
		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			if (m_pBackground)
			{
				int width2 = width + 20;
				if (width2 < m_pBackground->GetWidth())
					width2	= m_pBackground->GetWidth();
				SizeWindow(width2, m_pBackground->GetHeight());
				m_offset_x	= (width2 - width) / 2;
			}
		}
		else
			m_offset_x	= (m_rcWindow.right - m_rcWindow.left - width) / 2;

		m_offset_x += m_x_margin;

		if (m_pWndManager)
			RedrawWindow();
	}
	void OnCreate()
	{
		m_dwStyle |= SOMWND_TRANSPARENT;

		m_bHover	= false;
		m_bDown	= false;
	}
	void OnCaptureChanged()
	{
		if (m_bDown)
		{
			m_bDown	= false;
			RedrawWindow();

			if (m_bHover)
			{
				ATLASSERT(m_pWndManager != NULL);
				m_pWndManager->SomSendCommandMessage(this, BN_CLICKED, m_nID);
			}
		}
	}
	BOOL OnLButtonDown(LPPOINT pos)
	{
		if (!m_bDown)
		{
			m_bDown	= true;
			RedrawWindow();

			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomSetCapture(this);
		}

		return TRUE;
	}
	BOOL OnLButtonDblClk(LPPOINT pos)
	{
		return OnLButtonDown(pos);
	}
	BOOL OnLButtonUp(LPPOINT pos)
	{
		if (m_bDown)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}

		return TRUE;
	}
	void OnMouseEnter(LPPOINT pos)
	{
		if (!m_bHover)
		{
			m_bHover	= true;
			RedrawWindow();
		}
	}
	void OnMouseLeave()
	{
		if (m_bHover)
		{
			m_bHover	= false;
			RedrawWindow();
		}
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		int block = 0;

		if (!IsWindowEnabled())
			block	= 3;
		else if (m_bHover)
		{
			if (m_bDown)
				block	= 2;
			else
				block	= 1;
		}
		if (m_pBackground)
			m_pBackground->StretchBlt(hDC, 0, 0, m_rcWindow.right - m_rcWindow.left, block);

		int x = m_offset_x;
		if (m_pIcon)
		{
			m_pIcon->BitBlt(hDC, x, (m_rcWindow.bottom - m_rcWindow.top - m_pIcon->GetHeight()) / 2, m_icon_block_index);
			x	+= (m_pIcon->GetWidth() + 5);
		}

		if (m_WindowText.GetLength() && m_pBackground)
		{
			SetBkMode(hDC, TRANSPARENT);
			COLORREF oldColor = ::GetTextColor(hDC);
			HGDIOBJ hOldFont = SelectObject(hDC, m_font);
			::SetTextColor(hDC, IsWindowEnabled() ? m_color : 0xa6a6a6);
			CDC dc_temp(hDC);
			TEXTMETRIC TextM; 
			dc_temp.GetTextMetrics(&TextM);
			int n_cheight = TextM.tmHeight;
			int n_pic_height = m_pBackground->GetHeight();
			int n_align = (n_pic_height - n_cheight) / 2;
			n_align -= 1;
			n_align += m_y_margin;
			TextOut(hDC, x, m_bDown ? /*4 : 3*/n_align + 1 : n_align, m_WindowText, m_WindowText.GetLength());
			dc_temp.Detach();
			SelectObject(hDC, hOldFont);
			::SetTextColor(hDC, oldColor);
		}
	}
protected:
	bool m_bHover;
	bool m_bDown;
	CString m_WindowText;

	CSomBitmap* m_pIcon;
	int m_icon_block_index;
	CSomBitmap* m_pBackground;
	int m_offset_x;

	CFontHandle m_font;
	COLORREF m_color;
	int m_x_margin;
	int m_y_margin;
};

class CSomButton3 : public CSomWnd
{
public:
	CSomButton3()
	{
		m_pBackground	= NULL;
		m_pIcon			= NULL;
	}
	void LoadBitmap(CSomBitmap* lpBmp)
	{
		m_pBackground	= lpBmp;

		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			SizeWindow(m_pBackground->GetWidth(), m_pBackground->GetHeight());
		}
		if (m_pWndManager)
			RedrawWindow();
	}
	void SetIcon(CSomBitmap* lpIcon, int block_index = 0)
	{
		if (lpIcon != m_pIcon || block_index != m_icon_block_index)
		{
			m_icon_block_index	= block_index;
			m_pIcon				= lpIcon;

			RedrawWindow();
		}
	}
protected:
	void OnCreate()
	{
		m_dwStyle |= SOMWND_TRANSPARENT;
		m_bHover	= false;
		m_bDown	= false;

		if (m_pBackground && m_pBackground->GetWidth() && m_dwStyle & SOMWND_AUTOSIZE)
			SizeWindow(m_pBackground->GetWidth(), m_pBackground->GetHeight());
	}
	void OnCaptureChanged()
	{
		if (m_bDown)
		{
			m_bDown	= false;
			RedrawWindow();

			if (m_bHover)
			{
				ATLASSERT(m_pWndManager != NULL);
				m_pWndManager->SomSendCommandMessage(this, BN_CLICKED, m_nID);
			}
		}
	}
	BOOL OnLButtonDown(LPPOINT pos)
	{
		if (!m_bDown)
		{
			m_bDown	= true;
			RedrawWindow();

			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomSetCapture(this);
		}

		return TRUE;
	}
	BOOL OnLButtonDblClk(LPPOINT pos)
	{
		return OnLButtonDown(pos);
	}
	BOOL OnLButtonUp(LPPOINT pos)
	{
		if (m_bDown)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}

		return TRUE;
	}
	void OnMouseEnter(LPPOINT pos)
	{
		if (!m_bHover)
		{
			m_bHover	= true;
			RedrawWindow();
		}
	}
	void OnMouseLeave()
	{
		if (m_bHover)
		{
			m_bHover	= false;
			RedrawWindow();
		}
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		int block_index = 0;

		if (!IsWindowEnabled())
			block_index	= 3;
		else if (m_bHover)
		{
			if (m_bDown)
				block_index	= 2;
			else
				block_index	= 1;
		}
		m_pBackground->BitBlt(hDC, 0, 0, block_index);

		if (m_pIcon)
			m_pIcon->BitBlt(hDC, (m_rcWindow.right - m_rcWindow.left - m_pIcon->GetWidth()) / 2, (m_rcWindow.bottom - m_rcWindow.top - m_pIcon->GetHeight()) / 2, m_icon_block_index);
	}
protected:
	bool m_bHover;
	bool m_bDown;

	CSomBitmap* m_pIcon;
	int m_icon_block_index;
	CSomBitmap* m_pBackground;
};

class CSomCheckBox : public CSomWnd
{
public:
	CSomCheckBox(CSomBitmap* lpBmp, BOOL bRadioStyle = FALSE)
		: m_pBitmap(lpBmp)
		, m_bRadioStyle(bRadioStyle)
	{
		m_bHover		= FALSE;
		m_bCheck		= FALSE;
		m_bGrayCheck	= FALSE;
		m_bDown			= FALSE;
	}
	BOOL GetCheck() const
	{
		return m_bCheck;
	}
	void SetGrayCheck()
	{
		if (m_bGrayCheck == FALSE)
		{
			m_bGrayCheck	= TRUE;
			RedrawWindow();
		}
	}
	void SetCheck(BOOL bCheck = TRUE)
	{
		if (m_bCheck != bCheck || m_bGrayCheck == TRUE)
		{
			m_bGrayCheck	= FALSE;
			m_bCheck	= bCheck;
			RedrawWindow();
		}
	}
	void SetWindowText(LPCWSTR lpText)
	{
		m_WindowText	= lpText;

		UpdateSize();
	}
protected:
	void UpdateSize()
	{
		int width = m_pBitmap->GetWidth();
		int height = m_pBitmap->GetHeight();

		if (m_WindowText.GetLength())
		{
			SIZE size;
			g_pSomSkinManager->GetTextExtent(g_pSomSkinManager->m_Font3, m_WindowText, m_WindowText.GetLength(), &size);
			width	+= (5 + size.cx);
			height = max(height, size.cy);

			m_dwStyle |= SOMWND_TRANSPARENT;
		}
		else
			m_dwStyle &= ~SOMWND_TRANSPARENT;

		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			SizeWindow(width, height);
		}

		if (m_pWndManager)
			RedrawWindow();
	}
	virtual void OnCreate()
	{
		m_bHover	= FALSE;
		m_bCheck	= FALSE;
		m_bDown		= FALSE;

		UpdateSize();
	}
	virtual BOOL OnLButtonDown(LPPOINT pos)
	{
		if (!m_bDown)
		{
			m_bDown	= TRUE;
			RedrawWindow();

			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomSetCapture(this);
		}

		return TRUE;
	}
	virtual BOOL OnLButtonDblClk(LPPOINT pos)
	{
		return OnLButtonDown(pos);
	}
	virtual BOOL OnLButtonUp(LPPOINT pos)
	{
		if (m_bDown)
		{
			ATLASSERT(m_pWndManager != NULL);
			m_pWndManager->SomReleaseCapture(this);
		}

		return TRUE;
	}
	virtual void OnCaptureChanged()
	{
		if (m_bDown)
		{
			m_bDown	= FALSE;

			if (m_bHover)
			{
				if (m_bRadioStyle)
					m_bCheck	= TRUE;
				else
				{
					if (m_bGrayCheck)
					{
						m_bCheck	= TRUE;
						m_bGrayCheck	= FALSE;
					}
					else
						m_bCheck	= !m_bCheck;
				}
			}
			RedrawWindow();

			if (m_bHover)
			{
				ATLASSERT(m_pWndManager != NULL);
				m_pWndManager->SomSendCommandMessage(this, BN_CLICKED, m_nID);
			}
		}
	}
	void OnMouseEnter(LPPOINT pos)
	{
		if (!m_bHover)
		{
			m_bHover	= TRUE;
			RedrawWindow();
		}
	}
	void OnMouseLeave()
	{
		if (m_bHover)
		{
			m_bHover	= FALSE;
			RedrawWindow();
		}
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		int block_index = m_bCheck ? 4 : 0;

		if (!IsWindowEnabled())
			block_index	= m_bCheck ? 7 : 3;
		else if (m_bHover)
		{
			if (m_bDown)
				block_index	= m_bCheck ? 6 : 2;
			else
				block_index	= m_bCheck ? 5 : 1;
		}

		if (m_bGrayCheck && !m_bRadioStyle)
			block_index	= 8;

		if (m_WindowText.GetLength())
		{
			SetBkMode(hDC, TRANSPARENT);
			HGDIOBJ hOldFont = SelectObject(hDC, g_pSomSkinManager->m_Font3);
			TextOut(hDC, m_pBitmap->GetWidth() + 5, 0, m_WindowText, m_WindowText.GetLength());
			SelectObject(hDC, hOldFont);

			m_pBitmap->BitBlt(hDC, 0, 1, block_index);
		}
		else
			m_pBitmap->BitBlt(hDC, 0, 0, block_index);
	}
	BOOL m_bHover;
	BOOL m_bCheck;
	BOOL m_bGrayCheck;
	BOOL m_bDown;
	BOOL m_bRadioStyle;
	CSomBitmap* m_pBitmap;
	CString m_WindowText;
};

class CSomAnimation : public CSomWnd
{
public:
	CSomAnimation()
		: m_pBitmap(NULL)
	{

	}
	void OnCreate()
	{
		UpdateBitmap();
	}
	void LoadBitmap(CSomBitmap* lpBmp)
	{
		m_pBitmap	= lpBmp;
		UpdateBitmap();
	}
	void OnTimer()
	{
		if (!IsWindowVisible())
			return;

		m_AnimationIndex++;
		if (m_AnimationIndex >= m_pBitmap->GetBlockNums())
			m_AnimationIndex	= 0;

		RedrawWindow();
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		m_pBitmap->BitBlt(hDC, 0, 0, m_AnimationIndex);
	}
protected:
	void UpdateBitmap()
	{
		if (m_pBitmap)
		{
			if (m_pBitmap->IsTransparent())
				m_dwStyle |= SOMWND_TRANSPARENT;
			else
				m_dwStyle &= ~SOMWND_TRANSPARENT;

			m_AnimationIndex	= 0;

			if (m_dwStyle & SOMWND_AUTOSIZE)
			{
				SizeWindow(m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
			}
			RedrawWindow();
		}
	}
	int m_AnimationIndex;
	CSomBitmap* m_pBitmap;
};

typedef CSomWndTipImpl<CSomButton3> CSomButton3WithTip;

template<class TabItemExtraData>
class CSomTabCtrl : public CSomWnd
{
public:
	CSomTabCtrl(int default_button_cx, int default_button_cy)
		: m_iActive(-1)
		, m_iHover(-1)
		, m_iDefaultButtonWidth(default_button_cx)
		, m_iDefaultButtonHeight(default_button_cy)
	{

	}
	void AddTab(TabItemExtraData& ExtraData, int cx = -1, int margin = 0)
	{
		int x = (m_TabDataArray.size() == 0) ? margin : (m_TabDataArray[m_TabDataArray.size() - 1].rect.right + margin);
		if (cx == -1)
			cx	= m_iDefaultButtonWidth;

		TABDATA tabdata = { NULL, NULL, { x, 0, x + cx, m_iDefaultButtonHeight }, ExtraData };

		m_TabDataArray.push_back(tabdata);
		CalculateRect();
	}
	void GetItemRect(LPRECT lpRect)
	{
		*lpRect	= m_rcItem;
	}
	void SetItemRect(LPRECT lpRect)
	{
		m_rcItem	= *lpRect;

		if (m_iActive != -1)
		{
			HWND hActiveWnd = m_TabDataArray[m_iActive].hTabWnd;
			if (hActiveWnd != NULL)
				::SetWindowPos(hActiveWnd, NULL, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, SWP_NOZORDER);
		}
	}
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (m_iActive != -1)
		{
			HWND hActiveWnd = m_TabDataArray[m_iActive].hTabWnd;
			if (hActiveWnd != NULL)
				return (BOOL)::SendMessage(hActiveWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg);
		}

		return FALSE;
	}
	void SetCurSel(int index)
	{
		if (index < 0 || index >= (int)m_TabDataArray.size())
		{
			ATLASSERT(FALSE);
			return;
		}
		if (m_iActive == index)
			return;

		int iOldActive = m_iActive;
		m_iActive	= index;

		RedrawItem(m_iActive);
		RedrawItem(iOldActive);

		if (m_pWndManager)
		{
			NMHDR hdr = { (HWND)this, m_nID, TCN_SELCHANGE };
			m_pWndManager->SomSendMessage(WM_NOTIFY, 0, (LPARAM)&hdr);
		}
		if (iOldActive != -1)
		{
			HWND hOldWnd = m_TabDataArray[iOldActive].hTabWnd;
			if (hOldWnd != NULL)
				::ShowWindow(hOldWnd, FALSE);
		}
		if (m_iActive != -1)
		{
			HWND hActiveWnd = m_TabDataArray[m_iActive].hTabWnd;
			if (hActiveWnd != NULL)
			{
				::SetWindowPos(hActiveWnd, NULL, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, SWP_NOZORDER | SWP_SHOWWINDOW);
				::RedrawWindow(hActiveWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
			}
		}
	}
	int GetCurSel()
	{
		return m_iActive;
	}
	void SetTabInfo(int index, HWND hTabWnd, LPVOID pData = NULL)
	{
		if (index < 0 || index >= (int)m_TabDataArray.size())
		{
			ATLASSERT(FALSE);
			return;
		}
		m_TabDataArray[index].hTabWnd	= hTabWnd;;
		m_TabDataArray[index].pData		= pData;;
	}
	HWND GetTabHWND(int index)
	{
		if (index < 0 || index >= (int)m_TabDataArray.size())
		{
			ATLASSERT(FALSE);
			return NULL;
		}
		return m_TabDataArray[index].hTabWnd;
	}
	LPVOID GetTabData(int index)
	{
		if (index < 0 || index >= (int)m_TabDataArray.size())
		{
			ATLASSERT(FALSE);
			return NULL;
		}
		return m_TabDataArray[index].pData;
	}
	TabItemExtraData& GetTabParam(int index)
	{
		if (index < 0 || index >= (int)m_TabDataArray.size())
		{
			ATLASSERT(FALSE);
			return NULL;
		}
		return m_TabDataArray[index].ExtraData;
	}
protected:
	void RedrawItem(int iItem)
	{
		if (iItem != -1)
			RedrawWindow(&m_TabDataArray[iItem].rect, LPVOID(iItem + 1));
	}
	void CalculateRect()
	{
		if (m_dwStyle & SOMWND_AUTOSIZE)
		{
			if (m_TabDataArray.size() > 0)
			{
				SizeWindow(m_TabDataArray[m_TabDataArray.size() - 1].rect.right, m_iDefaultButtonHeight);
			}
		}
	}
	void OnCreate()
	{
		CalculateRect();		
	}
	BOOL OnLButtonDblClk(LPPOINT pos)
	{
		return TRUE;
	}
	BOOL OnLButtonDown(LPPOINT pos)
	{
		for (int i = 0; i < (int)m_TabDataArray.size(); i++)
		{
			TABDATA& tabdata = m_TabDataArray[i];
			if (PtInRect(&tabdata.rect, *pos))
			{
				SetCurSel(i);
				break;
			}
		}
		return m_iActive != -1;
	}
	BOOL OnMouseMove(LPPOINT pos)
	{
		for (int i = 0; i < (int)m_TabDataArray.size(); i++)
		{
			TABDATA& tabdata = m_TabDataArray[i];
			if (PtInRect(&tabdata.rect, *pos))
			{
				int iOldHover = m_iHover;
				if (m_iHover != i)
				{
					m_iHover	= i;
					RedrawItem(m_iHover);
				}
				RedrawItem(iOldHover);

				break;
			}
		}
		return FALSE; 
	}
	void OnMouseLeave()
	{
		if (m_iHover != -1)
		{
			int iOldHover = m_iHover;
			m_iHover	= -1;
			RedrawItem(iOldHover);
		}
	}

	typedef struct _TABDATA
	{
		HWND hTabWnd;
		LPVOID pData;
		RECT rect;
		TabItemExtraData ExtraData;

	} TABDATA, *PTABDATA;

	vector<TABDATA> m_TabDataArray;
	int m_iHover;
	int m_iActive;
	int m_iDefaultButtonWidth;
	int m_iDefaultButtonHeight;
	RECT m_rcItem;
};


typedef struct _ICONTABDATA
{
	CSomBitmap* pIcon;

} ICONTABDATA, *PICONTABDATA;

class CSomIconTabCtrl : public CSomTabCtrl<ICONTABDATA>
{
public:
	CSomIconTabCtrl(int default_button_cx, int default_button_cy)
		: CSomTabCtrl(default_button_cx, default_button_cy)
	{

	}
	void AddTab(CSomBitmap* pIcon, int cx = -1, int n_margin = 0)
	{
		ICONTABDATA icon_tabdata = { pIcon };

		CSomTabCtrl::AddTab(icon_tabdata, cx, n_margin);
	}

protected:
	virtual void OnDrawItem(HDC hDC, int max_paint_width, int iItem)
	{
		int block_index = 0;
		if (m_iActive == iItem)
			block_index = 2;
		else if (m_iHover == iItem)
			block_index = 1;

		TABDATA& tabdata = m_TabDataArray[iItem];

		RECT& rcItem = tabdata.rect;
		ICONTABDATA& icon_tabdata	= tabdata.ExtraData;
		icon_tabdata.pIcon->BitBlt(hDC, rcItem.left, rcItem.top, block_index);
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		if (lpRedrawParam != NULL)
			OnDrawItem(hDC, max_paint_width, (int)lpRedrawParam - 1);
		else
		{
			for (int i = 0; i < (int)m_TabDataArray.size(); i++)
				OnDrawItem(hDC, max_paint_width, i);
		}
	}
};

typedef struct _TEXTTABDATA
{
	CString caption;
	int iActiveTextOffset;

} TEXTTABDATA, *PTEXTTABDATA;

class CSomTextTabCtrl : public CSomTabCtrl<TEXTTABDATA>
{
public:
	CSomTextTabCtrl(CSomBitmap* pTabItemBackground, HFONT hNormalFont, HFONT hActiveFont)
		: CSomTabCtrl(pTabItemBackground->GetWidth(), pTabItemBackground->GetHeight())
		, m_pTabItemBackground(pTabItemBackground)
		, m_NormalFont(hNormalFont)
		, m_ActiveFont(hActiveFont)
	{

	}
	void AddTab(LPCWSTR lpText, int margin = 0)
	{
		TEXTTABDATA text_tabdata = { lpText };

		SIZE size, size2;
		g_pSomSkinManager->GetTextExtent(m_NormalFont, text_tabdata.caption, text_tabdata.caption.GetLength(), &size);
		g_pSomSkinManager->GetTextExtent(m_ActiveFont, text_tabdata.caption, text_tabdata.caption.GetLength(), &size2);

		text_tabdata.iActiveTextOffset	= (size2.cx - size.cx) / 2;

		CSomTabCtrl::AddTab(text_tabdata, size2.cx + 20, margin);
	}

protected:
	void OnCreate()
	{
		m_dwStyle |= SOMWND_TRANSPARENT;
		CSomTabCtrl<TEXTTABDATA>::OnCreate();
	}
	void OnDrawItem(HDC hDC, int max_paint_width, int iItem)
	{
		int block_index = 0;
		if (m_iActive == iItem)
			block_index = 2;
		else if (m_iHover == iItem)
			block_index = 1;

		TABDATA& tabdata = m_TabDataArray[iItem];

		RECT& rcItem = tabdata.rect;
		m_pTabItemBackground->StretchBlt(hDC, rcItem.left, rcItem.top, rcItem.right - rcItem.left, block_index);

		TEXTTABDATA& text_tabdata	= tabdata.ExtraData;

		HGDIOBJ hOldFont = SelectObject(hDC, (m_iActive == iItem) ? m_ActiveFont : m_NormalFont);
		TextOut(hDC, rcItem.left + 10 + ((m_iActive == iItem) ? 0 : text_tabdata.iActiveTextOffset), rcItem.top + 5, text_tabdata.caption, text_tabdata.caption.GetLength());
		SelectObject(hDC, hOldFont);
	}
	void OnPaint(HDC hDC, int max_paint_width, LPVOID lpRedrawParam)
	{
		SetBkMode(hDC, TRANSPARENT);

		if (lpRedrawParam != NULL)
			OnDrawItem(hDC, max_paint_width, (int)lpRedrawParam - 1);
		else
		{
			for (int i = 0; i < (int)m_TabDataArray.size(); i++)
				OnDrawItem(hDC, max_paint_width, i);
		}
	}
	CSomBitmap* m_pTabItemBackground;
	CFontHandle m_NormalFont;
	CFontHandle m_ActiveFont;
};


class CSomHeaderCtrl : public CWindowImpl<CSomHeaderCtrl, CHeaderCtrl>
{
public:
	BEGIN_MSG_MAP(CSomHeaderCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(HDM_LAYOUT, OnLayout)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_RELEASEDCAPTURE, OnReleasedCapture)
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_BEGINTRACK, OnBeginTrack)
		REFLECTED_NOTIFY_CODE_HANDLER(HDN_ENDTRACK, OnEndTrack)
	END_MSG_MAP()

	CSomHeaderCtrl(CSomBitmap* pHeaderBmp, CSomBitmap* pUpdownBmp)
		: m_pHeaderBmp(pHeaderBmp)
		, m_pUpdownBmp(pUpdownBmp)
	{
		m_SelectItem		= -1;
		m_HoverItem			= -1;
		m_SortItem			= -1;
		m_Ascending			= FALSE;
		m_bDragingDivider	= FALSE;
	}
	BOOL IsSortAscending()
	{
		return m_Ascending;
	}
	int GetSortItem()
	{
		return m_SortItem;
	}
	void SetSortItem(int iItem, BOOL bAscending)
	{
		m_Ascending	= bAscending;
		m_SortItem	= iItem;
		Invalidate();
	}
	LRESULT OnBeginTrack(int idCtrl, LPNMHDR lpNMHDR, BOOL& /*bHandled*/)
	{
		m_bDragingDivider	= TRUE;
		m_SelectItem		= -1;
		return FALSE;
	}
	LRESULT OnEndTrack(int idCtrl, LPNMHDR lpNMHDR, BOOL& /*bHandled*/)
	{
		m_bDragingDivider	= FALSE;
		return 0;
	}
	LRESULT OnReleasedCapture(int idCtrl, LPNMHDR lpNMHDR, BOOL& /*bHandled*/)
	{
		if (m_SelectItem != -1)
		{
			m_SelectItem	= -1;
			Invalidate();
		}
		return 0;
	}
	LRESULT OnLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPHDLAYOUT pLayout = (LPHDLAYOUT)lParam;
		LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);

		RECT* prc = pLayout->prc;

		WINDOWPOS* pwpos = pLayout->pwpos;

		pwpos->cy = prc->top = m_pHeaderBmp->GetHeight();

		return lResult;
	}
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CPaintDC dc(*this);

		RECT rcClient;
		GetClientRect(&rcClient);
		int xLastItem = 0;

		int count = GetItemCount();

		for (int i = 0; i < count; i++)
		{
			RECT rcItem;
			GetItemRect(i, &rcItem);

			xLastItem	= rcItem.right;

			if (rcItem.left >= rcClient.right || rcItem.right <= rcClient.left)
				continue;

			int block_index = 0;
			if (m_SelectItem == i)
				block_index	= 2;
			else if (m_HoverItem == i)
				block_index	= 1;
			else if (m_SortItem == i)
				block_index	= 3;

			m_pHeaderBmp->StretchBlt(dc, rcItem.left, rcItem.top, rcItem.right - rcItem.left, block_index);

			if (m_SortItem == i)
			{
				int x_updown = (rcItem.right + rcItem.left - m_pUpdownBmp->GetWidth()) / 2;
				if (x_updown >= rcItem.left)
					m_pUpdownBmp->BitBlt(dc, x_updown, 1, m_Ascending ? 0 : 1);
			}

			rcItem.left	+= 5;
			rcItem.right -= 5;
			if (rcItem.right > rcItem.left)
			{
				WCHAR szText[MAX_PATH];
				HDITEM hditem = { HDI_TEXT | HDI_FORMAT | HDI_ORDER, 0, szText, NULL, MAX_PATH };
				GetItem(i, &hditem);

				UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER;
				if (hditem.fmt & HDF_CENTER)
					uFormat |= DT_CENTER;
				else if (hditem.fmt & HDF_RIGHT)
					uFormat |= DT_RIGHT;
				else
					uFormat |= DT_LEFT;

				dc.SetBkMode(TRANSPARENT);
				HFONT hOld = dc.SelectFont(g_pSomSkinManager->m_Font3);
				dc.DrawText(szText, wcslen(szText), &rcItem, uFormat);
				dc.SelectFont(hOld);
			}
		}
		if (rcClient.right > xLastItem)
			m_pHeaderBmp->StretchBlt(dc, xLastItem, rcClient.top, rcClient.right - xLastItem, 0);

		return TRUE;
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return TRUE;
	}
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (m_HoverItem != -1)
		{
			m_HoverItem	= -1;
			Invalidate();
		}
		return 0;
	}
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled	= FALSE;

		if (m_bDragingDivider)
			return 0;

		HDHITTESTINFO hhti = { {LOWORD(lParam), HIWORD(lParam)} };
		int iItem = HitTest(&hhti);

		if (iItem != m_SelectItem)
		{
			m_SelectItem	= iItem;
			Invalidate();
		}
		return 0;
	}
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		if (m_bDragingDivider)
			return 0;

		HDHITTESTINFO hhti = { {LOWORD(lParam), HIWORD(lParam)} };
		int iItem = HitTest(&hhti);

		if (iItem != m_HoverItem)
		{
			if (iItem != -1)
			{
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, *this, HOVER_DEFAULT };
				TrackMouseEvent(&tme);
			}

			m_HoverItem	= iItem;
			Invalidate();
		}

		return 0;
	}
protected:
	int m_SelectItem;
	int m_HoverItem;
	int m_SortItem;
	BOOL m_Ascending;
	BOOL m_bDragingDivider;
	CSomBitmap* m_pHeaderBmp;
	CSomBitmap* m_pUpdownBmp;
};

template<class TBase>
class CSomListDrawWhenNoItem : public TBase
{
	BEGIN_MSG_MAP(CSomListDrawWhenNoItem)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	CSomListDrawWhenNoItem()
		: m_pDefaultIcon(NULL)
	{

	}
	virtual void OnDrawWhenNoItem(HDC hDC)
	{
		if (m_pDefaultIcon)
			m_pDefaultIcon->BitBlt(hDC, m_DefaultIconPt.x, m_DefaultIconPt.y, m_block_index);
		if (m_DefaultText.GetLength())
		{
			::SetBkMode(hDC, TRANSPARENT);
			g_pSomSkinManager->TextOut(hDC, m_DefaultTextPt.x, m_DefaultTextPt.y, m_DefaultText, m_DefaultText.GetLength(), m_DefaultTextFont, m_DefaultTextColor);
		}
	}
	void SetDefaultIconText(CSomBitmap* pIcon = NULL, LPCWSTR lpText = NULL, HFONT hFont = NULL, COLORREF color = 0, int block_index = 0)
	{
		m_pDefaultIcon		= pIcon;
		m_block_index		= block_index;
		m_DefaultText		= lpText;
		m_DefaultTextFont	= hFont;
		m_DefaultTextColor	= color;

		UpdateLayout();
	}
	void UpdateLayout(int cx = 0, int cy = 0)
	{
		if (cx == 0 && cy == 0)
		{
			RECT rc;
			GetClientRect(&rc);
			cx = rc.right;
			cy = rc.bottom;
		}
		int w = 0;
		SIZE szText = { 0 };
		if (m_pDefaultIcon)
			w	+= (m_pDefaultIcon->GetWidth() + 10);

		if (m_DefaultText.GetLength())
		{
			g_pSomSkinManager->GetTextExtent(m_DefaultTextFont, m_DefaultText, m_DefaultText.GetLength(), &szText);
			w	+= szText.cx;
		}
		m_DefaultIconPt.x	= (cx - w) / 2;
		if (m_pDefaultIcon)
		{
			m_DefaultIconPt.y	= (cy - m_pDefaultIcon->GetHeight()) / 2;
			m_DefaultTextPt.x	= m_DefaultIconPt.x + m_pDefaultIcon->GetWidth() + 10;
		}
		m_DefaultTextPt.y	= (cy - szText.cy) / 2;
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			UpdateLayout(cx, cy);
		}

		bHandled = FALSE;
		return 1;
	}
protected:
	CSomBitmap* m_pDefaultIcon;
	int m_block_index;
	CString m_DefaultText;
	CFontHandle m_DefaultTextFont;
	COLORREF m_DefaultTextColor;
	POINT m_DefaultIconPt;
	POINT m_DefaultTextPt;
};
class CSomScrollList;

class CSomHoverList : public CSomListBox
{
public:
	CSomHoverList(CSomBitmap* pListBackground, HBRUSH hBackgroundBrush)
		: m_pListBackground(pListBackground)
		, m_BackgroundBrush(hBackgroundBrush)
	{

	}
	void SetItemText(int nItem, LPCWSTR lpText)
	{
		CSomTextWnd* pSMTextWnd = (CSomTextWnd*)GetSomControlByIndex(nItem, 0);
		if (pSMTextWnd && wcscmp(pSMTextWnd->GetWindowText(), lpText) != 0)
		{
			pSMTextWnd->SetWindowText(lpText);

			RECT rcItem;
			GetItemRect(nItem, &rcItem);
			RedrawWindow(&rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}
	virtual void OnDrawItemBackgound(HDC hDC, BOOL bHover, BOOL bCheck, LPCRECT rcItem, int nItem, DWORD_PTR pItemKey, LPCRECT lpRectDraw = NULL)
	{
		if (nItem == -1)
		{
			if (lpRectDraw == NULL)
				lpRectDraw = rcItem;

			FillRect(hDC, lpRectDraw, m_BackgroundBrush);
		}
		else
		{
			POINT pt;
			HDC hTargetDC	= hDC;
			if (SomMemoryDC)
				hTargetDC = SomMemoryDC;
			else
				SetViewportOrgEx(hTargetDC, rcItem->left, rcItem->top, &pt);

			int block_index = 0;
			if (bCheck)
				block_index	= 2;
			else if (bHover)
				block_index	= 1;

			m_pListBackground->StretchBlt(hTargetDC, 0, 0, rcItem->right - rcItem->left, block_index);

			CSomTextWnd* pSMTextWnd = (CSomTextWnd*)GetSomControlByIndex(nItem, 0);
			if (pSMTextWnd)
			{
				CString strText	= pSMTextWnd->GetWindowText();
				SetBkMode(hTargetDC, TRANSPARENT);
				HGDIOBJ hOldFont = SelectObject(hTargetDC, bCheck ? g_pSomSkinManager->m_Font4 : g_pSomSkinManager->m_Font3);
				COLORREF oldColor = ::GetTextColor(hTargetDC);
				::SetTextColor(hTargetDC, bCheck ? 0xFFFFFF : 0);
				RECT rcText = { 20, bCheck ? 4 : 5, rcItem->right, rcItem->bottom };
				DrawText(hTargetDC, strText, strText.GetLength(), &rcText, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
				::SetTextColor(hTargetDC, oldColor);
				SelectObject(hTargetDC, hOldFont);
			}
			if (hTargetDC == hDC)
				SetViewportOrgEx(hTargetDC, pt.x, pt.y, NULL);
			else
				::BitBlt(hDC, rcItem->left, rcItem->top, rcItem->right - rcItem->left, m_pListBackground->GetHeight(), hTargetDC, 0, 0, SRCCOPY);
		}
	}
protected:
	CSomBitmap* m_pListBackground;
	CBrushHandle m_BackgroundBrush;
};

class CSomScrollList : public CWindowImpl<CSomScrollList>, public CSomWndManager
{
public:
	BEGIN_MSG_MAP(CSomScrollList)
		CHAIN_MSG_MAP(CSomWndManager)
		COMMAND_HANDLER(IDC_LIST_MAIN, LBN_SELCHANGE, OnLBSelChange)
		COMMAND_HANDLER(ID_BUTTON_UP, BN_CLICKED, OnButtonUp)
		COMMAND_HANDLER(ID_BUTTON_DOWN, BN_CLICKED, OnButtonDown)
		COMMAND_HANDLER(IDC_LIST_MAIN, SOM_LBN_SCROLL, OnSomlistScrollNotify)
		REFLECT_NOTIFICATIONS()
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	enum
	{
		ID_BUTTON_UP	= 0x8000,
		ID_BUTTON_DOWN	= 0x8001,
		IDC_LIST_MAIN	= 0x8002
	};
	enum
	{
		BACKGROUND_LEFT		= 0x1,
		BACKGROUND_RIGHT	= 0x2,
		BACKGROUND_TOP		= 0x4,
		BACKGROUND_BOTTOM	= 0x8
	};
	CSomScrollList(CSomBitmap* pListBackground, HPEN hBackgroundPen, int eBackroundBound, HBRUSH hBackgroundBrush, CSomBitmap* pButtonUp, CSomBitmap* pButtonDown)
		: m_pListBackground(pListBackground)
		, m_HoverList(pListBackground, hBackgroundBrush)
		, m_BackgroundPen(hBackgroundPen)
		, m_eBackroundBound(eBackroundBound)
		, m_BackgroundBrush(hBackgroundBrush)
		, m_pButtonUp(pButtonUp)
		, m_pButtonDown(pButtonDown)
	{

	}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_HoverList.Create(m_hWnd, NULL, 0, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LBS_OWNERDRAWVARIABLE | LBS_NOTIFY, 0, IDC_LIST_MAIN);
		m_HoverList.SetItemHeight(m_pListBackground->GetHeight());
		m_HoverList.EnableItemHover();

		m_BtnUp.CreateControl(this, ID_BUTTON_UP);
		m_BtnDown.CreateControl(this, ID_BUTTON_DOWN);

		m_BtnUp.LoadBitmap(m_pButtonUp);
		m_BtnDown.LoadBitmap(m_pButtonDown);

		bHandled	= FALSE;
		return 0;
	}
	BOOL AddString(LPCWSTR lpText)
	{
		CSomListBox::LBCONTROL lbcontrol[] = { 0, new CSomTextWnd(), 20, 5, 0, 0, 0, SOMWND_AUTOSIZE };

		BOOL bRet	= m_HoverList.AddString(lbcontrol, sizeof(lbcontrol) / sizeof(lbcontrol[0]));

		CSomTextWnd* pSMTextWnd = (CSomTextWnd*)lbcontrol[0].pSomWnd;
		pSMTextWnd->SetWindowText(lpText);

		return bRet;
	}
	int SetCurSel(int nSelect)
	{
		return m_HoverList.SetCurSel(nSelect);
	}
	int GetCurSel() const
	{
		return m_HoverList.GetCurSel();
	}
	void ResetContent()
	{
		m_HoverList.ResetContent();
	}
	int GetCount() const
	{
		return m_HoverList.GetCount();
	}
	void SetRedraw(BOOL bRedraw = TRUE)
	{
		m_HoverList.SetRedraw(bRedraw);
	}
	void SetItemText(int nItem, LPCWSTR lpText)
	{
		m_HoverList.SetItemText(nItem, lpText);
	}
	LRESULT OnLBSelChange(int iNofity, int idCtrl, HWND hCtrl, BOOL& bHandled)
	{
		return ::SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), iNofity), (LPARAM)(HWND)this);
	}
	LRESULT OnButtonUp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_HoverList.SendMessage(WM_VSCROLL, SB_LINEUP);
		return 0;
	}
	LRESULT OnButtonDown(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_HoverList.SendMessage(WM_VSCROLL, SB_LINEDOWN);
		return 0;
	}
	LRESULT OnSomlistScrollNotify(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		UpdateUpDownEnabled();

		return 0;
	}
	void UpdateUpDownEnabled()
	{
		int count = m_HoverList.GetCount();

		if (count == 0)
			return;

		if (m_BtnUp.IsWindowVisible())
			m_BtnUp.EnableWindow(m_HoverList.GetTopIndex() != 0);
		if (m_BtnDown.IsWindowVisible())
		{
			RECT rc;
			m_HoverList.GetClientRect(&rc);

			RECT rcTail;
			m_HoverList.GetItemRect(count - 1, &rcTail);

			m_BtnDown.EnableWindow(rcTail.bottom > rc.bottom);
		}
	}
	void UpdateLayout(int cx = 0, int cy = 0)
	{
		if (cx == 0 && cy == 0)
		{
			RECT rc;
			GetClientRect(&rc);
			cx = rc.right;
			cy = rc.bottom;
		}

		bool bShowUpDown	= FALSE;

		int count = m_HoverList.GetCount();
		if (count)
		{
			RECT rcTail;
			m_HoverList.GetItemRect(count - 1, &rcTail);
			if (rcTail.bottom >= cy || m_HoverList.GetTopIndex() != 0)
				bShowUpDown	= TRUE;
		}
		int x = (m_eBackroundBound & BACKGROUND_LEFT) ? 2 : 1;
		int y = (m_eBackroundBound & BACKGROUND_TOP) ? 2 : 1;
		int cx2 = cx - x - ((m_eBackroundBound & BACKGROUND_RIGHT) ? 2 : 1);
		int cy2 = cy - y - ((m_eBackroundBound & BACKGROUND_BOTTOM) ? 2 : 1);

		if (bShowUpDown)
		{
			m_BtnUp.ShowWindow();
			m_BtnDown.ShowWindow();
			m_BtnUp.SetWindowPos(x, y, cx2, m_pButtonUp->GetHeight());
			m_BtnDown.SetWindowPos(x, y + cy2 - m_pButtonDown->GetHeight(), cx2, m_pButtonDown->GetHeight());
			y += (m_pButtonUp->GetHeight() + 1);
			cy2 -= (m_pButtonUp->GetHeight() + m_pButtonDown->GetHeight() + 2);
		}
		else
		{
			m_BtnUp.ShowWindow(FALSE);
			m_BtnDown.ShowWindow(FALSE);

		}
		m_HoverList.SetWindowPos(NULL, x, y, cx2, cy2, SWP_NOZORDER);
		UpdateUpDownEnabled();
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			UpdateLayout(cx, cy);
		}

		bHandled = FALSE;
		return 1;
	}
	void OnDrawBackground(HDC hDC, LPCRECT lpRectDraw)
	{
		RECT rcClient;
		GetClientRect(&rcClient);

		RECT rcBack = rcClient;

		HGDIOBJ hOldPen = SelectObject(hDC, m_BackgroundPen);

		if (m_eBackroundBound & BACKGROUND_LEFT)
		{
			MoveToEx(hDC, 0, 0, NULL);
			LineTo(hDC, 0, rcClient.bottom);
			rcBack.left += 1;
		}
		if (m_eBackroundBound & BACKGROUND_TOP)
		{
			MoveToEx(hDC, 0, 0, NULL);
			LineTo(hDC, rcClient.right, 0);
			rcBack.top += 1;
		}
		if (m_eBackroundBound & BACKGROUND_RIGHT)
		{
			MoveToEx(hDC, rcClient.right - 1, 0, NULL);
			LineTo(hDC, rcClient.right - 1, rcClient.bottom);
			rcBack.right -= 1;
		}
		if (m_eBackroundBound & BACKGROUND_BOTTOM)
		{
			MoveToEx(hDC, 0, rcClient.bottom - 1, NULL);
			LineTo(hDC, rcClient.right, rcClient.bottom - 1);
			rcBack.bottom -= 1;
		}

		SelectObject(hDC, hOldPen);

		FillRect(hDC, &rcBack, m_BackgroundBrush);
	}
protected:
	CSomHoverList m_HoverList;
	CSomButton4 m_BtnUp;
	CSomButton4 m_BtnDown;
	CSomBitmap* m_pListBackground;
	CSomBitmap* m_pButtonUp;
	CSomBitmap* m_pButtonDown;
	CPenHandle m_BackgroundPen;
	int m_eBackroundBound;
	CBrushHandle m_BackgroundBrush;
};

class CSomDialog : public CIndirectDialogImpl<CSomDialog>, public CSomWndManager
{
public:
	BEGIN_MSG_MAP(CSomDialog)
		CHAIN_MSG_MAP(CSomWndManager)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_RANGE_CODE_HANDLER(IDOK, IDCONTINUE, BN_CLICKED, OnCloseCmd)
	END_MSG_MAP()

	BEGIN_DIALOG(0, 0, 187, 98)
	DIALOG_STYLE(m_dwDialogStyle)
	DIALOG_EXSTYLE(m_dwDialogExStyle)
	DIALOG_FONT(8, L"MS Shell Dlg")
	END_DIALOG()

	BEGIN_CONTROLS_MAP()
	END_CONTROLS_MAP()

	CSomDialog()
		: m_pBackground(NULL)
		, m_pCloseFace(NULL)
		, m_dwDialogStyle(WS_POPUP)
		, m_dwDialogExStyle(0)
	{

	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled	= FALSE;

		m_BtnSysClose.CreateControl(this, IDCANCEL);
		m_BtnSysClose.LoadBitmap(m_pCloseFace);

		return 0;
	}
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SendMessage(WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		return CSomWndManager::OnLButtonDown(uMsg,wParam,lParam,bHandled);
	}
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		::EndDialog(m_hWnd, wID);
		return 0;
	}
	virtual void OnSetWindowRgn(int cx, int cy)
	{
		CRgn WndRgn = CreateRoundRectRgn(0, 0, cx + 1, cy + 1, 7, 7);

		POINT pt[4] = { {1, 1}, {cx - 2, 1}, {1, cy - 2}, {cx - 2, cy -2} };
		for (int i = 0; i < 4; i++)
		{
			CRgn rgn;
			rgn.CreateRectRgn(pt[i].x, pt[i].y, pt[i].x + 1, pt[i].y + 1);
			WndRgn.CombineRgn(WndRgn, rgn, RGN_DIFF);
		}
		SetWindowRgn(WndRgn, TRUE);
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			OnSetWindowRgn(cx, cy);

			m_BtnSysClose.MoveWindow(cx - ((GetStyle() & WS_MAXIMIZE) ? 2 : 5) - m_BtnSysClose.GetClientWidth(), 1);
		}

		bHandled = FALSE;
		return 1;
	}
	void OnDrawBackground(HDC hDC, LPCRECT lpRectDraw)
	{
		RECT rcClient;
		GetClientRect(&rcClient);
		if (m_pBackground)
			m_pBackground->StretchBlt(hDC, 0, 0, rcClient.right, rcClient.bottom,  0);
	}
protected:
	CSomButton3 m_BtnSysClose;
	CSomBitmap* m_pBackground;
	CSomBitmap* m_pCloseFace;
	DWORD m_dwDialogStyle;
	DWORD m_dwDialogExStyle;
};


class CSomMessageBox : public CSomDialog
{
public:
	BEGIN_MSG_MAP(CSomMessageBox)
		CHAIN_MSG_MAP(CSomDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	CSomMessageBox(LPCWSTR lpText, UINT uType)
		: m_pButtonFace(NULL)
	{
		m_uType	= uType;

		m_logo	= NULL;
		switch (m_uType & 0xF0)
		{
		case MB_ICONHAND:
			m_logo	= LoadIcon(NULL, IDI_HAND);
			break;
		case MB_ICONQUESTION:
			m_logo	= LoadIcon(NULL, IDI_QUESTION);
			break;
		case MB_ICONEXCLAMATION:
			m_logo	= LoadIcon(NULL, IDI_EXCLAMATION);
			break;
		case MB_ICONASTERISK:
			m_logo	= LoadIcon(NULL, IDI_ASTERISK);
			break;
		}

		CString strText = lpText;
		wchar_t* pszContext = NULL;
		LPWSTR token = wcstok_s(strText.GetBuffer(), L"\n", &pszContext);					
		while (token != NULL)
		{
			CSomTextWnd* pSMTextWnd = new CSomTextWnd();
			m_TextContentVec.push_back(pSMTextWnd);
			pSMTextWnd->SetWindowText(token);
			token = wcstok_s(NULL, L"\n", &pszContext);
		}
		strText.ReleaseBuffer();
	}
	void UpdateLayout(int cx = 0, int cy = 0)
	{
		if (cx == 0 && cy == 0)
		{
			RECT rc;
			GetClientRect(&rc);
			cx = rc.right;
			cy = rc.bottom;
		}

		m_icon.MoveWindow(10, 4);
		m_Caption.MoveWindow(30, 4);

		int x = (m_logo == NULL ? 25 : 65);
		int y = m_TextContentVec.size() == 1 ?  59 : 50;

		for (vector<CSomTextWnd*>::iterator iter = m_TextContentVec.begin(); iter != m_TextContentVec.end(); iter++)
		{
			(*iter)->MoveWindow(x, y);
			y	+= (5 + (*iter)->GetClientHeight());
		}

		x = cx - 20 - m_Button3.GetClientWidth();
		m_Button3.MoveWindow(x, cy - 40);
		x	-= (15 + m_Button2.GetClientWidth());
		m_Button2.MoveWindow(x, cy - 40);
		x	-= (15 + m_Button1.GetClientWidth());
		m_Button1.MoveWindow(x, cy - 40);
	}
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_icon.Create(this);

		m_Caption.Create(this);
		m_Caption.SetWindowText(L"{&#font=4}360软件管家");

		for (vector<CSomTextWnd*>::iterator iter = m_TextContentVec.begin(); iter != m_TextContentVec.end(); iter++)
			(*iter)->Create(this);

		m_Button1.LoadBitmap(m_pButtonFace);
		m_Button2.LoadBitmap(m_pButtonFace);
		m_Button3.LoadBitmap(m_pButtonFace);

		switch (m_uType & 0xF)
		{
		case MB_OKCANCEL:
			m_Button1.CreateControl(this, IDOK, SOMWND_AUTOSIZE);
			m_Button2.CreateControl(this, IDOK);
			m_Button3.CreateControl(this, IDCANCEL);
			m_Button2.SetWindowText(L"确定");
			m_Button3.SetWindowText(L"取消");
			break;
		case MB_ABORTRETRYIGNORE:
			m_Button1.CreateControl(this, IDABORT);
			m_Button2.CreateControl(this, IDRETRY);
			m_Button3.CreateControl(this, IDIGNORE);
			m_Button1.SetWindowText(L"终止");
			m_Button2.SetWindowText(L"重试");
			m_Button3.SetWindowText(L"忽略");
			m_BtnSysClose.EnableWindow(FALSE);
			break;
		case MB_YESNOCANCEL:
			m_Button1.CreateControl(this, IDYES);
			m_Button2.CreateControl(this, IDNO);
			m_Button3.CreateControl(this, IDCANCEL);
			m_Button1.SetWindowText(L"是");
			m_Button2.SetWindowText(L"否");
			m_Button3.SetWindowText(L"取消");
			break;
		case MB_YESNO:
			m_Button1.CreateControl(this, IDOK, SOMWND_AUTOSIZE);
			m_Button2.CreateControl(this, IDYES);
			m_Button3.CreateControl(this, IDNO);
			m_Button2.SetWindowText(L"是");
			m_Button3.SetWindowText(L"否");
			//m_BtnSysClose.EnableWindow(FALSE);
			break;
		case MB_RETRYCANCEL:
			m_Button1.CreateControl(this, IDOK, SOMWND_AUTOSIZE);
			m_Button2.CreateControl(this, IDRETRY);
			m_Button3.CreateControl(this, IDCANCEL);
			m_Button2.SetWindowText(L"重试");
			m_Button3.SetWindowText(L"取消");
			break;
		case MB_CANCELTRYCONTINUE:
			m_Button1.CreateControl(this, IDCANCEL);
			m_Button2.CreateControl(this, IDTRYAGAIN);
			m_Button3.CreateControl(this, IDCONTINUE);
			m_Button1.SetWindowText(L"取消");
			m_Button2.SetWindowText(L"重试");
			m_Button3.SetWindowText(L"继续");
			break;
		default:
			m_Button1.CreateControl(this, IDOK, SOMWND_AUTOSIZE);
			m_Button2.CreateControl(this, IDOK, SOMWND_AUTOSIZE);
			m_Button3.CreateControl(this, IDOK);
			m_Button3.SetWindowText(L"确定");
		}	


		int message_width = 0;
		int message_height = 0;
		for (vector<CSomTextWnd*>::iterator iter = m_TextContentVec.begin(); iter != m_TextContentVec.end(); iter++)
		{
			CSomTextWnd* pSomTextWnd	= *iter;
			if (pSomTextWnd->GetClientWidth() > (DWORD)message_width)
				message_width	= pSomTextWnd->GetClientWidth();

			message_height	+= (5 + pSomTextWnd->GetClientHeight());
		}
		if (m_logo)
			message_width += 40;

		message_width	+= 50;
		message_height	+= 120;

		if (message_width < 290)
			message_width	= 290;
		if (message_height < 160)
			message_height	= 160;

		RECT rc = {0, 0, message_width, message_height};
		::AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());

		SetWindowPos(NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER|SWP_NOMOVE);

		CenterWindow();

		bHandled	= FALSE;

		return 0;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		for (vector<CSomTextWnd*>::iterator iter = m_TextContentVec.begin(); iter != m_TextContentVec.end(); iter++)
			delete *iter;

		m_TextContentVec.clear();

		bHandled	= FALSE;
		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			UpdateLayout(cx, cy);
		}

		bHandled = FALSE;
		return 1;
	}
	void OnDrawBackground(HDC hDC, LPCRECT lpRectDraw)
	{
		CSomDialog::OnDrawBackground(hDC, lpRectDraw);

		if (m_logo)
			DrawIconEx(hDC, 25, 50, m_logo, 32, 32, 0, NULL, DI_NORMAL);
	}
protected:
	CSomImageWnd m_icon;
	CSomTextWnd m_Caption;
	vector<CSomTextWnd*> m_TextContentVec;
	CSomButton4 m_Button1;
	CSomButton4 m_Button2;
	CSomButton4 m_Button3;
	HICON m_logo;
	UINT m_uType;
	CSomBitmap* m_pButtonFace;
};

template <class T, int cx, int cy>
class CSomResizableWnd
{
public:
	typedef CSomResizableWnd<T, cx, cy> SomResizableWnd;

	BEGIN_MSG_MAP(SomResizableWnd)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblclk)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
	END_MSG_MAP()

protected:
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if (pT->GetStyle() & WS_MAXIMIZE)
			return HTCLIENT;

		RECT rt;
		pT->GetClientRect(&rt);
		POINT pt;
		pt.x	= LOWORD(lParam);
		pt.y	= HIWORD(lParam);
		pT->ScreenToClient(&pt);
		if (pt.x > rt.right - rt.left - 5 && pt.y > rt.bottom - rt.top - 5)
			return HTBOTTOMRIGHT;
		if (pt.x < rt.left + 5 && pt.y > rt.bottom - rt.top - 5)
			return HTBOTTOMLEFT;
		if (pt.x < rt.left + 5 && pt.y < rt.top + 5)
			return HTTOPLEFT;
		if (pt.x > rt.right - rt.left - 5 && pt.y < rt.top + 5)
			return HTTOPRIGHT;
		if (pt.x < rt.left + 5)
			return HTLEFT;
		if (pt.x > rt.right - rt.left - 5)
			return HTRIGHT;
		if (pt.y < rt.top + 5)
			return HTTOP;
		if (pt.y > rt.bottom - rt.top - 5)
			return HTBOTTOM;

		//		if (pt.y < 18)
		//			return HTCAPTION;
		//		else
		return HTCLIENT;
	}
	LRESULT OnLButtonDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if (pT->GetStyle() & WS_MAXIMIZE)
			pT->ShowWindow(SW_RESTORE);
		else
			pT->ShowWindow(SW_MAXIMIZE);

		return 0;
	}
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMINMAXINFO	pMINMAXINFO	= (LPMINMAXINFO)lParam;

		pMINMAXINFO->ptMinTrackSize.x	= cx;
		pMINMAXINFO->ptMinTrackSize.y	= cy;

		RECT rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

		pMINMAXINFO->ptMaxPosition.x	= rcWorkArea.left - 2;
		pMINMAXINFO->ptMaxPosition.y	= rcWorkArea.top - 2;
		pMINMAXINFO->ptMaxSize.x		= rcWorkArea.right - rcWorkArea.left + 4;
		pMINMAXINFO->ptMaxSize.y		= rcWorkArea.bottom - rcWorkArea.top + 4;

		return 0;
	}
};