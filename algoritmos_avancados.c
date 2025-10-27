#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_HASH 10

// ---------- ESTRUTURAS ---------- //
typedef struct Sala {
    char nome[30];
    char pista[50];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

typedef struct PistaBST {
    char pista[50];
    struct PistaBST *esq;
    struct PistaBST *dir;
} PistaBST;

typedef struct Hash {
    char pista[50];
    char suspeito[30];
    struct Hash *prox;
} Hash;

// ---------- FUNÇÕES DE SALAS ---------- //
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esq = nova->dir = NULL;
    return nova;
}

// ---------- FUNÇÕES DE ÁRVORE DE PISTAS (BST) ---------- //
PistaBST* inserirPista(PistaBST *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaBST *nova = (PistaBST*)malloc(sizeof(PistaBST));
        strcpy(nova->pista, pista);
        nova->esq = nova->dir = NULL;
        return nova;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

void listarPistas(PistaBST *raiz) {
    if (raiz != NULL) {
        listarPistas(raiz->esq);
        printf(" - %s\n", raiz->pista);
        listarPistas(raiz->dir);
    }
}

// ---------- FUNÇÕES DE HASH ---------- //
int funcaoHash(const char *chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++)
        soma += chave[i];
    return soma % TAM_HASH;
}

void inserirNaHash(Hash *tabela[], const char *pista, const char *suspeito) {
    int h = funcaoHash(pista);
    Hash *novo = (Hash*)malloc(sizeof(Hash));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabela[h];
    tabela[h] = novo;
}

const char* encontrarSuspeito(Hash *tabela[], const char *pista) {
    int h = funcaoHash(pista);
    Hash *atual = tabela[h];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0)
            return atual->suspeito;
        atual = atual->prox;
    }
    return "Desconhecido";
}

// ---------- EXPLORAÇÃO DA MANSÃO ---------- //
void explorarSalas(Sala *atual, PistaBST **bst, Hash *tabela[]) {
    if (atual == NULL) return;

    printf("\nVocê está em: %s\n", atual->nome);
    if (strlen(atual->pista) > 0) {
        printf("Você encontrou uma pista: '%s'\n", atual->pista);
        *bst = inserirPista(*bst, atual->pista);
    }

    char opcao;
    printf("Escolha (e = esquerda, d = direita, s = sair): ");
    scanf(" %c", &opcao);
    opcao = tolower(opcao);

    if (opcao == 'e')
        explorarSalas(atual->esq, bst, tabela);
    else if (opcao == 'd')
        explorarSalas(atual->dir, bst, tabela);
    else if (opcao == 's')
        return;
    else
        printf("Opção inválida!\n");
}

// ---------- JULGAMENTO FINAL ---------- //
void verificarSuspeitoFinal(PistaBST *raiz, Hash *tabela[]) {
    char acusado[30];
    printf("\n--- FASE FINAL ---\n");
    printf("Pistas coletadas:\n");
    listarPistas(raiz);

    printf("\nQuem você acha que é o culpado? ");
    scanf(" %[^\n]", acusado);

    // Contar pistas ligadas ao acusado
    int contador = 0;

    if (raiz != NULL) {
        if (strcmp(encontrarSuspeito(tabela, raiz->pista), acusado) == 0)
            contador++;
        if (raiz->esq != NULL)
            verificarSuspeitoFinal(raiz->esq, tabela);
        if (raiz->dir != NULL)
            verificarSuspeitoFinal(raiz->dir, tabela);
    }

    if (contador >= 2)
        printf("\n Acusação correta! %s é realmente o culpado!\n", acusado);
    else
        printf("\n Acusação incorreta! %s é inocente.\n", acusado);
}

// ---------- LIBERAÇÃO DE MEMÓRIA ---------- //
void liberarSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

void liberarPistas(PistaBST *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

void liberarHash(Hash *tabela[]) {
    for (int i = 0; i < TAM_HASH; i++) {
        Hash *atual = tabela[i];
        while (atual != NULL) {
            Hash *tmp = atual;
            atual = atual->prox;
            free(tmp);
        }
    }
}

// ---------- MAIN ---------- //
int main() {
    // Montar a mansão (árvore binária)
    Sala *entrada = criarSala("Entrada", "Pegadas na lama");
    entrada->esq = criarSala("Biblioteca", "Livro rasgado");
    entrada->dir = criarSala("Cozinha", "Faca ensanguentada");
    entrada->esq->esq = criarSala("Escritório", "Bilhete ameaçador");
    entrada->esq->dir = criarSala("Jardim", "Luvas sujas");
    entrada->dir->esq = criarSala("Adega", "Taça quebrada");
    entrada->dir->dir = criarSala("Quarto", "Colar perdido");

    // Criar tabela hash com associações pista → suspeito
    Hash *tabela[TAM_HASH] = {NULL};
    inserirNaHash(tabela, "Pegadas na lama", "Jardineiro");
    inserirNaHash(tabela, "Livro rasgado", "Secretária");
    inserirNaHash(tabela, "Faca ensanguentada", "Chef");
    inserirNaHash(tabela, "Bilhete ameaçador", "Secretária");
    inserirNaHash(tabela, "Luvas sujas", "Jardineiro");
    inserirNaHash(tabela, "Taça quebrada", "Chef");
    inserirNaHash(tabela, "Colar perdido", "Dona da Mansão");

    PistaBST *pistas = NULL;

    printf("=== DETECTIVE QUEST ===\n");
    printf("Explore a mansão e colete pistas!\n");

    explorarSalas(entrada, &pistas, tabela);
    verificarSuspeitoFinal(pistas, tabela);

    liberarSalas(entrada);
    liberarPistas(pistas);
    liberarHash(tabela);

    return 0;
}
