#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    // Crear fifo2 de forma segura
    if (mkfifo("./fifo2", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo fifo2"); return 1;
    }

    printf("[Etapa 3 - Persistencia] Conectando a fifo2...\n");
    int fd_in = open("./fifo2", O_RDONLY);
    if (fd_in == -1) { perror("open fifo2"); return 1; }

    int fd_log = open("./datos_seguros.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_log == -1) { perror("open log"); return 1; }
    printf("[Etapa 3] Conectado. Guardando en datos_seguros.log...\n");

    char buffer[512];
    ssize_t n;
    while ((n = read(fd_in, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("[Etapa 3] Guardando: %s", buffer);
        write(fd_log, buffer, n);
    }

    close(fd_in);
    close(fd_log);
    printf("[Etapa 3] Archivo log guardado. Fin del pipeline.\n");
    return 0;
}