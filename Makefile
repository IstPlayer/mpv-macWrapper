# mpv-macWrapper v1.0.0 — macOS .app bundle for Homebrew mpv
# ===============================================================

APP_NAME      := mpv
BUNDLE        := $(APP_NAME).app
INSTALL_DIR   := /Applications

CC            := clang
CFLAGS        := -O2 -mmacosx-version-min=12.0

# Detect mpv path for test / display (use MPV_PATH override if set)
MPV_BIN       := $(or $(MPV_PATH),$(shell command -v mpv 2>/dev/null || echo mpv))
VERSION       := 1.0.0
MPV_VER       := $(shell $(MPV_BIN) --version 2>/dev/null | head -1 | awk '{print $$2}' || echo unknown)

.PHONY: all build install uninstall clean icon test help

all: build

# ── Build the .app bundle ────────────────────────────────

build: $(BUNDLE)

$(BUNDLE): src/launcher.c src/Info.plist icon/mpv.icns
	@echo "🔨 Building $(BUNDLE) v$(VERSION)  (mpv $(MPV_VER))..."
	@rm -rf $(BUNDLE)
	@mkdir -p $(BUNDLE)/Contents/MacOS
	@mkdir -p $(BUNDLE)/Contents/Resources
	$(CC) $(CFLAGS) -o $(BUNDLE)/Contents/MacOS/$(APP_NAME) src/launcher.c
	strip $(BUNDLE)/Contents/MacOS/$(APP_NAME)
	cp icon/mpv.icns $(BUNDLE)/Contents/Resources/
	sed 's/__VERSION__/$(VERSION)/' src/Info.plist > $(BUNDLE)/Contents/Info.plist
	plutil -lint $(BUNDLE)/Contents/Info.plist
	@echo "✅ $(BUNDLE) built (v$(VERSION))"

# ── Icon (pre-generated; regenerate only on demand) ─────

icon/mpv.icns:
	@echo "🎨 Regenerating icon..."
	python3 scripts/generate-icon.py

.PHONY: icon
icon: icon/mpv.icns

# ── Install / uninstall ─────────────────────────────────

install: build
	@echo "📦 Installing to $(INSTALL_DIR)/$(BUNDLE)..."
	sudo rm -rf "$(INSTALL_DIR)/$(BUNDLE)"
	sudo cp -R $(BUNDLE) "$(INSTALL_DIR)/"
	sudo xattr -dr com.apple.quarantine "$(INSTALL_DIR)/$(BUNDLE)" 2>/dev/null || true
	@echo "✅ Installed. Run: killall Dock   (to refresh icon cache)"

uninstall:
	@echo "🗑  Removing $(INSTALL_DIR)/$(BUNDLE)..."
	sudo rm -rf "$(INSTALL_DIR)/$(BUNDLE)"
	@echo "✅ Uninstalled."

# ── Test ────────────────────────────────────────────────

test: build
	@echo "🧪 Testing..."
	@printf "  Launcher binary: "
	@./$(BUNDLE)/Contents/MacOS/$(APP_NAME) --version | head -1
	@printf "  open -a (no args): "
	@open ./$(BUNDLE) 2>/dev/null && sleep 2; \
		pgrep -q $(APP_NAME) && echo "OK" && pkill $(APP_NAME) || echo "FAIL"
	@printf "  open -a (with file): "
	@touch /tmp/_mpv_test.mkv
	@open -a $(APP_NAME) /tmp/_mpv_test.mkv 2>/dev/null && sleep 2; \
		pgrep -q $(APP_NAME) && echo "OK" && pkill $(APP_NAME) || echo "FAIL"
	@rm -f /tmp/_mpv_test.mkv

# ── Release ─────────────────────────────────────────────

DMG_FILE := $(APP_NAME)-$(VERSION).dmg
DMG_ROOT := /tmp/$(APP_NAME)-dmg

.PHONY: release
release: build
	@echo "📀 Creating $(DMG_FILE)..."
	@rm -rf $(DMG_ROOT) $(DMG_FILE)
	@mkdir -p $(DMG_ROOT)
	@cp -R $(BUNDLE) $(DMG_ROOT)/
	@ln -s /Applications $(DMG_ROOT)/Applications
	@hdiutil create -volname "$(APP_NAME) v$(VERSION)" \
		-srcfolder $(DMG_ROOT) \
		-format UDZO \
		-fs HFS+ \
		$(DMG_FILE) >/dev/null 2>&1
	@rm -rf $(DMG_ROOT)
	@echo "✅ $(DMG_FILE)  ($$(du -h $(DMG_FILE) | cut -f1))"

# ── Clean ────────────────────────────────────────────────

clean:
	rm -rf $(BUNDLE)
	rm -f $(APP_NAME)-*.dmg
	rm -rf icon/mpv-icon-1024.png icon/mpv-icon-2048.png icon/mpv.svg

# ── Help ────────────────────────────────────────────────

help:
	@echo "mpv-macWrapper v$(VERSION) — macOS .app bundle for Homebrew mpv"
	@echo ""
	@echo "Targets:"
	@echo "  make          Build the .app bundle"
	@echo "  make release  Build + create .dmg disk image"
	@echo "  make install  Install to /Applications/"
	@echo "  make uninstall  Remove from /Applications/"
	@echo "  make test     Build and run smoke tests"
	@echo "  make clean    Remove build artifacts"
	@echo "  make icon     Regenerate icon from upstream PNG"
	@echo "  make help     Show this message"
	@echo ""
	@echo "Environment:"
	@echo "  MPV_PATH   Override path to mpv binary (default: auto-detect)"
