#!/usr/bin/env bash
# =========================================================
# Script: crear_ssh_pem_local.sh
# Autor: ChatGPT (GPT-5)
# Descripción:
#   Genera una clave SSH RSA (.pem + .pub)
#   Agrega el correo como comentario al final de la pública
#   y deja el .pem en el directorio actual listo para usar.
# =========================================================

set -euo pipefail

echo "========================================================="
echo "🔐 Generador de clave SSH (.pem) con comentario"
echo "========================================================="

# --- Preguntas interactivas ---
read -p "➡️  Nombre o etiqueta para la clave (ej: sdf12112025): " NAME
read -p "➡️  Correo (para comentario en la llave, ej: izafnat@freeshell.org): " EMAIL
read -p "➡️  Tamaño de la clave RSA (por defecto 4096): " SIZE
SIZE=${SIZE:-4096}

# --- Archivos ---
OUT_DIR="$(pwd)"
PEM_FILE="${OUT_DIR}/${NAME}.pem"
PUB_FILE="${OUT_DIR}/${NAME}.pub"

echo
echo "🔧 Generando clave RSA ${SIZE} bits..."
ssh-keygen -t rsa -b "$SIZE" -C "$EMAIL" -m PEM -f "$PEM_FILE" -N "" >/dev/null

# --- Generar la pública con el comentario explícito ---
PUB_NO_COMMENT="$(ssh-keygen -y -f "$PEM_FILE")"
echo "${PUB_NO_COMMENT} ${EMAIL}" > "$PUB_FILE"

# --- Permisos ---
chmod 400 "$PEM_FILE"
chmod 644 "$PUB_FILE"

echo
echo "✅ Claves generadas correctamente:"
echo "   🔒 Privada (.pem): $PEM_FILE"
echo "   🔑 Pública (.pub):  $PUB_FILE"
echo

echo "---------------------------------------------------------"
echo "📋 Contenido de la clave pública (lista para copiar a ~/.ssh/authorized_keys):"
echo "---------------------------------------------------------"
cat "$PUB_FILE"
echo
echo "---------------------------------------------------------"
echo "📌 Para usarla:"
echo "   1️⃣  Copia la línea anterior completa en tu servidor dentro de:"
echo "       ~/.ssh/authorized_keys"
echo "   2️⃣  Ajusta permisos en el servidor:"
echo "       chmod 700 ~/.ssh && chmod 600 ~/.ssh/authorized_keys"
echo "   3️⃣  Conéctate así:"
echo "       ssh -i \"$PEM_FILE\" usuario@servidor"
echo "---------------------------------------------------------"

# --- Opción de eliminar la pública local ---
read -p "¿Deseas eliminar el archivo .pub local después de copiarlo? (s/n): " DEL
if [[ "$DEL" == "s" || "$DEL" == "S" ]]; then
    rm -f "$PUB_FILE"
    echo "🗑️  Archivo público eliminado. Solo queda: $PEM_FILE"
fi

echo
echo "✨ Proceso completado."
echo "========================================================="
