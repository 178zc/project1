# MFC多文档记事本编辑器

一个功能完整的MFC多文档记事本编辑器，支持文本编辑、正则搜索、主题切换、双格式文件等功能。

## 功能特性

### 📝 核心功能
- **多文档界面**：支持同时编辑多个文件
- **撤销/重做**：无限次撤销/重做操作
- **正则搜索替换**：支持正则表达式的高级搜索
- **行号显示**：左侧显示行号，随文本滚动

### 🎨 界面特性
- **主题切换**：支持亮色/暗色主题
- **自定义绘制**：不使用CEditView，完全自定义绘制
- **行号区域**：独立行号显示区域

### 📁 文件格式
- **纯文本(.txt)**：标准文本文件格式
- **自定义格式(.mynote)**：
  - 第一行：学号占位符
  - 正文：用户文本内容
  - 最后一行：密钥+IV+简单哈希摘要

## 系统要求

- Windows 7/8/10/11
- Visual Studio 2019或更高版本
- MFC库（包含在VS安装中）

## 编译运行

### 方法一：使用Visual Studio
1. 打开 `MFCApplication1.sln`
2. 选择 `x86` 或 `x64` 配置
3. 点击"生成"→"生成解决方案"
4. 按F5运行

### 方法二：命令行编译
```bash
# 使用MSBuild
msbuild MFCApplication1.sln /p:Configuration=Debug /p:Platform=Win32

# 或使用devenv
devenv MFCApplication1.sln /Build Debug
```
## 项目结构
```bash
MFCApplication1/
├── MFCApplication1.h/cpp          # 应用程序类
├── MainFrm.h/cpp                  # 主框架窗口
├── ChildFrm.h/cpp                 # 子框架窗口
├── MFCApplication1Doc.h/cpp       # 文档类（文件管理）
├── MFCApplication1View.h/cpp      # 视图类（编辑功能）
├── SearchReplaceDlg.h/cpp         # 搜索替换对话框
├── SetStudentIDDlg.h/cpp          # 学号设置对话框
├── Encryption.h/cpp               # 加密功能类
├── InputDlg.h/cpp                 # 输入对话框
└── Resource/                      # 资源文件
```
## 使用方法
### 基本编辑
	新建文件：文件 → 新建 或 Ctrl+N
	打开文件：文件 → 打开 或 Ctrl+O
	保存文件：文件 → 保存 或 Ctrl+S
	另存为：文件 → 另存为 或 Ctrl+Shift+S
### 编辑操作
	撤销：编辑 → 撤销 或 Ctrl+Z
	重做：编辑 → 重做 或 Ctrl+Y
	复制/剪切/粘贴：标准快捷键
	全选：编辑 → 全选 或 Ctrl+A
### 搜索替换
	查找：编辑 → 查找 或 Ctrl+F
	替换：编辑 → 替换 或 Ctrl+H
	正则表达式：勾选"使用正则表达式"
	区分大小写：勾选"区分大小写"
### 主题切换
	亮色主题：视图 → 亮色主题
	暗色主题：视图 → 暗色主题
### 学号设置
	设置学号：工具 → 设置学号
	学号格式：自动添加尖括号（如<123456789>）
### 文件格式说明
纯文本格式 (.txt)
```bash
这是纯文本内容
直接保存用户输入的文本
```
MyNote格式 (.mynote)
```bash
<123456789>                    # 学号占位符
这是MyNote格式的内容          # 用户文本
<SECRET_KEY><IV><HASH:...>    # 密钥+IV+哈希摘要
```
## 技术架构
### 三层架构
表示层：CMainFrame, CChildFrame, 对话框

业务逻辑层：CMFCApplication1View, CEncryption

数据层：CMFCApplication1Doc, CFile, CArchive

## 设计模式
文档/视图模式：MFC标准架构

RAII模式：资源自动管理

观察者模式：视图更新机制

## 测试
项目包含单元测试，使用GoogleTest框架
```bash
# 运行测试

ctest -C Debug

# 查看覆盖率报告

OpenCppCoverage --export_type=html --output coverage_report
```
测试覆盖率 ≥ 70%，覆盖：

文件I/O操作

主题切换功能

行号同步显示

搜索替换功能


## 开发指南
### 代码规范
使用匈牙利命名法

类名以C开头

成员变量以m_开头

常量全大写

### 添加新功能
在对应类中添加成员变量和方法

在消息映射中添加消息处理

更新资源文件（菜单/工具栏）

添加单元测试

## 常见问题
Q: 编译时出现链接错误

A: 确保安装了MFC组件，在项目属性中设置"使用MFC"为"在共享DLL中使用MFC"

Q: 正则表达式搜索不工作

A: 检查正则表达式语法，确保勾选"使用正则表达式"选项

Q: 主题切换后界面不更新

A: 可能需要手动调整窗口大小或调用Invalidate()强制重绘

Q: 大文件打开慢

A: 本项目使用简单文本处理，对于超大文件建议使用更高效的数据结构
## 许可证
本项目仅供学习和参考使用。

注意：本项目为教育用途，加密功能仅为演示，不适用于生产环境。

