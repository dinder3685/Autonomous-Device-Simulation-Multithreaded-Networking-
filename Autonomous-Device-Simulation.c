#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SENSOR_COUNT 3
#define SERVER_PORT 8080
#define BUFFER_SIZE 256

// Sensör verisini temsil eden yapı
typedef struct {
    int id;
    float value;
} SensorData;

// Sensörleri temsil eden thread fonksiyonu
void* sensor_thread(void* arg) {
    SensorData* sensor = (SensorData*)arg;
    while (1) {
        sensor->value = (rand() % 100) / 10.0; // Rastgele veri üret
        printf("[Sensor %d] Veri: %.2f\n", sensor->id, sensor->value);
        sleep(2);
    }
    return NULL;
}

// TCP/IP sunucusu
void* server_thread(void* arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[Server] Bağlantı bekleniyor...\n");
    new_socket = accept(server_fd, NULL, NULL);
    printf("[Server] Bağlantı kabul edildi!\n");

    while (1) {
        recv(new_socket, buffer, BUFFER_SIZE, 0);
        printf("[Server] Gelen veri: %s\n", buffer);
        send(new_socket, "Alındı!", strlen("Alındı!"), 0);
    }

    return NULL;
}

int main() {
    pthread_t sensor_threads[SENSOR_COUNT];
    SensorData sensors[SENSOR_COUNT];
    pthread_t server;

    // Sensörleri başlat
    for (int i = 0; i < SENSOR_COUNT; i++) {
        sensors[i].id = i + 1;
        pthread_create(&sensor_threads[i], NULL, sensor_thread, &sensors[i]);
    }

    // Sunucu başlat
    pthread_create(&server, NULL, server_thread, NULL);

    // Threadleri beklet
    for (int i = 0; i < SENSOR_COUNT; i++) {
        pthread_join(sensor_threads[i], NULL);
    }
    pthread_join(server, NULL);
    
    return 0;
}
