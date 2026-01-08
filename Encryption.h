#pragma once
#include <string>
#include <vector>
#include <Windows.h>

// 简单的加密类
class CEncryption
{
public:
    // 构造函数
    CEncryption();

    // 设置学号
    void SetStudentID(const CString& strStudentID) { m_strStudentID = strStudentID; }

    // 设置密钥和IV（演示用，实际应用中应该更安全）
    void SetEncryptionKey(const BYTE* pKey, int nKeyLength);
    void SetEncryptionIV(const BYTE* pIV, int nIVLength);

    // 获取学生学号
    CString GetStudentID() const { return m_strStudentID; }

    // 生成SHA-1摘要
    CString GenerateSHA1Digest(const CString& strContent);

    // AES-CBC加密
    CString AES_CBC_Encrypt(const CString& strPlaintext);

    // AES-CBC解密
    CString AES_CBC_Decrypt(const CString& strCiphertext);

    // 生成加密的SHA-1摘要（用于文件尾）
    CString GenerateEncryptedSHA1Digest(const CString& strContent);

    // 验证加密的SHA-1摘要
    BOOL VerifyEncryptedSHA1Digest(const CString& strContent, const CString& strEncryptedDigest);

    // Base64编码/解码
    static CString Base64Encode(const BYTE* pData, DWORD dwDataLength);
    static std::vector<BYTE> Base64Decode(const CString& strBase64);

private:
    // 内部函数
    std::vector<BYTE> StringToByteArray(const CString& str);
    CString ByteArrayToString(const std::vector<BYTE>& byteArray);

private:
    CString m_strStudentID;
    std::vector<BYTE> m_key;
    std::vector<BYTE> m_iv;
    BYTE m_defaultKey[32];    // 256位密钥
    BYTE m_defaultIV[16];     // 128位IV
};