#include "buffer.h"

#include <stdio.h>

void buffer_init(circular_buffer_t *buf) {
  buf->in = 0;
  buf->out = 0;
  buf->count = 0;
  pthread_mutex_init(&buf->mutex, NULL);
  pthread_cond_init(&buf->full, NULL);
  pthread_cond_init(&buf->empty, NULL);
}

void buffer_destroy(circular_buffer_t *buf) {
  pthread_mutex_destroy(&buf->mutex);
  pthread_cond_destroy(&buf->full);
  pthread_cond_destroy(&buf->empty);
}

void buffer_put(circular_buffer_t *buf, int value) {
  // TODO: Implementar a função do produtor
  // Passos:
  // 1. Adquirir o mutex
  // 2. Enquanto buffer estiver cheio, aguardar na condition variable "empty"
  // 3. Inserir o item no buffer circular (usar buf->in)
  // 4. Atualizar buf->in = (buf->in + 1) % BUFFER_SIZE
  // 5. Incrementar buf->count
  // 6. Sinalizar condition variable "full"
  // 7. Liberar o mutex

  pthread_mutex_lock(&buf->mutex);

  while(buf->count == BUFFER_SIZE){
    printf("Buffer está cheio no momento, produtor n consegue inserir mais itens");
    pthread_cond_wait(&buf->empty, &buf->mutex);
  }

  buf->buffer[buf->in] = value;
  buf->in = (buf->in + 1) % BUFFER_SIZE;
  buf->count++;
  pthread_cond_signal(&buf->full);

  pthread_mutex_unlock(&buf->mutex);

  printf("[PRODUCER] Inseriu %d (buffer count: %d)\n", value, buf->count);
}

int buffer_get(circular_buffer_t *buf) {
  int value;

  // TODO: Implementar a função do consumidor
  // Passos:
  // 1. Adquirir o mutex
  // 2. Enquanto buffer estiver vazio, aguardar na condition variable "full"
  // 3. Remover o item do buffer circular (usar buf->out)
  // 4. Atualizar buf->out = (buf->out + 1) % BUFFER_SIZE
  // 5. Decrementar buf->count
  // 6. Sinalizar condition variable "empty"
  // 7. Liberar o mutex

  pthread_mutex_lock(&buf->mutex);

  while(buf->count == 0){
    printf("Buffer está vazio no momento, consumidor n consegue retirar mais itens");
    pthread_cond_wait(&buf->full, &buf->mutex);
  }

  value = buf->buffer[buf->out];
  buf->out = (buf->out + 1) % BUFFER_SIZE;
  buf->count--;

  pthread_cond_signal(&buf->empty);

  pthread_mutex_unlock(&buf->mutex);

  printf("[CONSUMER] Removeu %d (buffer count: %d)\n", value, buf->count);
  return value;
}

int buffer_is_empty(circular_buffer_t *buf) {
  int esta_vazio;
  pthread_mutex_lock(&buf->mutex);
  esta_vazio = (buf->count == 0);
  pthread_mutex_unlock(&buf->mutex);

  return esta_vazio;
}

int buffer_is_full(circular_buffer_t *buf) {
  int esta_cheio;
  pthread_mutex_lock(&buf->mutex);
  esta_cheio = (buf->count == BUFFER_SIZE);
  pthread_mutex_unlock(&buf->mutex);

  return esta_cheio;
}