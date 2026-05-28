# mpv-macWrapper

> Wrap your [Homebrew](https://brew.sh/) (or otherwise installed) **mpv CLI** as a native macOS `.app` — double-click to launch, pin to Dock, right-click files to open.

<p align="center">
  <a href="README.zh.md">中文</a>
  &nbsp;·&nbsp;
  <img src="https://img.shields.io/badge/platform-macOS%2012%2B-blue" alt="platform">
  <img src="https://img.shields.io/badge/version-1.0.0-yellow" alt="version">
  <img src="https://img.shields.io/badge/license-GPLv2%2B-green" alt="license">
</p>

## Why?

[Homebrew](https://brew.sh/) and other package managers install/compile **mpv** as a command-line binary. It can't:

- Launch from Finder with a double-click
- Stay in the Dock
- Appear in "Open With" menus
- Show up in Launchpad

This project builds a ~1.2 MB `.app` bundle that solves all of the above.

## Quick Start

```bash
git clone https://github.com/IstPlayer/mpv-macWrapper.git
cd mpv-macWrapper
make install
```

> **macOS Gatekeeper:** the app is unsigned. On first launch, macOS will block it with a
> _"cannot be opened because it is from an unidentified developer"_ dialog.
> **Right-click** (or Control-click) the app in Finder and choose **Open**, then confirm.
> You may also need to go to **System Settings → Privacy & Security** and scroll down to
> find the security prompt to allow it manually.
> Alternatively, `make install` strips the quarantine flag automatically, so Gatekeeper
> won't prompt on subsequent launches.

> **Tested on:** macOS Tahoe 26.5 (Apple Silicon).

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
2. `/opt/homebrew/bin/mpv` (Apple Silicon [Homebrew](https://brew.sh/))
3. `/usr/local/bin/mpv` (Intel [Homebrew](https://brew.sh/))
4. `/opt/local/bin/mpv` (MacPorts)

- **With arguments** → passes them through to mpv.
- **No arguments** → launches `mpv --idle=yes --force-window=yes` (blank window, ready for drag & drop).

**mpv updates require zero maintenance.** The launcher always resolves to the current
binary on your system.

## Configuration

All mpv behaviour is controlled by your `~/.config/mpv/`:

- `mpv.conf` — main configuration
- `scripts/` — Lua scripts (ModernZ, thumbfast, playlistmanager, …)
- `script-opts/` — script options

The launcher itself has zero configuration.

## Dependencies

| Dependency | Why | Install |
|---|---|---|
| mpv | The player itself | `brew install mpv` |
| clang | Compile the launcher | `xcode-select --install` |
| sips / iconutil | Icon packaging (pre-generated) | Built into macOS |

> Building the icon from scratch is optional (it's pre-generated). It requires Python 3
> + [Pillow](https://python-pillow.org/): `pip install Pillow`, then `make icon`.

## Development

The `.app` is pre-built and committed. To rebuild (e.g. after modifying the launcher):

```bash
make          # Build mpv.app
make test     # Build + smoke tests
make clean    # Remove build artifacts
make icon     # Regenerate the icon from upstream
```

### Custom mpv path

```bash
MPV_PATH=/opt/custom/bin/mpv make
```

## FAQ

### Do I need to reinstall after `brew upgrade mpv`?

**No.** The launcher automatically locates **mpv** at runtime following the
[lookup rules](#how-it-works).

### How do I uninstall?

```bash
make uninstall
```

## TODO

- [ ] Submit to [Homebrew core](https://github.com/Homebrew/homebrew-core) as a Formula.

## License

GPLv2+ © 2026 IstPlayer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

See [LICENSE](LICENSE) for the full text.
