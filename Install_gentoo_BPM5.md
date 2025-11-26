# ============================================================

# 🐧 **Guía Gentoo ARM64 — Banana Pi M5 v2.0**

## **(SDR + IC-705 + FreeDV RADE + Backend REST Edition)**

### Español técnico estilo Gentoo Handbook

### Kernel 6.12 minimalista – Amlogic S905X3 – eMMC Install

# ============================================================

Esta guía instala **Gentoo ARM64 en la eMMC** de la Banana Pi M5, optimizada para:

* RTL-SDR (rtl_tcp)
* Airspy HF+ Discovery
* IC-705 (CAT + USB Audio)
* FreeDV (incluyendo modo **RADE**)
* Backend REST para control remoto desde Android/iOS
* Funcionamiento **headless**
* Baja latencia, estabilidad y eficiencia
* Conexión por **USB-C ↔ RJ45 tethering**
* fish + neovim
* ZRAM y CPU governor performance

Incluye tu archivo de kernel:
👉 **`linux-bpi-m5-6.12-minimal.config`**

---

# ============================================================

# 1. 📦 **Requisitos previos**

# ============================================================

* Banana Pi M5 (Amlogic S905X3 + 4GB RAM + eMMC 16GB)
* microSD 16GB+ con Armbian CLI
* Conexión Ethernet
* Cable USB-C ↔ RJ45 (Samsung tethering)
* PC/macOS para generar la SD
* Teclado/SSH opcional

---

# ============================================================

# 2. 💾 **Crear microSD con Armbian (en macOS)**

# ============================================================

Verifica el disco:

```bash
diskutil list
diskutil unmountDisk /dev/diskX
sudo dd if=Armbian.img of=/dev/rdiskX bs=4m status=progress
sync
```

Arranca la Banana Pi desde la microSD.

---

# ============================================================

# 3. 🔍 **Identificar el eMMC**

# ============================================================

```bash
lsblk
```

Debes ver algo así:

* `mmcblk0` → microSD
* **`mmcblk1` → eMMC (16GB)**

---

# ============================================================

# 4. ⚙️ **Particionar el eMMC (esquema definitivo v2.0)**

# ============================================================

Abrir cfdisk:

```bash
cfdisk /dev/mmcblk1
```

Elegir: **gpt**

Crear:

| Partición | Tamaño       | Tipo             | Uso    |
| --------- | ------------ | ---------------- | ------ |
| mmcblk1p1 | 256M         | EFI System       | /boot  |
| mmcblk1p2 | 8G           | Linux filesystem | `/`    |
| mmcblk1p3 | resto (~7GB) | Linux filesystem | `/var` |

Escribir → `Write`

---

# ============================================================

# 5. 🧱 **Formatear particiones**

# ============================================================

```bash
mkfs.vfat -F32 /dev/mmcblk1p1
mkfs.ext4 /dev/mmcblk1p2
mkfs.ext4 /dev/mmcblk1p3
```

---

# ============================================================

# 6. 📂 **Montar estructura Gentoo**

# ============================================================

```bash
mkdir -p /mnt/gentoo
mount /dev/mmcblk1p2 /mnt/gentoo

mkdir /mnt/gentoo/boot
mount /dev/mmcblk1p1 /mnt/gentoo/boot

mkdir /mnt/gentoo/var
mount /dev/mmcblk1p3 /mnt/gentoo/var
```

---

# ============================================================

# 7. 📥 **Descargar Stage3 ARM64 OpenRC**

# ============================================================

```bash
cd /mnt/gentoo
wget https://distfiles.gentoo.org/releases/arm64/autobuilds/current-stage3-arm64-openrc/stage3-arm64-openrc-*.tar.xz
tar xpvf stage3-arm64-openrc-*.tar.xz --xattrs-include='*.*' --numeric-owner
```

---

# ============================================================

# 8. 🌐 **Copiar resolv.conf**

# ============================================================

```bash
cp -L /etc/resolv.conf /mnt/gentoo/etc/
```

---

# ============================================================

# 9. 🔗 **Montar pseudo-sistemas**

# ============================================================

```bash
mount -t proc /proc /mnt/gentoo/proc
mount --rbind /sys /mnt/gentoo/sys
mount --rbind /dev /mnt/gentoo/dev
mount --rbind /run /mnt/gentoo/run
```

---

# ============================================================

# 10. 🚪 **Entrar al chroot**

# ============================================================

```bash
export TERM=xterm-256color
chroot /mnt/gentoo /bin/bash
source /etc/profile
export PS1="(chroot) $PS1"
```

---

# ============================================================

# 11. ⚙️ **Configurar make.conf (ARM64 optimizado)**

# ============================================================

`/etc/portage/make.conf`:

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

# ============================================================

# 12. 🔄 **Sincronizar Portage**

# ============================================================

```bash
emerge --sync
```

---

# ============================================================

# 13. 🌍 **Locales (solución completa a setlocale)**

# ============================================================

Editar:

```bash
nvim /etc/locale.gen
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
eselect locale list
eselect locale set en_US.utf8
env-update
source /etc/profile
unset LC_ALL LANGUAGE LC_MESSAGES
```

---

# ============================================================

# 14. 📌 **Profile correcto**

# ============================================================

```bash
eselect profile list
eselect profile set default/linux/arm64/23.0
```

---

# ============================================================

# 15. ⬆️ **Actualizar todo @world**

# ============================================================

```bash
emerge -avuDU --with-bdeps=y @world
```

---

# ============================================================

# 16. 🐚 **Instalar fish + Neovim**

# ============================================================

```bash
emerge --ask app-shells/fish
emerge --ask app-editors/neovim
eselect editor set /usr/bin/nvim
```

Config fish:

```bash
mkdir -p ~/.config/fish
nvim ~/.config/fish/config.fish
```

```
if test -n "$TMUX"
    set -gx TERM screen-256color
else
    set -gx TERM xterm-256color
end
```

---

# ============================================================

# 17. 🧠 **ZRAM + CPU governor performance**

# ============================================================

Activar zram:

```bash
emerge --ask sys-block/zram-init
rc-update add zram-init default
```

CPU performance:

```bash
echo "GOVERNOR=\"performance\"" > /etc/conf.d/cpupower
rc-update add cpupower default
```

---

# ============================================================

# 18. 🧠 **Instalar Kernel 6.12 con tu `.config`**

# ============================================================

Instalar fuente:

```bash
emerge sys-kernel/gentoo-sources
cd /usr/src/linux
```

Copiar tu config:

```bash
cp /mnt/data/linux-bpi-m5-6.12-minimal.config .config
make olddefconfig
```

Compilar:

```bash
make -j5 Image dtbs modules
make modules_install
```

Copiar kernel y dtb:

```bash
cp arch/arm64/boot/Image /boot/kernel-6.12
cp arch/arm64/boot/dts/amlogic/meson-sm1-bananapi-m5.dtb /boot/
sync
```

---

# ============================================================

# 19. 🔥 **Instalar U-Boot**

# ============================================================

```bash
emerge sys-boot/u-boot-bananapi-m5
dd if=/usr/lib/u-boot/bananapi_m5/u-boot.bin of=/dev/mmcblk1 bs=512 seek=1 conv=fsync
sync
```

---

# ============================================================

# 20. 🚀 **Crear boot.ini**

# ============================================================

`/boot/boot.ini`:

```
setenv bootargs "console=ttyAML0,115200 root=/dev/mmcblk1p2 rw rootwait"
fatload mmc 1:1 ${kernel_addr} kernel-6.12
fatload mmc 1:1 ${fdt_addr} meson-sm1-bananapi-m5.dtb
booti ${kernel_addr} - ${fdt_addr}
```

---

# ============================================================

# 21. 🌐 **Red y SSH**

# ============================================================

```bash
rc-update add sshd default
ln -s /etc/init.d/net.lo /etc/init.d/net.eth0
echo 'config_eth0="dhcp"' > /etc/conf.d/net
rc-update add net.eth0 default
```

---

# ============================================================

# 22. 💿 **Salir del chroot y arrancar**

# ============================================================

```bash
exit
umount -l /mnt/gentoo/{dev,sys,proc,run,boot,var}
umount -l /mnt/gentoo
sync
poweroff
```

Quita la microSD → arranca desde eMMC.

---

# ============================================================

# 23. 📡 **SECCIÓN SDR — RTL-SDR (rtl_tcp)**

# ============================================================

```bash
emerge --ask net-wireless/rtl-sdr
echo "blacklist dvb_usb_rtl28xxu" > /etc/modprobe.d/blacklist-rtl.conf
```

Servicio rtl_tcp:

`/etc/init.d/rtl_tcp`:

```bash
#!/sbin/openrc-run
command="/usr/bin/rtl_tcp"
command_args="-a 0.0.0.0 -p 1234"
pidfile="/run/rtl_tcp.pid"
```

```bash
chmod +x /etc/init.d/rtl_tcp
rc-update add rtl_tcp default
```

---

# ============================================================

# 24. 📡 **SECCIÓN SDR — Airspy HF+**

# ============================================================

```bash
emerge --ask net-wireless/airspyhf
airspyhf_info
```

---

# ============================================================

# 25. 📻 **IC-705 — CAT + Audio + udev**

# ============================================================

Regla udev `/etc/udev/rules.d/99-ic705.rules`:

```
SUBSYSTEM=="tty", ATTRS{idVendor}=="2457", ATTRS{idProduct}=="0a02", MODE="0666", GROUP="dialout"
SUBSYSTEM=="sound", ATTRS{idVendor}=="2457", ATTRS{idProduct}=="0a02", MODE="0666", GROUP="audio"
```

Agregar usuario a grupos:

```bash
usermod -aG dialout,audio youruser
```

---

# ============================================================

# 26. 🎙 **FreeDV + RADE**

# ============================================================

```bash
emerge --ask media-sound/codec2
emerge --ask media-sound/freedv
```

Prueba:

```bash
freedv_rx --mode 800XA
freedv_tx --mode 800XA
```

Modo **RADE**:

```bash
freedv_tx --mode RADE
freedv_rx --mode RADE
```

---

# ============================================================

# 27. 🌐 **Backend REST (base)**

# ============================================================

Estructura:

```
/srv/radio-backend/
    main.py
    ic705.py
    freedv.py
    sdr.py
```

Ejemplo básico FastAPI:

```python
from fastapi import FastAPI
app = FastAPI()

@app.get("/api/ic705/freq")
def read_freq():
    return {"freq": "..."}
```

---

# ============================================================

# 28. 🔌 **USB-C ↔ RJ45 Tethering (Samsung → Banana Pi)**

# ============================================================

Activar tethering en Android.

Banana Pi verá una interfaz nueva:
`usb0` o `enx...`

Configurar DHCP:

```bash
ln -s /etc/init.d/net.lo /etc/init.d/net.usb0
echo 'config_usb0="dhcp"' >> /etc/conf.d/net
rc-update add net.usb0 default
```

---

# ============================================================

# 29. ☁️ **rclone (manual)**

# ============================================================

```bash
emerge --ask net-misc/rclone
rclone config
```

Montaje manual:

```bash
rclone mount mydrive: /mnt/cloud --daemon
```

---
