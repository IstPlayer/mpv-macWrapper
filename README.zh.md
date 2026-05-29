# mpv-macWrapper

> 将 [Homebrew](https://brew.sh/) 或其他工具安装的 **mpv CLI** 包装为 macOS 原生 `.app` — 双击启动、Dock 驻留、右键文件打开。

<p align="center">
  <a href="README.md">English</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/version-1.1.0-yellow" alt="version">
  <img src="https://img.shields.io/badge/license-GPLv2%2B-green" alt="license">
</p>

## 为什么需要？

[Homebrew](https://brew.sh/) 或其他工具安装/编译的 **mpv** 是命令行工具，无法:
- 在 Finder 中双击启动
- 拖到 Dock 固定
- 右键视频文件 → 打开方式 → mpv
- 从 Launchpad 启动

本项目构建约 1.2 MB 的 `.app` bundle（wrapper v1.0.0，mpv v0.41.0 测试通过），解决以上所有问题。

## 快速开始

```bash
brew install --cask IstPlayer/tap/mpv-macwrapper
```

或下载[最新 DMG](https://github.com/IstPlayer/mpv-macWrapper/releases/latest)，
双击打开，将 `mpv.app` 拖入 `/Applications/`。

> **macOS Gatekeeper: ** 应用未签名，首次启动时 macOS 会弹出
> _"无法打开，因为无法验证开发者"_ 的提示。
> 在 Finder 中**右键** (或 Control-点击) mpv.app → **打开**，确认即可。
> 也可能需要在 系统设置 → 隐私与安全性 → 下滑找到安全性提示后手动打开。
> 也可以直接使用 `make install`，它会自动移除隔离标记，后续启动不再提示。

> **测试于：** macOS Tahoe 26.5 (Apple Silicon)。

## 使用

| 操作 | 命令 / 手势 |
|---|---|
| 启动 (空白窗口)  | 双击 `mpv.app` |
| 打开文件 | `open -a mpv video.mkv` |
| 拖放 | 将视频拖到 mpv 窗口或 Dock 图标 |
| Finder 右键 | 右键视频 → 打开方式 → mpv |

## 原理

```
mpv.app/
└── Contents/
    ├── MacOS/mpv          ← 原生 arm64 启动器 (C, ~33 KB)
    ├── Resources/mpv.icns ← 预生成的官方图标
    └── Info.plist         ← 元数据 + 20 种文件格式关联
```

启动器按以下顺序查找 `mpv`:

1. `$MPV_PATH` 环境变量
2. `/opt/homebrew/bin/mpv` (Apple Silicon [Homebrew](https://brew.sh/))
3. `/usr/local/bin/mpv` (Intel [Homebrew](https://brew.sh/))
4. `/opt/local/bin/mpv` (MacPorts)

- **有参数** → 透传给 mpv
- **无参数** → `mpv --idle=yes --force-window=yes` (空白窗口等待拖放)

**mpv 更新后无需任何操作** —— 启动器运行时自动定位当前 mpv 二进制。

## 配置

mpv 行为完全由 `~/.config/mpv/` 控制:

- `mpv.conf` — 主配置
- `scripts/` — 脚本 (ModernZ、thumbfast、playlistmanager …)
- `script-opts/` — 脚本配置

启动器本身零配置。

## 依赖

| 依赖 | 用途 | 安装 |
|---|---|---|
| mpv | 播放器本体 | `brew install mpv` |
| clang | 编译启动器 | `xcode-select --install` |
| sips / iconutil | 图标打包 (已预生成)  | macOS 内置 |

> 如需重新生成图标 (可选，已预生成) ，额外需要 Python 3 + [Pillow](https://python-pillow.org/):
> `pip install Pillow`，然后 `make icon`。

## 开发

`.app` 已预构建并提交。如需重新构建 (例如修改启动器后) :

```bash
make          # 构建 mpv.app
make test     # 构建 + 冒烟测试
make clean    # 清理构建产物
make icon     # 重新生成图标
```

### 自定义 mpv 路径

```bash
MPV_PATH=/opt/custom/bin/mpv make
```

## FAQ

### mpv 更新后需要重装吗？

**不需要。** 启动器运行时自动根据[规则](README.zh#原理)定位 **mpv CLI**

### 如何卸载？

```bash
brew uninstall --cask mpv-macwrapper
brew untap IstPlayer/tap      # 可选：移除 tap
```

若从源码安装：`make uninstall`。

## License

GPLv2+ © 2026 IstPlayer。

本程序为自由软件;您可依据自由软件基金会发布的 GNU 通用公共许可证条款
对其再分发和/或修改;适用许可证第 2 版，或 (由您选择) 任何更新版本。

详见 [LICENSE](LICENSE)。
