#include "pch.h"
#include "MFCApplication1.h"
#include "SearchReplaceDlg.h"
#include "InputDlg.h"

IMPLEMENT_DYNAMIC(CSearchReplaceDlg, CDialog)

CSearchReplaceDlg::CSearchReplaceDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_SEARCH_REPLACE_DIALOG, pParent)
    , m_strSearchText(_T(""))
    , m_strReplaceText(_T(""))
    , m_bIsSearchOnly(TRUE)
    , m_bUseRegex(FALSE)
    , m_bMatchCase(FALSE)
    , m_bWholeWord(FALSE)
    , m_bReplaceAll(FALSE)
{
}

CSearchReplaceDlg::~CSearchReplaceDlg()
{
}

void CSearchReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearchText);
    DDX_Text(pDX, IDC_EDIT_REPLACE, m_strReplaceText);
    DDX_Check(pDX, IDC_CHECK_REGEX, m_bUseRegex);
    DDX_Check(pDX, IDC_CHECK_MATCHCASE, m_bMatchCase);
    DDX_Check(pDX, IDC_CHECK_WHOLEWORD, m_bWholeWord);
    DDX_Check(pDX, IDC_CHECK_REPLACEALL, m_bReplaceAll);
}

BEGIN_MESSAGE_MAP(CSearchReplaceDlg, CDialog)
    ON_BN_CLICKED(IDC_CHECK_REGEX, &CSearchReplaceDlg::OnBnClickedCheckRegex)
END_MESSAGE_MAP()

BOOL CSearchReplaceDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 根据模式设置标题
    if (m_bIsSearchOnly)
    {
        SetWindowText(_T("搜索"));
        // 隐藏替换相关控件
        CWnd* pReplaceLabel = GetDlgItem(IDC_STATIC_REPLACE);
        CWnd* pReplaceEdit = GetDlgItem(IDC_EDIT_REPLACE);
        CWnd* pReplaceAllCheck = GetDlgItem(IDC_CHECK_REPLACEALL);

        if (pReplaceLabel) pReplaceLabel->ShowWindow(SW_HIDE);
        if (pReplaceEdit) pReplaceEdit->ShowWindow(SW_HIDE);
        if (pReplaceAllCheck) pReplaceAllCheck->ShowWindow(SW_HIDE);

        SetDlgItemText(IDOK, _T("搜索(&F)"));
    }
    else
    {
        SetWindowText(_T("替换"));
        SetDlgItemText(IDOK, _T("替换(&R)"));
    }

    // 确保所有复选框都是启用的
    GetDlgItem(IDC_CHECK_REGEX)->EnableWindow(TRUE);
    GetDlgItem(IDC_CHECK_MATCHCASE)->EnableWindow(TRUE);
    GetDlgItem(IDC_CHECK_WHOLEWORD)->EnableWindow(TRUE);

    // 更新UI状态
    UpdateUIControls();

    return TRUE;
}

void CSearchReplaceDlg::UpdateUIControls()
{
    // 如果使用正则表达式，禁用全词匹配（正则表达式自己可以处理）
    CWnd* pWholeWordCheck = GetDlgItem(IDC_CHECK_WHOLEWORD);
    CWnd* pRegexCheck = GetDlgItem(IDC_CHECK_REGEX);

    if (pWholeWordCheck && pRegexCheck)
    {
        BOOL bIsRegexChecked = (IsDlgButtonChecked(IDC_CHECK_REGEX) == BST_CHECKED);

        pWholeWordCheck->EnableWindow(!bIsRegexChecked);

        // 如果正则表达式已选中且全词匹配也被选中，取消全词匹配
        if (bIsRegexChecked && (IsDlgButtonChecked(IDC_CHECK_WHOLEWORD) == BST_CHECKED))
        {
            CheckDlgButton(IDC_CHECK_WHOLEWORD, BST_UNCHECKED);
            m_bWholeWord = FALSE;
        }
    }
}

void CSearchReplaceDlg::OnBnClickedCheckRegex()
{
    // 更新控件状态
    UpdateUIControls();
}

void CSearchReplaceDlg::OnOK()
{
    // 获取控件状态
    UpdateData(TRUE);

    if (m_strSearchText.IsEmpty())
    {
        AfxMessageBox(_T("请输入要搜索的内容"));
        GetDlgItem(IDC_EDIT_SEARCH)->SetFocus();
        return;
    }

    // 确保对话框关闭前状态正确
    UpdateData(FALSE);

    CDialog::OnOK();
}