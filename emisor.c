#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>   // agregado
#include <errno.h>    // agregado

int main() {
//    signal(SIGPIPE, SIG_IGN);   // para ver EPIPE en vez de morir

    const char *lineas[] = {
        "Juan Perez, DNI 30123456, tarjeta 4111111111111234\n",
        "Ana Gomez, DNI 27888999, tarjeta 5500000000009876\n",
        "Luis Diaz, DNI 31222333, tarjeta 4000123456781111\n"
    };
    int cantidad = 3;

    int fd = open("./datos_financieros", O_WRONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    for (int i = 0; i < cantidad; i++) {
        ssize_t n = write(fd, lineas[i], strlen(lineas[i]));
        if (n == -1) {
            if (errno == EPIPE)
                printf("write falló con EPIPE: no hay lector del otro lado.\n");
            else
                perror("write");
            close(fd);
            return 1;
        }
        printf("Emisor envió %zd bytes\n", n);
        sleep(3);   // pausa para poder matar al receptor
    }

    close(fd);
    printf("Emisor cerró el pipe.\n");
    return 0;
}
