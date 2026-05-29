#!/bin/bash
# mpv-macWrapper — source-based installer (no Homebrew required)
# Usage: /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/IstPlayer/mpv-macWrapper/main/scripts/install.sh)"
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

REPO="https://github.com/IstPlayer/mpv-macWrapper.git"
DIR="mpv-macWrapper"

echo -e "${GREEN}→${NC} mpv-macWrapper installer (source)"

# ── macOS version check ──────────────────────────────────

if [[ "$(uname)" != "Darwin" ]]; then
    echo -e "${RED}✗${NC} macOS only."
    exit 1
fi

MAJOR=$(sw_vers -productVersion | cut -d. -f1)
if [[ "$MAJOR" -lt 12 ]]; then
    echo -e "${RED}✗${NC} macOS 12 (Monterey) or later required."
    exit 1
fi

# ── clang check ──────────────────────────────────────────

if ! command -v clang &>/dev/null; then
    echo -e "${YELLOW}!${NC} clang not found.  Install the Command Line Tools:"
    echo "  xcode-select --install"
    exit 1
fi

# ── mpv check ────────────────────────────────────────────

if ! command -v mpv &>/dev/null; then
    echo -e "${YELLOW}!${NC} mpv not found on PATH."
    echo "  Install it first (e.g. brew install mpv), then re-run this script."
    echo "  Or set MPV_PATH after installation to point to your mpv binary."
    exit 1
fi

# ── Clone or update ──────────────────────────────────────

if [[ -d "$DIR" ]]; then
    echo -e "${GREEN}→${NC} $DIR already exists.  Pulling latest..."
    cd "$DIR"
    git pull --ff-only
else
    echo -e "${GREEN}→${NC} Cloning mpv-macWrapper..."
    git clone "$REPO" "$DIR"
    cd "$DIR"
fi

# ── Build & install ──────────────────────────────────────

echo -e "${GREEN}→${NC} Building..."
make

echo -e "${GREEN}→${NC} Installing to /Applications/mpv.app..."
make install

# ── Done ─────────────────────────────────────────────────

echo ""
echo -e "${GREEN}✓${NC} mpv-macWrapper installed!"
echo "  App:  /Applications/mpv.app"
echo "  Source: $(pwd)"
echo ""
echo "  To update:  cd $(pwd) && git pull && make install"
