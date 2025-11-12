#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char name[256], email[256], size[16];
    char pem_file[512], pub_file[512], cmd[1024];
    char choice;

    printf("=========================================================\n");
    printf("🔐 Generador de clave SSH (.pem) con comentario (C version)\n");
    printf("=========================================================\n\n");

    // --- Entrada interactiva ---
    printf("➡️  Nombre o etiqueta para la clave (ej: sdf12112025): ");
    if (!fgets(name, sizeof(name), stdin)) return 1;
    name[strcspn(name, "\n")] = 0;

    printf("➡️  Correo (para comentario en la llave, ej: izafnat@freeshell.org): ");
    if (!fgets(email, sizeof(email), stdin)) return 1;
    email[strcspn(email, "\n")] = 0;

    printf("➡️  Tamaño de la clave RSA (por defecto 4096): ");
    if (!fgets(size, sizeof(size), stdin)) return 1;
    size[strcspn(size, "\n")] = 0;
    if (strlen(size) == 0) strcpy(size, "4096");

    // --- Construir nombres de archivo ---
    snprintf(pem_file, sizeof(pem_file), "%s.pem", name);
    snprintf(pub_file, sizeof(pub_file), "%s.pub", name);

    printf("\n🔧 Generando clave RSA %s bits...\n", size);

    // --- Generar la clave privada ---
    snprintf(cmd, sizeof(cmd),
             "ssh-keygen -t rsa -b %s -C \"%s\" -m PEM -f \"%s\" -N \"\" >/dev/null",
             size, email, pem_file);
    if (system(cmd) != 0) {
        fprintf(stderr, "❌ Error al generar la clave con ssh-keygen.\n");
        return 1;
    }

    // --- Generar la pública con comentario ---
    snprintf(cmd, sizeof(cmd),
             "ssh-keygen -y -f \"%s\" > \"%s\"", pem_file, pub_file);
    if (system(cmd) != 0) {
        fprintf(stderr, "❌ Error al generar la clave pública.\n");
        return 1;
    }

    snprintf(cmd, sizeof(cmd),
             "echo ' %s' >> \"%s\"", email, pub_file);
    system(cmd);

    // --- Ajustar permisos ---
    snprintf(cmd, sizeof(cmd), "chmod 400 \"%s\" && chmod 644 \"%s\"", pem_file, pub_file);
    system(cmd);

    // --- Mostrar información final ---
    printf("\n✅ Claves generadas correctamente:\n");
    printf("   🔒 Privada (.pem): %s\n", pem_file);
    printf("   🔑 Pública (.pub):  %s\n\n", pub_file);

    printf("---------------------------------------------------------\n");
    printf("📋 Contenido de la clave pública:\n");
    snprintf(cmd, sizeof(cmd), "cat \"%s\"", pub_file);
    system(cmd);
    printf("---------------------------------------------------------\n");
    printf("📌 Para usarla:\n");
    printf("   1️⃣  Copia la línea anterior en ~/.ssh/authorized_keys\n");
    printf("   2️⃣  chmod 700 ~/.ssh && chmod 600 ~/.ssh/authorized_keys\n");
    printf("   3️⃣  ssh -i \"%s\" usuario@servidor\n", pem_file);
    printf("---------------------------------------------------------\n");

    printf("¿Deseas eliminar el archivo .pub local después de copiarlo? (s/n): ");
    scanf(" %c", &choice);

    if (choice == 's' || choice == 'S') {
        snprintf(cmd, sizeof(cmd), "rm -f \"%s\"", pub_file);
        system(cmd);
        printf("🗑️  Archivo público eliminado. Solo queda: %s\n", pem_file);
    }

    printf("\n✨ Proceso completado.\n");
    printf("=========================================================\n");

    return 0;
}

