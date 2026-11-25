# TP-SO-Pipeline

# Pipeline de Seguridad y Sanitización de Datos Financieros
**Materia:** Sistemas Operativos - 2C 2026
**Profesor:** Ing. Gabriel Esquivel
**Alumnos:** Juan Ignacio Mamani Tapia

## Descripción
Implementación de un pipeline de 3 etapas utilizando Named Pipes (FIFOs) en Linux, programado en C. El sistema recibe datos sensibles (nombres, DNI, tarjetas), los enmascara y luego los persiste en disco, garantizando que los datos sin procesar nunca toquen el almacenamiento.

## Etapas
1. `recepcion.c`: Captura datos y los envía por `fifo1`.
2. `sanitizacion.c`: Lee de `fifo1`, enmascara datos y envía por `fifo2`.
3. `persistencia.c`: Lee de `fifo2` y guarda en `datos_seguros.log`.

## Conceptos aplicados
- Inter-Process Communication (IPC) con FIFOs.
- Manejo de EOF (fin de archivo) y `read()` retornando 0.
- Manejo de señales: `SIGPIPE` y error `EPIPE`.
- Creación idempotente de FIFOs con `mkfifo()` y `errno == EEXIST`.

## Instalación y Ejecución
```bash
gcc recepcion.c -o recepcion
gcc sanitizacion.c -o sanitizacion
gcc persistencia.c -o persistencia

# En terminales separadas:
./persistencia
./sanitizacion
./recepcion

# Ver el log
cat datos_seguros.log
