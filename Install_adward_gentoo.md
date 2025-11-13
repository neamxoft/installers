# 🛡️ **MANUAL COMPLETO DE INSTALACIÓN — ADGUARD HOME EN GENTOO 2025 (OpenRC)**

> **Compatibilidad:** Gentoo Linux (OpenRC), amd64, instalación manual en `/opt`.
> **Método:** binario oficial AdGuard Home (sin ebuild).
> **Servicio:** OpenRC nativo (creado automáticamente por AdGuard).
> **Uso:** Filtrado DNS de publicidad + privacidad para toda la red.
> **Privilegios:** Se asume que usas `root` o `doas` donde sea necesario.

---

# 📦 **1. Descargar AdGuard Home**

Gentoo ya no incluye `net-dns/adguardhome`, así que usamos la versión oficial.

```bash
cd /opt
wget https://static.adguard.com/adguardhome/release/AdGuardHome_linux_amd64.tar.gz
tar xvf AdGuardHome_linux_amd64.tar.gz
mv AdGuardHome adguardhome
```

Permisos recomendados:

```bash
chmod 700 /opt/adguardhome
chown -R root:root /opt/adguardhome
```

---

# 🌐 **2. Instalar el servicio OpenRC oficial**

AdGuard Home incluye un instalador que crea:

* `/etc/init.d/AdGuardHome` (servicio OpenRC)
* `/opt/adguardhome/AdGuardHome.yaml` (configuración)
* `/opt/adguardhome/data/` (base de datos y estado)
* `/opt/adguardhome/work/` (archivos temporales)

Ejecuta:

```bash
cd /opt/adguardhome
./AdGuardHome -s install
```

Salida esperada:

```
AdGuard Home is installed and will run as a service
Go to http://<IP>:3000
```

Esto **NO inicia todavía la aplicación**.
Solo instala los archivos y el servicio.

---

# 🚀 **3. Iniciar AdGuard Home por primera vez (wizard web)**

Ahora sí arrancamos:

```bash
rc-service AdGuardHome start
```

Verifica:

```bash
rc-service AdGuardHome status
```

Debe mostrar:

```
status: started
```

Ahora abre en tu navegador:

```
http://<IP-DE-TU-GENTOO>:3000
```

Ejemplo:

```
http://192.168.1.4:3000
```

Verás el **wizard de configuración inicial**.

---

# 🧩 **4. Completar instalación web (wizard)**

El wizard tiene 6 pasos:

1. **Crear usuario administrador**
2. **Seleccionar interfaz (ej. `enp1s0`)**
3. **Puerto DNS: 53**
4. **Elegir upstream DNS (recomendado):**

   ```
   https://cloudflare-dns.com/dns-query
   https://dns.quad9.net/dns-query
   ```
5. **Seleccionar listas de bloqueo:**

   * AdGuard Base
   * AdGuard Tracking
   * OISD Basic (opcional pero recomendado)
6. **Finalizar instalación**

Al terminar, AdGuard Home generará:

```
/opt/adguardhome/AdGuardHome.yaml
```

Y moverá el panel web del puerto **3000 → 8000**.

---

# 🧹 **5. Desactivar servidor DHCP (recomendado)**

Si no usarás AdGuard como DHCP —lo normal— ponlo en off.

Edita:

```bash
vi /opt/adguardhome/AdGuardHome.yaml
```

Busca:

```
dhcp:
  enabled: true
```

Cámbialo a:

```
dhcp:
  enabled: false
```

Guarda:

```
:wq
```

Reinicia:

```bash
rc-service AdGuardHome restart
```

---

# 📡 **6. Configurar el router / módem**

En tu router TP-Link / Telmex / Fibra:

## ✔ DNS Primario → AdGuard Home (tu Gentoo)

Ejemplo:

```
192.168.1.4
```

## ✔ DNS Secundario → DNS externo (respaldo)

(Recomendado para que no se caiga el internet si apagas tu PC)

```
1.1.1.1
9.9.9.9
8.8.8.8
```

Esto garantiza:

* Si Gentoo está encendido → Internet sin publicidad
* Si Gentoo está apagado → Internet normal (usa DNS2)

---

# 🖥️ **7. Configurar Gentoo para usar AdGuard Home localmente**

Edita:

```bash
vi /etc/resolv.conf
```

Coloca:

```
nameserver 127.0.0.1
nameserver 1.1.1.1
```

---

# 🔥 **8. Ajustes finales del servicio OpenRC**

AdGuard ya creó su propio servicio.
Solo asegúrate que arranque siempre:

```bash
rc-update add AdGuardHome default
```

Revisión del estado:

```bash
rc-service AdGuardHome status
```

Logs:

```bash
tail -f /var/log/AdGuardHome/AdGuardHome.log
```

---

# 🔧 **9. Verificación general**

DNS activo:

```bash
nslookup google.com 127.0.0.1
```

Puertos:

```bash
ss -tulnp | grep -E "53|8000"
```

Archivos:

```bash
ls -l /opt/adguardhome/
```

Panel web:

```
http://192.168.1.4:8000
```

---

# 🛡️ **10. Seguridad opcional**

## 🔐 Bloquear acceso al panel web desde toda la red:

En `AdGuardHome.yaml`:

```
http:
  address: 127.0.0.1:8000
```

Solo accesible localmente.

## 🔐 HTTPS para el panel:

AdGuard Home soporta:

* Certificados Let’s Encrypt
* Certificados propios

Se configura en:

```
tls:
  enabled: true
```

## 🔐 Sólo tu PC puede usar el DNS:

Firewall:

```bash
iptables -A INPUT -p tcp --dport 53 -s 192.168.1.0/24 -j ACCEPT
iptables -A INPUT -p udp --dport 53 -s 192.168.1.0/24 -j ACCEPT
```

---

# 🎯 **MANUAL COMPLETAMENTE LISTO**

Este manual:

✔ no depende de systemd
✔ instala desde /opt
✔ usa servicio OpenRC oficial
✔ desactiva DHCP
✔ configura DNS seguro
✔ garantiza que tu red siga funcionando si tu PC se apaga
✔ cumple exactamente con tu flujo Gentoo

---