#include "poloNorte.hpp"
#include <iostream>
/*
 * Converte uma máscara de bits em um vetor de índices globais.
 * - mascara: bitmask indicando os elementos selecionados.
 * - deslocamento: offset para transformar índices relativos em índices globais.
 * - saida: vetor preenchido com os índices correspondentes aos bits 1. */
void poloNorte::mascaraParaVetor(uint32_t mascara, int deslocamento,
                                            std::vector<int>& idsDuendesDoSubConj) {
    idsDuendesDoSubConj.clear();
    int i = 0;
    while (mascara) {
        if (mascara & 1u)
            idsDuendesDoSubConj.push_back(i + deslocamento);
        mascara >>= 1u;
        i++;
    }
}

/*
 * Compara vetores lexicograficamente.
 * Retorna true se A < B na ordem lexicográfica.
 * Usado para desempatar subconjuntos com mesmo tamanho.
 */
bool poloNorte::vetorLexMenor(const std::vector<int>& equipeDuendesA,
                                         const std::vector<int>& equipeDuendesB) {
    size_t n = std::min(equipeDuendesA.size(), equipeDuendesB.size());
    for (size_t i = 0; i < n; i++) {
        if (equipeDuendesA[i] < equipeDuendesB[i]) return true;
        if (equipeDuendesA[i] > equipeDuendesB[i]) return false;
    }
    return equipeDuendesA.size() < equipeDuendesB.size();
}

/*
 * Construtor da estrutura.
 * Divide o conjunto total em duas metades (programação meet-in-the-middle).
 * Preprocessa máscaras de adjacência para acelerar as operações de independência.
 */
poloNorte::poloNorte(
        int n, const std::vector<std::pair<int,int>>& conflitos) {

    totalDuendes = n;

    // Divide o conjunto em duas metades
    tamanhoEsquerda = n / 2;
    tamanhoDireita  = n - tamanhoEsquerda;

    // Máscara de adjacência global (64 bits para segurança)
    conflitosDuendes.assign(n, 0ULL);

    /*
     * Constrói a matriz de adjacência em formato condizente com bitsets.
     * Cada duende "a" guarda um bit por duende "b" com quem não pode coexistir.
     */
    for (auto &p : conflitos) {
        int a = p.first, b = p.second;
        if (a != b) {
            conflitosDuendes[a] |= (1ULL << b);
            conflitosDuendes[b] |= (1ULL << a);
        }
    }

    /*
     * Pré-processamento: separa adjacências entre:
     * - esquerda × esquerda
     * - direita × direita
     * - esquerda × direita
     * - direita × esquerda
     * para evitar operações com shifts repetidos na fase DP.
     */
    conflitosEsquerda.assign(tamanhoEsquerda, 0u);
    conflitosDireita.assign(tamanhoDireita, 0u);
    conflitosEsquerdaParaDireita.assign(tamanhoEsquerda, 0u);
    conflitosDireitaParaEsquerda.assign(tamanhoDireita, 0u);

    // Preenche adjacências referentes aos conflitos internos e cruzados da metade esquerda
    for (int idDuendeEsq = 0; idDuendeEsq < tamanhoEsquerda; idDuendeEsq++) {
        uint64_t conflitosDuendeEsq = conflitosDuendes[idDuendeEsq];

        // Esquerda → Esquerda
        for (int idOutroDuendeEsq = 0; idOutroDuendeEsq < tamanhoEsquerda; idOutroDuendeEsq++)
            if (conflitosDuendeEsq & (1ULL << idOutroDuendeEsq)) // Se houver conflito entre os duendes da esquerda
                conflitosEsquerda[idDuendeEsq] |= (1u << idOutroDuendeEsq);

        // Esquerda → Direita
        for (int idOutroDuendeDir = 0; idOutroDuendeDir < tamanhoDireita; idOutroDuendeDir++)
            if (conflitosDuendeEsq & (1ULL << (idOutroDuendeDir + tamanhoEsquerda))) // Se houver conflitos entre o duende da esquerda com o da direita
                conflitosEsquerdaParaDireita[idDuendeEsq] |= (1u << idOutroDuendeDir);
    }

    // Repete para os duendes da direita
    for (int idDuendeDir = 0; idDuendeDir < tamanhoDireita; idDuendeDir++) {
        uint64_t conflitosDuendeDir = conflitosDuendes[idDuendeDir + tamanhoEsquerda];

        // Direita → Direita
        for (int idOutroDuendeDir = 0; idOutroDuendeDir < tamanhoDireita; idOutroDuendeDir++)
            if (conflitosDuendeDir & (1ULL << (idOutroDuendeDir + tamanhoEsquerda))) // Se houver conflito entre os duendes da direita
                conflitosDireita[idDuendeDir] |= (1u << idOutroDuendeDir);

        // Direita → Esquerda
        for (int idOutroDuendeEsq = 0; idOutroDuendeEsq < tamanhoEsquerda; idOutroDuendeEsq++)
            if (conflitosDuendeDir & (1ULL << idOutroDuendeEsq)) // Se houver conflitos entre o duende da direita com o da esquerda
                conflitosDireitaParaEsquerda[idDuendeDir] |= (1u << idOutroDuendeEsq);
    }
}

/*
 * Resolve o problema: retorna o maior conjunto independente
 * (com desempate lexicográfico mínimo).
 *
 * A estratégia é meet-in-the-middle:
 * 1. Gera todos subconjuntos independentes na metade esquerda.
 * 2. Usa DP para calcular, para cada máscara da direita, o melhor subconjunto permitido.
 * 3. Combina cada solução da esquerda com sua melhor solução da direita.
 */
std::vector<int> poloNorte::formarEquipe() {

    // ------------------------
    // ETAPA 1: Encontrar os conjuntos (máscaras) independentes dos duendes da esquerda
    // ------------------------

    int totalCombinacoesEsq = 1 << tamanhoEsquerda; // Calcula todas as combinações possíveis entre os duendes da esquerda
    mascarasIndependentesEsquerda.clear();

    for (uint32_t equipeDuendes = 0; equipeDuendes < (uint32_t)totalCombinacoesEsq; equipeDuendes++) {
        bool equipeValida = true;
        uint32_t equipeDuendesTeste = equipeDuendes; // Faz uma cópia da lista de duendes do subgrupo

        // Enquanto ainda houver duendes numa equipe válida
        while (equipeDuendesTeste && equipeValida) {
            uint32_t duendeMenorId = equipeDuendesTeste & -equipeDuendesTeste; // Isola o duende de menor índice ("último da fila")
            int idDoDuende = __builtin_ctz(equipeDuendesTeste); // Conta os zeros à direita para obter o id do duende
            if (conflitosEsquerda[idDoDuende] & equipeDuendes) // Se o duende tem algum conflito com algum duende da equipe (interseção)
                equipeValida = false; // Esse subgrupo não é possível
            equipeDuendesTeste ^= duendeMenorId; // Remove o duende da cópia (passa pro próximo)
        }

        // Se o subgrupo não tem conflitos entre si
        if (equipeValida)
            mascarasIndependentesEsquerda.push_back(equipeDuendes);
    }

    // ------------------------
    // ETAPA 2: Memoização para a metade direita
    // ------------------------

    int totalCombinacoesDir = 1 << tamanhoDireita; // Calcula todas as combinações de duendes possíveis

    std::vector<int> tamanhoEquipes(totalCombinacoesDir, -1); // Vetor com os tamanhos das equipes (inicializados com -1)
    std::vector<uint32_t> equipes(totalCombinacoesDir, 0); // Vetor com as máscaras de bits das equipes

    // 2.1 — Gerar conjuntos (máscaras) independentes de duendes da direita 
    for (uint32_t equipeDuendes = 0; equipeDuendes < (uint32_t)totalCombinacoesDir; equipeDuendes++) {
        bool equipeValida = true;
        uint32_t equipeDuendesTeste = equipeDuendes;

        // Enquanto ainda houver duendes numa equipe válida
        while (equipeDuendesTeste && equipeValida) {
            uint32_t duendeMenorId = equipeDuendesTeste & -equipeDuendesTeste; // Isola o duende de menor índice ("último da fila")
            int idDoDuende = __builtin_ctz(equipeDuendesTeste); // Conta os zeros à direita para obter o id do duende
            if (conflitosDireita[idDoDuende] & equipeDuendes) // Se o duende tem algum conflito com algum duende do subgrupo (interseção)
                equipeValida = false;
            equipeDuendesTeste ^= duendeMenorId; // Remove o duende da cópia (passa pro próximo)
        }

        if (equipeValida) {
            tamanhoEquipes[equipeDuendes] = __builtin_popcount(equipeDuendes); // Recebe o tamanho da equipe
            equipes[equipeDuendes]    = equipeDuendes; // Recebe a máscara de bits da equipe
        }
    }

    // 2.2 — Correção de submáscaras inválidas (substituição pelo maior conj. indep. dentro delas) 
    for (uint32_t equipe = 0; equipe < (uint32_t)totalCombinacoesDir; equipe++) {
        for (int idDuende = 0; idDuende < tamanhoDireita; idDuende++) {
            if (equipe & (1u << idDuende)) { // Se a equipe contiver o duende
                uint32_t subequipe = equipe ^ (1u << idDuende); // Remove o duende da equipe
                bool subMelhor =
                // Se a submáscara da equipe possuir tamanho maior, é sinal de que a máscara da equipe é inválida (tamanho=-1) e temos que substitui-la.
                // Se for igual, então a equipe era inválida, se tornou válida e é, atualmente, alguma de suas submáscaras. Então ficamos com a menor lexicograficamente.
                // Se não, é uma máscara sempre válida e não faz nada
                (tamanhoEquipes[subequipe] > tamanhoEquipes[equipe]) ||
                    (tamanhoEquipes[subequipe] == tamanhoEquipes[equipe] &&
                     equipes[subequipe] < equipes[equipe]);

                // Se a subequipe for melhor (por tamanho ou lexicograficamente)
                if (subMelhor) {
                    tamanhoEquipes[equipe] = tamanhoEquipes[subequipe]; // Atualiza o tamanho
                    equipes[equipe]    = equipes[subequipe]; // Atualiza a máscara
                }
            }
        }
    }

    // ------------------------
    // ETAPA 3: Combinação final esquerda + melhor direita
    // ------------------------

    std::vector<int> melhorResultado; // Vetor com a maior equipe possível
    size_t melhorTamanho = 0;

    for (uint32_t mascaraEsq : mascarasIndependentesEsquerda) { // Percorre toda equipe (máscara) válida na esquerda

        // Converte a equipe (máscara) para um vetor
        std::vector<int> vetorEsq;
        mascaraParaVetor(mascaraEsq, 0, vetorEsq); 

        uint32_t bloqueioDireita = 0u; // Duendes da direita que tem algum conflito com os da equipeEsq (setados com 0)
        uint32_t m = mascaraEsq;

        // Iteração pelos duendes para obtenção dos duendes conflitantes da direita
        while (m) {
            uint32_t lsb = m & -m; // Isola o duende de menor id da equipe
            int idDuende = __builtin_ctz(m); // Pega o id do duende isolado
            bloqueioDireita |= conflitosEsquerdaParaDireita[idDuende]; // Insere duendes com conflito na máscara (operação OR)
            m ^= lsb; // Remove o duende da lista (passa pro próximo)
        }

        // Obtém os duendes da direita permitidos para essa equipe da esquerda
        uint32_t permitidasDireita =
            ((1u << tamanhoDireita) - 1u) & ~bloqueioDireita; // Faz o complemento dos proibidos apenas
            // para as posições relativas aos ids dos duendes da direita

        // Melhor equipe (máscara) da direita com estes duendes
        uint32_t melhorMascara = equipes[permitidasDireita];

        // Transforma esta máscara em vetor
        std::vector<int> vetorDireita;
        mascaraParaVetor(melhorMascara, tamanhoEsquerda, vetorDireita);

        // Combina os dois vetores (duendes da direita e da esquerda)
        std::vector<int> combinacao;
        combinacao.reserve(vetorEsq.size() + vetorDireita.size());

        size_t i = 0, j = 0;
        while (i < vetorEsq.size() && j < vetorDireita.size()) {
            if (vetorEsq[i] < vetorDireita[j]) combinacao.push_back(vetorEsq[i++]);
            else                                combinacao.push_back(vetorDireita[j++]);
        }
        while (i < vetorEsq.size())     combinacao.push_back(vetorEsq[i++]);
        while (j < vetorDireita.size()) combinacao.push_back(vetorDireita[j++]);

        // Atualiza a maior/melhor equipe
        size_t tamanhoAtual = combinacao.size();
        // Se o tamanho do atual for maior ou 
        // Se os tamanhos forem iguais e o vetor final estiver vazio ou o atual for lexicograficamente menor
        if (tamanhoAtual > melhorTamanho ||
            (tamanhoAtual == melhorTamanho &&
             (melhorResultado.empty() || vetorLexMenor(combinacao, melhorResultado)))) {

            melhorTamanho = tamanhoAtual; // Atualiza o tamanho
            melhorResultado = combinacao; // Atualiza combinação
        }
    }

    return melhorResultado;
}
