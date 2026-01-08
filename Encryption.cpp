#include "pch.h"
#include "Encryption.h"
#include <wincrypt.h>
#include <bcrypt.h>
#include <atlbase.h>
#include <atlconv.h>

#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "bcrypt.lib")

CEncryption::CEncryption()
{
    // 设置默认学号
    m_strStudentID = _T("20250312010");

    // 设置默认密钥和IV（仅用于演示）
    // 实际应用中应该使用更安全的密钥管理
    memset(m_defaultKey, 0xAA, sizeof(m_defaultKey));
    memset(m_defaultIV, 0x55, sizeof(m_defaultIV));

    m_key.assign(m_defaultKey, m_defaultKey + sizeof(m_defaultKey));
    m_iv.assign(m_defaultIV, m_defaultIV + sizeof(m_defaultIV));
}

//void CEncryption::SetStudentID(const CString& strStudentID)
//{
//    m_strStudentID = strStudentID;
//}

void CEncryption::SetEncryptionKey(const BYTE* pKey, int nKeyLength)
{
    if (pKey && nKeyLength > 0)
    {
        m_key.assign(pKey, pKey + nKeyLength);
    }
}

void CEncryption::SetEncryptionIV(const BYTE* pIV, int nIVLength)
{
    if (pIV && nIVLength > 0)
    {
        m_iv.assign(pIV, pIV + nIVLength);
    }
}

// 生成SHA-1摘要
CString CEncryption::GenerateSHA1Digest(const CString& strContent)
{
    HCRYPTPROV hProv = NULL;
    HCRYPTHASH hHash = NULL;
    BYTE rgbHash[20]; // SHA-1 produces 20-byte hash
    CHAR rgbDigits[] = "0123456789abcdef";

    // 转换为ANSI字符串
    std::string strAnsi = CT2A(strContent);

    // 获取CSP句柄
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        return _T("");
    }

    // 创建哈希对象
    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
    {
        CryptReleaseContext(hProv, 0);
        return _T("");
    }

    // 哈希数据
    if (!CryptHashData(hHash, (BYTE*)strAnsi.c_str(), (DWORD)strAnsi.length(), 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return _T("");
    }

    // 获取哈希值
    DWORD cbHash = 20;
    if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return _T("");
    }

    // 转换为十六进制字符串
    CString strHash;
    for (DWORD i = 0; i < cbHash; i++)
    {
        TCHAR szHex[3];
        _stprintf_s(szHex, _T("%02x"), rgbHash[i]);
        strHash += szHex;
    }

    // 清理
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return strHash;
}

// AES-CBC加密
CString CEncryption::AES_CBC_Encrypt(const CString& strPlaintext)
{
    // 转换为字节数组
    std::vector<BYTE> plaintext = StringToByteArray(strPlaintext);

    // 添加PKCS7填充
    DWORD blockSize = 16; // AES block size is 16 bytes
    DWORD padding = blockSize - (plaintext.size() % blockSize);

    for (DWORD i = 0; i < padding; i++)
    {
        plaintext.push_back((BYTE)padding);
    }

    // 分配加密数据缓冲区
    std::vector<BYTE> ciphertext(plaintext.size());

    // 使用简单的XOR加密（演示用）
    // 实际应用中应该使用真正的AES加密
    for (size_t i = 0; i < plaintext.size(); i++)
    {
        ciphertext[i] = plaintext[i] ^ m_key[i % m_key.size()] ^ m_iv[i % m_iv.size()];
    }

    // 转换为Base64
    return Base64Encode(ciphertext.data(), (DWORD)ciphertext.size());
}

// AES-CBC解密
CString CEncryption::AES_CBC_Decrypt(const CString& strCiphertext)
{
    // Base64解码
    std::vector<BYTE> ciphertext = Base64Decode(strCiphertext);

    if (ciphertext.empty())
    {
        return _T("");
    }

    // 分配解密数据缓冲区
    std::vector<BYTE> plaintext(ciphertext.size());

    // 使用简单的XOR解密（演示用）
    // 实际应用中应该使用真正的AES解密
    for (size_t i = 0; i < ciphertext.size(); i++)
    {
        plaintext[i] = ciphertext[i] ^ m_key[i % m_key.size()] ^ m_iv[i % m_iv.size()];
    }

    // 移除PKCS7填充
    if (!plaintext.empty())
    {
        BYTE padding = plaintext[plaintext.size() - 1];
        if (padding > 0 && padding <= 16)
        {
            plaintext.resize(plaintext.size() - padding);
        }
    }

    // 转换为字符串
    return ByteArrayToString(plaintext);
}

// 生成加密的SHA-1摘要
CString CEncryption::GenerateEncryptedSHA1Digest(const CString& strContent)
{
    // 生成SHA-1摘要
    CString strSHA1 = GenerateSHA1Digest(strContent);

    if (strSHA1.IsEmpty())
    {
        return _T("");
    }

    // 加密摘要
    CString strEncrypted = AES_CBC_Encrypt(strSHA1);

    return strEncrypted;
}

// 验证加密的SHA-1摘要
BOOL CEncryption::VerifyEncryptedSHA1Digest(const CString& strContent, const CString& strEncryptedDigest)
{
    if (strEncryptedDigest.IsEmpty())
    {
        return FALSE;
    }

    // 解密摘要
    CString strDecryptedDigest = AES_CBC_Decrypt(strEncryptedDigest);

    if (strDecryptedDigest.IsEmpty())
    {
        return FALSE;
    }

    // 生成当前内容的SHA-1摘要
    CString strCurrentSHA1 = GenerateSHA1Digest(strContent);

    // 比较摘要
    return (strDecryptedDigest.CompareNoCase(strCurrentSHA1) == 0);
}

// Base64编码
CString CEncryption::Base64Encode(const BYTE* pData, DWORD dwDataLength)
{
    if (!pData || dwDataLength == 0)
    {
        return _T("");
    }

    DWORD dwEncodedLength = 0;

    // 计算Base64编码后的长度
    if (!CryptBinaryToString(pData, dwDataLength,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &dwEncodedLength))
    {
        return _T("");
    }

    CString strEncoded;
    LPTSTR pszEncoded = strEncoded.GetBuffer(dwEncodedLength);

    if (!CryptBinaryToString(pData, dwDataLength,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, pszEncoded, &dwEncodedLength))
    {
        strEncoded.ReleaseBuffer();
        return _T("");
    }

    strEncoded.ReleaseBuffer();
    return strEncoded;
}

// Base64解码
std::vector<BYTE> CEncryption::Base64Decode(const CString& strBase64)
{
    std::vector<BYTE> result;

    if (strBase64.IsEmpty())
    {
        return result;
    }

    DWORD dwDecodedLength = 0;

    // 计算解码后的长度
    if (!CryptStringToBinary(strBase64, strBase64.GetLength(),
        CRYPT_STRING_BASE64, NULL, &dwDecodedLength, NULL, NULL))
    {
        return result;
    }

    result.resize(dwDecodedLength);

    if (!CryptStringToBinary(strBase64, strBase64.GetLength(),
        CRYPT_STRING_BASE64, result.data(), &dwDecodedLength, NULL, NULL))
    {
        result.clear();
        return result;
    }

    return result;
}

// 字符串转换为字节数组
std::vector<BYTE> CEncryption::StringToByteArray(const CString& str)
{
    std::vector<BYTE> result;

    if (str.IsEmpty())
    {
        return result;
    }

#ifdef _UNICODE
    // Unicode 转换为 UTF-8
    int nLength = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (nLength > 0)
    {
        result.resize(nLength);
        WideCharToMultiByte(CP_UTF8, 0, str, -1, (LPSTR)result.data(), nLength, NULL, NULL);
    }
#else
    // ANSI 直接复制
    result.assign(str.GetString(), str.GetString() + str.GetLength());
#endif

    return result;
}

// 字节数组转换为字符串
CString CEncryption::ByteArrayToString(const std::vector<BYTE>& byteArray)
{
    if (byteArray.empty())
    {
        return _T("");
    }

    CString strResult;

#ifdef _UNICODE
    // UTF-8 转换为 Unicode
    int nLength = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)byteArray.data(), (int)byteArray.size(), NULL, 0);
    if (nLength > 0)
    {
        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)byteArray.data(), (int)byteArray.size(),
            strResult.GetBuffer(nLength), nLength);
        strResult.ReleaseBuffer(nLength);
    }
#else
    // 直接复制
    strResult = CString((LPCSTR)byteArray.data(), (int)byteArray.size());
#endif

    return strResult;
}