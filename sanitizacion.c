#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

char *enmascarar(const char *original, int visibles) {
    int largo = strlen(original);
    if (largo <= visibles) return strdup(original);
    char *res = malloc(largo + 1);
    int ocultos = largo - visibles;
    for (int i = 0; i < ocultos; i++) res[i] = '*';
    for (int i = ocultos; i < largo; i++) res[i] = original[i];
    res[largo] = '\0';
    return res;
}

void procesar_y_enviar(char *linea, int fd_salida) {
    char copia[256], salida[512];
    strncpy(copia, linea, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';
    
    char *campo = strtok(copia, ",");
    snprintf(salida, sizeof(salida), "Procesado: ");
    
    while (campo != NULL) {
        while (*campo == ' ') campo++;
        if (strncmp(campo, "DNI ", 4) == 0) {
            char *m = enmascarar(campo + 4, 3);
            strcat(salida, "DNI "); strcat(salida, m); strcat(salida, " | ");
            free(m);
        } else if (strncmp(campo, "tarjeta ", 8) == 0) {
            char *m = enmascarar(campo + 8, 4);
            strcat(salida, "Tarjeta "); strcat(salida, m); strcat(salida, " | ");
            free(m);
        } else {
            strcat(salida, campo); strcat(salida, " | ");
        }
        campo = strtok(NULL, ",");
    }
    strcat(salida, "\n");
    write(fd_salida, salida, strlen(salida));
}

int main() {
    // Crear AMBOS fifos de forma segura
    if (mkfifo("./fifo1", 0666) == -1 && errno != EEXIST) { perror("mkfifo fifo1"); return 1; }
    if (mkfifo("./fifo2", 0666) == -1 && errno != EEXIST) { perror("mkfifo fifo2"); return 1; }

    printf("[Etapa 2 - Sanitizacion] Conectando a fifo1 (lectura)...\n");
    int fd_in = open("./fifo1", O_RDONLY);
    if (fd_in == -1) { perror("open fifo1"); return 1; }

    printf("[Etapa 2 - Sanitizacion] Conectando a fifo2 (escritura)...\n");
    int fd_out = open("./fifo2", O_WRONLY);
    if (fd_out == -1) { perror("open fifo2"); return 1; }
    printf("[Etapa 2] Conectado a ambas tuberias. Procesando...\n");

    char buffer[256];
    ssize_t n;
    while ((n = read(fd_in, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        char *linea = strtok(buffer, "\n");
        while (linea != NULL) {
            procesar_y_enviar(linea, fd_out);
            linea = strtok(NULL, "\n");
        }
    }

    close(fd_in);
    close(fd_out);
    printf("[Etapa 2] EOF recibido. Cerrando.\n");
    return 0;
}