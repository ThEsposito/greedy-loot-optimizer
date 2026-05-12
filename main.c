#include <stdio.h>
#include <string.h>

typedef struct {
    char *nome;
    float pesoKg;
    float valor;
    char *categoria;
} Item;

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada_jogo.txt saida_jogo.txt\n", argv[0]);
        return 1;
    }

    char* path_entrada = argv[1];
    char* path_saida = argv[2];

    return 0;
}
