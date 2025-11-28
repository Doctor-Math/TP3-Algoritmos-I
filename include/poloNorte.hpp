#ifndef POLO_NORTE_HPP
#define POLO_NORTE_HPP

#include <vector>
#include <cstdint>

/*
 * Classe responsável por calcular um conjunto independente máximo
 * em um grafo não direcionado usando a técnica de "meet-in-the-middle".
 *
 * As adjacências são representadas por máscaras de bits para permitir
 * operações rápidas de verificação de conflitos.
 */
class poloNorte {
private:
    // Número total de vértices
    int totalDuendes;

    // Particionamento do grafo em duas metades
    int tamanhoEsquerda;
    int tamanhoDireita;

    // Máscara global de adjacências (64 bits: suporta até 64 vértices)
    std::vector<uint64_t> conflitosDuendes;

    // Conflitos dos duendes da esquerda
    std::vector<uint32_t> conflitosEsquerda;

    // Conflitos dos duendes da direita
    std::vector<uint32_t> conflitosDireita;

    // Conflitos da esquerda com a direita
    std::vector<uint32_t> conflitosEsquerdaParaDireita;

    // Conflitos da direita com a esquerda
    std::vector<uint32_t> conflitosDireitaParaEsquerda;

    // Lista de máscaras que representam conjuntos independentes válidos da metade esquerda
    std::vector<uint32_t> mascarasIndependentesEsquerda;

    // Conta quantos bits iguais a 1 existem na máscara x
    // int contarBits(uint32_t x);

    // Converte uma máscara de bits para um vetor de índices globais
    void mascaraParaVetor(uint32_t mascara, int deslocamento, std::vector<int>& saida);

    // Retorna true se A for lexicograficamente menor que B
    static bool vetorLexMenor(const std::vector<int>& A, const std::vector<int>& B);

public:
    // Constrói o grafo e inicializa as estruturas de adjacência
    poloNorte(int n, const std::vector<std::pair<int,int>>& conflitos);

    // Resolve o problema do conjunto independente máximo e retorna os vértices escolhidos
    std::vector<int> formarEquipe();
};

#endif