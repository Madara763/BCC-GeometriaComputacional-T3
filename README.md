# BCC-GeometriaComputacional-T3
Usado para desenvolvimento do terceiro trabalho da matéria de Geometria Computacional no curso de Ciencia da Computacao na UFPR 

Sobre as estruturas de dados:
Foi criado uma estrutura hiperplano que possui:
    d: os coeficientes produzidos pelo triângulo que o gera 
    normal: o vetor normal que indica o deslocamento do hiperplano, necessário para verificar em qual meio espaço um objeto está
    Para explicar a necessidade desses atributos, vamos começar com a definição do hiperplano em R^n que é:
        a1c1 + a2c2 + ... + aNcN + cN+1 = 0
    temos que normal são os c1,c2,..., cn: o vetor normal do plano
    e que d é o cN+1: o termo independente do plano
    funções/métodos:
        meioEspaco(ponto p): dá o valor que indica de qual lado do hiperplano o ponto está
        ladoSegmento(segmento s): retorna o lado no qual o segmento se encontra
        ladoTrianguglo(triangulo t): retorna o lado no qual o triângulo se encontra
É criado uma struct BSPNode, que representa um nó da árvore, ele é definido do seguinte modo:
    atributos:
        planoDivisor: um hiperplano, que separa o conjunto de triângulos em dois grupo (esquerda e direita)
        triangulosSobreSegmento: os triangulos que intersectam o plano
        esquerda: subárvore que fica atrás do plano
        direita: subárvore que fica à frente do plano
    construtores:
        há duas maneiras de se criar um BSPNode:
            1. um nó interno, então o nodo possui hiperplano
            2. uma folha, sem hiperplano
        os dois não precisam de criações diretas para seus filhos (esquerda e direita), já que é criado as struturas de baixo para cima (os filhos precisam estar processados para criar o nó pai).
    funções/métodos:
        ehFolha(): Uma função que verifica se é folha

O algoritmo tem base no apresentado em Computational Geometry: A Modern Approach. A função que gera a BSP espera que não haja interseções entre os triângulos, o que não é um impedimento para o que foi proposto para o trabalho : procurar onde um segmento de reta se encontra, se guiando pelas partições

A função principal que engloba a criação da BSP é a função criaBSP, abaixo está a definição do mesmo
Seja 
    o conjunto de triângulos S = {t1,t2,t3,...,tn},
    h⁺ = o conjunto de objetos à direita do hiperplano h
    h⁻ = o conjunto de objetos à esquerda do hiperplano h
    P = o conjunto de objetos que se interseccionam em todos os pontos com o hiperplano h
    criaBSP(S):
        Se |S| <= 1
            cria folha
        Se não
            Cria o hiperplano h a partir de t1 
            Para cada triangulo t em S - {t1}
                Se t está à direita de h
                    insere em h⁺
                Se t está à esquerda de h
                    insere em h⁻
                Se t está em h
                    insere em P
            esquerdo <- criaBSP(h⁺)
            filhoDireito <- criaBSP(h⁻)

            cria nó p
            p.esquerda = h⁻
            p.direita = h⁺
            p.triangulosSobreSegmento = P
            retorna nó p
