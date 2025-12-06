# 🎄 Trabalho Prático 3 — Algoritmos I  

## 🏷️ Status e Tecnologias
![Feito em C++](https://img.shields.io/badge/C%2B%2B-Feito%20em-blue)
![Badge Concluído](https://img.shields.io/badge/Status-Conclu%C3%ADdo-brightgreen)

### Maximum Independent Set com Meet-in-the-Middle

**Autor:** Matheus Soares dos Santos de Freitas  
**Matrícula:** 2024080043  
**Email:** matheussdsf@ufmg.br  

---

## 📌 Sobre o Projeto

O objetivo é resolver uma variação do **Maximum Independent Set (MIS)**: escolher o maior grupo de duendes que não possuem rivalidades.  
Em caso de empate entre conjuntos máximos, o critério de desempate é **lexicográfico** (o vetor de índices deve ser o menor possível).

Embora MIS seja NP-difícil, o limite de **N ≤ 40** permite uma solução eficiente usando:

- **Bitmasks**  
- **Meet-in-the-Middle (MITM)**  
- **DP em submáscaras (bottom-up)**  

O método final encontra o maior conjunto independente e garante a menor solução lexicográfica entre empates.

---

## 🧠 Estrutura da Solução

A lógica é concentrada na classe `poloNorte`, que contém:

- Máscaras de adjacência para cada duende (`conflitosDuendes`)  
- Separação em metade esquerda e direita (MITM)  
- Conflitos internos e cruzados entre as metades  
- Funções auxiliares para converter máscaras e comparar vetores lexicograficamente  

### Função principal: `formarEquipe()`

Opera em três etapas:

1. **Enumeração da metade esquerda**  
   - Gera todos os subconjuntos e filtra os independentes.

2. **DP na metade direita**  
   - Enumera todas as máscaras, seleciona as independentes e usa programação dinâmica sobre submáscaras para registrar a melhor solução possível para cada máscara.

3. **Combinação final (MITM)**  
   - Para cada subconjunto independente da esquerda, calcula-se a parte permitida à direita.  
   - A melhor combinação esquerda + direita é selecionada por tamanho e, em empate, por lexicografia.

---

## ⚙️ Complexidade

- **Tempo:**  
  \[
  O(n \cdot 2^{n/2})
  \]

- **Memória:**  
  \[
  O(2^{n/2})
  \]

Viável e eficiente para **n ≤ 40**.

---

## ✅ Conclusão

A abordagem combina bitmask, MITM e DP para resolver MIS de forma eficiente e determinística.  
Além de encontrar o maior subconjunto independente, garante o **menor vetor lexicográfico**, atendendo ao requisito temático do problema.

---

## 📚 Referências

- Cormen et al. *Introduction to Algorithms*  
- Kleinberg & Tardos. *Algorithm Design*  
- Garey & Johnson. *Computers and Intractability*  
