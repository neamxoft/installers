# ============================================================

# 🐧 **Guía Gentoo ARM64 — Banana Pi M5 v2.6**

## **(SDR + IC-705 + FreeDV RADE + Backend REST Edition)**

### Español técnico estilo Gentoo Handbook

### Kernel 6.12 minimalista – Amlogic S905X3 – Instalación en eMMC

# ============================================================

Optimizada para:

* RTL-SDR, Airspy HF+
* IC-705 (CAT + USB Audio)
* FreeDV (incluye modo **RADE**)
* Backend REST FastAPI
* Uso **headless**
* Neovim (usado como `vi`)
* ZRAM + CPU performance
* Kernel minimalista 6.12
* Conexión USB-C ↔ RJ45 (Android Samsung)
* Baja escritura en eMMC (compilación en /var)

Incluye tu archivo:
👉 **`linux-bpi-m5-6.12-minimal.config`**

---

╔════════════════════════════════════════╗
**1. 📦 Requisitos Previos**
╚════════════════════════════════════════╝

* Banana Pi M5 (Amlogic S905X3 + 4GB RAM + eMMC 16GB)
* microSD 16GB+ con Armbian
* macOS o Linux para generar SD
* Conexión Ethernet o USB tethering
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
**4. ⚙️ Particionar eMMC (Esquema ideal v2.2)**
╚════════════════════════════════════════╝

```bash
cfdisk /dev/mmcblk1
```

Elegir **gpt**:

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
mkdir /mnt/gentoo
mount /dev/mmcblk1p2 /mnt/gentoo

mkdir /mnt/gentoo/boot
mount /dev/mmcblk1p1 /mnt/gentoo/boot

mkdir /mnt/gentoo/var
mount /dev/mmcblk1p3 /mnt/gentoo/var
```

---

╔════════════════════════════════════════╗
**7. 📥 Descargar Stage3 ARM64 (OpenRC)**
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
**11. 🌍 Configurar Locales (ANTES DEL SYNC)**
╚════════════════════════════════════════╝

```bash
nano /etc/locale.gen
```

Agregar:

```
en_US.UTF-8 UTF-8
en_US ISO-8859-1
C.UTF-8 UTF-8
```

Aplicar:

```bash
locale-gen
eselect locale set en_US.utf8
env-update
source /etc/profile
unset LC_ALL LANGUAGE LC_MESSAGES
```

---

╔════════════════════════════════════════╗
**12. ⚙️ Configurar make.conf (nano)**
╚════════════════════════════════════════╝

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
emerge --ask --sync
```

---

╔════════════════════════════════════════╗
**14. 📝 Instalar Neovim y vincular a `vi`**
╚════════════════════════════════════════╝

```bash
emerge --ask app-editors/neovim
ln -sf /usr/bin/nvim /usr/bin/vi
ln -sf /usr/bin/nvim /usr/bin/vim
eselect editor set /usr/bin/nvim
```

Desde aquí, **solo usar:**

```bash
vi archivo
```

---

╔════════════════════════════════════════╗
**15. ⬆️ Actualizar @world**
╚════════════════════════════════════════╝

```bash
emerge --ask -avuDU --with-bdeps=y @world
```

---

╔════════════════════════════════════════╗
**16. 🐚 Instalar fish**
╚════════════════════════════════════════╝

```bash
emerge --ask app-shells/fish
mkdir -p ~/.config/fish
vi ~/.config/fish/config.fish
```

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
emerge --ask sys-block/zram-init
rc-update add zram-init default
```

CPU Governor:

```bash
emerge --ask sys-power/cpupower
echo 'GOVERNOR="performance"' > /etc/conf.d/cpupower
rc-update add cpupower default
```

---

╔════════════════════════════════════════╗
**18. 🧠 Instalar Kernel 6.12 minimalista**
╚════════════════════════════════════════╝

Instalar fuentes:

```bash
emerge --ask sys-kernel/gentoo-sources
```

Seleccionar el kernel con eselect:

```bash
eselect kernel list
eselect kernel set 1
```

Verificar que `/usr/src/linux` apunta al kernel correcto:

```bash
ls -l /usr/src/linux
```

Entrar:

```bash
cd /usr/src/linux
```

Copiar configuración minimalista:

```bash
cp /mnt/data/linux-bpi-m5-6.12-minimal.config .config
```

Aplicar defaults:

```bash
make olddefconfig
```

Compilar:

```bash
make -j5 Image dtbs modules
make modules_install
```

Instalar kernel:

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
emerge --ask sys-boot/u-boot-bananapi-m5
dd if=/usr/lib/u-boot/bananapi_m5/u-boot.bin of=/dev/mmcblk1 bs=512 seek=1 conv=fsync
sync
```

---

╔════════════════════════════════════════╗
**20. 🚀 boot.ini**
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
**21. 🌐 Red + SSH**
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

Retira la microSD → arrancar desde eMMC.

---

# ███ SECCIONES SDR – IC705 – FREEDV – BACKEND – RCLONE ███

*(Incluidas con `emerge --ask` y uso de `vi`, como acordado)*

---
