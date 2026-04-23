# GraphicsExp

图形学实验工程（MFC 方向），按实验目录组织，每个实验尽量保持最小可交付实现。

## 项目结构

- `docs/`：实验题目与课程参考资料；
- `exp2/`：实验二（多面体观察投影），当前仅实现 21 组及以后对应内容；
- `exp6/`：实验六（曲面体真实感显示），当前仅实现 21 组及以后对应内容；
- `exp3/`：实验三相关目录（待补充/整理）；
- `build_mfc.bat`：统一构建脚本（可指定实验目录）；
- `.cursor/rules/`：本仓库的 Cursor 规则文件。

## 环境要求

- Windows 10/11；
- Visual Studio 2022（建议 Community）；
- 已安装 MFC 组件：`C++ MFC for latest v143 build tools (x86 & x64)`；
- 可用的 `VsDevCmd.bat`（默认脚本使用：
  `D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`）。

## 快速构建

在仓库根目录运行：

```bat
build_mfc.bat exp2
build_mfc.bat exp6
```

说明：

- 不传参时默认构建 `exp6`；
- 脚本会在对应目录生成 `expX.exe`；
- 若目录下不存在 `main.cpp`，脚本会报错退出。

## 已实现实验

### 实验2（`exp2`）

- 主题：多面体观察投影（线框）；
- 实现范围：仅 21 组及以后（八面体）；
- 关键点：`InitParameter` 观察变换、`PerProject` 透视投影、`DrawObject` 线框绘制。

### 实验6（`exp6`）

- 主题：曲面体真实感显示（建模与消隐）；
- 实现范围：仅 21 组及以后（圆环体 + 绿宝石材质）；
- 关键点：参数曲面离散、光照材质设置、深度测试与背面剔除。

## 开发约定

- 默认使用 MFC + OpenGL，不引入 GLUT/Qt 等替代框架；
- 按题目要求优先实现最小可交付版本，避免无关扩展；
- 每次实质修改后优先通过 `build_mfc.bat` 本地验证。
