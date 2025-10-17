#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// --------------------------- Estruturas ------------------------------------

// Estrutura para representar cada sala da mansão
typedef struct sala {
    char nome[50];
    char pista[100];          // Pista encontrada no cômodo (pode estar vazia)
    struct sala *esquerda;    // Caminho à esquerda
    struct sala *direita;     // Caminho à direita
} Sala;

// Estrutura para representar cada nó da árvore de pistas (BST)
typedef struct pistaNode {
    char pista[100];
    struct pistaNode *esquerda;
    struct pistaNode *direita;
} PistaNode;

// --------------------------- Funções da Mansão -----------------------------

// Função: criarSala
// Cria dinamicamente um novo cômodo com nome e pista opcional
Sala* criarSala(char nome[], char pista[]) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a sala!\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// --------------------------- Funções da BST de Pistas ----------------------

// Função: inserirPista
// Insere uma nova pista na BST em ordem alfabética
PistaNode* inserirPista(PistaNode* raiz, char pista[]) {
    if (strlen(pista) == 0) return raiz; // ignora se não há pista

    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    // se for igual, não insere duplicado

    return raiz;
}

// Função: exibirPistas
// Exibe as pistas coletadas em ordem alfabética (in-ordem)
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}

// Função: liberarBST
// Libera toda a memória usada pela árvore de pistas
void liberarBST(PistaNode* raiz) {
    if (raiz != NULL) {
        liberarBST(raiz->esquerda);
        liberarBST(raiz->direita);
        free(raiz);
    }
}

// --------------------------- Exploração da Mansão --------------------------

// Função: explorarSalasComPistas
// Permite que o jogador explore os cômodos e colete pistas automaticamente
void explorarSalasComPistas(Sala* salaAtual, PistaNode** arvorePistas) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVocê está na %s.\n", salaAtual->nome);

        // Se a sala tiver uma pista, coleta automaticamente
        if (strlen(salaAtual->pista) > 0) {
            printf("🕵️  Pista encontrada: \"%s\"\n", salaAtual->pista);
            *arvorePistas = inserirPista(*arvorePistas, salaAtual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        printf("\nEscolha o caminho (e = esquerda, d = direita, s = sair): ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (salaAtual->esquerda != NULL)
                salaAtual = salaAtual->esquerda;
            else
                printf("Não há sala à esquerda!\n");
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (salaAtual->direita != NULL)
                salaAtual = salaAtual->direita;
            else
                printf("Não há sala à direita!\n");
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// Função: liberarArvoreSalas
// Libera toda a memória usada pela árvore da mansão
void liberarArvoreSalas(Sala* raiz) {
    if (raiz != NULL) {
        liberarArvoreSalas(raiz->esquerda);
        liberarArvoreSalas(raiz->direita);
        free(raiz);
    }
}

// --------------------------- Função principal ------------------------------

int main() {
    // Montagem fixa do mapa da mansão
    Sala* hall = criarSala("Hall de Entrada", "Um broche dourado caído no chão.");
    hall->esquerda = criarSala("Sala de Estar", "Um livro aberto com anotações estranhas.");
    hall->direita = criarSala("Cozinha", "Uma xícara de chá ainda quente.");

    hall->esquerda->esquerda = criarSala("Biblioteca", "Uma carta rasgada com a assinatura do mordomo.");
    hall->esquerda->direita = criarSala("Jardim Interno", "");
    hall->direita->esquerda = criarSala("Despensa", "Pegadas de sapato molhado.");
    hall->direita->direita = criarSala("Sala de Jantar", "Um colar quebrado sobre a mesa.");

    PistaNode* arvorePistas = NULL;

    printf("=========================================\n");
    printf("  DETECTIVE QUEST: A MANSÃO MISTERIOSA\n");
    printf("=========================================\n");
    printf("\nVocê começará no Hall de Entrada.\n");

    // Inicia a exploração interativa
    explorarSalasComPistas(hall, &arvorePistas);

    // Exibe as pistas coletadas
    printf("\n=========================================\n");
    printf("        PISTAS COLETADAS (A-Z)\n");
    printf("=========================================\n");

    if (arvorePistas == NULL)
        printf("Nenhuma pista foi coletada.\n");
    else
        exibirPistas(arvorePistas);

    // Liberação de memória
    liberarArvoreSalas(hall);
    liberarBST(arvorePistas);

    printf("\nMemória liberada. Fim da jornada do detetive!\n");
    return 0;
}
