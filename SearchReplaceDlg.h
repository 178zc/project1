#pragma once

class CSearchReplaceDlg : public CDialog
{
public:
    CSearchReplaceDlg(CWnd* pParent = nullptr);
    virtual ~CSearchReplaceDlg();

    // 对话框数据
    enum { IDD = IDD_SEARCH_REPLACE_DIALOG };

    CString m_strSearchText;     // 搜索文本
    CString m_strReplaceText;    // 替换文本
    BOOL m_bIsSearchOnly;        // 是否为只搜索模式

    // 搜索选项
    BOOL m_bUseRegex;            // 是否使用正则表达式
    BOOL m_bMatchCase;           // 是否区分大小写
    BOOL m_bWholeWord;           // 是否全词匹配
    BOOL m_bReplaceAll;          // 是否替换全部

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    // UI更新函数
    void UpdateUIControls();

    // 消息处理函数
    afx_msg void OnBnClickedCheckRegex();
    afx_msg void OnBnClickedCheckMatchCase();
    afx_msg void OnBnClickedCheckWholeWord();

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CSearchReplaceDlg)  // 添加这行
};