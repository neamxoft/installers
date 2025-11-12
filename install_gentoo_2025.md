# 🧩 Guía de Instalación Gentoo 2025 — OpenRC + ZFS (Cross-Platform Workflow)

> **Versión:**  v2.6 (ZFS + kernel manual + Fish + SSH + hostname + post-instalación)  
> **Equipo:**  Intel N5105 (Jasper Lake)  
> **Arquitectura:**  amd64 (nomultilib)  
> **Origen USB:**  macOS / Linux / Windows

----------

## 🪜 STEP 0 — Crear el USB Live

Gentoo publica ISOs “minimal” actualizadas semanalmente:  
👉  [https://mirror.leaseweb.com/gentoo/releases/amd64/autobuilds/](https://mirror.leaseweb.com/gentoo/releases/amd64/autobuilds/)

Ejemplo:

```
install-amd64-minimal-20251026T170339Z.iso

```

----------

### 🧩 A) macOS

```bash
diskutil list
diskutil unmountDisk /dev/disk4
sudo dd if=~/Downloads/install-amd64-minimal-20251026T170339Z.iso of=/dev/rdisk4 bs=4m status=progress
diskutil eject /dev/disk4

```

> 💡 Verifica el número de disco con  `diskutil list`.

----------

### 🧩 B) Linux

```bash
lsblk                        # identifica el USB (ej. /dev/sdb)
sudo umount /dev/sdb*
sudo dd if=~/Downloads/install-amd64-minimal-20251026T170339Z.iso of=/dev/sdb bs=4M status=progress conv=fsync
sync

```

> 💡 Usa el dispositivo completo (/dev/sdX) y  **verifica dos veces**  antes de ejecutar  `dd`.

----------

### 🧩 C) Windows

1.  Descarga la ISO de Gentoo.
    
2.  Usa  **Rufus**  ([https://rufus.ie](https://rufus.ie/)) o  **balenaEtcher**  ([https://balena.io/etcher](https://balena.io/etcher)).
    
3.  Selecciona la ISO → USB → modo  **GPT + UEFI**  →  _Start_.
    
4.  Expulsa el USB de forma segura.
    

----------

## 🪜 STEP 1 — Arranque e inicio de SSH

-   Habilita  **UEFI Boot**  en BIOS y arranca desde el USB.
    
-   Si no obtienes IP automáticamente:
    
    ```bash
    ifconfig
    dhcpcd
    
    ```
    
-   Activa SSH en el LiveCD:
    
    ```bash
    /etc/init.d/sshd start
    passwd
    ifconfig   # anota la IP
    
    ```
    
-   Desde macOS/Linux/Windows (con PuTTY o terminal):
    
    ```bash
    ssh root@<ip_del_gentoo_live>
    
    ```
    

----------

## 🪜 STEP 2 — Limpieza total del SSD

```bash
umount -R /mnt/gentoo || true
zpool export -a || true
zpool destroy -f tank 2>/dev/null || true
zpool labelclear -f /dev/sda2 2>/dev/null || true
wipefs -a /dev/sda

```

Verifica con  `parted /dev/sda print`  → debe mostrar “unknown label”.

----------

## 🪜 STEP 3 — Crear particiones GPT

```bash
parted /dev/sda mklabel gpt
parted /dev/sda mkpart EFI fat32 1MiB 512MiB
parted /dev/sda set 1 esp on
parted /dev/sda mkpart zfsroot 512MiB 100%
mkfs.vfat -F32 /dev/sda1

```

----------

## 🪜 STEP 4 — Crear el pool ZFS y datasets

```bash
modprobe zfs
zpool create -f \
  -o ashift=12 \
  -O compression=lz4 \
  -O atime=off \
  -O xattr=sa \
  -O acltype=posixacl \
  -m none tank /dev/sda2

zfs create -o mountpoint=none tank/ROOT
zfs create -o mountpoint=/ tank/ROOT/gentoo
zfs create -o mountpoint=/home tank/home
zpool set bootfs=tank/ROOT/gentoo tank

```

----------

## 🪜 STEP 5 — Montaje del sistema base

```bash
zfs set mountpoint=legacy tank/ROOT/gentoo
mount -t zfs tank/ROOT/gentoo /mnt/gentoo
mkdir -p /mnt/gentoo/boot
mount /dev/sda1 /mnt/gentoo/boot

```

----------

## 🪜 STEP 6 — Instalar Stage3

```bash
cd /mnt/gentoo
wget -O stage3.tar.xz https://mirror.leaseweb.com/gentoo/releases/amd64/autobuilds/current-stage3-amd64-nomultilib-openrc/stage3-amd64-nomultilib-openrc-20251109T170053Z.tar.xz
tar xpvf stage3*.tar.xz --xattrs-include='*.*' --numeric-owner

```

----------

## 🪜 STEP 7 — Entrar al entorno real (chroot)

```bash
mount -t proc /proc /mnt/gentoo/proc
mount --rbind /sys /mnt/gentoo/sys && mount --make-rslave /mnt/gentoo/sys
mount --rbind /dev /mnt/gentoo/dev && mount --make-rslave /mnt/gentoo/dev
mount --bind /run /mnt/gentoo/run
cp -L /etc/resolv.conf /mnt/gentoo/etc/
chroot /mnt/gentoo /bin/bash
source /etc/profile
export PS1="(Install-Gentoo) ${PS1}"

```

----------

## 🪜 STEP 8 — Configuración inicial y herramientas base

```bash
emerge-webrsync
eselect profile set 9
env-update && source /etc/profile
emerge --ask app-editors/neovim

```

### `/etc/portage/make.conf`

```bash
vi /etc/portage/make.conf

```

Contenido sugerido:

```bash
COMMON_FLAGS="-O2 -pipe -march=goldmont-plus"
CFLAGS="${COMMON_FLAGS}"
CXXFLAGS="${COMMON_FLAGS}"
MAKEOPTS="-j5"
USE="zfs rclone minimal openssh acl ssl -systemd -gtk -qt5"
ACCEPT_LICENSE="*"
GENTOO_MIRRORS="https://mirror.leaseweb.com/gentoo/"
EMERGE_DEFAULT_OPTS="--jobs=4 --load-average=4.5 --with-bdeps=y"
FEATURES="parallel-fetch"
PORTAGE_TMPDIR="/var/tmp"
DISTDIR="/var/cache/distfiles"
PKGDIR="/var/cache/binpkgs"

```

----------

## 🪜 STEP 9 — Kernel manual + ZFS

```bash
emerge --ask sys-kernel/gentoo-sources
cd /usr/src/linux
cp /root/config-gentoo-n5105-zfs-fuse-clean .config
make olddefconfig
make -j5
make modules_install install
emerge --ask sys-fs/zfs sys-kernel/dracut
dracut --kver 6.12.41-gentoo --force /boot/initramfs-6.12.41-gentoo.img

```

----------

## 🪜 STEP 10 — Bootloader y usuarios

```bash
emerge --ask sys-boot/grub
grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=Gentoo --recheck
vi /etc/default/grub   # Editar GRUB_CMDLINE_LINUX="root=ZFS=tank/ROOT/gentoo"
grub-mkconfig -o /boot/grub/grub.cfg

passwd
useradd -m -G users,wheel,audio,video -s /bin/bash neftali
passwd neftali
emerge --ask app-admin/sudo
visudo   # habilita: %wheel ALL=(ALL:ALL) ALL

```

----------

## 🪜 STEP 11 — Shell, SSH y seguridad

```bash
emerge --ask app-shells/fish net-misc/openssh
chsh -s /bin/fish neftali

rc-update add sshd default
rc-service sshd start

```

Editar configuración:

```bash
vi /etc/ssh/sshd_config

```

Asegúrate de tener:

```bash
PermitRootLogin no
PasswordAuthentication yes
AllowUsers neftali

```

Luego reinicia:

```bash
rc-service sshd restart

```

----------

## 🪜 STEP 12 — Export y reinicio

```bash
zfs unmount -a
exit
umount -Rl /mnt/gentoo/boot
umount -Rl /mnt/gentoo/dev
umount -Rl /mnt/gentoo/sys
umount -Rl /mnt/gentoo/proc
zpool export tank
reboot

```

----------

## 🪜 STEP 13 — Post-instalación y servicios base

### ZFS en arranque

```bash
rc-update add zfs-import boot
rc-update add zfs-mount boot
rc-update add zfs-share boot

```

### Red e internet

```bash
rc-service dhcpcd start
rc-update add dhcpcd default

```

### Utilidades base

```bash
emerge --ask app-editors/neovim net-misc/rclone app-containers/docker app-containers/docker-compose
rc-update add docker default
rc-service docker start

```

### Sincronización (Nextcloud / Rclone)

```bash
rclone config
rclone bisync nextcloud:Documents ~/Documents --resync

```

### Verificación general

```bash
zpool status
zfs list
rc-status

```

----------

## 🪜 STEP 14 — Configurar hostname y /etc/hosts

**1️⃣ Configura el nombre del sistema:**

```bash
vi /etc/conf.d/hostname

```

Agrega o edita:

```bash
hostname="gentoo-nas"

```

**2️⃣ Aplica el cambio:**

```bash
/etc/init.d/hostname restart

```

**3️⃣ Edita el archivo de hosts:**

```bash
vi /etc/hosts

```

Contenido sugerido:

```
127.0.0.1   gentoo-nas localhost
::1         gentoo-nas localhost

```

**4️⃣ Verifica:**

```bash
hostname
hostnamectl

```

----------
