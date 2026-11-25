#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>     // agregado
#include <errno.h>      // agregado
#include <sys/stat.h>   // agregado: para mkfifo

char *enmascarar(const char *original, int visibles) {
    int largo = strlen(original);
    if (largo <= visibles) return strdup(original);
    char *resultado = malloc(largo + 1);
    if (resultado == NULL) return NULL;
    int ocultos = largo - visibles;
    for (int i = 0; i < ocultos; i++) resultado[i] = '*';
    for (int i = ocultos; i < largo; i++) resultado[i] = original[i];
    resultado[largo] = '\0';
    return resultado;
}

void procesar_linea(char *linea) {
    char copia[256];
    strncpy(copia, linea, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';
    char *campo = strtok(copia, ",");
    while (campo != NULL) {
        while (*campo == ' ') campo++;
        if (strncmp(campo, "DNI ", 4) == 0) {
            char *m = enmascarar(campo + 4, 3);
            printf("DNI enmascarado: %s\n", m);
            free(m);
        } else if (strncmp(campo, "tarjeta ", 8) == 0) {
            char limpio[64];
            strncpy(limpio, campo + 8, sizeof(limpio) - 1);
            limpio[sizeof(limpio) - 1] = '\0';
            limpio[strcspn(limpio, "\n")] = '\0';
            char *m = enmascarar(limpio, 4);
            printf("Tarjeta enmascarada: %s\n", m);
            free(m);
        } else {
            printf("Campo: %s\n", campo);
        }
        campo = strtok(NULL, ",");
    }
    printf("---\n");
}

int main() {
    // 1) Crear el FIFO si no existe (idempotente).
    //    Si ya existe, mkfifo falla con EEXIST y lo ignoramos.
    if (mkfifo("./datos_financieros", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    // 2) Abrir el FIFO para lectura (bloquea hasta que haya escritor).
    int fd = open("./datos_financieros", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("Receptor conectado. Esperando datos...\n");

    // 3) Loop de lectura hasta EOF.
    char buffer[256];
    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        char *linea = strtok(buffer, "\n");
        while (linea != NULL) {
            procesar_linea(linea);
            linea = strtok(NULL, "\n");
        }
    }

    if (n == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("EOF detectado: el emisor cerró el pipe. Receptor finaliza.\n");
    close(fd);
    return 0;
}
