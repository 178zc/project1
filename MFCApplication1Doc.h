#pragma once

class CMFCApplication1Doc : public CDocument
{
protected:
    CMFCApplication1Doc() noexcept;
    DECLARE_DYNCREATE(CMFCApplication1Doc)

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

#ifdef SHARED_HANDLERS
    virtual void InitializeSearchContent();
    virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif

public:
    virtual ~CMFCApplication1Doc();
    afx_msg void OnFileSaveText();
    afx_msg void OnFileSaveMynote();

    // 添加设置学号命令
    afx_msg void OnSetStudentID();

    // 获取和设置学号
    CString GetStudentID() const { return m_strStudentID; }
    void SetStudentID(const CString& strID) { m_strStudentID = strID; }

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
    void SetSearchContent(const CString& value);
#endif

private:
    CString m_strStudentID;          // 学号
    CString m_strSecretKey;          // 密钥
    CString m_strIV;                 // IV

    // 添加一个简单的加密/解密函数（占位符版本）
    CString GenerateSimpleHash(const CString& strContent);
    BOOL VerifySimpleHash(const CString& strContent, const CString& strHash);
};