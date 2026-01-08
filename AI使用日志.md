# AI使用日志：MFC多文档记事本编辑器开发全流程记录


### 对话1

**我提问**：我需要创建一个三层架构的MFC多文档记事本编辑器。要求使用文档/视图架构，不使用CEditView，而是基于CScrollView自定义实现。请帮我设计项目整体架构。

**AI回答**：
对于三层架构的MFC应用程序，建议按以下结构设计：

```
1. 表示层 (Presentation Layer)
   - CMainFrame: 主框架窗口
   - CChildFrame: 子框架窗口
   - 对话框类

2. 业务逻辑层 (Business Logic Layer)
   - CMFCApplication1View: 视图类，处理文本编辑逻辑
   - 工具类：如搜索、加密等功能

3. 数据层 (Data Layer)
   - CMFCApplication1Doc: 文档类，负责数据持久化
   - 文件操作类
```

具体实现代码结构如下：

```cpp
// 应用程序类
class CMFCApplication1App : public CWinAppEx {
public:
    virtual BOOL InitInstance();
    // ...
};

// 文档类
class CMFCApplication1Doc : public CDocument {
protected:
    DECLARE_DYNCREATE(CMFCApplication1Doc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    // ...
};

// 视图类
class CMFCApplication1View : public CScrollView {
protected:
    DECLARE_DYNCREATE(CMFCApplication1View)
public:
    virtual void OnDraw(CDC* pDC);
    // ...
};
```

需要在应用程序初始化时创建多文档模板：

```cpp
BOOL CMFCApplication1App::InitInstance() {
    // ... 其他初始化
    
    CMultiDocTemplate* pDocTemplate = new CMultiDocTemplate(
        IDR_MFCApplication1TYPE,
        RUNTIME_CLASS(CMFCApplication1Doc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CMFCApplication1View)
    );
    
    if (!pDocTemplate) return FALSE;
    AddDocTemplate(pDocTemplate);
    
    // ...
}
```

**我的问题**：我不太理解如何不使用CEditView而基于CScrollView实现文本编辑功能。

**AI回答**：
这需要手动处理键盘输入和文本绘制。以下是关键步骤：

1. **重写OnChar处理字符输入**：
```cpp
void CMFCApplication1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
    switch (nChar) {
    case 8:  // Backspace
        if (m_nCursorPos > 0) {
            m_strContent.Delete(m_nCursorPos - 1, 1);
            m_nCursorPos--;
        }
        break;
    case 13: // Enter
        m_strContent.Insert(m_nCursorPos, _T('\n'));
        m_nCursorPos++;
        break;
    default:
        if (nChar >= 32) {  // 可打印字符
            m_strContent.Insert(m_nCursorPos, (TCHAR)nChar);
            m_nCursorPos++;
        }
        break;
    }
    Invalidate();  // 重绘
    CScrollView::OnChar(nChar, nRepCnt, nFlags);
}
```

2. **在OnDraw中绘制文本**：
```cpp
void CMFCApplication1View::OnDraw(CDC* pDC) {
    // 设置字体
    CFont* pOldFont = pDC->SelectObject(&m_fontText);
    
    // 计算可见行并绘制
    int nFirstVisibleLine = scrollPos.y / nCharHeight;
    int nLastVisibleLine = min((scrollPos.y + rect.Height()) / nCharHeight, lines.GetSize());
    
    for (int i = nFirstVisibleLine; i < nLastVisibleLine; i++) {
        pDC->TextOut(pt.x, pt.y, lines[i]);
    }
    
    // 绘制光标
    if (m_bCursorVisible) {
        // 计算光标位置并绘制
        // ...
    }
    
    pDC->SelectObject(pOldFont);
}
```

3. **需要管理的成员变量**：
```cpp
class CMFCApplication1View : public CScrollView {
protected:
    CString m_strContent;      // 文本内容
    int m_nCursorPos;          // 光标位置
    BOOL m_bCursorVisible;     // 光标是否可见
    CFont m_fontText;          // 文本字体
    // ...
};
```

### 对话2

**我提问**：我需要让每个文档窗口的标题显示"[格式] 文件名"的格式，比如"[文本格式] test.txt"或"[MyNote格式] test.mynote"。如何实现？

**AI回答**：
可以在文档序列化时根据文件格式设置标题。在CMFCApplication1Doc::Serialize方法中：

```cpp
void CMFCApplication1Doc::Serialize(CArchive& ar) {
    // 设置文档标题显示格式
    CString strPath = GetPathName();
    if (!strPath.IsEmpty()) {
        if (strPath.Find(_T(".mynote")) != -1) {
            SetTitle(_T("[MyNote格式] ") + GetTitle());
        }
        else {
            SetTitle(_T("[文本格式] ") + GetTitle());
        }
    }
    
    // ... 序列化逻辑
}
```

另外，在保存文件时也需要更新标题：

```cpp
void CMFCApplication1Doc::OnFileSaveText() {
    // ... 保存文件逻辑
    
    // 设置文档信息
    SetPathName(strPath, FALSE);
    SetModifiedFlag(FALSE);
    
    // 更新标题
    SetTitle(_T("[文本格式] ") + GetTitle());
}

void CMFCApplication1Doc::OnFileSaveMynote() {
    // ... 保存文件逻辑
    
    // 设置文档信息
    SetPathName(strPath, FALSE);
    SetModifiedFlag(FALSE);
    
    // 更新标题
    SetTitle(_T("[MyNote格式] ") + GetTitle());
}
```

### 对话3

**我提问**：我需要实现无限次撤销/重做功能。应该如何设计数据结构和算法？

**AI回答**：
建议使用栈结构保存编辑历史。每个编辑动作保存完整的文档状态和光标位置：

```cpp
// 定义编辑动作结构
struct EditAction {
    CString strContent;  // 文本内容
    int nCursorPos;      // 光标位置
    
    EditAction(const CString& content = _T(""), int cursorPos = 0)
        : strContent(content), nCursorPos(cursorPos) {}
};

// 在视图类中添加成员变量
class CMFCApplication1View : public CScrollView {
protected:
    std::vector<EditAction> m_undoStack;  // 撤销栈
    std::vector<EditAction> m_redoStack;  // 重做栈
    // ...
};
```

实现撤销逻辑：

```cpp
void CMFCApplication1View::OnEditUndo() {
    if (!m_undoStack.empty()) {
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

void CMFCApplication1View::OnEditRedo() {
    if (!m_redoStack.empty()) {
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
```

在每次编辑操作前保存状态：

```cpp
void CMFCApplication1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
    // 在修改前保存当前状态（包括光标位置）
    m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
    m_redoStack.clear();  // 清空重做栈
    
    // ... 执行编辑操作
}
```

**我的问题**：我发现当用户连续快速输入时，每个字符都保存一个完整状态会导致内存占用过大。有什么优化方案？

**AI回答**：
可以使用两种优化策略：

1. **设置撤销步数限制**：
```cpp
const int MAX_UNDO_STEPS = 100;

void CMFCApplication1View::PushUndoState() {
    m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
    
    // 限制栈大小
    if (m_undoStack.size() > MAX_UNDO_STEPS) {
        m_undoStack.erase(m_undoStack.begin());
    }
    
    m_redoStack.clear();
}
```

2. **合并连续的字符输入**（更高级的优化）：
```cpp
class CMFCApplication1View : public CScrollView {
protected:
    CString m_strLastTypedText;  // 上次输入的文本
    int m_nLastCursorPos;        // 上次光标位置
    DWORD m_dwLastTypedTime;     // 上次输入时间
    // ...
};

void CMFCApplication1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
    DWORD dwCurrentTime = GetTickCount();
    
    // 如果距离上次输入时间很短（比如300ms内），且是连续输入，则合并
    if (dwCurrentTime - m_dwLastTypedTime < 300 && 
        IsContinuousInput(nChar)) {
        // 更新最后一次状态，不新建撤销点
        m_undoStack.back() = EditAction(m_strContent, m_nCursorPos);
    } else {
        // 新建撤销点
        m_undoStack.push_back(EditAction(m_strContent, m_nCursorPos));
        if (m_undoStack.size() > MAX_UNDO_STEPS) {
            m_undoStack.erase(m_undoStack.begin());
        }
        m_redoStack.clear();
    }
    
    m_dwLastTypedTime = dwCurrentTime;
    // ... 执行编辑操作
}
```

### 对话4

**我提问**：我需要实现正则表达式搜索和替换功能。

**AI回答**：
MFC本身不直接支持正则表达式,可以使用C++11的regex库。首先在项目中启用C++11：

```cpp
// 在stdafx.h或项目设置中
#include <regex>
```

实现正则表达式搜索：

```cpp
void CMFCApplication1View::DoRegexSearch(const CString& strPattern, 
                                         BOOL bMatchCase, 
                                         BOOL bWholeWord) {
    try {
        // 转换为std::string
        std::string text = ConvertCStringToStdString(m_strContent);
        std::string pattern = ConvertCStringToStdString(strPattern);
        
        // 设置正则表达式标志
        std::regex_constants::syntax_option_type flags = 
            std::regex_constants::ECMAScript;
        
        if (!bMatchCase) {
            flags |= std::regex_constants::icase;
        }
        
        if (bWholeWord) {
            // 如果要求全词匹配，添加单词边界
            pattern = "\\b" + pattern + "\\b";
        }
        
        // 编译正则表达式
        std::regex reg;
        try {
            reg.assign(pattern, flags);
        } catch (const std::regex_error& e) {
            CString strError;
            strError.Format(_T("正则表达式语法错误: %s\n模式: %s"),
                          CString(e.what()), strPattern);
            AfxMessageBox(strError);
            return;
        }
        
        // 搜索所有匹配
        auto words_begin = std::sregex_iterator(text.begin(), text.end(), reg);
        auto words_end = std::sregex_iterator();
        
        // 存储所有匹配位置
        m_searchResults.clear();
        for (auto i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            m_searchResults.push_back((int)match.position());
        }
        
        // 定位到第一个匹配
        if (!m_searchResults.empty()) {
            m_nCursorPos = m_searchResults[0];
            m_currentMatchIndex = 0;
            UpdateViewMetrics();
            Invalidate();
        }
    } catch (const std::exception& e) {
        AfxMessageBox(_T("搜索过程中发生错误"));
    }
}
```

需要实现CString和std::string的转换函数：

```cpp
std::string CMFCApplication1View::ConvertCStringToStdString(const CString& cstr) {
#ifdef _UNICODE
    int len = WideCharToMultiByte(CP_UTF8, 0, cstr, -1, NULL, 0, NULL, NULL);
    std::string result(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, cstr, -1, &result[0], len, NULL, NULL);
    return result;
#else
    return std::string(cstr);
#endif
}

CString CMFCApplication1View::ConvertStdStringToCString(const std::string& str) {
#ifdef _UNICODE
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    CString result;
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 
                       result.GetBuffer(len), len);
    result.ReleaseBuffer();
    return result;
#else
    return CString(str.c_str());
#endif
}
```

**我的问题**：我发现正则表达式搜索在大型文档中性能很差。如何优化？

**AI回答**：
有几个优化策略：

1. **增量搜索**：不要一次性搜索整个文档
2. **超时控制**：设置搜索超时时间
3. **进度反馈**：显示搜索进度

```cpp
void CMFCApplication1View::DoRegexSearchOptimized(const CString& strPattern, 
                                                 BOOL bMatchCase) {
    // 设置超时时间（3秒）
    DWORD dwStartTime = GetTickCount();
    const DWORD MAX_SEARCH_TIME = 3000;
    
    try {
        std::string pattern = ConvertCStringToStdString(strPattern);
        std::regex_constants::syntax_option_type flags = 
            std::regex_constants::ECMAScript;
        
        if (!bMatchCase) {
            flags |= std::regex_constants::icase;
        }
        
        std::regex reg(pattern, flags);
        
        // 分块搜索，避免卡顿
        const int CHUNK_SIZE = 10240;  // 每块10KB
        std::string text = ConvertCStringToStdString(m_strContent);
        size_t totalSize = text.size();
        
        m_searchResults.clear();
        
        for (size_t startPos = 0; startPos < totalSize; startPos += CHUNK_SIZE) {
            // 检查是否超时
            if (GetTickCount() - dwStartTime > MAX_SEARCH_TIME) {
                AfxMessageBox(_T("搜索超时，已找到 ") + 
                             CString(std::to_string(m_searchResults.size()).c_str()) + 
                             _T(" 个结果"));
                break;
            }
            
            size_t chunkSize = min(CHUNK_SIZE, totalSize - startPos);
            std::string chunk = text.substr(startPos, chunkSize);
            
            auto words_begin = std::sregex_iterator(chunk.begin(), chunk.end(), reg);
            auto words_end = std::sregex_iterator();
            
            for (auto i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                m_searchResults.push_back(startPos + (int)match.position());
            }
        }
        
        // 显示结果
        if (!m_searchResults.empty()) {
            // ... 定位到第一个结果
        }
    } catch (...) {
        AfxMessageBox(_T("搜索错误"));
    }
}
```

### 对话5

**我提问**：我需要在编辑器左侧显示行号，并且行号要随文本滚动同步。应该如何实现？

**AI回答**：
需要在视图类中添加行号相关成员，并重写OnDraw方法：

```cpp
class CMFCApplication1View : public CScrollView {
protected:
    int m_nLineNumberWidth;           // 行号区域宽度
    CFont m_fontLineNumber;           // 行号字体
    COLORREF m_crLineNumberText;      // 行号文本颜色
    COLORREF m_crLineNumberBg;        // 行号背景颜色
    // ...
};
```

在OnDraw中绘制行号：

```cpp
void CMFCApplication1View::DrawLineNumbers(CDC* pDC, CRect& rect) {
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
    
    // 计算总行数
    int nTotalLines = 0;
    int nPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1) {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++;  // 最后一行
    
    // 计算可见行范围
    int nFirstVisibleLine = scrollPos.y / nCharHeight;
    int nLastVisibleLine = min((scrollPos.y + rect.Height()) / nCharHeight, nTotalLines);
    
    // 绘制可见行号
    for (int i = nFirstVisibleLine; i < nLastVisibleLine; i++) {
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
```

在OnDraw中调用：

```cpp
void CMFCApplication1View::OnDraw(CDC* pDC) {
    CMFCApplication1Doc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc) return;
    
    CRect rect;
    GetClientRect(&rect);
    
    // 绘制行号区域
    DrawLineNumbers(pDC, rect);
    
    // 设置文本区域
    CRect rectText = rect;
    rectText.left = m_nLineNumberWidth;
    
    // ... 绘制文本内容
}
```

动态计算行号区域宽度：

```cpp
void CMFCApplication1View::CalculateLineNumberWidth() {
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_fontLineNumber);
    
    // 计算总行数
    int nTotalLines = 0;
    int nPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1) {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++;  // 最后一行
    
    // 计算最大行号所需的宽度
    CString strMaxLine;
    strMaxLine.Format(_T("%d"), nTotalLines);
    
    CSize size = dc.GetTextExtent(strMaxLine);
    m_nLineNumberWidth = size.cx + 20;  // 增加一些边距
    
    dc.SelectObject(pOldFont);
}
```

**我的问题**：我发现行号刷新不及时，有时新增行后行号区域没有更新宽度。

**AI回答**：
需要在文本内容变化时重新计算行号宽度并更新滚动大小：

```cpp
void CMFCApplication1View::UpdateViewMetrics() {
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
    while ((nPos = m_strContent.Find(_T('\n'), nPos)) != -1) {
        nTotalLines++;
        nPos++;
    }
    nTotalLines++;  // 最后一行
    
    // 计算最大行长度
    int nMaxLineLength = 0;
    nPos = 0;
    int nPrevPos = 0;
    while ((nPos = m_strContent.Find(_T('\n'), nPrevPos)) != -1) {
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
    sizeTotal.cx = m_nLineNumberWidth + 5 + nMaxLineLength * nCharWidth + 20;
    sizeTotal.cy = nTotalLines * nCharHeight + 20;
    
    SetScrollSizes(MM_TEXT, sizeTotal);
    
    dc.SelectObject(pOldFont);
}
```

在文本内容变化时调用：

```cpp
void CMFCApplication1View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
    // ... 保存状态和执行编辑操作
    
    UpdateViewMetrics();  // 更新视图度量
    Invalidate();         // 重绘
}
```

### 对话6

**我提问**：我需要实现亮色和暗色两套主题，可以通过菜单切换。应该如何设计颜色系统？

**AI回答**：
可以设计一个颜色管理系统，在视图类中存储两套颜色方案：

```cpp
class CMFCApplication1View : public CScrollView {
protected:
    // 主题状态
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
    
    // 当前使用的颜色
    COLORREF m_crTextColor;
    COLORREF m_crBackgroundColor;
    COLORREF m_crLineNumberText;
    COLORREF m_crLineNumberBg;
    
public:
    void ApplyTheme(BOOL bDarkTheme);
    // ...
};
```

初始化颜色：

```cpp
CMFCApplication1View::CMFCApplication1View() {
    // 设置初始主题为亮色
    m_bDarkTheme = FALSE;
    
    // 初始化亮色主题颜色
    m_crLightText = RGB(0, 0, 0);           // 黑色文字
    m_crLightBg = RGB(255, 255, 255);       // 白色背景
    m_crLightLineNumberText = RGB(100, 100, 100);  // 深灰色行号文字
    m_crLightLineNumberBg = RGB(240, 240, 240);    // 浅灰色行号背景
    
    // 初始化暗色主题颜色
    m_crDarkText = RGB(255, 255, 255);      // 白色文字
    m_crDarkBg = RGB(30, 30, 30);           // 深灰色背景
    m_crDarkLineNumberText = RGB(180, 180, 180);   // 浅灰色行号文字
    m_crDarkLineNumberBg = RGB(50, 50, 50);        // 深灰色行号背景
    
    // 应用初始主题
    m_crTextColor = m_crLightText;
    m_crBackgroundColor = m_crLightBg;
    m_crLineNumberText = m_crLightLineNumberText;
    m_crLineNumberBg = m_crLightLineNumberBg;
}
```

主题切换函数：

```cpp
void CMFCApplication1View::ApplyTheme(BOOL bDarkTheme) {
    m_bDarkTheme = bDarkTheme;
    
    if (m_bDarkTheme) {
        // 应用暗色主题
        m_crTextColor = m_crDarkText;
        m_crBackgroundColor = m_crDarkBg;
        m_crLineNumberText = m_crDarkLineNumberText;
        m_crLineNumberBg = m_crDarkLineNumberBg;
    } else {
        // 应用亮色主题
        m_crTextColor = m_crLightText;
        m_crBackgroundColor = m_crLightBg;
        m_crLineNumberText = m_crLightLineNumberText;
        m_crLineNumberBg = m_crLightLineNumberBg;
    }
    
    // 重绘视图
    Invalidate();
}
```

菜单处理函数：

```cpp
void CMFCApplication1View::OnViewDarkTheme() {
    ApplyTheme(TRUE);
}

void CMFCApplication1View::OnViewLightTheme() {
    ApplyTheme(FALSE);
}

// 更新菜单状态
void CMFCApplication1View::OnUpdateViewDarkTheme(CCmdUI* pCmdUI) {
    pCmdUI->SetCheck(m_bDarkTheme);
}

void CMFCApplication1View::OnUpdateViewLightTheme(CCmdUI* pCmdUI) {
    pCmdUI->SetCheck(!m_bDarkTheme);
}
```

**我的问题**：主题切换后，应用程序的其他部分（如菜单、工具栏）没有跟着切换。如何让整个应用程序都支持主题？

**AI回答**：
要让整个应用程序支持主题，需要：

1. **主框架窗口也支持主题**：

```cpp
class CMainFrame : public CMDIFrameWndEx {
protected:
    BOOL m_bDarkTheme;
    
public:
    void ApplyTheme(BOOL bDarkTheme);
    // ...
};

void CMainFrame::ApplyTheme(BOOL bDarkTheme) {
    m_bDarkTheme = bDarkTheme;
    
    // 更新菜单栏、工具栏颜色
    CMFCVisualManager::SetDefaultManager(
        m_bDarkTheme ? RUNTIME_CLASS(CMFCVisualManagerVS2015) : 
                      RUNTIME_CLASS(CMFCVisualManagerVS2012)
    );
    
    // 如果使用MFC功能包的高级UI功能
    if (CMFCVisualManager::GetInstance() != NULL) {
        CMFCVisualManager::GetInstance()->RedrawAll();
    }
    
    // 更新所有子窗口
    RecalcLayout();
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW);
}
```

2. **在应用程序类中统一管理主题**：

```cpp
class CMFCApplication1App : public CWinAppEx {
protected:
    BOOL m_bDarkTheme;
    
public:
    BOOL GetCurrentTheme() { return m_bDarkTheme; }
    void SetTheme(BOOL bDarkTheme);
    // ...
};

void CMFCApplication1App::SetTheme(BOOL bDarkTheme) {
    m_bDarkTheme = bDarkTheme;
    
    // 通知所有打开的文档
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        CDocTemplate* pTemplate = GetNextDocTemplate(pos);
        POSITION docPos = pTemplate->GetFirstDocPosition();
        while (docPos != NULL) {
            CDocument* pDoc = pTemplate->GetNextDoc(docPos);
            POSITION viewPos = pDoc->GetFirstViewPosition();
            while (viewPos != NULL) {
                CView* pView = pDoc->GetNextView(viewPos);
                if (pView->IsKindOf(RUNTIME_CLASS(CMFCApplication1View))) {
                    ((CMFCApplication1View*)pView)->ApplyTheme(bDarkTheme);
                }
            }
        }
    }
    
    // 通知主框架
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrame) {
        pMainFrame->ApplyTheme(bDarkTheme);
    }
    
    // 保存主题到注册表，下次启动时使用
    WriteProfileInt(_T("Settings"), _T("DarkTheme"), bDarkTheme);
}
```

### 对话7

**我提问**：我需要实现自定义的.mynote格式，要求文件头保存学号，文件尾附加加密的SHA-1摘要。应该如何设计文件格式？

**AI回答**：
设计.mynote文件格式如下：

```
第一行：<学号>
第二行到倒数第二行：文本内容
最后一行：<SECRET_KEY><IV><加密的SHA-1摘要>
```

实现序列化：

```cpp
void CMFCApplication1Doc::Serialize(CArchive& ar) {
    // 判断文件格式
    CString strPath = GetPathName();
    BOOL bIsMyNote = strPath.Find(_T(".mynote")) != -1;
    
    if (ar.IsStoring()) {
        // 保存文档
        CString strContent = GetContentFromView();  // 从视图获取内容
        
        if (bIsMyNote) {
            // .mynote格式：添加学号头和简单摘要尾
            CString strMyNoteContent;
            
            // 第一行：学号
            strMyNoteContent = m_strStudentID + _T("\n");
            
            // 中间：实际文本内容
            strMyNoteContent += strContent;
            
            // 生成简单摘要
            CString strSimpleHash = GenerateSimpleHash(strContent);
            
            // 最后一行：密钥+IV+简单摘要
            if (!strContent.IsEmpty() && strContent[strContent.GetLength() - 1] != _T('\n'))
                strMyNoteContent += _T("\n");
            strMyNoteContent += m_strSecretKey + m_strIV + strSimpleHash;
            
            ar << strMyNoteContent;
        } else {
            // .txt格式：直接保存纯文本
            ar << strContent;
        }
    } else {
        // 加载文档
        CString strContent;
        ar >> strContent;
        
        if (bIsMyNote) {
            // 处理.mynote格式
            // 1. 去掉第一行的学号占位符
            int nFirstNewLine = strContent.Find(_T('\n'));
            if (nFirstNewLine != -1) {
                CString strFirstLine = strContent.Left(nFirstNewLine);
                if (strFirstLine.Find(_T('<')) != -1 && strFirstLine.Find(_T('>')) != -1) {
                    // 可以在这里保存学号
                    m_strStudentID = strFirstLine;
                }
                strContent = strContent.Mid(nFirstNewLine + 1);
            }
            
            // 2. 去掉最后一行（密钥+IV+简单摘要）
            int nLastNewLine = strContent.ReverseFind(_T('\n'));
            if (nLastNewLine != -1) {
                CString strLastLine = strContent.Mid(nLastNewLine + 1);
                if (strLastLine.Find(_T("<SECRET_KEY>")) != -1) {
                    // 提取并验证摘要
                    CString strRemainingContent = strContent.Left(nLastNewLine);
                    
                    // 这里可以添加摘要验证逻辑
                    strContent = strRemainingContent;
                }
            }
        }
        
        SetContentToView(strContent);  // 设置内容到视图
    }
}
```

实现简单的哈希函数：

```cpp
CString CMFCApplication1Doc::GenerateSimpleHash(const CString& strContent) {
    if (strContent.IsEmpty()) {
        return _T("<EMPTY>");
    }
    
    // 简单计算：内容长度 + 前10个字符的简单组合
    int nLen = strContent.GetLength();
    
    // 取前5个字符和后5个字符的组合作为简单摘要
    CString strPrefix = strContent.Left(5);
    CString strSuffix;
    if (nLen > 5) {
        strSuffix = strContent.Right(5);
    }
    
    // 转换为大写
    strPrefix.MakeUpper();
    strSuffix.MakeUpper();
    
    // 组合成摘要格式：<HASH:前5字符_后5字符_长度>
    CString strHash;
    strHash.Format(_T("<HASH:%s_%s_%d>"), strPrefix, strSuffix, nLen);
    
    return strHash;
}

BOOL CMFCApplication1Doc::VerifySimpleHash(const CString& strContent, const CString& strHash) {
    // 生成当前内容的哈希
    CString strGeneratedHash = GenerateSimpleHash(strContent);
    
    // 简单比较
    return (strGeneratedHash == strHash);
}
```

**我的问题**：如何实现真正的AES-CBC加密，而不是简单的哈希？

**AI回答**：
如果需要真正的AES-CBC加密，可以使用Windows CryptoAPI。以下是简化版本：

```cpp
class CEncryption {
public:
    CEncryption();
    
    // 设置学号
    void SetStudentID(const CString& strStudentID) { m_strStudentID = strStudentID; }
    
    // 生成SHA-1摘要
    CString GenerateSHA1Digest(const CString& strContent);
    
    // AES-CBC加密
    CString AES_CBC_Encrypt(const CString& strPlaintext);
    
    // AES-CBC解密
    CString AES_CBC_Decrypt(const CString& strCiphertext);
    
private:
    CString m_strStudentID;
    std::vector<BYTE> m_key;
    std::vector<BYTE> m_iv;
    BYTE m_defaultKey[32];    // 256位密钥
    BYTE m_defaultIV[16];     // 128位IV
    
    std::vector<BYTE> StringToByteArray(const CString& str);
    CString ByteArrayToString(const std::vector<BYTE>& byteArray);
};

CString CEncryption::GenerateSHA1Digest(const CString& strContent) {
    HCRYPTPROV hProv = NULL;
    HCRYPTHASH hHash = NULL;
    BYTE rgbHash[20]; // SHA-1 produces 20-byte hash
    
    // 转换为ANSI字符串
    std::string strAnsi = CT2A(strContent);
    
    // 获取CSP句柄
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return _T("");
    }
    
    // 创建哈希对象
    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return _T("");
    }
    
    // 哈希数据
    if (!CryptHashData(hHash, (BYTE*)strAnsi.c_str(), (DWORD)strAnsi.length(), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return _T("");
    }
    
    // 获取哈希值
    DWORD cbHash = 20;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return _T("");
    }
    
    // 转换为十六进制字符串
    CString strHash;
    for (DWORD i = 0; i < cbHash; i++) {
        TCHAR szHex[3];
        _stprintf_s(szHex, _T("%02x"), rgbHash[i]);
        strHash += szHex;
    }
    
    // 清理
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    return strHash;
}
```

### 对话8

**我提问**：项目要求使用RAII包装CFile和CArchive，捕获异常并弹出对话框。应该如何实现？

**AI回答**：
创建RAII包装类，确保资源在离开作用域时自动释放：

```cpp
// RAII包装类
class CFileRAII {
public:
    CFileRAII() : m_pFile(nullptr) {}
    
    BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL) {
        m_pFile = new CFile();
        return m_pFile->Open(lpszFileName, nOpenFlags, pError);
    }
    
    ~CFileRAII() {
        if (m_pFile) {
            m_pFile->Close();
            delete m_pFile;
            m_pFile = nullptr;
        }
    }
    
    CFile* GetFile() { return m_pFile; }
    operator CFile* () { return m_pFile; }
    
private:
    CFile* m_pFile;
};

class CArchiveRAII {
public:
    CArchiveRAII(CFile* pFile, UINT nMode, int nBufSize = 4096, void* lpBuf = NULL)
        : m_archive(pFile, nMode, nBufSize, lpBuf) {
    }
    
    ~CArchiveRAII() {
        // CArchive析构时会自动关闭，不需要手动调用Close
    }
    
    CArchive* operator->() { return &m_archive; }
    CArchive& GetArchive() { return m_archive; }
    
private:
    CArchive m_archive;
};
```

在文件操作中使用RAII和异常处理：

```cpp
void CMFCApplication1Doc::OnFileSaveText() {
    try {
        CFileDialog dlg(FALSE, _T("txt"), _T("NewDocument.txt"),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
            _T("Text Files (*.txt)|*.txt||"));
        
        if (dlg.DoModal() == IDOK) {
            CString strPath = dlg.GetPathName();
            
            // RAII包装
            CFileRAII file;
            CFileException fe;
            
            if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &fe)) {
                CString strError;
                strError.Format(_T("无法创建文件: %s\n错误代码: %d"), strPath, fe.m_cause);
                AfxMessageBox(strError);
                return;
            }
            
            CArchiveRAII archive(file.GetFile(), CArchive::store);
            
            // 获取内容并保存
            CString strContent = GetContentFromView();
            archive.GetArchive() << strContent;
            
            // 设置文档信息
            SetPathName(strPath, FALSE);
            SetModifiedFlag(FALSE);
            
            AfxMessageBox(_T("文件保存成功！"));
        }
    } catch (CFileException* e) {
        CString strError;
        strError.Format(_T("文件操作错误: %d"), e->m_cause);
        AfxMessageBox(strError);
        e->Delete();
    } catch (CArchiveException* e) {
        CString strError;
        strError.Format(_T("序列化错误: %d"), e->m_cause);
        AfxMessageBox(strError);
        e->Delete();
    } catch (CException* e) {
        CString strError;
        e->GetErrorMessage(strError.GetBuffer(256), 256);
        strError.ReleaseBuffer();
        AfxMessageBox(strError);
        e->Delete();
    } catch (...) {
        AfxMessageBox(_T("未知错误"));
    }
}
```

**我的问题**：我发现当同时处理多种异常时，代码重复很多。如何优化异常处理？

**AI回答**：
可以创建一个统一的异常处理宏或函数：

```cpp
// 异常处理辅助函数
void HandleException(const CString& strContext) {
    try {
        throw;  // 重新抛出当前异常
    } catch (CFileException* e) {
        CString strError;
        strError.Format(_T("%s - 文件操作错误: %d"), strContext, e->m_cause);
        AfxMessageBox(strError);
        e->Delete();
    } catch (CArchiveException* e) {
        CString strError;
        strError.Format(_T("%s - 序列化错误: %d"), strContext, e->m_cause);
        AfxMessageBox(strError);
        e->Delete();
    } catch (CException* e) {
        CString strError;
        e->GetErrorMessage(strError.GetBuffer(256), 256);
        strError.ReleaseBuffer();
        AfxMessageBox(strContext + _T(" - ") + strError);
        e->Delete();
    } catch (std::exception& e) {
        CString strError;
        strError.Format(_T("%s - STL异常: %s"), strContext, CString(e.what()));
        AfxMessageBox(strError);
    } catch (...) {
        AfxMessageBox(strContext + _T(" - 未知错误"));
    }
}

// 使用示例
void CMFCApplication1Doc::OnFileSaveText() {
    CFileDialog dlg(FALSE, _T("txt"), _T("NewDocument.txt"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        _T("Text Files (*.txt)|*.txt||"));
    
    if (dlg.DoModal() != IDOK) return;
    
    CString strPath = dlg.GetPathName();
    
    try {
        // RAII包装
        CFileRAII file;
        CFileException fe;
        
        if (!file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &fe)) {
            CString strError;
            strError.Format(_T("无法创建文件: %s\n错误代码: %d"), strPath, fe.m_cause);
            AfxMessageBox(strError);
            return;
        }
        
        CArchiveRAII archive(file.GetFile(), CArchive::store);
        
        // 获取内容并保存
        CString strContent = GetContentFromView();
        archive.GetArchive() << strContent;
        
        // 设置文档信息
        SetPathName(strPath, FALSE);
        SetModifiedFlag(FALSE);
        
        AfxMessageBox(_T("文件保存成功！"));
    } catch (...) {
        HandleException(_T("保存文件时"));
    }
}
```



### 对话9

**我提问**：我需要为项目添加单元测试，要求测试覆盖率≥70%。应该如何设置测试框架？

**AI回答**：
可以使用Google Test框架。以下是设置步骤：

1. **安装Google Test**：
```bash
# 使用vcpkg安装
vcpkg install gtest:x86-windows
```

2. **创建测试项目**：
```cpp
// test_core.cpp
#include "pch.h"
#include "gtest/gtest.h"
#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

// 测试文档类
TEST(DocumentTest, GenerateSimpleHash) {
    CMFCApplication1Doc doc;
    
    // 测试空字符串
    CString emptyHash = doc.GenerateSimpleHash(_T(""));
    EXPECT_EQ(emptyHash, _T("<EMPTY>"));
    
    // 测试短字符串
    CString shortHash = doc.GenerateSimpleHash(_T("Hello"));
    EXPECT_TRUE(shortHash.Find(_T("HELLO")) != -1);
    
    // 测试长字符串
    CString longText = _T("This is a long text for testing hash function");
    CString longHash = doc.GenerateSimpleHash(longText);
    EXPECT_TRUE(longHash.Find(_T("HASH:")) != -1);
}

// 测试视图类
class ViewTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建视图对象
        m_pView = new CMFCApplication1View();
    }
    
    void TearDown() override {
        delete m_pView;
    }
    
    CMFCApplication1View* m_pView;
};

TEST_F(ViewTest, CharIndexToLineColumn) {
    m_pView->SetText(_T("Line1\nLine2\nLine3"));
    
    int line, column;
    
    // 测试第一行第一个字符
    m_pView->CharIndexToLineColumn(0, line, column);
    EXPECT_EQ(line, 0);
    EXPECT_EQ(column, 0);
    
    // 测试第二行开始
    m_pView->CharIndexToLineColumn(6, line, column);
    EXPECT_EQ(line, 1);
    EXPECT_EQ(column, 0);
}

TEST_F(ViewTest, LineColumnToCharIndex) {
    m_pView->SetText(_T("Line1\nLine2\nLine3"));
    
    // 测试第一行
    int index = m_pView->LineColumnToCharIndex(0, 2);
    EXPECT_EQ(index, 2);  // "Li" - 第2个字符
    
    // 测试第二行
    index = m_pView->LineColumnToCharIndex(1, 3);
    EXPECT_EQ(index, 9);  // "Line2"的第3个字符
}
```

3. **创建测试覆盖率报告**：
```bash
# 使用OpenCppCoverage生成覆盖率报告
OpenCppCoverage --sources MyNoteEditor --modules MyNoteEditor.exe --excluded_sources ".*\\ThirdParty\\.*" --export_type=html --output coverage_report
```

4. **创建测试场景**：
```cpp
// 文件I/O测试
TEST(FileIOTest, SaveAndLoadText) {
    CMFCApplication1Doc doc;
    CString testContent = _T("Test content\nMultiple lines\nEnd of file");
    
    // 模拟保存和加载
    // 这里需要模拟文件操作
    // ...
}

// 主题切换测试
TEST(ThemeTest, SwitchTheme) {
    CMFCApplication1View view;
    
    // 初始应为亮色主题
    EXPECT_FALSE(view.m_bDarkTheme);
    
    // 切换到暗色主题
    view.ApplyTheme(TRUE);
    EXPECT_TRUE(view.m_bDarkTheme);
    
    // 切换回亮色主题
    view.ApplyTheme(FALSE);
    EXPECT_FALSE(view.m_bDarkTheme);
}

// 搜索功能测试
TEST(SearchTest, RegexSearch) {
    CMFCApplication1View view;
    view.SetText(_T("Hello World\nTest Pattern\nAnother Test"));
    
    // 测试简单搜索
    view.DoRegexSearch(_T("Test"), FALSE, FALSE);
    EXPECT_GT(view.m_searchResults.size(), 0);
    
    // 测试正则表达式搜索
    view.DoRegexSearch(_T("T.*t"), FALSE, FALSE);  // 匹配以T开头以t结尾
    EXPECT_GT(view.m_searchResults.size(), 0);
}
```

**我的问题**：如何测试MFC的UI组件？有些功能需要用户交互。

**AI回答**：
对于UI组件的测试，可以使用以下策略：

1. **分离业务逻辑**：将核心算法与UI分离，单独测试算法
2. **模拟用户输入**：使用测试框架模拟消息
3. **使用UI测试框架**：如Microsoft的Coded UI Test

```cpp
// 模拟键盘输入测试
TEST(EditTest, KeyboardInput) {
    CMFCApplication1View view;
    
    // 模拟输入字符'A'
    view.OnChar('A', 1, 0);
    EXPECT_EQ(view.GetText(), _T("A"));
    
    // 模拟退格键
    view.OnChar(8, 1, 0);  // Backspace
    EXPECT_EQ(view.GetText(), _T(""));
    
    // 模拟回车键
    view.OnChar(13, 1, 0);  // Enter
    EXPECT_EQ(view.GetText(), _T("\n"));
}

// 测试撤销/重做
TEST(UndoTest, UndoRedo) {
    CMFCApplication1View view;
    
    // 输入一些文本
    view.OnChar('H', 1, 0);
    view.OnChar('e', 1, 0);
    view.OnChar('l', 1, 0);
    view.OnChar('l', 1, 0);
    view.OnChar('o', 1, 0);
    
    EXPECT_EQ(view.GetText(), _T("Hello"));
    
    // 撤销
    view.OnEditUndo();
    EXPECT_EQ(view.GetText(), _T("Hell"));
    
    // 再撤销
    view.OnEditUndo();
    EXPECT_EQ(view.GetText(), _T("Hel"));
    
    // 重做
    view.OnEditRedo();
    EXPECT_EQ(view.GetText(), _T("Hell"));
    
    // 再重做
    view.OnEditRedo();
    EXPECT_EQ(view.GetText(), _T("Hello"));
}

// 模拟鼠标点击测试
TEST(MouseTest, TextSelection) {
    CMFCApplication1View view;
    view.SetText(_T("Line 1\nLine 2\nLine 3"));
    
    // 模拟鼠标在特定位置点击
    CPoint point(50, 20);  // 假设这个位置对应第二行
    
    // 需要设置视图大小和字体才能准确计算
    // 这是一个简化的测试示例
    // ...
}
```

### 对话10

**我提问**：当打开大型文件（超过1MB）时，编辑器响应很慢。如何优化性能？

**AI回答**：
对于大型文件，需要以下优化策略：

1. **延迟加载和分页**：
```cpp
class CLazyTextBuffer {
private:
    struct TextPage {
        int nStartLine;
        int nEndLine;
        CStringArray lines;
    };
    
    std::vector<TextPage> m_pages;
    CString m_strFullPath;
    int m_nTotalLines;
    int m_nPageSize;  // 每页行数
    
public:
    CLazyTextBuffer() : m_nTotalLines(0), m_nPageSize(100) {}
    
    BOOL LoadFile(LPCTSTR lpszFileName) {
        m_strFullPath = lpszFileName;
        
        // 首先快速计算总行数
        CStdioFile file;
        if (!file.Open(lpszFileName, CFile::modeRead)) {
            return FALSE;
        }
        
        CString strLine;
        m_nTotalLines = 0;
        while (file.ReadString(strLine)) {
            m_nTotalLines++;
        }
        file.Close();
        
        return TRUE;
    }
    
    CString GetLine(int nLine) {
        // 计算该行属于哪一页
        int nPage = nLine / m_nPageSize;
        
        // 如果该页未加载，则加载
        if (nPage >= m_pages.size()) {
            LoadPage(nPage);
        }
        
        // 返回该行
        int nLineInPage = nLine % m_nPageSize;
        if (nLineInPage < m_pages[nPage].lines.GetSize()) {
            return m_pages[nPage].lines[nLineInPage];
        }
        
        return _T("");
    }
    
private:
    void LoadPage(int nPage) {
        // 加载指定页
        // ...
    }
};
```

2. **虚拟化文本渲染**：
```cpp
void CMFCApplication1View::OnDraw(CDC* pDC) {
    // 只绘制可见区域
    CRect rectClip;
    pDC->GetClipBox(rectClip);
    
    // 计算可见行范围
    CPoint scrollPos = GetScrollPosition();
    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    int nCharHeight = tm.tmHeight + tm.tmExternalLeading;
    
    int nFirstVisibleLine = scrollPos.y / nCharHeight;
    int nLastVisibleLine = nFirstVisibleLine + (rectClip.Height() / nCharHeight) + 1;
    
    // 只获取和绘制可见行
    for (int i = nFirstVisibleLine; i <= nLastVisibleLine; i++) {
        CString strLine = GetLineFromBuffer(i);  // 从缓冲获取
        if (!strLine.IsEmpty()) {
            // 绘制该行
            // ...
        }
    }
}
```

3. **优化搜索算法**：
```cpp
void CMFCApplication1View::SearchInLargeFile(const CString& strSearch) {
    // 使用内存映射文件进行快速搜索
    CFile file;
    if (!file.Open(m_strFilePath, CFile::modeRead | CFile::shareDenyWrite)) {
        return;
    }
    
    HANDLE hFileMap = CreateFileMapping(
        (HANDLE)file.m_hFile,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL);
    
    if (hFileMap) {
        LPCSTR pFileData = (LPCSTR)MapViewOfFile(
            hFileMap,
            FILE_MAP_READ,
            0,
            0,
            0);
        
        if (pFileData) {
            // 使用Boyer-Moore等高效搜索算法
            std::string strFileData(pFileData, file.GetLength());
            std::string strSearchA = CT2A(strSearch);
            
            // 高效搜索
            size_t pos = 0;
            while ((pos = strFileData.find(strSearchA, pos)) != std::string::npos) {
                m_searchResults.push_back((int)pos);
                pos += strSearchA.length();
            }
            
            UnmapViewOfFile(pFileData);
        }
        
        CloseHandle(hFileMap);
    }
    
    file.Close();
}
```

4. **异步文件操作**：
```cpp
void CMFCApplication1View::LoadFileAsync(LPCTSTR lpszFileName) {
    // 使用工作线程加载文件
    AfxBeginThread(LoadFileThread, this);
}

UINT CMFCApplication1View::LoadFileThread(LPVOID pParam) {
    CMFCApplication1View* pView = (CMFCApplication1View*)pParam;
    
    // 在后台线程中加载文件
    CStdioFile file;
    if (file.Open(pView->m_strFilePath, CFile::modeRead)) {
        CString strLine;
        CStringArray lines;
        
        while (file.ReadString(strLine)) {
            lines.Add(strLine);
            
            // 每加载100行通知一次UI更新进度
            if (lines.GetSize() % 100 == 0) {
                // 发送消息更新进度条
                // ...
            }
        }
        
        file.Close();
        
        // 加载完成后，通知主线程更新UI
        // ...
    }
    
    return 0;
}
```