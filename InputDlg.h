#pragma once

// 最简单的输入对话框
class CInputDlg : public CDialog
{
public:
    CInputDlg(CWnd* pParent = nullptr);
    virtual ~CInputDlg() {}  // 添加析构函数

    CString m_strPrompt;  // 提示文字
    CString m_strInput;   // 用户输入

    // 重写 DoDataExchange
    virtual void DoDataExchange(CDataExchange* pDX);

    // 重写 OnInitDialog
    virtual BOOL OnInitDialog();

    // 重写 OnOK
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CInputDlg)  // 添加动态创建声明
};