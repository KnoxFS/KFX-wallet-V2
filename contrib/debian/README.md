
Debian
====================
This directory contains files used to package kfxd/kfx-qt
for Debian-based Linux systems. If you compile kfxd/kfx-qt yourself, there are some useful files here.

## kfx: URI support ##


kfx-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install kfx-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your kfxqt binary to `/usr/bin`
and the `../../share/pixmaps/kfx128.png` to `/usr/share/pixmaps`

kfx-qt.protocol (KDE)

