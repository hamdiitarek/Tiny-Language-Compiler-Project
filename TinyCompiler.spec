# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['guii.py'],
    pathex=[],
    binaries=[],
    datas=[('scanner.c', '.'), ('parser.c', '.'), ('token_strings.c', '.'), ('tokens.h', '.'), ('run.sh', '.'), ('input.tiny', '.'), ('Makefile', '.')],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='TinyCompiler',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='TinyCompiler',
)
app = BUNDLE(
    coll,
    name='TinyCompiler.app',
    icon=None,
    bundle_identifier=None,
)
