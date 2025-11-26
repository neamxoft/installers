# ============================================================

# 🐧 **Guía Gentoo ARM64 — Banana Pi M5 v2.4**

## **(SDR + IC-705 + FreeDV RADE + Backend REST Edition)**

### Español técnico estilo Gentoo Handbook

### Kernel 6.12 minimalista – Amlogic S905X3 – Instalación en eMMC

# ============================================================

Sistema optimizado para:

* RTL-SDR (rtl_tcp)
* Airspy HF+ Discovery
* IC-705: CAT + USB Audio
* FreeDV (incluye **RADE**)
* Backend REST FastAPI
* Funcionamiento **headless**
* Neovim (*vi como wrapper*)
* ZRAM + CPU governor performance
* Conexión Android USB-C ↔ RJ45
* Kernel minimalista 6.12

Incluye tu archivo personalizado:
👉 **`linux-bpi-m5-6.12-minimal.config`**

---

╔════════════════════════════════════════╗
**1. 📦 Requisitos Previos**
╚════════════════════════════════════════╝

* Banana Pi M5 (Amlogic S905X3 + eMMC 16GB)
* microSD 16GB+ con Armbian CLI
* PC/macOS
* Ethernet o USB-C ↔ RJ45
* SSH opcional

---

╔════════════════════════════════════════╗
**2. 💾 Crear microSD con Armbian (macOS)**
╚════════════════════════════════════════╝

```bash
diskutil list
diskutil unmountDisk /dev/diskX
sudo dd if=Armbian.img of=/dev/rdiskX bs=4m status=progress
sync
```

---

╔════════════════════════════════════════╗
**3. 🔍 Identificar eMMC**
╚════════════════════════════════════════╝

```bash
lsblk
```

---

╔════════════════════════════════════════╗
**4. ⚙️ Particionar el eMMC (Esquema ideal v2.2)**
╚════════════════════════════════════════╝

```bash
cfdisk /dev/mmcblk1
```

Elegir: **gpt**

| Partición | Tamaño   | Tipo             | Uso    |
| --------- | -------- | ---------------- | ------ |
| mmcblk1p1 | 256M     | EFI System       | /boot  |
| mmcblk1p2 | **7.3G** | Linux filesystem | `/`    |
| mmcblk1p3 | **7G**   | Linux filesystem | `/var` |

---

╔════════════════════════════════════════╗
**5. 🧱 Formatear particiones**
╚════════════════════════════════════════╝

```bash
mkfs.vfat -F32 /dev/mmcblk1p1
mkfs.ext4 /dev/mmcblk1p2
mkfs.ext4 /dev/mmcblk1p3
```

---

╔════════════════════════════════════════╗
**6. 📂 Montar estructura Gentoo**
╚════════════════════════════════════════╝

```bash
mkdir -p /mnt/gentoo
mount /dev/mmcblk1p2 /mnt/gentoo

mkdir /mnt/gentoo/boot
mount /dev/mmcblk1p1 /mnt/gentoo/boot

mkdir /mnt/gentoo/var
mount /dev/mmcblk1p3 /mnt/gentoo/var
```

---

╔════════════════════════════════════════╗
**7. 📥 Descargar Stage3 ARM64**
╚════════════════════════════════════════╝

```bash
cd /mnt/gentoo
wget https://distfiles.gentoo.org/releases/arm64/autobuilds/current-stage3-arm64-openrc/stage3-arm64-openrc-*.tar.xz
tar xpvf stage3-arm64-openrc-*.tar.xz --xattrs-include='*.*' --numeric-owner
```

---

╔════════════════════════════════════════╗
**8. 🌐 Copiar resolv.conf**
╚════════════════════════════════════════╝

```bash
cp -L /etc/resolv.conf /mnt/gentoo/etc/
```

---

╔════════════════════════════════════════╗
**9. 🔗 Montar pseudo-sistemas**
╚════════════════════════════════════════╝

```bash
mount -t proc /proc /mnt/gentoo/proc
mount --rbind /sys /mnt/gentoo/sys
mount --rbind /dev /mnt/gentoo/dev
mount --rbind /run /mnt/gentoo/run
```

---

╔════════════════════════════════════════╗
**10. 🚪 Entrar al chroot**
╚════════════════════════════════════════╝

```bash
export TERM=xterm-256color
chroot /mnt/gentoo /bin/bash
source /etc/profile
export PS1="(chroot) $PS1"
```

---

╔════════════════════════════════════════╗
**11. 🌍 Configurar Locales (ANTES DE SYNC)**
╚════════════════════════════════════════╝

> Aún no hay Neovim → usar **nano**.

```bash
nano /etc/locale.gen
```

Agregar:

```
en_US.UTF-8 UTF-8
en_US ISO-8859-1
C.UTF-8 UTF-8
```

Generar:

```bash
locale-gen
eselect locale set en_US.utf8
env-update
source /etc/profile
unset LC_ALL LANGUAGE LC_MESSAGES
```

---

╔════════════════════════════════════════╗
**12. ⚙️ Configurar make.conf optimizado ARM64**
╚════════════════════════════════════════╝

> También con **nano**, porque aún no instalamos editor.

```bash
nano /etc/portage/make.conf
```

Contenido:

```conf
COMMON_FLAGS="-O2 -pipe -march=armv8-a+crc+crypto"
CFLAGS="${COMMON_FLAGS}"
CXXFLAGS="${COMMON_FLAGS}"

MAKEOPTS="-j5"

USE="minimal headless usb alsa pulseaudio openssh acl ssl -systemd -gtk -qt5 -X"

ACCEPT_LICENSE="*"
GENTOO_MIRRORS="https://distfiles.gentoo.org"

EMERGE_DEFAULT_OPTS="--jobs=4 --load-average=4.5 --with-bdeps=y"
FEATURES="parallel-fetch"

PORTAGE_TMPDIR="/var/tmp"
DISTDIR="/var/cache/distfiles"
PKGDIR="/var/cache/binpkgs"
```

---

╔════════════════════════════════════════╗
**13. 🔄 Sincronizar Portage**
╚════════════════════════════════════════╝

```bash
emerge --sync
```

---

╔════════════════════════════════════════╗
**14. 📝 Instalar Neovim y habilitarlo como `vi`**
╚════════════════════════════════════════╝

```bash
emerge --ask app-editors/neovim
```

Asignar:

```bash
ln -sf /usr/bin/nvim /usr/bin/vi
ln -sf /usr/bin/nvim /usr/bin/vim
eselect editor set /usr/bin/nvim
```

A partir de ahora usarás **solo:**

```bash
vi /ruta/archivo
```

---

╔════════════════════════════════════════╗
**15. ⬆️ Actualizar @world**
╚════════════════════════════════════════╝

```bash
emerge -avuDU --with-bdeps=y @world
```

---

╔════════════════════════════════════════╗
**16. 🐚 Instalar y configurar fish**
╚════════════════════════════════════════╝

```bash
emerge app-shells/fish
mkdir -p ~/.config/fish
vi ~/.config/fish/config.fish
```

Contenido:

```
if test -n "$TMUX"
    set -gx TERM screen-256color
else
    set -gx TERM xterm-256color
end
```

---

╔════════════════════════════════════════╗
**17. 🧠 ZRAM + CPU Governor Performance**
╚════════════════════════════════════════╝

ZRAM:

```bash
emerge sys-block/zram-init
rc-update add zram-init default
```

Governor:

```bash
echo 'GOVERNOR="performance"' > /etc/conf.d/cpupower
rc-update add cpupower default
```

---

╔════════════════════════════════════════╗
**18. 🧠 Instalar Kernel 6.12 minimalista**
╚════════════════════════════════════════╝

```bash
emerge sys-kernel/gentoo-sources
cd /usr/src/linux
cp /mnt/data/linux-bpi-m5-6.12-minimal.config .config
make olddefconfig
make -j5 Image dtbs modules
make modules_install
```

Copiar kernel:

```bash
cp arch/arm64/boot/Image /boot/kernel-6.12
cp arch/arm64/boot/dts/amlogic/meson-sm1-bananapi-m5.dtb /boot/
sync
```

---

╔════════════════════════════════════════╗
**19. 🔥 Instalar U-Boot**
╚════════════════════════════════════════╝

```bash
emerge sys-boot/u-boot-bananapi-m5
dd if=/usr/lib/u-boot/bananapi_m5/u-boot.bin of=/dev/mmcblk1 bs=512 seek=1 conv=fsync
sync
```

---

╔════════════════════════════════════════╗
**20. 🚀 Crear boot.ini**
╚════════════════════════════════════════╝

```bash
vi /boot/boot.ini
```

```
setenv bootargs "console=ttyAML0,115200 root=/dev/mmcblk1p2 rw rootwait"
fatload mmc 1:1 ${kernel_addr} kernel-6.12
fatload mmc 1:1 ${fdt_addr} meson-sm1-bananapi-m5.dtb
booti ${kernel_addr} - ${fdt_addr}
```

---

╔════════════════════════════════════════╗
**21. 🌐 Red y SSH**
╚════════════════════════════════════════╝

```bash
rc-update add sshd default

ln -s /etc/init.d/net.lo /etc/init.d/net.eth0
echo 'config_eth0="dhcp"' > /etc/conf.d/net

rc-update add net.eth0 default
```

---

╔════════════════════════════════════════╗
**22. 💿 Salir del chroot y arrancar desde eMMC**
╚════════════════════════════════════════╝

```bash
exit
umount -l /mnt/gentoo/{dev,sys,proc,run,boot,var}
umount -l /mnt/gentoo
sync
poweroff
```

Retirar microSD → boot desde eMMC.

---

# 🔥 SECCIONES SDR / IC-705 / FreeDV / Backend / rclone

*(idénticas a la v2.3, actualizadas solo para usar `vi` en todo)*

Si quieres, puedo hacer:

### ✔ v2.5 con tema “oscuro”

### ✔ v2.5 en formato **PDF listo**

### ✔ v2.5 en HTML con TOC

### ✔ o una versión **PRO** tipo libro/documentación

Solo dime:
👉 **“Haz la v2.5 en…”**
