#pragma once
#include <regex>
#include <string>
#include <vector>
struct EditAction
{
    CString strContent;
    int nCursorPos;

    EditAction(const CString& content = _T(""), int cursorPos = 0)
        : strContent(content), nCursorPos(cursorPos) {
    }
};
class CMFCApplication1View : public CScrollView
{
protected:
    CMFCApplication1View();
    DECLARE_DYNCREATE(CMFCApplication1View)

public:
    CMFCApplication1Doc* GetDocument() const;

public:
    // 编辑器状态
    CString m_strContent;           // 完整的文本内容
    int m_nCursorPos;               // 光标在文本中的位置（字符索引）
    BOOL m_bCursorVisible;          // 光标是否可见
    int m_nCursorBlinkCounter;      // 光标闪烁计数器

    // 行号显示
    int m_nLineNumberWidth;
    CFont m_fontLineNumber;
    COLORREF m_crLineNumberText;
    COLORREF m_crLineNumberBg;

    // 字体和颜色
    CFont m_fontText;
    COLORREF m_crTextColor;
    COLORREF m_crBackgroundColor;

    // 编辑历史
    std::vector<EditAction> m_undoStack;  // 原来是 std::vector<CString>
    std::vector<EditAction> m_redoStack;  // 原来是 std::vector<CString>
    int m_nPrevCursorPos;  // 用于保存操作前的光标位置

    // 搜索状态
    int m_nCurrentSearchPos;
    std::vector<int> m_searchResults;  // 存储所有匹配位置
    int m_currentMatchIndex;  // 当前匹配索引
public:
    CString GetText() const { return m_strContent; }
    void SetText(LPCTSTR lpszText);

    // 文本操作
    void InsertChar(TCHAR ch);
    void DeleteChar();
    void InsertString(LPCTSTR lpszText);

    // 光标操作
    void MoveCursor(int nCharOffset);
    void MoveCursorTo(int nLine, int nColumn);
    void GetCursorPosition(int& nLine, int& nColumn) const;
    int GetCursorCharIndex() const { return m_nCursorPos; }

    // 行号相关
    void CalculateLineNumberWidth();
    void DrawLineNumbers(CDC* pDC, CRect& rect);
    void UpdateViewMetrics();

    // 文本布局计算
    int CharIndexToLineColumn(int nCharIndex, int& nLine, int& nColumn) const;
    int LineColumnToCharIndex(int nLine, int nColumn) const;

    // 文本选择相关
    int m_nSelectionStart;
    int m_nSelectionEnd;
    BOOL m_bHasSelection;

    // 辅助函数
    void ClearSelection();
    CString GetSelectedText() const;
    void DeleteSelectedText();
    BOOL IsTextSelected() const { return m_bHasSelection; }

    // 剪贴板操作
    void CopyToClipboard(const CString& strText);
    CString GetFromClipboard();

    // 辅助函数
    int CalculateTextWidth(CDC* pDC, const CString& strLine, int nChars);
    CString GetLineAt(int nLine);

    // 正则表达式搜索/替换相关
    void DoRegexSearch(const CString& strPattern, BOOL bMatchCase = FALSE, BOOL bWholeWord = FALSE);
    int DoRegexReplace(const CString& strPattern, const CString& strReplace,
        BOOL bMatchCase = FALSE, BOOL bWholeWord = FALSE, BOOL bReplaceAll = FALSE);

    void CMFCApplication1View::EnsureCursorVisible();

    std::string ConvertCStringToStdString(const CString& cstr);
    CString ConvertStdStringToCString(const std::string& str);

    // 绘制选择背景
    void DrawSelection(CDC* pDC, const CRect& rect);
public:
    virtual void OnDraw(CDC* pDC);
    virtual void OnInitialUpdate();
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
    virtual ~CMFCApplication1View();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    // 添加鼠标事件处理函数声明
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnEditSelectAll();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnViewDarkTheme();
    afx_msg void OnViewLightTheme();
    afx_msg void OnUpdateViewDarkTheme(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewLightTheme(CCmdUI* pCmdUI);
public:

    // 添加这4个函数声明
    void OnEditFind();
    void OnEditReplace();
    void SearchText(const CString& strSearch);
    int ReplaceText(const CString& strSearch, const CString& strReplace);
public:
    // 主题相关
    BOOL m_bDarkTheme;  // TRUE:暗色主题, FALSE:亮色主题

    // 亮色主题颜色
    COLORREF m_crLightText;
    COLORREF m_crLightBg;
    COLORREF m_crLightLineNumberText;
    COLORREF m_crLightLineNumberBg;

    // 暗色主题颜色
    COLORREF m_crDarkText;
    COLORREF m_crDarkBg;
    COLORREF m_crDarkLineNumberText;
    COLORREF m_crDarkLineNumberBg;

    // 方法
    void ApplyTheme(BOOL bDarkTheme);
};

#ifndef _DEBUG
inline CMFCApplication1Doc* CMFCApplication1View::GetDocument() const
{
    return reinterpret_cast<CMFCApplication1Doc*>(m_pDocument);
}
#endif