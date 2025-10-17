#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// DETECTIVE QUEST - Sistema de exploração da mansão

// Estrutura que representa cada sala da mansão
typedef struct sala {
    char nome[50];
    struct sala *esquerda;  // Ponteiro para a sala à esquerda
    struct sala *direita;   // Ponteiro para a sala à direita
} Sala;

// ---------------------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma nova sala com o nome fornecido
// ---------------------------------------------------------------
Sala* criarSala(char nome[]) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ---------------------------------------------------------------
// Função: explorarSalas
// Permite que o jogador explore a mansão a partir da sala atual.
// O jogador escolhe entre esquerda (e), direita (d) ou sair (s).
// A exploração termina quando chega a um cômodo sem caminhos ou o jogador decide encerrar.
// ---------------------------------------------------------------
void explorarSalas(Sala* salaAtual) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVocê está na %s.\n", salaAtual->nome);

        // Verifica se é um nó folha (sem caminhos)
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Não há mais caminhos. Fim da exploração!\n");
            break;
        }

        printf("Escolha o caminho (e = esquerda, d = direita, s = sair): ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (salaAtual->esquerda != NULL) {
                salaAtual = salaAtual->esquerda;
            } else {
                printf("Não há sala à esquerda!\n");
                break;
            }
        } else if (escolha == 'd' || escolha == 'D') {
            if (salaAtual->direita != NULL) {
                salaAtual = salaAtual->direita;
            } else {
                printf("Não há sala à direita!\n");
                break;
            }
        } else if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada pelo jogador.\n");
            break;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// ---------------------------------------------------------------
// Função: liberarArvore
// Libera a memória de todas as salas (liberação pós-ordem).
// ---------------------------------------------------------------
void liberarArvore(Sala* raiz) {
    if (raiz != NULL) {
        liberarArvore(raiz->esquerda);
        liberarArvore(raiz->direita);
        free(raiz);
    }
}

// ---------------------------------------------------------------
// Função principal: main
// Cria o mapa fixo da mansão e inicia a exploração.
// ---------------------------------------------------------------
int main() {
    // Montagem automática da árvore da mansão
    Sala* hall = criarSala("Hall de Entrada");
    hall->esquerda = criarSala("Sala de Estar");
    hall->direita = criarSala("Cozinha");

    hall->esquerda->esquerda = criarSala("Biblioteca");
    hall->esquerda->direita = criarSala("Jardim Interno");

    hall->direita->esquerda = criarSala("Despensa");
    hall->direita->direita = criarSala("Sala de Jantar");

    printf("=========================================\n");
    printf("     BEM-VINDO AO DETECTIVE QUEST!\n");
    printf(" Explore a mansão e descubra segredos!\n");
    printf("=========================================\n");

    printf("\nVocê começará no Hall de Entrada.\n");
    explorarSalas(hall);

    liberarArvore(hall);

    printf("\nMemória liberada. Programa encerrado.\n");
    return 0;
}
