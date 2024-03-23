#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sched.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)
#define MAX_MSG_SIZE 256

typedef struct {
    long msg_type;
    char msg_text[MAX_MSG_SIZE];
} message;

int msgid;
message msg;

void cleanNclose() {
    // Mesaj kuyruğunu sil
    msgctl(msgid, IPC_RMID, NULL);
    printf("Server kapatılıyor...\n");
}

void signal_handler(int signal) {
    // SIGINT (Ctrl-C) sinyali geldiğinde cleanup fonksiyonunu çağır
    cleanNclose();
    exit(0);
}

void process_message(const message* msg) {

    if (strcmp(msg->msg_text, "exit\n") == 0) {
        exit(1);
    } else {
        // Client'a mesajı gönder
        msgsnd(msgid, (void*)msg, strlen(msg->msg_text)+1, 0);
    }
    
}

void child_func(void* arg) {
    
    if (msgrcv(msgid, (void*)&msg, MAX_MSG_SIZE, 0, 0) == -1) {
                perror("msgrcv");
                exit(1);
    }
    if (strcmp(msg.msg_text, "exit\n") == 0) {
        printf("Child process: Exiting...\n");
        exit(0);
    }
    process_message(&msg);
}

void create_client_process(long client_id) {
    char* stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        exit(1);
    }
    // Clone ve yeni process için stack ayarlarını yap
    pid_t pid = clone(child_func, stack + STACK_SIZE, CLONE_VM | CLONE_VFORK, (void*)&client_id);
    if (pid == -1) {
        perror("clone");
        exit(1);
    }

    if (pid > 0) {
        // Ana thread
        wait(NULL);
        free(stack);
    } else {
        // Child thread
        exit(0);
    }
    
}

int main() {
    key_t key;
    message msg;
    
    // Ctrl-C (SIGINT) sinyali için signal_handler fonksiyonunu ayarla
    signal(SIGINT, signal_handler);

    // Özel bir anahtar oluştur
    key = ftok(".", 'S');

    // Mesaj kuyruğu oluştur
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
      
    printf("Sunucu başlatıldı. Mesaj bekleniyor...\n");

    while (1) {
          // Mesajı al
          if (msgrcv(msgid, (void*)&msg, MAX_MSG_SIZE, 0, 0) == -1) {
              perror("msgrcv");
              exit(1);
          }
          
          if (strcmp(msg.msg_text, "Ben olustum") == 0) {
            printf("\n");
            printf("Sunucu: Mesaj kimliği %ld client için worker thread oluşturuldu\n", msg.msg_type);
            // Gelen mesajı işle
            process_message(&msg);
            create_client_process(msg.msg_type);
          }
      }
  return 0;
}
