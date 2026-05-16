#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILE_LINE_BUFFER_SIZE 256
#define ITEMS_ARR_SIZE 100 // TODO: GAMBIARRA! PENSAR EM MANEIRA MAIS ELEGANTE DEPOIS.

typedef struct {
    char *nome;
    float pesoKg;
    float valor;
    char *categoria;
} Item;

typedef struct {
    char *nome_fase; // Aparentemente ambos os "atributos" servem para a mesma coisa
    char regra[FILE_LINE_BUFFER_SIZE];;
    float capacidade;
    int qtde_itens;
    Item itens[];
} Fase;

int contar_itens_fase(FILE *f) {
    long pos_original = ftell(f);
    char linha[FILE_LINE_BUFFER_SIZE];
    int count = 0;

    while (fgets(linha, FILE_LINE_BUFFER_SIZE, f)) {
        if (strncmp(linha, "FASE:", 5) == 0 && count > 0)
            break; // Chegou na próxima fase, para
        if (strncmp(linha, "ITEM:", 5) == 0)
            count++;
    }

    fseek(f, pos_original, SEEK_SET); // Restaura ponteiro
    return count;
}

float calcular_beneficio(Item item) {
    return item.valor / item.pesoKg;
}
// Intercala dois subarrays de arr[].
// Subarray esquerdo é arr[esq..meio]
// Subarray direito é arr[meio..dir]

// Faz ajustes nos precos dos itens da categoria selecionada
// Util para as fases Floresta Encantada e Montanhas Geladas
void merge(Item arr[], int esq, int meio, int dir,
    char* categoria_valor_alterado, float multiplicador_categoria) {
    int i, j, k;
    int n1 = meio - esq + 1;
    int n2 = dir - meio;

    // Create temporary arrays
    Item leftArr[n1], rightArr[n2];

    // Copy data to temporary arrays
    for (i = 0; i < n1; i++)
        leftArr[i] = arr[esq + i];
    for (j = 0; j < n2; j++)
        rightArr[j] = arr[meio + 1 + j];

    // Merge the temporary arrays back into arr[left..right]
    i = 0;
    j = 0;
    k = esq;
    while (i < n1 && j < n2) {
        float beneficio_esq = calcular_beneficio(leftArr[i]);
        if (categoria_valor_alterado != NULL && strcmp(leftArr[i].categoria, categoria_valor_alterado) == 0) {
            beneficio_esq *= multiplicador_categoria;
        }

        float beneficio_dir = calcular_beneficio(rightArr[j]);
        if (categoria_valor_alterado != NULL && strcmp(rightArr[j].categoria, categoria_valor_alterado) == 0) {
            beneficio_dir *= multiplicador_categoria;
        }

        if (beneficio_esq >= beneficio_dir) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArr[], if any
    while (i < n1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArr[], if any
    while (j < n2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

// Ordena itens de forma decrescente pela razão Valor/Peso, considerando modificadores de preco da fase
// Use categoria_valor_alterado = NULL para ordenar sem considerar alterações de valor
void ordenar_itens_por_beneficio(Item arr[], int esq, int dir,
    char* categoria_valor_alterado, float modificador_valor) {
    if (esq < dir) {

        // Calculate the midpoint
        int meio = esq + (dir - esq) / 2;

        // Sort first and second halves
        ordenar_itens_por_beneficio(arr, esq, meio, categoria_valor_alterado, modificador_valor);
        ordenar_itens_por_beneficio(arr, meio + 1, dir, categoria_valor_alterado, modificador_valor);

        // Merge the sorted halves
        merge(arr, esq, meio, dir,categoria_valor_alterado, modificador_valor);
    }
}

// Floresta Encantada e Montanhas Geladas - Itens de certos tipos têm o seu valor alterado
void resolver_fase_com_ajuste_valor(Item itens_disponiveis[], int n, float qtde_selecionada[], float peso_maximo,
    char* categoria_valor_alterado, float multiplicador) {
    ordenar_itens_por_beneficio(itens_disponiveis, 0, n-1, categoria_valor_alterado, multiplicador);

    int i = 0;
    while (i < n && peso_maximo > 0.0) {
        float peso_atual = itens_disponiveis[i].pesoKg;

        if (peso_atual <= peso_maximo) {
            qtde_selecionada[i] = peso_atual;
            peso_maximo -= peso_atual;
            i++;
        } else {
            qtde_selecionada[i] = peso_maximo;
            peso_maximo = 0.0;
        }
    }
}

/*
Como mencionado em aula, não existe estratégia gulosa para obter solução ótima para o problema
da mochila binária.

Portanto, utilizei a mesma estratégia para resolver este caso, mesmo que não obtenha a melhor solução.
*/
void resolver_fase_itens_inteiros(Item itens_disponiveis[], int n, float peso_maximo, int selecionados[]) {

    ordenar_itens_por_beneficio(itens_disponiveis, 0, n-1, NULL, 1);

    int i = 0;
    int j = 0;
    while (i < n && peso_maximo > 0.0) {
        if (itens_disponiveis[i].pesoKg <= peso_maximo) {
            selecionados[j] = i; // Guarda o índice para o item selecionado
            peso_maximo -= itens_disponiveis[i].pesoKg;
            j++;
        }
        i++;
        // Se não for possivel incluir o item inteiro, ignora e passa para o próximo
    }
}

// Templo Subterrâneo: seleciona os 3 itens com maior benefício (valor/peso)
// que caibam na mochila, em uma única passagem O(n).
// selecionados[] deve ter seus elementos inicializados com -1.
void resolver_fase_top_3_beneficio(Item itens_disponiveis[], int n, int selecionados[3], float peso_maximo) {
    // Mantemos os índices dos 3 melhores encontrados até agora.
    // top[0] = melhor, top[1] = segundo, top[2] = terceiro

    for (int i = 0; i < n; i++) {
        // Ignora itens que não cabem na mochila
        if (itens_disponiveis[i].pesoKg > peso_maximo) continue;

        float beneficio_atual = calcular_beneficio(itens_disponiveis[i]);

        // Verifica se entra no top 3 (compara do pior para o melhor)
        for (int slot = 2; slot >= 0; slot--) {
            if (selecionados[slot] == -1 ||
                beneficio_atual > calcular_beneficio(itens_disponiveis[selecionados[slot]])) {

                // Empurra os piores uma posição para baixo
                if (slot < 2) selecionados[slot + 1] = selecionados[slot];
                selecionados[slot] = i;
                } else {
                    break; // Já achou a posição certa, não precisa continuar
                }
        }
    }
}
/*

    char *nome_fase; // Aparentemente ambos os "atributos" servem para a mesma coisa
    char regra[FILE_LINE_BUFFER_SIZE];;
    float capacidade;
    int qtde_itens;
    Item itens[];
*/
void resolver_fase(char nome_fase[], char regra[], float capacidade, int qtde_itens, Item itens[]) {
    if (strcmp(nome_fase, "Floresta Encantada") == 0) {
        float solucao[qtde_itens];
        resolver_fase_com_ajuste_valor(itens, qtde_itens, solucao, capacidade, "magico", 2.0f);

    } else if (strcmp(nome_fase, "Ruínas Perdidas") == 0) {
        int solucao[qtde_itens];

        resolver_fase_itens_inteiros(itens, qtde_itens, capacidade, solucao);

    } else if (strcmp(nome_fase, "Montanhas Geladas") == 0) {
        float solucao[qtde_itens];
        resolver_fase_com_ajuste_valor(itens, qtde_itens, solucao, capacidade, "sobrevivencia", 0.8f);

    }
    else if (strcmp(nome_fase, "Templo Subterrâneo") == 0) {
        int indices_solucao[3] = {-1, -1, -1};;
        resolver_fase_top_3_beneficio(itens, qtde_itens, indices_solucao, capacidade);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada_jogo.txt saida_jogo.txt\n", argv[0]);
        return 1;
    }

    char* path_entrada = argv[1];
    char* path_saida = argv[2];

    FILE* f_entrada = fopen(path_entrada, "r+");

    if (f_entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada: %s.\n", path_entrada);
        return 1;
    }

    char linha[FILE_LINE_BUFFER_SIZE];

    /*
    FASE: <nome da fase>
    CAPACIDADE: <capacidade da mochila em kg>
    REGRA: <identificador da regra especial>
    ITEM: <nome do item>, <peso em kg>, <valor em R$>, <tipo do item>
    ITEM: ...
    */
    char *nome_fase;
    char *regra;
    float capacidade;

    Item *itens = NULL;
    int qtde_itens_fase = 0;

    while (fgets(linha, FILE_LINE_BUFFER_SIZE, f_entrada)) {
        if (strncmp(linha, "FASE:", 5) == 0) {

            // Resolve a fase anterior antes de sobrescrever os dados
            if (itens != NULL) {
                // Fase fase = {nome_fase, regra, capacidade, qtde_itens_fase, itens};
                // resolver_fase(&fase);
                resolver_fase(nome_fase, regra, capacidade, qtde_itens_fase, itens);
                free(itens);
                itens = NULL;
            }

            qtde_itens_fase = 0;
            sscanf(linha, "FASE: %ms", &nome_fase); // %ms aloca a string automaticamente

            qtde_itens_fase = contar_itens_fase(f_entrada);

            itens = malloc(qtde_itens_fase * sizeof(Item));
            if (itens == NULL) {
                fprintf(stderr, "Erro de alocação de memória.\n");
                return 1;
            }
        } else if (strncmp(linha, "CAPACIDADE: ", strlen("CAPACIDADE: ")) == 0) {
            sscanf(linha, "CAPACIDADE: %f", &capacidade);

        } else if (strncmp(linha, "REGRA: ", strlen("REGRA: ")) == 0) {
            sscanf(linha, "REGRA: %ms", &regra);

        } else if (strncmp(linha, "ITEM: ", strlen("ITEM: ")) == 0) {
            sscanf(linha, "ITEM: %[^,], %f, %f, %[^\n]",
                itens[qtde_itens_fase].nome, &itens[qtde_itens_fase].pesoKg, &itens[qtde_itens_fase].valor, itens[qtde_itens_fase].categoria);

            qtde_itens_fase++;
        }

        // Não esquecer de resolver a última fase após o loop
        if (itens != NULL) {
            // Fase fase = {nome_fase, regra, capacidade, qtde_itens_fase, itens};
            resolver_fase(nome_fase, regra, capacidade, qtde_itens_fase, itens);
            free(itens);
        }
    }

    fclose(f_entrada);
    return 0;
}

/*
TODO`s:
 - Braba cabulosa: em como o vetor é ordenado em cada fase, é importante avaliar se o uso de índices
                   (vetor de bool ou vetor de indices) não pode causar bugs mais pra frente.

 - Implementar leitura do arquivo e parser para structs do tipo Item.\
 - Basicamente, implementar a função main inteira.
*/
