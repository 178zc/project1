﻿#include <regex>
#include <atlconv.h>  // 用于字符转换
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MFCApplication1.h"
#endif

#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"
#include "SearchReplaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "InputDlg.h"
IMPLEMENT_DYNCREATE(CMFCApplication1View, CScrollView)

BEGIN_MESSAGE_MAP(CMFCApplication1View, CScrollView)
    ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_MOUSEMOVE()      // 添加鼠标移动
    ON_WM_LBUTTONUP()      // 添加鼠标左键释放
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
    ON_COMMAND(ID_EDIT_FIND, &CMFCApplication1View::OnEditFind)
    ON_COMMAND(ID_EDIT_REPLACE, &CMFCApplication1View::OnEditReplace)
    ON_COMMAND(ID_VIEW_DARK_THEME, &CMFCApplication1View::OnViewDarkTheme)
    ON_COMMAND(ID_VIEW_LIGHT_THEME, &CMFCApplication1View::OnViewLightTheme)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DARK_THEME, &CMFCApplication1View::OnUpdateViewDarkTheme)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHT_THEME, &CMFCApplication1View::OnUpdateViewLightTheme)
END_MESSAGE_MAP()

CMFCApplication1View::CMFCApplication1View()
{
    m_nCursorPos = 0;
    m_bCursorVisible = TRUE;
    m_nCursorBlinkCounter = 0;
    m_nLineNumberWidth = 50;

    // 添加这些初始化
    m_nCurrentSearchPos = 0;
    m_currentMatchIndex = 0;
    m_searchResults.clear();

    // 初始化选择相关变量
    m_nSelectionStart = -1;
    m_nSelectionEnd = -1;
    m_bHasSelection = FALSE;

    // 设置初始主题为亮色
    m_bDarkTheme = FALSE;

    // 初始化亮色主题颜色
    m_crLightText = RGB(0, 0, 0);          // 黑色文字
    m_crLightBg = RGB(255, 255, 255);      // 白色背景
    m_crLightLineNumberText = RGB(100, 100, 100);      // 深灰色行号文字
    m_crLightLineNumberBg = RGB(240, 240, 240);        // 浅灰色行号背景

    // 初始化暗色主题颜色
    m_crDarkText = RGB(255, 255, 255);     // 白色文字
    m_crDarkBg = RGB(30, 30, 30);          // 深灰色背景
    m_crDarkLineNumberText = RGB(180, 180, 180);       // 浅灰色行号文字
    m_crDarkLineNumberBg = RGB(50, 50, 50);            // 深灰色行号背景

    // 应用初始主题
    m_crTextColor = m_crLightText;
    m_crBackgroundColor = m_crLightBg;
    m_crLineNumberText = m_crLightLineNumberText;
    m_crLineNumberBg = m_crLightLineNumberBg;
}
void CMFCApplication1View::ApplyTheme(BOOL bDarkTheme)
{
    m_bDarkTheme = bDarkTheme;

    if (m_bDarkTheme)
    {
        // 应用暗色主题
        m_crTextColor = m_crDarkText;
        m_crBackgroundColor = m_crDarkBg;
        m_crLineNumberText = m_crDarkLineNumberText;
        m_crLineNumberBg = m_crDarkLineNumberBg;
    }
    else
    {
        // 应用亮色主题
        m_crTextColor = m_crLightText;
        m_crBackgroundColor = m_crLightBg;
        m_crLineNumberText = m_crLightLineNumberText;
        m_crLineNumberBg = m_crLightLineNumberBg;
    }

    // 重绘视图
    Invalidate();
}
void CMFCApplication1View::OnViewDarkTheme()
{
    ApplyTheme(TRUE);
}

// 切换为亮色主题
void CMFCApplication1View::OnViewLightTheme()
{
    ApplyTheme(FALSE);
}

// 更新暗色主题菜单状态
void CMFCApplication1View::OnUpdateViewDarkTheme(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bDarkTheme);
}

// 更新亮色主题菜单状态
void CMFCApplication1View::OnUpdateViewLightTheme(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(!m_bDarkTheme);
}
CMFCApplication1View::~CMFCApplication1View()
{
}

int CMFCApplication1View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CScrollView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // 设置光标闪烁定时器
    SetTimer(1, 500, NULL); // 500ms闪烁一次

    return 0;
}

void CMFCApplication1View::OnDestroy()
{
    // 清除定时器
    KillTimer(1);
    CScrollView::OnDestroy();
}

void CMFCApplication1View::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        // 光标闪烁
        m_bCursorVisible = !m_bCursorVisible;
        m_nCursorBlinkCounter++;

        // 每10次闪烁（5秒）重置一次，避免长时间运行后计数器溢出
        if (m_nCursorBlinkCounter > 10)
            m_nCursorBlinkCounter = 0;

        // 如果当前有焦点，重绘光标
        if (GetFocus() == this)
        {
            CRect rect;
            GetClientRect(&rect);
            InvalidateRect(&rect);
        }
    }

    CScrollView::OnTimer(nIDEvent);
}

BOOL CMFCApplication1View::PreCreateWindow(CREATESTRUCT& cs)
{
    return CScrollView::PreCreateWindow(cs);
}

// 计算指定行前n个字符的像素宽度
int CMFCApplication1View::CalculateTextWidth(CDC* pDC, const CString& strLine, int nChars)
{
    if (nChars <= 0) return 0;

    // 获取前n个字符的子字符串
    CString strSub;
    if (nChars >= strLine.GetLength())
        strSub = strLine;
    else
        strSub = strLine.Left(nChars);

    // 使用GetTextExtentPoint32获取准确宽度
    CSize size;
    if (GetTextExtentPoint32(pDC->GetSafeHdc(), strSub, strSub.GetLength(), &size))
    {
        return size.cx;
    }

    // 如果失败，使用粗略估算（每个字符平均宽度）
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    return strSub.GetLength() * tm.tmAveCharWidth;
}

// 获取指定行的字符串
CString CMFCApplication1View::GetLineAt(int nLine)
{
    if (nLine < 0) return _T("");

    CString strResult;
    int nCurrentLine = 0;
    int nStart = 0;
    int nEnd = 0;

    while (nCurrentLine < nLine && nStart < m_strContent.GetLength())
    {
        nEnd = m_strContent.Find(_T('\n'), nStart);
        if (nEnd == -1)
        {
            // 没有找到足够的行数
            return _T("");
        }
        nStart = nEnd + 1;
        nCurrentLine++;
    }

    if (nCurrentLine == nLine)
    {
        nEnd = m_strContent.Find(_T('\n'), nStart);
        if (nEnd == -1)
            nEnd = m_strContent.GetLength();

        strResult = m_strContent.Mid(nStart, nEnd - nStart);
    }

    return strResult;
}

void CMFCApplication1View::OnDraw(CDC* pDC)
{
    CMFCApplication1Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    CRect rect;
    GetClientRect(&rect);

    // 绘制行号区域
    DrawLineNumbers(pDC, rect);

    // 设置文本区域
    CRect rectText = rect;
    rectText.left = m_nLineNumberWidth;

    // 填充文本背景
    CBrush brushBg(m_crBackgroundColor);
    pDC->FillRect(rectText, &brushBg);

    // 绘制选择背景（如果有选择）
    DrawSelection(pDC, rectText);

    // 设置文本字体和颜色
    CFont* pOldFont = pDC->SelectObject(&m_fontText);
    COLORREF crOldText = pDC->SetTextColor(m_crTextColor);
    int nOldBkMode = pDC->SetBkMode(TRANSPARENT);


    // 获取文本度量
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    int nCharWidth = tm.tmAveCharWidth;

    // 计算可见区域
    CPoint scrollPos = GetScrollPosition();
    CRect rectClip;
    pDC->GetClipBox(rectClip);

    // 分割文本为行
    CStringArray lines;
    int nStart = 0;
    int nEnd = m_strContent.Find(_T('\n'));

    while (nEnd != -1)
    {
        lines.Add(m_strContent.Mid(nStart, nEnd - nStart));
        nStart = nEnd + 1;
        nEnd = m_strContent.Find(_T('\n'), nStart);
    }

    // 添加最后一行
    lines.Add(m_strContent.Mid(nStart));

    // 计算可见行范围
    int nFirstVisibleLine = scrollPos.y / nCharHeight;
    int nLastVisibleLine = min((scrollPos.y + rect.Height() + nCharHeight - 1) / nCharHeight, lines.GetSize());

    // 绘制可见文本行
    for (int i = nFirstVisibleLine; i < nLastVisibleLine; i++)
    {
        CPoint pt;
        pt.x = m_nLineNumberWidth + 5 - scrollPos.x;
        pt.y = i * nCharHeight - scrollPos.y;

        pDC->TextOut(pt.x, pt.y, lines[i]);
    }

    // 绘制光标
    if (GetFocus() == this && m_bCursorVisible)
    {
        // 获取光标位置
        int nCursorLine, nCursorColumn;
        CharIndexToLineColumn(m_nCursorPos, nCursorLine, nCursorColumn);

        // 获取当前行字符串
        CString strLine = GetLineAt(nCursorLine);

        // 计算光标屏幕坐标
        int nCursorX = m_nLineNumberWidth + 5 + CalculateTextWidth(pDC, GetLineAt(nCursorLine), nCursorColumn) - scrollPos.x;
        int nCursorY = nCursorLine * nCharHeight - scrollPos.y;

        // 确保光标在可见区域内
        if (nCursorY >= -nCharHeight && nCursorY <= rect.Height())
        {
            // 绘制光标
            CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
            CPen* pOldPen = pDC->SelectObject(&pen);

            pDC->MoveTo(nCursorX, nCursorY);
            pDC->LineTo(nCursorX, nCursorY + nCharHeight);

            pDC->SelectObject(pOldPen);
        }
    }

    // 恢复原始设置
    pDC->SetBkMode(nOldBkMode);
    pDC->SetTextColor(crOldText);
    pDC->SelectObject(pOldFont);
}

void CMFCApplication1View::DrawLineNumbers(CDC* pDC, CRect& rect)
{
    int nSaveDC = pDC->SaveDC();

    CRect rectLineNumbers = rect;
    rectLineNumbers.right = m_nLineNumberWidth;

    // 填充背景
    CBrush brushBg(m_crLineNumberBg);
    pDC->FillRect(rectLineNumbers, &brushBg);

    // 绘制分隔线
    CPen penLine(PS_SOLID, 1, RGB(200, 200, 200));
    CPen* pOldPen = pDC->SelectObject(&penLine);
    pDC->MoveTo(rectLineNumbers.right, rect.top);
    pDC->LineTo(rectLineNumbers.right, rect.bottom);

    // 设置字体和颜色
    pDC->SelectObject(&m_fontLineNumber);
    pDC->SetTextColor(m_crLineNumberText);
    pDC->SetBkColor(m_crLineNumberBg);

    // 获取文本度量
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;

    // 计算可见区域
    CPoint scrollPos = GetScrollPosition();
    CRect rectClip;
    pDC->GetClipBox(rectClip);

    // 计算总行数
    int nTotalLines = 0;
    int nPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1)
    {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++; // 最后一行

    // 计算可见行范围
    int nFirstVisibleLine = scrollPos.y / nCharHeight;
    int nLastVisibleLine = min((scrollPos.y + rect.Height() + nCharHeight - 1) / nCharHeight, nTotalLines);

    // 绘制可见行号
    for (int i = nFirstVisibleLine; i < nLastVisibleLine; i++)
    {
        CRect rectLine;
        rectLine.left = rectLineNumbers.left + 5;
        rectLine.right = rectLineNumbers.right - 5;
        rectLine.top = i * nCharHeight - scrollPos.y;
        rectLine.bottom = rectLine.top + nCharHeight;

        CString strLine;
        strLine.Format(_T("%d"), i + 1);
        pDC->DrawText(strLine, rectLine, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    }

    pDC->SelectObject(pOldPen);
    pDC->RestoreDC(nSaveDC);
}

void CMFCApplication1View::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    // 创建字体
    LOGFONT lf = { 0 };
    lf.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);
    lf.lfWeight = FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    _tcscpy_s(lf.lfFaceName, _T("Consolas"));

    // 创建文本字体
    m_fontText.CreateFontIndirect(&lf);

    // 创建行号字体
    lf.lfHeight = -MulDiv(9, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);
    m_fontLineNumber.CreateFontIndirect(&lf);

    // 更新视图度量
    UpdateViewMetrics();

    // 更新文档标题
    CString strPath = GetDocument()->GetPathName();
    if (!strPath.IsEmpty())
    {
        CString strTitle;
        int nPos = strPath.ReverseFind('\\');
        if (nPos != -1)
            strTitle = strPath.Mid(nPos + 1);
        else
            strTitle = strPath;
        GetDocument()->SetTitle(strTitle);
    }
}

void CMFCApplication1View::UpdateViewMetrics()
{
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_fontText);

    // 计算文本度量
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    int nCharWidth = tm.tmAveCharWidth;

    // 计算总行数
    int nTotalLines = 0;
    int nPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1)
    {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++; // 最后一行

    // 计算最大行长度
    int nMaxLineLength = 0;
    nPos = 0;
    int nPrevPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPrevPos)) != -1)
    {
        int nLineLength = nPos - nPrevPos;
        if (nLineLength > nMaxLineLength)
            nMaxLineLength = nLineLength;
        nPrevPos = nPos + 1;
    }

    // 最后一行
    int nLastLineLength = m_strContent.GetLength() - nPrevPos;
    if (nLastLineLength > nMaxLineLength)
        nMaxLineLength = nLastLineLength;

    // 更新行号宽度
    CalculateLineNumberWidth();

    // 设置滚动大小
    CSize sizeTotal;
    sizeTotal.cx = m_nLineNumberWidth + 5 + nMaxLineLength * nCharWidth + 20; // 加一些边距
    sizeTotal.cy = nTotalLines * nCharHeight + 20;

    SetScrollSizes(MM_TEXT, sizeTotal);

    dc.SelectObject(pOldFont);
}

void CMFCApplication1View::CalculateLineNumberWidth()
{
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_fontLineNumber);

    // 计算总行数
    int nTotalLines = 0;
    int nPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1)
    {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++; // 最后一行

    // 计算最大行号所需的宽度
    CString strMaxLine;
    strMaxLine.Format(_T("%d"), nTotalLines);

    CSize size = dc.GetTextExtent(strMaxLine);
    m_nLineNumberWidth = size.cx + 20; // 增加一些边距

    dc.SelectObject(pOldFont);
}

void CMFCApplication1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 在修改前保存当前状态（包括光标位置）
    m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
    m_redoStack.clear();  // 清空重做栈

    switch (nChar)
    {
    case 8:  // Backspace
        if (m_nCursorPos > 0)
        {
            m_strContent.Delete(m_nCursorPos - 1, 1);
            m_nCursorPos--;
            Invalidate();
        }
        break;

    case 13: // Enter
        m_strContent.Insert(m_nCursorPos, _T('\n'));
        m_nCursorPos++;
        UpdateViewMetrics();
        Invalidate();
        break;

    default:
        if (nChar >= 32)
        {
            m_strContent.Insert(m_nCursorPos, (TCHAR)nChar);
            m_nCursorPos++;
            UpdateViewMetrics();
            Invalidate();
        }
        break;
    }

    CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CMFCApplication1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 检查Ctrl+Z和Ctrl+Y快捷键
    BOOL bCtrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    BOOL bShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    if (bCtrlPressed)
    {
        if (nChar == 'Z')  // Ctrl+Z 撤销
        {
            if (!m_undoStack.empty())
            {
                OnEditUndo();
                return;
            }
        }
        else if (nChar == 'Y')  // Ctrl+Y 重做
        {
            if (!m_redoStack.empty())
            {
                OnEditRedo();
                return;
            }
        }
        else if (nChar == 'C')  // Ctrl+C 复制
        {
            OnEditCopy();
            return;
        }
        else if (nChar == 'V')  // Ctrl+V 粘贴
        {
            OnEditPaste();
            return;
        }
        else if (nChar == 'X')  // Ctrl+X 剪切
        {
            OnEditCut();
            return;
        }
        else if (nChar == 'A')  // Ctrl+A 全选
        {
            OnEditSelectAll();
            return;
        }
    }
    int nOldCursorPos = m_nCursorPos;

    switch (nChar)
    {
    case VK_LEFT:
        if (m_nCursorPos > 0)
        {
            m_nCursorPos--;
        }
        break;

    case VK_RIGHT:
        if (m_nCursorPos < m_strContent.GetLength())
        {
            m_nCursorPos++;
        }
        break;

    case VK_UP:
    {
        int nCurrentLine, nCurrentColumn;
        CharIndexToLineColumn(m_nCursorPos, nCurrentLine, nCurrentColumn);

        if (nCurrentLine > 0)
        {
            // 移动到上一行相同列位置
            m_nCursorPos = LineColumnToCharIndex(nCurrentLine - 1, nCurrentColumn);
        }
    }
    break;

    case VK_DOWN:
    {
        int nCurrentLine, nCurrentColumn;
        CharIndexToLineColumn(m_nCursorPos, nCurrentLine, nCurrentColumn);

        // 计算总行数
        int nTotalLines = 0;
        int nPos = 0;
        while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1)
        {
            nTotalLines++;
            nPos++;
        }
        nTotalLines++; // 最后一行

        if (nCurrentLine < nTotalLines - 1)
        {
            // 移动到下一行相同列位置
            m_nCursorPos = LineColumnToCharIndex(nCurrentLine + 1, nCurrentColumn);
        }
    }
    break;

    case VK_HOME:
    {
        int nCurrentLine, nCurrentColumn;
        CharIndexToLineColumn(m_nCursorPos, nCurrentLine, nCurrentColumn);

        // 移动到当前行开头
        m_nCursorPos = LineColumnToCharIndex(nCurrentLine, 0);
    }
    break;

    case VK_END:
    {
        int nCurrentLine, nCurrentColumn;
        CharIndexToLineColumn(m_nCursorPos, nCurrentLine, nCurrentColumn);

        // 计算当前行长度
        int nLineStart = LineColumnToCharIndex(nCurrentLine, 0);
        int nLineEnd = m_strContent.Find(_T('\n'), nLineStart);
        if (nLineEnd == -1)
            nLineEnd = m_strContent.GetLength();

        // 移动到当前行末尾
        m_nCursorPos = nLineEnd;
    }
    break;
    }

    // 处理 Shift 选择
    if (bShiftPressed)
    {
        if (!m_bHasSelection)
        {
            // 开始新的选择
            m_nSelectionStart = nOldCursorPos;
            m_nSelectionEnd = m_nCursorPos;
            m_bHasSelection = TRUE;
        }
        else
        {
            // 扩展现有选择
            m_nSelectionEnd = m_nCursorPos;
        }
    }
    else
    {
        // 如果没有按 Shift，清除选择
        ClearSelection();
    }

    Invalidate();

    CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMFCApplication1View::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus();  // 获取焦点

    // 获取滚动位置
    CPoint scrollPos = GetScrollPosition();

    // 获取文本度量
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_fontText);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    int nCharWidth = tm.tmAveCharWidth;  // 保留这个变量，虽然可能用不上

    // 计算点击位置对应的行
    int nClickLine = (point.y + scrollPos.y) / nCharHeight;

    // 获取该行的字符串
    CString strLine = GetLineAt(nClickLine);

    // 计算点击位置在文本区域内的X坐标
    int nClickX = point.x + scrollPos.x - m_nLineNumberWidth - 5;
    if (nClickX < 0) nClickX = 0;

    // 逐字符计算宽度，找到点击位置对应的列
    int nClickColumn = 0;

    // 处理空行的情况
    if (!strLine.IsEmpty())
    {
        for (int i = 0; i <= strLine.GetLength(); i++)
        {
            CString strSub = strLine.Left(i);
            CSize size;
            if (GetTextExtentPoint32(dc.GetSafeHdc(), strSub, strSub.GetLength(), &size))
            {
                // 如果点击位置在当前字符宽度内，则选择这个字符
                if (nClickX <= size.cx)
                {
                    nClickColumn = i;
                    break;
                }
            }
        }

        // 如果点击位置超过最后一个字符，则放在行末
        if (nClickColumn == 0 && nClickX > 0 && strLine.GetLength() > 0)
        {
            CSize size;
            if (GetTextExtentPoint32(dc.GetSafeHdc(), strLine, strLine.GetLength(), &size))
            {
                if (nClickX > size.cx)
                {
                    nClickColumn = strLine.GetLength();
                }
            }
        }
    }
    else
    {
        // 如果是空行，列位置为0
        nClickColumn = 0;
    }

    // 将行和列转换为字符索引 - 这里是关键部分
    int nNewCursorPos = LineColumnToCharIndex(nClickLine, nClickColumn);

    // 处理 Shift+点击选择
    BOOL bShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    if (bShiftPressed && m_bHasSelection)
    {
        // 扩展选择
        m_nSelectionEnd = nNewCursorPos;
        m_nCursorPos = nNewCursorPos;
    }
    else if (bShiftPressed)
    {
        // 开始新的选择
        m_nSelectionStart = m_nCursorPos;
        m_nSelectionEnd = nNewCursorPos;
        m_nCursorPos = nNewCursorPos;
        m_bHasSelection = TRUE;
    }
    else
    {
        // 移动到点击位置，清除选择
        m_nCursorPos = nNewCursorPos;
        ClearSelection();
    }

    // 重置光标闪烁
    m_bCursorVisible = TRUE;
    m_nCursorBlinkCounter = 0;

    dc.SelectObject(pOldFont);
    Invalidate();

    // 设置鼠标捕获，以便处理鼠标拖动选择
    SetCapture();

    CScrollView::OnLButtonDown(nFlags, point);
}

// 添加鼠标移动事件处理（支持拖动选择）
void CMFCApplication1View::OnMouseMove(UINT nFlags, CPoint point)
{
    if (nFlags & MK_LBUTTON)  // 如果左键被按下
    {
        // 获取滚动位置
        CPoint scrollPos = GetScrollPosition();

        // 获取文本度量
        CClientDC dc(this);
        CFont* pOldFont = dc.SelectObject(&m_fontText);
        TEXTMETRIC tm;
        dc.GetTextMetrics(&tm);
        int nCharHeight = tm.tmHeight + tm.tmExternalLeading;

        // 计算鼠标位置对应的行
        int nClickLine = (point.y + scrollPos.y) / nCharHeight;

        // 获取该行的字符串
        CString strLine = GetLineAt(nClickLine);

        // 计算点击位置在文本区域内的X坐标
        int nClickX = point.x + scrollPos.x - m_nLineNumberWidth - 5;
        if (nClickX < 0) nClickX = 0;

        // 逐字符计算宽度，找到点击位置对应的列
        int nClickColumn = 0;

        for (int i = 0; i <= strLine.GetLength(); i++)
        {
            CString strSub = strLine.Left(i);
            CSize size;
            if (GetTextExtentPoint32(dc.GetSafeHdc(), strSub, strSub.GetLength(), &size))
            {
                // 如果点击位置在当前字符宽度的一半内，则选择这个字符
                if (nClickX <= size.cx)
                {
                    nClickColumn = i;
                    break;
                }
            }
        }

        // 如果点击位置超过最后一个字符，则放在行末
        if (nClickColumn == 0 && nClickX > 0 && strLine.GetLength() > 0)
        {
            CSize size;
            if (GetTextExtentPoint32(dc.GetSafeHdc(), strLine, strLine.GetLength(), &size))
            {
                if (nClickX > size.cx)
                {
                    nClickColumn = strLine.GetLength();
                }
            }
        }

        // 将行和列转换为字符索引
        int nNewCursorPos = LineColumnToCharIndex(nClickLine, nClickColumn);

        // 更新选择
        if (!m_bHasSelection)
        {
            m_nSelectionStart = m_nCursorPos;
            m_bHasSelection = TRUE;
        }
        m_nSelectionEnd = nNewCursorPos;
        m_nCursorPos = nNewCursorPos;

        dc.SelectObject(pOldFont);
        Invalidate();
    }

    CScrollView::OnMouseMove(nFlags, point);
}

// 添加鼠标左键释放事件
void CMFCApplication1View::OnLButtonUp(UINT nFlags, CPoint point)
{
    // 释放鼠标捕获
    ReleaseCapture();

    CScrollView::OnLButtonUp(nFlags, point);
}

void CMFCApplication1View::OnSetFocus(CWnd* pOldWnd)
{
    CScrollView::OnSetFocus(pOldWnd);

    // 获得焦点时显示光标
    m_bCursorVisible = TRUE;
    m_nCursorBlinkCounter = 0;
    Invalidate();
}

void CMFCApplication1View::OnKillFocus(CWnd* pNewWnd)
{
    CScrollView::OnKillFocus(pNewWnd);
    Invalidate();
}

void CMFCApplication1View::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
    Invalidate();
}

void CMFCApplication1View::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
    Invalidate();
}

void CMFCApplication1View::SetText(LPCTSTR lpszText)
{
    m_strContent = lpszText;
    m_nCursorPos = 0;
    UpdateViewMetrics();
    Invalidate();
}

void CMFCApplication1View::InsertChar(TCHAR ch)
{
    m_strContent.Insert(m_nCursorPos, ch);
    m_nCursorPos++;
    UpdateViewMetrics();
    Invalidate();
}

void CMFCApplication1View::DeleteChar()
{
    if (m_nCursorPos > 0)
    {
        m_strContent.Delete(m_nCursorPos - 1, 1);
        m_nCursorPos--;
        UpdateViewMetrics();
        Invalidate();
    }
}

void CMFCApplication1View::InsertString(LPCTSTR lpszText)
{
    m_strContent.Insert(m_nCursorPos, lpszText);
    m_nCursorPos += (int)_tcslen(lpszText);
    UpdateViewMetrics();
    Invalidate();
}

void CMFCApplication1View::MoveCursor(int nCharOffset)
{
    int nNewPos = m_nCursorPos + nCharOffset;
    if (nNewPos >= 0 && nNewPos <= m_strContent.GetLength())
    {
        m_nCursorPos = nNewPos;
        Invalidate();
    }
}

void CMFCApplication1View::MoveCursorTo(int nLine, int nColumn)
{
    m_nCursorPos = LineColumnToCharIndex(nLine, nColumn);
    Invalidate();
}

void CMFCApplication1View::GetCursorPosition(int& nLine, int& nColumn) const
{
    CharIndexToLineColumn(m_nCursorPos, nLine, nColumn);
}

int CMFCApplication1View::CharIndexToLineColumn(int nCharIndex, int& nLine, int& nColumn) const
{
    nLine = 0;
    nColumn = 0;

    int nCurrentPos = 0;
    int nPrevLineStart = 0;

    // 遍历查找换行符
    while (nCurrentPos < nCharIndex && nCurrentPos < m_strContent.GetLength())
    {
        if (m_strContent[nCurrentPos] == _T('\n'))
        {
            nLine++;
            nPrevLineStart = nCurrentPos + 1;
        }
        nCurrentPos++;
    }

    // 计算列
    nColumn = nCharIndex - nPrevLineStart;

    return nCharIndex;
}

int CMFCApplication1View::LineColumnToCharIndex(int nLine, int nColumn) const
{
    int nCurrentLine = 0;
    int nCurrentPos = 0;

    // 移动到指定行
    while (nCurrentLine < nLine && nCurrentPos < m_strContent.GetLength())
    {
        if (m_strContent[nCurrentPos] == _T('\n'))
            nCurrentLine++;
        nCurrentPos++;
    }

    // 如果在查找过程中到达文本末尾
    if (nCurrentLine < nLine)
        return m_strContent.GetLength();

    // 移动到指定列
    int nLineLength = 0;
    while (nCurrentPos < m_strContent.GetLength() && m_strContent[nCurrentPos] != _T('\n') && nLineLength < nColumn)
    {
        nCurrentPos++;
        nLineLength++;
    }

    // 如果列位置超过行长度，调整到行末
    if (nLineLength < nColumn)
    {
        // 已经到达行末，不需要调整
    }

    return nCurrentPos;
}

void CMFCApplication1View::OnEditCut()
{
    CString strSelectedText = GetSelectedText();
    if (!strSelectedText.IsEmpty())
    {
        // 先复制
        CopyToClipboard(strSelectedText);

        // 然后删除选中的文本
        DeleteSelectedText();

        // 更新显示
        UpdateViewMetrics();
        Invalidate();
    }
    else
    {
        // 如果没有选择，剪切整个文档
        CopyToClipboard(m_strContent);

        // 保存到撤销栈
        m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
        m_redoStack.clear();

        // 清空文档
        m_strContent.Empty();
        m_nCursorPos = 0;

        // 更新显示
        UpdateViewMetrics();
        Invalidate();
    }
}

void CMFCApplication1View::OnEditCopy()
{
    CString strSelectedText = GetSelectedText();
    if (!strSelectedText.IsEmpty())
    {
        CopyToClipboard(strSelectedText);
    }
    else
    {
        // 如果没有选择，复制整个文档
        CopyToClipboard(m_strContent);
    }
}

void CMFCApplication1View::OnEditPaste()
{
    CString strClipboardText = GetFromClipboard();
    if (!strClipboardText.IsEmpty())
    {
        // 如果有选中的文本，先删除它
        if (m_bHasSelection)
        {
            DeleteSelectedText();
        }

        // 保存到撤销栈
        m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
        m_redoStack.clear();

        // 插入剪贴板文本
        m_strContent.Insert(m_nCursorPos, strClipboardText);
        m_nCursorPos += strClipboardText.GetLength();

        // 更新显示
        UpdateViewMetrics();
        Invalidate();
    }
}

void CMFCApplication1View::OnEditSelectAll()
{
    if (!m_strContent.IsEmpty())
    {
        m_nSelectionStart = 0;
        m_nSelectionEnd = m_strContent.GetLength();
        m_bHasSelection = TRUE;
        m_nCursorPos = m_strContent.GetLength();
        Invalidate();
    }
}

void CMFCApplication1View::OnEditUndo()
{
    if (!m_undoStack.empty())
    {
        // 将当前状态保存到重做栈
        m_redoStack.push_back(EditAction(m_strContent, m_nCursorPos));

        // 恢复撤销栈顶的状态
        EditAction& action = m_undoStack.back();
        m_strContent = action.strContent;
        m_nCursorPos = action.nCursorPos;

        m_undoStack.pop_back();

        UpdateViewMetrics();
        Invalidate();
    }
}

void CMFCApplication1View::OnEditRedo()
{
    if (!m_redoStack.empty())
    {
        // 将当前状态保存到撤销栈
        m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));

        // 恢复重做栈顶的状态
        EditAction& action = m_redoStack.back();
        m_strContent = action.strContent;
        m_nCursorPos = action.nCursorPos;

        m_redoStack.pop_back();

        UpdateViewMetrics();
        Invalidate();
    }
}

void CMFCApplication1View::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_undoStack.empty());
}

void CMFCApplication1View::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_redoStack.empty());
}

void CMFCApplication1View::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_strContent.IsEmpty());  // 只要文档不为空就可以剪切
}

void CMFCApplication1View::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_strContent.IsEmpty());  // 只要文档不为空就可以复制
}

void CMFCApplication1View::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CMFCApplication1View::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!m_strContent.IsEmpty());
}

BOOL CMFCApplication1View::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CMFCApplication1View::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

void CMFCApplication1View::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

#ifdef _DEBUG
void CMFCApplication1View::AssertValid() const
{
    CScrollView::AssertValid();
}

void CMFCApplication1View::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}

CMFCApplication1Doc* CMFCApplication1View::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication1Doc)));
    return (CMFCApplication1Doc*)m_pDocument;
}
#endif


// 最简单的搜索函数
void CMFCApplication1View::SearchText(const CString& strSearch)
{
    if (strSearch.IsEmpty())
    {
        AfxMessageBox(_T("请输入要搜索的内容"));
        return;
    }

    if (m_strContent.IsEmpty())
    {
        AfxMessageBox(_T("文档为空，无可搜索内容"));
        return;
    }

    // 从当前位置开始搜索
    int nFindPos = m_strContent.Find(strSearch, m_nCurrentSearchPos);

    if (nFindPos == -1)
    {
        // 从头开始搜索
        nFindPos = m_strContent.Find(strSearch);
        if (nFindPos == -1)
        {
            AfxMessageBox(_T("未找到匹配的文本"));
            m_nCurrentSearchPos = 0;
            return;
        }
    }

    // 找到匹配
    m_nCurrentSearchPos = nFindPos;
    m_nCursorPos = nFindPos;

    // 更新显示
    UpdateViewMetrics();
    Invalidate();

    // 简单滚动到光标位置
    CPoint pt = GetScrollPosition();
    SetScrollPos(SB_HORZ, 0);
    SetScrollPos(SB_VERT, 0);
}

// 最简单的替换函数
int CMFCApplication1View::ReplaceText(const CString& strSearch, const CString& strReplace)
{
    if (strSearch.IsEmpty())
    {
        AfxMessageBox(_T("请输入要搜索的内容"));
        return 0;
    }

    if (m_strContent.IsEmpty())
    {
        AfxMessageBox(_T("文档为空"));
        return 0;
    }

    // 找到匹配位置
    int nFindPos = m_strContent.Find(strSearch, m_nCurrentSearchPos);
    if (nFindPos == -1)
    {
        nFindPos = m_strContent.Find(strSearch);
        if (nFindPos == -1)
        {
            AfxMessageBox(_T("未找到匹配的文本"));
            return 0;
        }
    }

    // 保存到撤销栈
    m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
    m_redoStack.clear();

    // 执行替换
    m_strContent.Delete(nFindPos, strSearch.GetLength());
    m_strContent.Insert(nFindPos, strReplace);

    // 更新光标位置
    m_nCursorPos = nFindPos;
    m_nCurrentSearchPos = nFindPos;

    // 更新显示
    UpdateViewMetrics();
    Invalidate();

    AfxMessageBox(_T("替换完成"));
    return 1;
}

// 查找菜单命令
void CMFCApplication1View::OnEditFind()
{
    CSearchReplaceDlg dlg;
    dlg.m_bIsSearchOnly = TRUE;
    dlg.m_strSearchText = _T("");  // 清空默认值
    dlg.m_bUseRegex = FALSE;       // 默认不使用正则表达式
    dlg.m_bMatchCase = FALSE;
    dlg.m_bWholeWord = FALSE;

    if (dlg.DoModal() == IDOK)
    {
        if (!dlg.m_strSearchText.IsEmpty())
        {
            if (dlg.m_bUseRegex)
            {
                // 使用正则表达式搜索
                DoRegexSearch(dlg.m_strSearchText, dlg.m_bMatchCase, dlg.m_bWholeWord);
            }
            else
            {
                // 使用简单文本搜索
                SearchText(dlg.m_strSearchText);
            }
        }
    }
}


// 替换菜单命令
void CMFCApplication1View::OnEditReplace()
{
    CSearchReplaceDlg dlg;
    dlg.m_bIsSearchOnly = FALSE;
    dlg.m_strSearchText = _T("");
    dlg.m_strReplaceText = _T("");
    dlg.m_bUseRegex = FALSE;       // 默认不使用正则表达式
    dlg.m_bMatchCase = FALSE;
    dlg.m_bWholeWord = FALSE;
    dlg.m_bReplaceAll = FALSE;

    if (dlg.DoModal() == IDOK)
    {
        if (!dlg.m_strSearchText.IsEmpty())
        {
            if (dlg.m_bUseRegex)
            {
                // 使用正则表达式替换
                DoRegexReplace(dlg.m_strSearchText, dlg.m_strReplaceText,
                    dlg.m_bMatchCase, dlg.m_bWholeWord, dlg.m_bReplaceAll);
            }
            else
            {
                // 使用简单文本替换
                ReplaceText(dlg.m_strSearchText, dlg.m_strReplaceText);
            }
        }
    }
}

// CString 到 std::string 的转换函数
std::string CStringToStdString(const CString& cstr)
{
    CT2CA pszConvertedAnsiString(cstr);
    return std::string(pszConvertedAnsiString);
}

// std::string 到 CString 的转换函数
CString StdStringToCString(const std::string& str)
{
    return CString(str.c_str());
}

// 实现视图类中的转换函数（作为成员函数）
std::string CMFCApplication1View::ConvertCStringToStdString(const CString& cstr)
{
    return CStringToStdString(cstr);
}

CString CMFCApplication1View::ConvertStdStringToCString(const std::string& str)
{
    return StdStringToCString(str);
}

// 正则表达式搜索函数
void CMFCApplication1View::DoRegexSearch(const CString& strPattern, BOOL bMatchCase, BOOL bWholeWord)
{
    if (strPattern.IsEmpty())
    {
        AfxMessageBox(_T("请输入搜索模式"));
        return;
    }

    if (m_strContent.IsEmpty())
    {
        AfxMessageBox(_T("文档为空"));
        return;
    }

    try
    {
        // 清除之前的搜索结果
        m_searchResults.clear();
        m_currentMatchIndex = 0;

        // 转换为std::string
        std::string text = ConvertCStringToStdString(m_strContent);
        std::string pattern = ConvertCStringToStdString(strPattern);

        // 设置正则表达式标志
        std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;

        if (!bMatchCase)
        {
            flags |= std::regex_constants::icase;
        }

        if (bWholeWord)
        {
            // 如果要求全词匹配，添加单词边界
            // 注意：正则表达式中已经包含转义字符
            pattern = "\\b" + pattern + "\\b";
        }

        // 编译正则表达式
        std::regex reg;
        try
        {
            reg.assign(pattern, flags);
        }
        catch (const std::regex_error& e)
        {
            CString strError;
            strError.Format(_T("正则表达式语法错误: %s\n模式: %s"),
                CString(e.what()), strPattern);
            AfxMessageBox(strError);
            return;
        }

        // 搜索所有匹配
        auto words_begin = std::sregex_iterator(text.begin(), text.end(), reg);
        auto words_end = std::sregex_iterator();

        // 修改这里，避免使用std::distance
        int matchCount = 0;
        for (auto i = words_begin; i != words_end; ++i)
        {
            matchCount++;
        }

        if (matchCount == 0)
        {
            AfxMessageBox(_T("未找到匹配项"));
            return;
        }

        // 存储所有匹配位置
        for (auto i = words_begin; i != words_end; ++i)
        {
            std::smatch match = *i;
            m_searchResults.push_back((int)match.position());
        }

        // 定位到第一个匹配
        if (!m_searchResults.empty())
        {
            m_nCursorPos = m_searchResults[0];
            m_currentMatchIndex = 0;

            // 更新显示
            UpdateViewMetrics();
            Invalidate();

            // 滚动到光标位置
            EnsureCursorVisible();
        }

        // 显示找到的结果数量
        CString strMessage;
        strMessage.Format(_T("找到 %d 个匹配项"), matchCount);
        AfxMessageBox(strMessage);
    }
    catch (const std::exception& e)
    {
        CString strError;
        strError.Format(_T("搜索过程中发生错误: %s"), CString(e.what()));
        AfxMessageBox(strError);
    }
    catch (...)
    {
        AfxMessageBox(_T("搜索过程中发生未知错误"));
    }
}

// 添加辅助函数确保光标可见
void CMFCApplication1View::EnsureCursorVisible()
{
    // 获取滚动位置
    CPoint scrollPos = GetScrollPosition();
    CRect clientRect;
    GetClientRect(&clientRect);

    // 获取文本度量
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_fontText);
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;

    // 计算光标位置
    int nCursorLine, nCursorColumn;
    CharIndexToLineColumn(m_nCursorPos, nCursorLine, nCursorColumn);

    // 计算光标Y坐标
    int nCursorY = nCursorLine * nCharHeight;

    // 如果光标不在可见区域内，滚动到光标位置
    if (nCursorY < scrollPos.y || nCursorY > scrollPos.y + clientRect.Height())
    {
        // 设置新的滚动位置，使光标位于视图中央
        int nNewScrollY = nCursorY - clientRect.Height() / 2;
        if (nNewScrollY < 0) nNewScrollY = 0;

        SetScrollPos(SB_VERT, nNewScrollY);
        Invalidate();
    }

    dc.SelectObject(pOldFont);
}

// 正则表达式替换函数
int CMFCApplication1View::DoRegexReplace(const CString& strPattern, const CString& strReplace,
    BOOL bMatchCase, BOOL bWholeWord, BOOL bReplaceAll)
{
    if (strPattern.IsEmpty())
    {
        AfxMessageBox(_T("请输入搜索模式"));
        return 0;
    }

    if (m_strContent.IsEmpty())
    {
        AfxMessageBox(_T("文档为空"));
        return 0;
    }

    try
    {
        // 保存到撤销栈
        m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
        m_redoStack.clear();

        // 转换为std::string
        std::string text = ConvertCStringToStdString(m_strContent);
        std::string pattern = ConvertCStringToStdString(strPattern);
        std::string replacement = ConvertCStringToStdString(strReplace);

        // 设置正则表达式标志
        std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript;

        if (!bMatchCase)
        {
            flags |= std::regex_constants::icase;
        }

        if (bWholeWord)
        {
            // 如果要求全词匹配，添加单词边界
            pattern = "\\b" + pattern + "\\b";
        }

        // 编译正则表达式
        std::regex reg(pattern, flags);

        int replaceCount = 0;

        if (bReplaceAll)
        {
            // 替换所有匹配
            std::string result = std::regex_replace(text, reg, replacement);
            m_strContent = ConvertStdStringToCString(result);
            replaceCount = 1; // regex_replace 替换所有，但我们只统计一次操作
        }
        else
        {
            // 只替换当前匹配或第一个匹配
            std::sregex_iterator words_begin(text.begin(), text.end(), reg);
            std::sregex_iterator words_end;

            if (words_begin != words_end)
            {
                std::smatch match = *words_begin;

                // 构建替换后的字符串
                std::string result = match.prefix().str() + replacement + match.suffix().str();
                m_strContent = ConvertStdStringToCString(result);
                replaceCount = 1;
            }
        }

        // 更新显示
        UpdateViewMetrics();
        Invalidate();

        if (replaceCount > 0)
        {
            CString strMessage;
            if (bReplaceAll)
            {
                strMessage.Format(_T("完成全部替换"));
            }
            else
            {
                strMessage.Format(_T("替换完成"));
            }
            AfxMessageBox(strMessage);
        }
        else
        {
            AfxMessageBox(_T("未找到匹配的文本"));
        }

        return replaceCount;
    }
    catch (const std::regex_error& e)
    {
        CString strError;
        strError.Format(_T("正则表达式错误: %s"), CString(e.what()));
        AfxMessageBox(strError);
        return 0;
    }
    catch (...)
    {
        AfxMessageBox(_T("替换过程中发生未知错误"));
        return 0;
    }
}

// 清空选择
void CMFCApplication1View::ClearSelection()
{
    m_nSelectionStart = -1;
    m_nSelectionEnd = -1;
    m_bHasSelection = FALSE;
    Invalidate();
}

// 获取选中的文本
CString CMFCApplication1View::GetSelectedText() const
{
    if (!m_bHasSelection || m_nSelectionStart == -1 || m_nSelectionEnd == -1)
        return _T("");

    int nStart = min(m_nSelectionStart, m_nSelectionEnd);
    int nEnd = max(m_nSelectionStart, m_nSelectionEnd);

    return m_strContent.Mid(nStart, nEnd - nStart);
}

// 删除选中的文本
void CMFCApplication1View::DeleteSelectedText()
{
    if (!m_bHasSelection || m_nSelectionStart == -1 || m_nSelectionEnd == -1)
        return;

    // 保存到撤销栈
    m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
    m_redoStack.clear();

    int nStart = min(m_nSelectionStart, m_nSelectionEnd);
    int nEnd = max(m_nSelectionStart, m_nSelectionEnd);

    // 删除选中的文本
    m_strContent.Delete(nStart, nEnd - nStart);

    // 移动光标到删除位置
    m_nCursorPos = nStart;

    // 清除选择
    ClearSelection();

    // 更新显示
    UpdateViewMetrics();
    Invalidate();
}

// 复制到剪贴板
void CMFCApplication1View::CopyToClipboard(const CString& strText)
{
    if (strText.IsEmpty())
        return;

    if (OpenClipboard())
    {
        EmptyClipboard();

        // 分配全局内存
        HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (strText.GetLength() + 1) * sizeof(TCHAR));
        if (hClipboardData)
        {
            // 锁定内存
            TCHAR* pchData = (TCHAR*)GlobalLock(hClipboardData);
            if (pchData)
            {
                // 复制文本
                _tcscpy_s(pchData, strText.GetLength() + 1, strText);
                GlobalUnlock(hClipboardData);

                // 设置剪贴板数据
#ifdef _UNICODE
                SetClipboardData(CF_UNICODETEXT, hClipboardData);
#else
                SetClipboardData(CF_TEXT, hClipboardData);
#endif
            }
        }
        CloseClipboard();
    }
}

// 从剪贴板获取文本
CString CMFCApplication1View::GetFromClipboard()
{
    CString strResult;

    if (OpenClipboard())
    {
#ifdef _UNICODE
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
#else
        HANDLE hData = GetClipboardData(CF_TEXT);
#endif
        if (hData)
        {
            TCHAR* pchData = (TCHAR*)GlobalLock(hData);
            if (pchData)
            {
                strResult = pchData;
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }

    return strResult;
}

// 绘制选择背景
void CMFCApplication1View::DrawSelection(CDC* pDC, const CRect& rect)
{
    if (!m_bHasSelection || m_nSelectionStart == -1 || m_nSelectionEnd == -1)
        return;

    int nStart = min(m_nSelectionStart, m_nSelectionEnd);
    int nEnd = max(m_nSelectionStart, m_nSelectionEnd);

    // 获取滚动位置
    CPoint scrollPos = GetScrollPosition();

    // 获取文本度量
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    int nCharWidth = tm.tmAveCharWidth;

    // 计算选择区域的起始行和列
    int nStartLine, nStartColumn, nEndLine, nEndColumn;
    CharIndexToLineColumn(nStart, nStartLine, nStartColumn);
    CharIndexToLineColumn(nEnd, nEndLine, nEndColumn);

    // 创建半透明的蓝色画刷用于选择区域
    CBrush brushSelection;
    brushSelection.CreateSolidBrush(RGB(173, 216, 230));  // 浅蓝色

    // 逐行绘制选择背景
    for (int i = nStartLine; i <= nEndLine; i++)
    {
        // 获取当前行的字符串
        CString strLine = GetLineAt(i);

        // 计算当前行的起始位置
        int nLineStart = LineColumnToCharIndex(i, 0);

        // 计算当前行选择的起始和结束列
        int nLineSelectionStart = 0;
        int nLineSelectionEnd = strLine.GetLength();

        if (i == nStartLine)
            nLineSelectionStart = nStartColumn;
        if (i == nEndLine)
            nLineSelectionEnd = nEndColumn;

        // 跳过无效的选择范围
        if (nLineSelectionStart >= nLineSelectionEnd)
            continue;

        // 计算选择区域在屏幕上的位置
        int nSelectXStart = m_nLineNumberWidth + 5 +
            CalculateTextWidth(pDC, strLine, nLineSelectionStart) - scrollPos.x;
        int nSelectXEnd = m_nLineNumberWidth + 5 +
            CalculateTextWidth(pDC, strLine, nLineSelectionEnd) - scrollPos.x;
        int nSelectY = i * nCharHeight - scrollPos.y;

        // 创建选择矩形
        CRect rectSelect;
        rectSelect.left = nSelectXStart;
        rectSelect.right = nSelectXEnd;
        rectSelect.top = nSelectY;
        rectSelect.bottom = nSelectY + nCharHeight;

        // 确保矩形在客户区内
        rectSelect.IntersectRect(rectSelect, rect);

        // 绘制选择背景
        pDC->FillRect(rectSelect, &brushSelection);
    }
}