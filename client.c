#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MAX_MSG_SIZE 256

typedef struct {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
} message;

int msgid;
pid_t listen_pid;
pid_t send_pid;

void signal_handler(int signal) {
    // SIGINT (Ctrl-C) sinyali geldiğinde client'ı sonlandır
    printf("Client kapatılıyor...\n");
    exit(0);
}

void receive_messages(long client_id) {
    message msg;
    while (1) {
        // Client'a gelen mesajı al
        if (msgrcv(msgid, (void*)&msg, MAX_MSG_SIZE, client_id, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        // Gelen mesajı yazdır
        printf("\nMesaj alındı: %s\n", msg.msg_text);
    }
}

void send_message(long client_id) {
    message msg;
    while (1) {
        char send_msg[MAX_MSG_SIZE];
        // Kime mesaj gönderileceğini ve mesaj içeriğini kullanıcıdan al
        printf("Kime mesaj göndermek istiyorsunuz? (İstemci kimliği): ");
        scanf("%ld", &msg.msg_type);
        getchar();
        
        printf("Mesajınızı girin: ", client_id);
        fgets(send_msg, MAX_MSG_SIZE, stdin);

        // Mesajı sunucuya gönder
        strncpy(msg.msg_text, send_msg, MAX_MSG_SIZE);
        msgsnd(msgid, (void*)&msg, strlen(msg.msg_text)+1, 0);
        
        // Check if the entered message is "exit"
        if (strcmp(send_msg, "exit\n") == 0) {
            printf("İstemci %ld: Çıkış yapılıyor...\n", client_id);
            
            kill(listen_pid, SIGKILL);
            kill(send_pid, SIGKILL);
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    key_t key;
    long client_id;
  
    // Ctrl-C (SIGINT) sinyali için signal_handler fonksiyonunu ayarla
    signal(SIGINT, signal_handler);

    if (argc != 2) {
        fprintf(stderr, "Kullanım: %s <istemci_kimlik>\n", argv[0]);
        exit(1);
    }

    // İstemci kimliğini al
    client_id = strtol(argv[1], NULL, 10);

    // Özel bir anahtar oluştur
    key = ftok(".", 'S');

    // Mesaj kuyruğunu al
    msgid = msgget(key, 0);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("İstemci %ld başlatıldı. Mesaj alışverişi yapabilirsiniz.\n", client_id);
    
    // "Ben oluştum" mesajını sunucuya gönder
    message hello;
    strncpy(hello.msg_text, "Ben olustum", MAX_MSG_SIZE);
    hello.msg_type = client_id;
    if (msgsnd(msgid, (void*)&hello, strlen(hello.msg_text) + 1, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    
    // Dinleme ve mesaj göndermek için iki ayrı process kullan
    listen_pid = fork();
    if (listen_pid == 0) {
        // Client dinleme süreci
        receive_messages(client_id);
        exit(0);
    } else if (listen_pid > 0) {
        // Main processs
        send_pid = fork();
        if (send_pid == 0) {
            // Client mesaj gönderme süreci
            send_message(client_id);
            exit(0);
        } else if (send_pid > 0) {
            // Main processs
            // Bekleme süreçlerinin tamamlanmasını bekle
            waitpid(listen_pid, NULL, 0);
            waitpid(send_pid, NULL, 0);
        } else {
            // Fork hatası
            perror("fork");
            exit(1);
        }
    } else {
        // Fork hatası
        perror("fork");
        exit(1);
    }

    send_message(client_id);
    receive_messages(client_id);

    return 0;
}
