#include "poloNorte.hpp"
#include <iostream>
#include <vector>

int main() {
    int numDuendes, numConflitos;
    
    // Lê a quantidade de duendes e de conflito
    std::cin >> numDuendes >> numConflitos;

    // Vetor onde cada elemento é um par (a, b) representando um conflito entre duendes
    std::vector<std::pair<int,int>> conflitos(numConflitos);

    // Lê os conflitos fornecidos na entrada
    for (int i = 0; i < numConflitos; i++) {
        int duendeA, duendeB;
        std::cin >> duendeA >> duendeB;
        conflitos[i] = std::make_pair(duendeA, duendeB);
    }

    // Cria o grafo com os duendes e suas arestas de conflito
    poloNorte duendes(numDuendes, conflitos);

    // Executa o algoritmo e obtém a maior equipe sem conflitos (conjunto independente máximo) encontrada
    std::vector<int> equipeFinal = duendes.formarEquipe();

    // Imprime o tamanho da equipe de duendes
    std::cout << equipeFinal.size() << "\n";

    // Imprime os duendes escolhidos
    for (size_t i = 0; i < equipeFinal.size(); i++) {
        if (i > 0)
            std::cout << " ";
        std::cout << equipeFinal[i];
    }
    std::cout << "\n";

    return 0;
}