#include "pch.h"
#include "MFCApplication1.h"
#include "SetStudentIDDlg.h"

IMPLEMENT_DYNAMIC(CSetStudentIDDlg, CDialog)

CSetStudentIDDlg::CSetStudentIDDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_SET_STUDENTID_DIALOG, pParent)
    , m_strStudentID(_T("123456789"))  // 默认学号
{
}

CSetStudentIDDlg::~CSetStudentIDDlg()
{
}

void CSetStudentIDDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_STUDENTID, m_strStudentID);
}

BEGIN_MESSAGE_MAP(CSetStudentIDDlg, CDialog)
    // 可以在这里添加消息处理
END_MESSAGE_MAP()

BOOL CSetStudentIDDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 如果有需要，在这里进行初始化
    return TRUE;  // 返回 TRUE 除非您将焦点设置到控件
}

void CSetStudentIDDlg::OnOK()
{
    // 更新数据
    UpdateData(TRUE);

    // 验证输入
    if (m_strStudentID.IsEmpty())
    {
        AfxMessageBox(_T("请输入学号"));
        GetDlgItem(IDC_EDIT_STUDENTID)->SetFocus();
        return;
    }

    // 确保学号格式为 <学号>
    if (m_strStudentID.GetAt(0) != _T('<') || m_strStudentID.GetAt(m_strStudentID.GetLength() - 1) != _T('>'))
    {
        m_strStudentID = _T("<") + m_strStudentID + _T(">");
        UpdateData(FALSE);  // 更新显示
    }

    CDialog::OnOK();
}