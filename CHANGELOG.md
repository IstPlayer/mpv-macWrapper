# Changelog

## v1.1.7 (2026-05-29)

- Added `$PATH` search as an additional fallback for locating mpv—covers any
  non-standard installation path
- File picker dialog now shows symlinks (removed UTI type filter)
- CI actions upgraded to Node 24 native versions (checkout@v5, action-gh-release@v3)

## v1.1.5 (2026-05-29)

- Switched from Formula to Cask for Homebrew tap distribution—`.app` now
  auto-installs to `/Applications/` via `brew install --cask`
- Removed `sudo` from Makefile; `/Applications/` is admin-group-writable
- Simplified install to one-liner: `brew install --cask IstPlayer/tap/mpv-macwrapper`

## v1.1.0 (2026-05-28)

- Initial public release
- Custom Homebrew tap with automated DMG + Cask distribution
- Configurable mpv path via `$MPV_PATH` environment variable
- Manual file picker saves chosen path to `~/.config/mpv-macWrapper/path.conf`
- Gatekeeper-aware: `make install` strips quarantine flag
- Rebuilt icon pipeline using official PNG → sips upscale (zero extra deps)
- Demo screenshot added to README
