# mpv-mac-app

> 将 Homebrew 安装的 mpv 包装为 macOS 原生 `.app`，支持双击启动、Dock 驻留、文件关联。

<p align="center">
  <a href="README.md">English</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/mpv-v0.41.0-yellow" alt="mpv version">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="license">
</p>

## 为什么需要？

Homebrew 安装的 mpv 是命令行工具，无法：
- 在 Finder 中双击启动
- 拖到 Dock 固定
- 右键视频文件 → 打开方式 → mpv
- 从 Launchpad 启动

本项目生成约 130 KB 的 `.app` bundle，使用 mpv 官方图标，解决以上所有问题。

## 快速开始

```bash
git clone https://github.com/IstPlayer/mpv-mac-app.git
cd mpv-mac-app
make install
```

刷新图标缓存：

```bash
killall Dock
```

## 使用

| 操作 | 命令 / 手势 |
|---|---|
| 启动（空白窗口） | 双击 `/Applications/mpv.app` |
| 打开文件 | `open -a mpv video.mkv` |
| 拖放 | 将视频拖到 mpv 窗口或 Dock 图标 |
| Finder 右键 | 右键视频 → 打开方式 → mpv |

## 原理

```
mpv.app/
└── Contents/
    ├── MacOS/mpv          ← 原生 arm64 启动器 (C, ~33 KB)
    ├── Resources/mpv.icns ← 应用图标
    └── Info.plist         ← 元数据 + 20 种文件格式关联
```

启动器编译为原生 Mach-O 二进制，调用 `/opt/homebrew/bin/mpv`：

- **有参数** → 透传给 mpv
- **无参数** → `mpv --idle=yes --force-window=yes`（空白窗口等待拖放）

**mpv 更新后无需任何操作** —— 启动器始终指向 Homebrew 的现行版本。

## 构建

```bash
make          # 构建 mpv.app
make test     # 构建 + 冒烟测试
make clean    # 清理构建产物
```

## 依赖

| 依赖 | 用途 | 安装 |
|---|---|---|
| mpv | 播放器本体 | `brew install mpv` |
| clang | 编译启动器 | `xcode-select --install` |
| Python 3 + Pillow | 图标生成 | `pip install Pillow` |
| sips / iconutil | 图标打包 | macOS 内置 |

## 配置

mpv 配置和行为完全由你的 `~/.config/mpv/` 控制：

- `mpv.conf` — 主配置
- `scripts/` — Lua 脚本 (ModernZ, thumbfast, playlistmanager, …)
- `script-opts/` — 脚本配置

启动器本身零配置。

## FAQ

### mpv 更新后需要重装吗？

**不需要。** 启动器调用 `/opt/homebrew/bin/mpv`，`brew upgrade mpv` 后自动生效。

### thumbfast 持续报 "client script write failed"？

确认 `~/.config/mpv/script-opts/thumbfast.conf` 路径为绝对路径（`~` 不会被 Lua 展开）：

```ini
socket=/Users/你的用户名/.cache/mpv/thumbnailSocket
thumbnail=/Users/你的用户名/.cache/mpv/thumbnail
mpv_path=/opt/homebrew/bin/mpv
```

### 图标没有刷新？

```bash
killall Dock
```

### 如何卸载？

```bash
make uninstall
```

## License

MIT © 2026 IstPlayer
