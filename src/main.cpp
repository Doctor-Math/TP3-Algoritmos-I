#include "poloNorte.hpp"
#include <iostream>
#include <vector>

int main() {
    int numDuendes, numConflitos;
    
    // Lê a quantidade de vértices (N) e de arestas de conflito (M)
    std::cin >> numDuendes >> numConflitos;

    // Vetor onde cada elemento é um par (a, b) representando um conflito entre vértices
    std::vector<std::pair<int,int>> conflitos(numConflitos);

    // Lê os conflitos fornecidos na entrada
    for (int i = 0; i < numConflitos; i++) {
        int a, b;
        std::cin >> a >> b;
        conflitos[i] = std::make_pair(a, b);
    }

    // Cria o solucionador para o problema do conjunto independente
    poloNorte duendes(numDuendes, conflitos);

    // Executa o algoritmo e obtém o conjunto independente máximo encontrado
    std::vector<int> resultado = duendes.formarEquipe();

    // Imprime o tamanho do conjunto independente
    std::cout << resultado.size() << "\n";

    // Imprime os vértices escolhidos, separados por espaço
    for (size_t i = 0; i < resultado.size(); i++) {
        if (i > 0)
            std::cout << " ";
        std::cout << resultado[i];
    }
    std::cout << "\n";

    return 0;
}