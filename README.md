# mpv-macWrapper

> Wrap your Homebrew-installed mpv as a native macOS `.app` — double-click to launch, pin to Dock, right-click files to open.

<p align="center">
  <a href="README.zh.md">中文</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="license">
</p>

## Why?

Homebrew's mpv is a command-line binary. It can't:
- Launch from Finder with a double-click
- Stay in the Dock
- Appear in "Open With" menus
- Show up in Launchpad

This project provides a pre-built, ready-to-use `.app` bundle (~1.2 MB) that solves all of the above.

## Quick Start

Download `mpv.dmg` from the [latest release](https://github.com/IstPlayer/mpv-macWrapper/releases/latest),
open it, and drag `mpv.app` into `/Applications/`.

> **Planned:** Homebrew Cask (`brew install --cask mpv-macWrapper`). See [TODO.md](TODO.md).

Or build from source:

```bash
git clone https://github.com/IstPlayer/mpv-macWrapper.git
cd mpv-macWrapper
make dmg
```

> **macOS Gatekeeper:** the app is unsigned. Right-click → **Open** the first time you launch it.

## Usage

| Action | Command / Gesture |
|---|---|
| Launch (idle window) | Double-click `mpv.app` |
| Open a file | `open -a mpv video.mkv` |
| Drag & drop | Drop a video onto the mpv window or Dock icon |
| Finder right-click | Right-click a video → Open With → mpv |

## How It Works

```
mpv.app/
└── Contents/
    ├── MacOS/mpv          ← native arm64 launcher (C, ~33 KB)
    ├── Resources/mpv.icns ← pre-generated app icon
    └── Info.plist         ← metadata + 20 format associations
```

The launcher locates `mpv` at runtime by checking, in order:

1. `$MPV_PATH` environment variable
2. `/opt/homebrew/bin/mpv` (Apple Silicon Homebrew)
3. `/usr/local/bin/mpv` (Intel Homebrew)
4. `/opt/local/bin/mpv` (MacPorts)

- **With arguments** → passes them through to mpv.
- **No arguments** → launches `mpv --idle=yes --force-window=yes` (blank window, ready for drag & drop).

**mpv updates require zero maintenance.** The launcher always resolves to the current binary on `$PATH`.

## Configuration

All mpv behaviour is controlled by your `~/.config/mpv/`:

- `mpv.conf` — main configuration
- `scripts/` — Lua scripts (ModernZ, thumbfast, playlistmanager, …)
- `script-opts/` — script options

The launcher itself has zero configuration.

## Development

The `.app` is committed pre-built. To rebuild (e.g. after modifying the launcher):

```bash
make          # Build mpv.app
make test     # Build + smoke tests
make dmg      # Build + create .dmg disk image
make clean    # Remove build artifacts
make icon     # Regenerate the icon from upstream
```

### Custom mpv path

```bash
MPV_PATH=/opt/custom/bin/mpv make
```

## FAQ

### Do I need to reinstall after `brew upgrade mpv`?

**No.** The launcher resolves `mpv` at runtime — a Homebrew upgrade is picked up automatically.

### thumbfast spams "client script write failed"?

Make sure `~/.config/mpv/script-opts/thumbfast.conf` uses absolute paths (`~` is not expanded by Lua):

```ini
socket=/Users/yourname/.cache/mpv/thumbnailSocket
thumbnail=/Users/yourname/.cache/mpv/thumbnail
mpv_path=/opt/homebrew/bin/mpv
```

### The icon hasn't updated?

```bash
killall Dock
```

## License

MIT © 2026 IstPlayer
