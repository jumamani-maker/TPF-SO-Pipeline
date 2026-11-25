#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main() {
    // Crear fifo1 de forma segura
    if (mkfifo("./fifo1", 0666) == -1 && errno != EEXIST) {
        perror("mkfifo fifo1"); return 1;
    }

    printf("[Etapa 1 - Recepcion] Conectando a fifo1...\n");
    int fd = open("./fifo1", O_WRONLY);
    if (fd == -1) { perror("open fifo1"); return 1; }
    printf("[Etapa 1] Conectado. Iniciando envio...\n");

    const char *datos[] = {
        "Carlos Tevez, DNI 20123456, tarjeta 4511111111111111\n",
        "Lionel Messi, DNI 28999888, tarjeta 4522222222222222\n",
        "Diego Maradona, DNI 15555666, tarjeta 4533333333333333\n"
    };

    for (int i = 0; i < 3; i++) {
        write(fd, datos[i], strlen(datos[i]));
        printf("[Etapa 1] Enviado a sanitizar: %s", datos[i]);
        sleep(1); // Pausa para ver el flujo en tiempo real
    }

    close(fd);
    printf("[Etapa 1] Fin de la transmision.\n");
    return 0;
}