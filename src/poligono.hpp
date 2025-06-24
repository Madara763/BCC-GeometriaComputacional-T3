/*
  Biblioteca define funcoes e estruturas de dados
  para identificacao de poligonos e pontos
  Criado por Davi Garcia Lazzarin e Matheus Telles Batista
  Data: 22/06/25
*/

#include <vector>
#include <iostream>
#include <memory>

#ifndef _POLIGONO_
#define _POLIGONO_

// ====================================== Definicoes ======================================

//Alterar o tipo confome implementacao
#define t_ponto double 
const double EPS = 1E-9;

//Typedefs---------------------------------------------------------------------------------
template <typename T> struct ponto {
  T x{0}, y{0}, z{0}; 

  //Sobregarga comparacao de igualdade
  bool operator==(const ponto<T>& outro) const {
    if constexpr (std::is_floating_point<T>::value) //Se o tipo for de ponto flutuante
      return std::abs(x - outro.x) < EPS && std::abs(y - outro.y) < EPS && std::abs(z - outro.z) < EPS;
    else //se o tipo for inteiro
      return x == outro.x && y == outro.y && z == outro.z;
  }
  
  // Sobregarga de subtração
  ponto operator-(const ponto& other) const {
    return {x - other.x, y - other.y, z - other.z};
  }
};

template <typename T> struct segmento{
  ponto<T> ini, fim; 
};

// Enums para saber de qual lado está os objetos em relação ao plano
enum class LadoPlano {
  Positivo,
  Negativo,
  Intersecta,
  NoPlano
};

template <typename T> struct triangulo{
  std::vector<ponto<T>> vertices;

  // Retorna dois vetores formados entre os três pontos do triângulo
  std::pair<ponto<T>, ponto<T>> getVetores() const {
    const ponto<T>& p1 = vertices[0];
    const ponto<T>& p2 = vertices[1];
    const ponto<T>& p3 = vertices[2];

    ponto<T> v1 = p2 - p1;
    ponto<T> v2 = p3 - p1;

    return {v1, v2};
  }
};

// Um hiperplano baseado em um triangulo qualquer
template <typename T> struct hiperplano {
	T d;              // ax1+bx2+cx3+d=0
	ponto<T> normal;  // (a, b, c)

	// Construtor padrao e com triangulo
  hiperplano() = default;

	hiperplano(const triangulo<T>& triangulo) {
		auto [v1, v2] = triangulo.getVetores();

		// Calcula o vetor normal via produto vetorial
		normal = prod_vetorial(v1, v2);

		// Calcula d = -normal · P1 (P1 = primeiro vértice)
		const ponto<T>& p1 = triangulo.vertices[0];
		d = -(normal.x * p1.x + normal.y * p1.y + normal.z * p1.z);
	}

	// calcula f(P) = a*x + b*y + c*z + d
	T meioEspaco(const ponto<T>& p) const {
		return normal.x * p.x + normal.y * p.y + normal.z * p.z + d;
	}

	// Avalia o lado do segmento em relação ao hiperplano
	LadoPlano ladoSegmento(const segmento<T>& s) const {
		T val_ini = meioEspaco(s.ini);
		T val_fim = meioEspaco(s.fim);

		bool ini_zero = std::abs(val_ini) < EPS;
		bool fim_zero = std::abs(val_fim) < EPS;

		// segmento todo no plano
		if(ini_zero && fim_zero)
			return LadoPlano::NoPlano;

		if((val_ini > EPS && val_fim > EPS) || (val_ini < -EPS && val_fim < -EPS)) {
			return val_ini > 0 ? LadoPlano::Positivo : LadoPlano::Negativo;
		}
    return LadoPlano::Intersecta;
	}

	// Avalia o lado do triangulo em relação ao hiperplano
	LadoPlano ladoTriangulo(const triangulo<T>& t) const {

		int numPos = 0, numNeg = 0, numLadoSegmento = 0;

		for(const auto& ponto : t.vertices) {
			T val = meioEspaco(ponto);

			if(std::abs(val) < EPS)
				numLadoSegmento++;
			else if(val > 0)
				numPos++;
			else
				numNeg++;
		}

		// Todos os pontos exatamente no plano
		if(numLadoSegmento == 3)
			return LadoPlano::NoPlano;

		// Todos de um mesmo lado
		if(numNeg == 0 && numPos > 0)
			return LadoPlano::Positivo;

		if(numPos == 0 && numNeg > 0)
			return LadoPlano::Negativo;

		// Se tem pontos dos dois lados, o triângulo cruza o plano
		return LadoPlano::Intersecta;
	}
};

template <typename T> struct BSPNode {
  // Hiperplano que divide a BSP em duas subárvores
  hiperplano<T> planoDivisor;

  // Triângulos que estão exatamente sobre o plano
  std::vector<triangulo<T>> triangulosSobreSegmento; 

  // Subárvore do lado negativo (atrás do plano)
  BSPNode<T>* esquerda = nullptr;

  // Subárvore do lado positivo (à frente do plano)
  BSPNode<T>* direita = nullptr;

  // Construtor para nó interno
  BSPNode(const hiperplano<T>& plano,
          const std::vector<triangulo<T>>& triangulos,
          BSPNode<T>* esq = nullptr,
          BSPNode<T>* dir = nullptr)
      : planoDivisor(plano), triangulosSobreSegmento(triangulos), esquerda(esq), direita(dir) {}

  // Construtor para folha
  BSPNode(const std::vector<triangulo<T>>& triangulos)
      : triangulosSobreSegmento(triangulos), esquerda(nullptr), direita(nullptr) {}

  // Verifica se é folha
  bool ehFolha() const {
    return esquerda == nullptr && direita == nullptr;
  }

  // Destrutor para liberar memória recursivamente
  ~BSPNode() {
    delete esquerda;
    delete direita;
  }
};


// ====================================== Auxiliares ======================================

//Sobrecargas------------------------------------------------------------------------------

// Sobrecarga para impressao do ponto 
template<typename T> std::ostream& operator<<(std::ostream& os, const ponto<T>& p) {
  #ifdef DEBUG
  os << "(" << p.x << ", " << p.y << ", " << p.z << ") ";
  #else
  os << p.x <<" "<< p.y <<", " << p.z;
  #endif
  return os;
}

// Sobrecarga para impressao do segmento
template<typename T> std::ostream& operator<<(std::ostream& os, const segmento<T>& a) {
  os << a.ini << " -> " << a.fim;
  return os;
}

// Sobrecarga para impressao do poligono
template<typename T> std::ostream& operator<<(std::ostream& os, const triangulo<T>& p) {
  os << "N vertices: " << p.vertices.size()<< " --> ";
  for(int i=0; i< static_cast<int>(p.vertices.size()); i++)
    os << p.vertices[i];

  return os;
}

// =============================== Funcoes gerais poligonos ===============================

// Realiza o produto vetorial em R3
template <typename T> ponto<T> prod_vetorial(const ponto<T>& a, const ponto<T>& b) {
  return {
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
  };
}

// Realiza o produto escalar em R3
template <typename T> T prod_escalar(const ponto<T>& a, const ponto<T>& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cria uma BSP (binary space partitioning) para um conjunto S de triangulos
template <typename T> 
BSPNode<T>* criaBSP(const std::vector<triangulo<T>>& s) {
  
  // Caso base: folha
  if (s.size() <= 1) {
    return new BSPNode<T>(s);
  }

  // Cria hiperplano com o primeiro triângulo
  hiperplano<T> plano(s[0]);

  std::vector<triangulo<T>> sPlus, sMinus, sPlane;

  auto it = s.begin();
  for(; it != s.end(); it++) {
	  auto t = *it;
	  if(it == s.begin()) continue;
	  auto lado = plano.ladoTriangulo(t);

	  switch(lado) {
	  case LadoPlano::Positivo:
		  sPlus.push_back(t);
		  break;
	  case LadoPlano::Negativo:
		  sMinus.push_back(t);
		  break;
	  case LadoPlano::Intersecta:
		  sPlus.push_back(t);
		  sMinus.push_back(t);
		  break;
	  case LadoPlano::NoPlano:
		  sPlane.push_back(t);
		  break;
	  default:
		  break;
	  }
  }

  BSPNode<T>* esquerda = criaBSP(sMinus);
  BSPNode<T>* direita = criaBSP(sPlus);

  sPlane.push_back(s[0]);

  return new BSPNode<T>(plano, sPlane, esquerda, direita);
}

template <typename T>
void printBSP(const BSPNode<T>* node, int nivel = 0) {
  if (!node) return;

  std::string indent(nivel * 2, ' ');

  if (node->ehFolha()) {
    std::cout << indent << "Folha: " << node->triangulosSobreSegmento.size() << " triângulo(s)" << std::endl;
  } else {
    std::cout << indent << "Nó Interno: Plano ["
              << node->planoDivisor.normal.x << ", "
              << node->planoDivisor.normal.y << ", "
              << node->planoDivisor.normal.z << "] + d = "
              << node->planoDivisor.d << std::endl;

    std::cout << indent << "  Triângulos no plano: " << node->triangulosSobreSegmento.size() << std::endl;

    printBSP(node->esquerda, nivel + 1);
    printBSP(node->direita, nivel + 1);
  }
}


#endif