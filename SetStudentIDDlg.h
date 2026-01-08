#pragma once

// CSetStudentIDDlg 对话框
class CSetStudentIDDlg : public CDialog
{
    DECLARE_DYNAMIC(CSetStudentIDDlg)  // 添加这行

public:
    CSetStudentIDDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CSetStudentIDDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SET_STUDENTID_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    CString m_strStudentID;  // 学号

    // 重写 OnOK
    virtual void OnOK();
};