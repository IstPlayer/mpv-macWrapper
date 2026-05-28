# mpv-macWrapper

> 将 Homebrew 安装的 mpv 包装为 macOS 原生 `.app` — 双击启动、Dock 驻留、右键文件打开。

<p align="center">
  <a href="README.md">English</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="license">
</p>

## 为什么需要？

Homebrew 安装的 mpv 是命令行工具，无法：
- 在 Finder 中双击启动
- 拖到 Dock 固定
- 右键视频文件 → 打开方式 → mpv
- 从 Launchpad 启动

本项目提供预构建、开箱即用的 `.app` bundle（约 1.2 MB），解决以上所有问题。

## 快速开始

从 [Releases](https://github.com/IstPlayer/mpv-macWrapper/releases/latest) 下载 `mpv.dmg`，
双击打开，将 `mpv.app` 拖入 `/Applications/`。

> **计划中：** Homebrew Cask（`brew install --cask mpv-macWrapper`）。见 [TODO.md](TODO.md)。

或从源码构建：

```bash
git clone https://github.com/IstPlayer/mpv-macWrapper.git
cd mpv-macWrapper
make         # 构建 mpv.app
make install # 或构建 + 安装到 /Applications/
```

> **macOS Gatekeeper：** 应用未签名，首次启动需**右键 → 打开**。

## 使用

| 操作 | 命令 / 手势 |
|---|---|
| 启动（空白窗口） | 双击 `mpv.app` |
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

启动器按以下顺序查找 `mpv`：

1. `$MPV_PATH` 环境变量
2. `/opt/homebrew/bin/mpv`（Apple Silicon Homebrew）
3. `/usr/local/bin/mpv`（Intel Homebrew）
4. `/opt/local/bin/mpv`（MacPorts）

- **有参数** → 透传给 mpv
- **无参数** → `mpv --idle=yes --force-window=yes`（空白窗口等待拖放）

**mpv 更新后无需任何操作** —— 启动器运行时自动定位当前 mpv 二进制。

## 配置

mpv 行为完全由 `~/.config/mpv/` 控制：

- `mpv.conf` — 主配置
- `scripts/` — Lua 脚本 (ModernZ, thumbfast, playlistmanager, …)
- `script-opts/` — 脚本配置

启动器本身零配置。

## 开发

`.app` 已预构建并提交。如需重新构建（例如修改启动器后）：

```bash
make          # 构建 mpv.app
make test     # 构建 + 冒烟测试
make dmg      # 构建 + 创建 .dmg 磁盘映像
make clean    # 清理构建产物
make icon     # 重新生成图标
```

### 自定义 mpv 路径

```bash
MPV_PATH=/opt/custom/bin/mpv make
```

## FAQ

### mpv 更新后需要重装吗？

**不需要。** 启动器运行时定位 mpv，`brew upgrade mpv` 后自动生效。

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

## License

MIT © 2026 IstPlayer
