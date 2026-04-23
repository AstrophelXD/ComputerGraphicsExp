# 实验2：多面体观察投影

根据 `docs/图形实验二.docx` 的要求，本目录实现了一个最小 MFC 版本的多面体观察投影线框显示。

## 实现内容

- 在 `CView::OnDraw` 中设置映射模式并绘制坐标轴；
- 实现观察变换参数计算 `InitParameter()`；
- 实现透视投影函数 `PerProject()`；
- 实现线框绘制函数 `DrawObject()`；
- 仅实现 21 组及以后要求：八面体线框观察投影（固定实现，不提供组号输入）。

## 编译运行

推荐在 Visual Studio 安装 MFC 组件后，使用开发者命令行编译：

```bat
cd /d d:\GraphicsExp\exp2
cl /nologo /std:c++17 /EHsc /MD /D_AFXDLL /Fe:exp2.exe main.cpp /link /NOLOGO /SUBSYSTEM:WINDOWS
```

运行 `exp2.exe` 后即可看到坐标轴与八面体的观察投影视图。

## 已知限制

- 当前版本仅显示线框，不含消隐与光照；
- 仅保留 21 组及以后对应内容，不包含四面体/六面体分支。
