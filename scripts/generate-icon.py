#!/usr/bin/env python3
"""
Generate mpv macOS .icns icon from official PNG.

Downloads the 128x128 PNG from mpv's GitHub repo, upscales to 1024x1024
with LANCZOS resampling, then packs into .icns via sips + iconutil.

Requires: Python 3 + Pillow, macOS built-in tools (sips, iconutil).
"""

import subprocess
import tempfile
import urllib.request
from pathlib import Path

from PIL import Image

MPV_ICON_URL = (
    "https://raw.githubusercontent.com/mpv-player/mpv/master/"
    "etc/mpv-icon-8bit-128x128.png"
)


def download_png(dest: Path) -> None:
    print(f"  Downloading {MPV_ICON_URL}")
    urllib.request.urlretrieve(MPV_ICON_URL, dest)


def upscale(src: Path, dst: Path, size: int = 1024) -> None:
    print(f"  Upscaling {src.stat().st_size // 1024}KB PNG → {size}x{size}")
    img = Image.open(src).convert("RGBA")
    img = img.resize((size, size), Image.LANCZOS)
    img.save(dst)
    print(f"  → {dst.stat().st_size // 1024}KB")


def pack_icns(png_path: Path, icns_path: Path) -> None:
    print("  Packing .icns...")
    sizes = {
        "icon_16x16.png": 16,
        "icon_16x16@2x.png": 32,
        "icon_32x32.png": 32,
        "icon_32x32@2x.png": 64,
        "icon_128x128.png": 128,
        "icon_128x128@2x.png": 256,
        "icon_256x256.png": 256,
        "icon_256x256@2x.png": 512,
        "icon_512x512.png": 512,
        "icon_512x512@2x.png": 1024,
    }

    with tempfile.TemporaryDirectory() as tmpdir:
        iconset = Path(tmpdir) / "mpv.iconset"
        iconset.mkdir()

        for fname, size in sizes.items():
            out = iconset / fname
            subprocess.run(
                ["sips", "-z", str(size), str(size), str(png_path), "--out", str(out)],
                check=True, capture_output=True,
            )

        subprocess.run(
            ["iconutil", "-c", "icns", str(iconset), "-o", str(icns_path)],
            check=True,
        )


def main() -> None:
    output_dir = Path(__file__).resolve().parent.parent / "icon"
    output_dir.mkdir(exist_ok=True)

    png_128 = output_dir / "mpv-icon-128.png"
    png_1024 = output_dir / "mpv-icon-1024.png"
    icns_path = output_dir / "mpv.icns"

    print("Generating mpv icon from official PNG...")
    download_png(png_128)
    upscale(png_128, png_1024)
    pack_icns(png_1024, icns_path)
    print(f"  Done → {icns_path}  ({icns_path.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    main()
