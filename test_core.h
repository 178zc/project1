// test_core.h - 纯逻辑测试头文件，不依赖 MFC
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <regex>

// 使用标准 C++ 类型，不依赖 MFC
namespace CoreFunctions {
    // 哈希计算函数（从 MFC 代码移植）
    std::string CalculateSimpleHash(const std::string& strContent) {
        if (strContent.empty()) {
            return "00000000";
        }

        // 使用类似 FNV-1a 的简单哈希算法
        uint32_t hash = 2166136261U;  // FNV 偏移基础值
        const uint32_t prime = 16777619U;  // FNV 质数

        // 遍历字符串的每个字符
        for (char ch : strContent) {
            // 将字符转换为其值
            uint32_t charValue = static_cast<uint32_t>(static_cast<unsigned char>(ch));

            // 使用 FNV-1a 算法
            hash ^= charValue;      // 1. 异或当前字符
            hash *= prime;          // 2. 乘以质数
        }

        // 返回 8 位十六进制字符串
        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%08X", hash);
        return std::string(buffer);
    }

    // 整数转十六进制字符串
    std::string IntToHexString(uint32_t nValue) {
        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%08X", nValue);
        return std::string(buffer);
    }

    // 计算总行数
    int CountLines(const std::string& text) {
        if (text.empty()) {
            return 1;  // 空文本也算一行
        }

        int count = 1;
        size_t pos = 0;

        while ((pos = text.find('\n', pos)) != std::string::npos) {
            count++;
            pos++;
        }

        return count;
    }

    // 计算最大行号宽度（以字符数为单位）
    int CalculateLineNumberWidth(int totalLines) {
        if (totalLines <= 0) return 20;  // 默认宽度

        // 计算行号需要的字符数
        int digits = 1;
        int n = totalLines;
        while (n >= 10) {
            digits++;
            n /= 10;
        }

        // 加上边距
        return digits * 10 + 20;
    }

    // 解析 .mynote 格式
    struct MyNoteData {
        std::string studentID;
        std::string content;
        std::string hash;
        std::string secretKey;
        std::string iv;
    };

    MyNoteData ParseMyNoteFormat(const std::string& data) {
        MyNoteData result;

        if (data.empty()) {
            return result;
        }

        // 查找第一行（学号）
        size_t firstNewline = data.find('\n');
        if (firstNewline != std::string::npos) {
            result.studentID = data.substr(0, firstNewline);

            // 查找最后一行（密钥+IV+哈希）
            size_t lastNewline = data.rfind('\n');
            if (lastNewline != std::string::npos && lastNewline > firstNewline) {
                // 正文在学号行和最后一行之间
                result.content = data.substr(firstNewline + 1, lastNewline - firstNewline - 1);

                // 最后一行包含密钥、IV 和哈希
                std::string lastLine = data.substr(lastNewline + 1);

                // 假设格式为：<SECRET_KEY><IV>哈希值
                size_t secretKeyPos = lastLine.find("<SECRET_KEY>");
                if (secretKeyPos != std::string::npos) {
                    result.secretKey = "<SECRET_KEY>";

                    size_t ivPos = lastLine.find("<IV>", secretKeyPos);
                    if (ivPos != std::string::npos) {
                        result.iv = "<IV>";
                        result.hash = lastLine.substr(ivPos + 4);  // 跳过 "<IV>"
                    }
                }
            }
            else {
                // 没有最后一行，整个剩余部分都是内容
                result.content = data.substr(firstNewline + 1);
            }
        }
        else {
            // 没有换行符，整个字符串作为内容
            result.content = data;
        }

        return result;
    }

    // 构建 .mynote 格式
    std::string BuildMyNoteFormat(const std::string& studentID,
        const std::string& content,
        const std::string& secretKey = "<SECRET_KEY>",
        const std::string& iv = "<IV>") {
        std::string hash = CalculateSimpleHash(content);

        std::string result = studentID + "\n" + content;

        // 如果内容不以换行符结尾，添加一个
        if (!content.empty() && content.back() != '\n') {
            result += "\n";
        }

        result += secretKey + iv + hash;
        return result;
    }

    // 简单的文本搜索
    std::vector<size_t> FindAllMatches(const std::string& text, const std::string& pattern, bool caseSensitive = true) {
        std::vector<size_t> results;

        if (pattern.empty() || text.empty()) {
            return results;
        }

        std::string searchText = text;
        std::string searchPattern = pattern;

        if (!caseSensitive) {
            // 转换为小写进行大小写不敏感搜索
            std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
            std::transform(searchPattern.begin(), searchPattern.end(), searchPattern.begin(), ::tolower);
        }

        size_t pos = 0;
        while ((pos = searchText.find(searchPattern, pos)) != std::string::npos) {
            results.push_back(pos);
            pos += searchPattern.length();
        }

        return results;
    }

    // 正则表达式搜索
    std::vector<size_t> RegexSearch(const std::string& text, const std::string& pattern, bool caseSensitive = true) {
        std::vector<size_t> results;

        try {
            std::regex::flag_type flags = std::regex::ECMAScript;
            if (!caseSensitive) {
                flags |= std::regex::icase;
            }

            std::regex reg(pattern, flags);

            auto words_begin = std::sregex_iterator(text.begin(), text.end(), reg);
            auto words_end = std::sregex_iterator();

            for (auto i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                results.push_back(match.position());
            }
        }
        catch (const std::regex_error&) {
            // 正则表达式语法错误
        }

        return results;
    }

    // 编辑历史管理
    class EditHistory {
    private:
        struct EditAction {
            std::string content;
            size_t cursorPos;

            EditAction(const std::string& c = "", size_t p = 0)
                : content(c), cursorPos(p) {
            }
        };

        std::vector<EditAction> undoStack;
        std::vector<EditAction> redoStack;
        std::string currentContent;
        size_t currentCursor;

    public:
        EditHistory() : currentCursor(0) {}

        void SaveState(const std::string& content, size_t cursor) {
            undoStack.push_back(EditAction(currentContent, currentCursor));
            redoStack.clear();
            currentContent = content;
            currentCursor = cursor;
        }

        bool CanUndo() const { return !undoStack.empty(); }
        bool CanRedo() const { return !redoStack.empty(); }

        bool Undo(std::string& content, size_t& cursor) {
            if (!CanUndo()) return false;

            redoStack.push_back(EditAction(currentContent, currentCursor));
            EditAction last = undoStack.back();
            content = last.content;
            cursor = last.cursorPos;
            undoStack.pop_back();

            return true;
        }

        bool Redo(std::string& content, size_t& cursor) {
            if (!CanRedo()) return false;

            undoStack.push_back(EditAction(currentContent, currentCursor));
            EditAction next = redoStack.back();
            content = next.content;
            cursor = next.cursorPos;
            redoStack.pop_back();

            return true;
        }
    };

    // 光标位置计算：将字符索引转换为行和列
    void CharIndexToLineColumn(const std::string& text, size_t charIndex,
        int& line, int& column) {
        line = 0;
        column = 0;

        size_t currentPos = 0;
        size_t prevLineStart = 0;

        // 遍历查找换行符
        while (currentPos < charIndex && currentPos < text.length()) {
            if (text[currentPos] == '\n') {
                line++;
                prevLineStart = currentPos + 1;
            }
            currentPos++;
        }

        // 计算列
        column = static_cast<int>(charIndex - prevLineStart);
    }

    // 光标位置计算：将行和列转换为字符索引
    size_t LineColumnToCharIndex(const std::string& text, int line, int column) {
        int currentLine = 0;
        size_t currentPos = 0;

        // 移动到指定行
        while (currentLine < line && currentPos < text.length()) {
            if (text[currentPos] == '\n') {
                currentLine++;
            }
            currentPos++;
        }

        // 如果在查找过程中到达文本末尾
        if (currentLine < line) {
            return text.length();
        }

        // 移动到指定列
        int lineLength = 0;
        while (currentPos < text.length() && text[currentPos] != '\n' && lineLength < column) {
            currentPos++;
            lineLength++;
        }

        // 如果列位置超过行长度，调整到行末
        if (lineLength < column) {
            // 已经到达行末，不需要调整
        }

        return currentPos;
    }

    // 主题颜色计算
    struct ThemeColors {
        struct Color {
            uint8_t r, g, b;

            Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
                : r(red), g(green), b(blue) {
            }

            bool operator==(const Color& other) const {
                return r == other.r && g == other.g && b == other.b;
            }
        };

        Color textColor;
        Color backgroundColor;
        Color lineNumberText;
        Color lineNumberBg;

        ThemeColors(bool darkTheme) {
            if (darkTheme) {
                // 暗色主题
                textColor = Color(255, 255, 255);           // 白色文字
                backgroundColor = Color(30, 30, 30);        // 深灰色背景
                lineNumberText = Color(180, 180, 180);      // 浅灰色行号文字
                lineNumberBg = Color(50, 50, 50);           // 深灰色行号背景
            }
            else {
                // 亮色主题
                textColor = Color(0, 0, 0);                 // 黑色文字
                backgroundColor = Color(255, 255, 255);     // 白色背景
                lineNumberText = Color(100, 100, 100);      // 深灰色行号文字
                lineNumberBg = Color(240, 240, 240);        // 浅灰色行号背景
            }
        }
    };
}