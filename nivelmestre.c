#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_SIZE 101    /* tamanho da tabela hash */
#define MAX_STR 128

/* --- Estruturas --- */

/* Nó da árvore binária de cômodos */
typedef struct Sala {
    char nome[MAX_STR];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

/* Nó da BST de pistas (armazena string da pista) */
typedef struct PistaNode {
    char pista[MAX_STR];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/* Associação pista -> suspeito na tabela hash (encadeamento) */
typedef struct HashNode {
    char pista[MAX_STR];
    char suspeito[MAX_STR];
    struct HashNode *prox;
} HashNode;

/* Tabela hash */
typedef struct {
    HashNode *v[HASH_SIZE];
} HashTable;

/* --- Protótipos --- */

/* criarSala() – cria dinamicamente um cômodo. */
Sala *criarSala(const char *nome);

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas. */
void explorarSalas(Sala *root, PistaNode **colecao, HashTable *ht);

/* inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas. */
PistaNode *inserirPista(PistaNode *root, const char *pista);
void listarPistasInorder(PistaNode *root);
int contarPistas(PistaNode *root);

/* inserirNaHash() – insere associação pista/suspeito na tabela hash. */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito);

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista. */
const char *encontrarSuspeito(HashTable *ht, const char *pista);

/* verificarSuspeitoFinal() – conduz à fase de julgamento final. */
void verificarSuspeitoFinal(PistaNode *colecao, HashTable *ht, const char *acusado);

/* Funções auxiliares */
unsigned long hash_djb2(const char *str);
void initHashTable(HashTable *ht);
void liberarHash(HashTable *ht);
void liberarPistas(PistaNode *root);
void liberarSalas(Sala *root);

/* Dada o nome da sala, retorna a pista associada por regra codificada */
const char *pistaParaSala(const char *nome);

/* --- Implementação --- */

/* criarSala() – cria dinamicamente um cômodo.
   Recebe o nome do cômodo, aloca e inicializa o nó da árvore de salas. */
Sala *criarSala(const char *nome) {
    Sala *s = (Sala *)malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro de memória ao criar sala\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAX_STR-1);
    s->nome[MAX_STR-1] = '\0';
    s->esq = s->dir = NULL;
    return s;
}

/* inserirNaHash() – insere associação pista/suspeito na tabela hash.
   Usa encadeamento; se a pista já existir, sobrescreve o suspeito. */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashNode *cur = ht->v[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            /* atualiza suspeito */
            strncpy(cur->suspeito, suspeito, MAX_STR-1);
            cur->suspeito[MAX_STR-1] = '\0';
            return;
        }
        cur = cur->prox;
    }
    /* insere no início da lista */
    HashNode *n = (HashNode *)malloc(sizeof(HashNode));
    if (!n) { fprintf(stderr, "Erro de memória na hash\n"); exit(EXIT_FAILURE); }
    strncpy(n->pista, pista, MAX_STR-1);
    n->pista[MAX_STR-1] = '\0';
    strncpy(n->suspeito, suspeito, MAX_STR-1);
    n->suspeito[MAX_STR-1] = '\0';
    n->prox = ht->v[h];
    ht->v[h] = n;
}

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
   Retorna ponteiro para string interna (não liberar) ou NULL se não encontrado. */
const char *encontrarSuspeito(HashTable *ht, const char *pista) {
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashNode *cur = ht->v[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

/* inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas.
   Mantém ordem. Se já existir a pista, não insere. */
PistaNode *inserirPista(PistaNode *root, const char *pista) {
    if (!root) {
        PistaNode *n = (PistaNode *)malloc(sizeof(PistaNode));
        if (!n) { fprintf(stderr, "Erro de memória na BST de pistas\n"); exit(EXIT_FAILURE); }
        strncpy(n->pista, pista, MAX_STR-1);
        n->pista[MAX_STR-1] = '\0';
        n->esq = n->dir = NULL;
        printf("Pista \"%s\" adicionada à coleção.\n", pista);
        return n;
    }
    int cmp = strcmp(pista, root->pista);
    if (cmp == 0) {
        printf("Você já coletou a pista \"%s\" antes. Não duplicando.\n", pista);
        return root;
    } else if (cmp < 0) {
        root->esq = inserirPista(root->esq, pista);
    } else {
        root->dir = inserirPista(root->dir, pista);
    }
    return root;
}

/* imprimir as pistas coletadas em ordem */
void listarPistasInorder(PistaNode *root) {
    if (!root) return;
    listarPistasInorder(root->esq);
    printf(" - %s\n", root->pista);
    listarPistasInorder(root->dir);
}

int contarPistas(PistaNode *root) {
    if (!root) return 0;
    return 1 + contarPistas(root->esq) + contarPistas(root->dir);
}

/* verificarSuspeitoFinal() – conduz à fase de julgamento final.
   Conta quantas pistas na coleção apontam para o suspeito acusado.
   Se >= 2 -> acusação sustentada; senão -> insuficiente. */
void verificarSuspeitoFinal(PistaNode *colecao, HashTable *ht, const char *acusado) {
    if (!colecao) {
        printf("Nenhuma pista coletada. Acusação impossível.\n");
        return;
    }
    /* Faz travessia e conta */
    int contador = 0;
    /* função interna recursiva */
    void contarRec(PistaNode *node) {
        if (!node) return;
        contarRec(node->esq);
        const char *s = encontrarSuspeito(ht, node->pista);
        if (s && strcmp(s, acusado) == 0) contador++;
        contarRec(node->dir);
    }
    contarRec(colecao);
    printf("\nResultado do julgamento contra \"%s\":\n", acusado);
    if (contador >= 2) {
        printf("=> Acusação SUSTENTADA! %d pista(s) apontam para %s.\n", contador, acusado);
    } else if (contador == 1) {
        printf("=> Acusação FRACA. Apenas 1 pista aponta para %s.\n", acusado);
    } else {
        printf("=> Acusação INSUFICIENTE. Nenhuma pista aponta para %s.\n", acusado);
    }
}

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas.
   Interatividade: e (esquerda), d (direita), s (sair/terminar exploração).
   Ao entrar em uma sala, exibe sua pista (se existir) e pergunta se jogador quer coletar. */
void explorarSalas(Sala *root, PistaNode **colecao, HashTable *ht) {
    if (!root) return;
    Sala *atual = root;
    char cmd[8];
    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        const char *pista = pistaParaSala(atual->nome);
        if (pista && strlen(pista) > 0) {
            printf("Você encontra uma pista: \"%s\"\n", pista);
            printf("Deseja coletar esta pista? (s/n): ");
            if (!fgets(cmd, sizeof(cmd), stdin)) exit(EXIT_FAILURE);
            if (tolower(cmd[0]) == 's') {
                *colecao = inserirPista(*colecao, pista);
            } else {
                printf("Você deixou a pista para trás.\n");
            }
        } else {
            printf("Nenhuma pista óbvia nesta sala.\n");
        }

        /* opções de movimento */
        printf("\nEscolha: (e) esquerda  (d) direita  (s) sair da exploração e iniciar julgamento\n");
        printf("Digite sua escolha: ");
        if (!fgets(cmd, sizeof(cmd), stdin)) exit(EXIT_FAILURE);
        char c = tolower(cmd[0]);
        if (c == 'e') {
            if (atual->esq) {
                atual = atual->esq;
            } else {
                printf("Não há sala à esquerda. Permanece em %s.\n", atual->nome);
            }
        } else if (c == 'd') {
            if (atual->dir) {
                atual = atual->dir;
            } else {
                printf("Não há sala à direita. Permanece em %s.\n", atual->nome);
            }
        } else if (c == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            break;
        } else {
            printf("Comando inválido. Tente novamente.\n");
        }
    }
}

/* --- Auxiliares --- */

/* djb2 hash (string para unsigned long) */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned char)c; /* hash * 33 + c */
    return hash;
}

void initHashTable(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->v[i] = NULL;
}

void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = ht->v[i];
        while (cur) {
            HashNode *t = cur;
            cur = cur->prox;
            free(t);
        }
        ht->v[i] = NULL;
    }
}

void liberarPistas(PistaNode *root) {
    if (!root) return;
    liberarPistas(root->esq);
    liberarPistas(root->dir);
    free(root);
}

void liberarSalas(Sala *root) {
    if (!root) return;
    liberarSalas(root->esq);
    liberarSalas(root->dir);
    free(root);
}

/* Mapeamento codificado: associa um nome de sala a uma pista.
   Se não houver pista, retorna NULL (ou string vazia). */
const char *pistaParaSala(const char *nome) {
    /* Utiliza as pistas definidas anteriormente */
    if (strcmp(nome, "Hall de Entrada") == 0) return "Um broche dourado caído no chão.";
    if (strcmp(nome, "Sala de Estar") == 0) return "Um livro aberto com anotações estranhas.";
    if (strcmp(nome, "Cozinha") == 0) return "Uma xícara de chá ainda quente.";
    if (strcmp(nome, "Biblioteca") == 0) return "Uma carta rasgada com a assinatura do mordomo.";
    if (strcmp(nome, "Jardim Interno") == 0) return ""; /* sem pista */
    if (strcmp(nome, "Despensa") == 0) return "Pegadas de sapato molhado.";
    if (strcmp(nome, "Sala de Jantar") == 0) return "Um colar quebrado sobre a mesa.";
    return NULL;
}

/* -- main: monta o mapa, pré-carrega hash com pistas->suspeitos e inicia jogo --- */
int main(void) {
    printf("=== Detective Quest — Enigma Studios ===\n");
    printf("Bem-vindo(a)! Explore a mansão, colete pistas e acuse o culpado.\n\n");

    /* Montagem manual (fixa) do mapa da mansão — árvore binária
           Hall de Entrada
            /             \
       Sala de Estar     Cozinha
         /      \         /     \
    Biblioteca Jardim  Despensa Sala de Jantar
    */
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim Interno");
    Sala *despensa = criarSala("Despensa");
    Sala *salaJantar = criarSala("Sala de Jantar");

    /* ligações */
    hall->esq = salaEstar;
    hall->dir = cozinha;
    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;
    cozinha->esq = despensa;
    cozinha->dir = salaJantar;

    /* Inicializa tabela hash e popula com pistas -> suspeitos (mapa pré-definido) */
    HashTable ht;
    initHashTable(&ht);

    /* Associações pré-definidas (pistas como chaves, suspeitos como valores) */
    inserirNaHash(&ht, "Um broche dourado caído no chão.", "Sr. Black");
    inserirNaHash(&ht, "Um livro aberto com anotações estranhas.", "Sra. White");
    inserirNaHash(&ht, "Uma xícara de chá ainda quente.", "Sr. Green");
    inserirNaHash(&ht, "Uma carta rasgada com a assinatura do mordomo.", "Mordomo");
    inserirNaHash(&ht, "Pegadas de sapato molhado.", "Sr. Black");
    inserirNaHash(&ht, "Um colar quebrado sobre a mesa.", "Sra. White");
    /* nota: Jardim Interno não tem pista, logo não precisa mapear */

    /* coleção de pistas inicialmente vazia (BST) */
    PistaNode *colecao = NULL;

    /* Inicia exploração */
    explorarSalas(hall, &colecao, &ht);

    /* Fase final: listar pistas e pedir acusação */
    printf("\n=== FASE DE JULGAMENTO ===\n");
    int total = contarPistas(colecao);
    if (total == 0) {
        printf("Você não coletou pistas. Não é possível formar uma acusação com base em evidências.\n");
    } else {
        printf("Pistas coletadas (%d):\n", total);
        listarPistasInorder(colecao);

        char acusado[MAX_STR];
        printf("\nQuem você acusa? Digite o nome do suspeito (ex.: \"Sr. Black\"): ");
        if (!fgets(acusado, sizeof(acusado), stdin)) exit(EXIT_FAILURE);
        /* remove newline */
        acusado[strcspn(acusado, "\n")] = '\0';
        /* trim de espaços iniciais/finais */
        char *start = acusado;
        while (*start && isspace((unsigned char)*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) { *end = '\0'; end--; }

        if (strlen(start) == 0) {
            printf("Nome do acusado vazio. Encerrando sem acusação.\n");
        } else {
            verificarSuspeitoFinal(colecao, &ht, start);
        }
    }

    /* limpa memória */
    liberarPistas(colecao);
    liberarHash(&ht);
    liberarSalas(hall);

    printf("\nObrigado por jogar Detective Quest!\n");
    return 0;
}
