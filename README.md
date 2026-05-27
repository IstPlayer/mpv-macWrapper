# mpv-mac-app

> Wrap your Homebrew-installed mpv as a native macOS `.app` with double-click launch, Dock pinning, and file associations.

<p align="center">
  <a href="README.zh.md">中文</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/mpv-v0.41.0-yellow" alt="mpv version">
  <img src="https://img.shields.io/badge/license-MIT-green" alt="license">
</p>

## Why?

Homebrew's mpv is a command-line binary. It can't:
- Launch from Finder with a double-click
- Stay in the Dock
- Appear in "Open With" menus
- Show up in Launchpad

This project generates a ~130 KB `.app` bundle that solves all of the above.

## Quick Start

```bash
git clone https://github.com/IstPlayer/mpv-mac-app.git
cd mpv-mac-app
make install
```

Then refresh the icon cache:

```bash
killall Dock
```

## Usage

| Action | Command / Gesture |
|---|---|
| Launch (idle window) | Double-click `/Applications/mpv.app` |
| Open a file | `open -a mpv video.mkv` |
| Drag & drop | Drop a video onto the mpv window or Dock icon |
| Finder right-click | Right-click a video → Open With → mpv |

## How It Works

```
mpv.app/
└── Contents/
    ├── MacOS/mpv          ← native arm64 launcher (C, ~33 KB)
    ├── Resources/mpv.icns ← app icon
    └── Info.plist         ← metadata + 20 format associations
```

The launcher is a compiled Mach-O binary that calls `/opt/homebrew/bin/mpv`:

- **With arguments** → passes them straight through to mpv.
- **No arguments** → launches `mpv --idle=yes --force-window=yes` (blank window, ready for drag & drop).

**mpv updates require zero maintenance.** The launcher always points at the current Homebrew binary.

## Build

```bash
make          # Build mpv.app
make test     # Build + smoke tests
make clean    # Remove artifacts
```

## Dependencies

| Dependency | Why | Install |
|---|---|---|
| mpv | The player itself | `brew install mpv` |
| clang | Compile the launcher | `xcode-select --install` |
| Python 3 + Pillow | Icon generation | `pip install Pillow` |
| sips / iconutil | Icon packaging | Built into macOS |

## Configuration

All mpv behaviour is controlled by your `~/.config/mpv/`:

- `mpv.conf` — main configuration
- `scripts/` — Lua scripts (ModernZ, thumbfast, playlistmanager, …)
- `script-opts/` — script options

The launcher itself has zero configuration.

## FAQ

### Do I need to reinstall after `brew upgrade mpv`?

**No.** The launcher calls `/opt/homebrew/bin/mpv`, so a Homebrew upgrade is picked up automatically.

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

### How do I uninstall?

```bash
make uninstall
```

## License

MIT © 2026 IstPlayer
