#include "pch.h"
#include "MFCApplication1.h"
#include "InputDlg.h"

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)  // 添加这行

CInputDlg::CInputDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_INPUT_DIALOG, pParent)
    , m_strPrompt(_T("请输入:"))
    , m_strInput(_T(""))
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_INPUT, m_strInput);
}

BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
END_MESSAGE_MAP()

BOOL CInputDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置提示文字
    SetDlgItemText(IDC_STATIC_PROMPT, m_strPrompt);

    return TRUE;
}

void CInputDlg::OnOK()
{
    UpdateData(TRUE);

    if (m_strInput.IsEmpty())
    {
        AfxMessageBox(_T("请输入内容"));
        return;
    }

    CDialog::OnOK();
}