/*
  Biblioteca define funcoes e estruturas de dados
  para identificacao de poligonos e pontos
  Criado por Davi Garcia Lazzarin e Matheus Telles Batista
  Data: 22/06/25
*/

#include <algorithm>  // Para sort e unique
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>     // Para string no print
#include <vector>

#ifndef _POLIGONO_
#define _POLIGONO_

// ====================================== Definicoes ======================================

// Alterar o tipo confome implementacao
#define t_ponto double
const double EPS = 1E-9;

template <typename T>
struct ponto;
template <typename T>
struct triangulo;
template <typename T>
struct hiperplano;
template <typename T>
struct BSPNode;

// Typedefs---------------------------------------------------------------------------------
template <typename T>
struct ponto {
	T x{0}, y{0}, z{0};

	bool operator==(const ponto<T> &outro) const {
		if constexpr(std::is_floating_point<T>::value)
			return std::abs(x - outro.x) < EPS && std::abs(y - outro.y) < EPS && std::abs(z - outro.z) < EPS;
		else
			return x == outro.x && y == outro.y && z == outro.z;
	}

	ponto operator-(const ponto &other) const { return {x - other.x, y - other.y, z - other.z}; }
	ponto operator+(const ponto &other) const { return {x + other.x, y + other.y, z + other.z}; }
	ponto operator*(T scalar) const { return {x * scalar, y * scalar, z * scalar}; }
};

template <typename T>
struct segmento {
	ponto<T> ini, fim;
};

enum class LadoPlano {
	Positivo,
	Negativo,
	Intersecta,
	NoPlano
};

template <typename T>
struct triangulo {
	std::vector<ponto<T>> vertices;
	int id;

	std::pair<ponto<T>, ponto<T>> getVetores() const {
		if(vertices.size() < 3)
			throw std::runtime_error("Triangulo com menos de 3 vertices.");
		return {vertices[1] - vertices[0], vertices[2] - vertices[0]};
	}
};

template <typename T>
struct hiperplano {
	T d;
	ponto<T> normal;

	hiperplano() = default;
	hiperplano(const triangulo<T> &triangulo) {
		auto [v1, v2] = triangulo.getVetores();
		normal = prod_vetorial(v1, v2);
		const ponto<T> &p1 = triangulo.vertices[0];
		d = -(normal.x * p1.x + normal.y * p1.y + normal.z * p1.z);
	}

	T meioEspaco(const ponto<T> &p) const { return normal.x * p.x + normal.y * p.y + normal.z * p.z + d; }

	LadoPlano ladoSegmento(const segmento<T> &s) const {
		T val_ini = meioEspaco(s.ini);
		T val_fim = meioEspaco(s.fim);
		bool ini_positivo = val_ini > EPS;
		bool fim_positivo = val_fim > EPS;
		bool ini_negativo = val_ini < -EPS;
		bool fim_negativo = val_fim < -EPS;
		if(ini_positivo && fim_positivo)
			return LadoPlano::Positivo;
		if(ini_negativo && fim_negativo)
			return LadoPlano::Negativo;
		if((!ini_positivo && !ini_negativo) && (!fim_positivo && !fim_negativo))
			return LadoPlano::NoPlano;
		return LadoPlano::Intersecta;
	}

	LadoPlano ladoTriangulo(const triangulo<T> &t) const {
		int numPos = 0, numNeg = 0, numNoPlano = 0;
		for(const auto &ponto : t.vertices) {
			T val = meioEspaco(ponto);
			if(std::abs(val) < EPS)
				numNoPlano++;
			else if(val > 0)
				numPos++;
			else
				numNeg++;
		}
		if(numNoPlano == 3)
			return LadoPlano::NoPlano;
		if(numPos > 0 && numNeg == 0)
			return LadoPlano::Positivo;
		if(numNeg > 0 && numPos == 0)
			return LadoPlano::Negativo;
		return LadoPlano::Intersecta;
	}
};

template <typename T>
struct BSPNode {
	hiperplano<T> planoDivisor;
	std::vector<triangulo<T>> triangulosSobrePlano;
	BSPNode<T> *esquerda = nullptr;
	BSPNode<T> *direita = nullptr;

	BSPNode(const hiperplano<T> &plano, const std::vector<triangulo<T>> &triangulos, BSPNode<T> *esq = nullptr, BSPNode<T> *dir = nullptr)
	    : planoDivisor(plano), triangulosSobrePlano(triangulos), esquerda(esq), direita(dir) {}
	BSPNode(const std::vector<triangulo<T>> &triangulos)
	    : triangulosSobrePlano(triangulos), esquerda(nullptr), direita(nullptr) {}
	bool ehFolha() const { return esquerda == nullptr && direita == nullptr; }
	~BSPNode() {
		delete esquerda;
		delete direita;
	}
};


// ====================================== Funcoes Auxiliares ======================================

//funções de produto vetorial/escalar
template <typename T>
ponto<T> prod_vetorial(const ponto<T> &a, const ponto<T> &b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }

template <typename T>
T prod_escalar(const ponto<T> &a, const ponto<T> &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }


//Funcao intersecta_plano_segmento 
template <typename T>
ponto<T> intersecta_plano_segmento(const ponto<T> &p1, const ponto<T> &p2, const hiperplano<T> &plano) {
	ponto<T> direcao = p2 - p1;
	T dot_normal_direcao = prod_escalar(plano.normal, direcao);
	T t = -(plano.meioEspaco(p1)) / dot_normal_direcao;
	return p1 + (direcao * t);
}

//Funções de split_triangulo 
template <typename T>
std::pair<std::vector<triangulo<T>>, std::vector<triangulo<T>>>
split_triangulo(const triangulo<T> &t, const hiperplano<T> &plano) {
	std::vector<ponto<T>> pontos_pos, pontos_neg, pontos_no_plano;

	//Classifica cada vertice em uma de tres listas
	for(const auto &v : t.vertices) {
		T lado = plano.meioEspaco(v);
		if(lado > EPS) {
			pontos_pos.push_back(v);
		} else if(lado < -EPS) {
			pontos_neg.push_back(v);
		} else {
			pontos_no_plano.push_back(v);
		}
	}

	std::vector<triangulo<T>> triangulos_positivos;
	std::vector<triangulo<T>> triangulos_negativos;

	// Se o triangulo nao cruza o plano (esta todo de um lado ou apenas toca o plano),
	// não há o que dividir retorna vetores vazios
	if(pontos_pos.empty() || pontos_neg.empty()) {
		return {triangulos_positivos, triangulos_negativos};
	}

	// Caso 1: 1 vertice positivo, 2 negativos
	if(pontos_pos.size() == 1 && pontos_neg.size() == 2) {
		ponto<T> p_pos = pontos_pos[0];
		ponto<T> p_neg1 = pontos_neg[0];
		ponto<T> p_neg2 = pontos_neg[1];

		ponto<T> i1 = intersecta_plano_segmento(p_pos, p_neg1, plano);
		ponto<T> i2 = intersecta_plano_segmento(p_pos, p_neg2, plano);

		triangulos_positivos.push_back({{p_pos, i1, i2}, t.id});
		triangulos_negativos.push_back({{p_neg1, p_neg2, i1}, t.id});
		triangulos_negativos.push_back({{p_neg2, i2, i1}, t.id});
	}
	// Caso 2: 2 vertices positivos, 1 negativo
	else if(pontos_pos.size() == 2 && pontos_neg.size() == 1) {
		ponto<T> p_pos1 = pontos_pos[0];
		ponto<T> p_pos2 = pontos_pos[1];
		ponto<T> p_neg = pontos_neg[0];

		ponto<T> i1 = intersecta_plano_segmento(p_neg, p_pos1, plano);
		ponto<T> i2 = intersecta_plano_segmento(p_neg, p_pos2, plano);

		triangulos_positivos.push_back({{p_pos1, p_pos2, i1}, t.id});
		triangulos_positivos.push_back({{p_pos2, i2, i1}, t.id});
		triangulos_negativos.push_back({{p_neg, i1, i2}, t.id});
	}
	// Caso 3: 1 pos, 1 neg, 1 sobre o plano
	else if(pontos_pos.size() == 1 && pontos_neg.size() == 1 && pontos_no_plano.size() == 1) {
		ponto<T> p_pos = pontos_pos[0];
		ponto<T> p_neg = pontos_neg[0];
		ponto<T> p_plano = pontos_no_plano[0];

		// A aresta que cruza o plano 
		ponto<T> i = intersecta_plano_segmento(p_pos, p_neg, plano);

		// O ponto sobre o plano (p_plano) e a interseção i formaa a nova aresta comum
		triangulos_positivos.push_back({{p_pos, i, p_plano}, t.id});
		triangulos_negativos.push_back({{p_neg, i, p_plano}, t.id});
	}

	return {triangulos_positivos, triangulos_negativos};
}

// Cria uma BSP para um conjunto S de triangulos
template <typename T>
BSPNode<T> *criaBSP(const std::vector<triangulo<T>> &s) {
	// Caso base: folha
	if(s.empty()) {
		return nullptr;  // Retorna nulo se não há triangulos, representando uma região vazia.
	}

	// Cria hiperplano com o primeiro triângulo como pivô
	triangulo<T> pivo = s[0];
	hiperplano<T> plano(pivo);

	std::vector<triangulo<T>> sPlus, sMinus, sPlane;
	sPlane.push_back(pivo);

	for(size_t i = 1; i < s.size(); ++i) {
		const auto &t = s[i];
		auto lado = plano.ladoTriangulo(t);

		switch(lado) {
		case LadoPlano::Positivo:
			sPlus.push_back(t);
			break;
		case LadoPlano::Negativo:
			sMinus.push_back(t);
			break;
		case LadoPlano::NoPlano:
			sPlane.push_back(t);
			break;
		case LadoPlano::Intersecta: {
			auto [novos_positivos, novos_negativos] = split_triangulo(t, plano);
			sPlus.insert(sPlus.end(), novos_positivos.begin(), novos_positivos.end());
			sMinus.insert(sMinus.end(), novos_negativos.begin(), novos_negativos.end());
			break;
		}
		default:
			break;
		}
	}

	BSPNode<T> *esquerda = criaBSP(sMinus);
	BSPNode<T> *direita = criaBSP(sPlus);

	return new BSPNode<T>(plano, sPlane, esquerda, direita);
}

// =============================== Novas Funções para a Busca ===============================
// --- Novas Funções Auxiliares para o Caso Coplanar ---

// Verifica a orientação de um trio de pontos coplanares (colinear, horário, anti-horário).
// Usa o produto vetorial para determinar a orientação em relação à normal do plano.
template <typename T>
int orientacao_coplanar(const ponto<T> &p, const ponto<T> &q, const ponto<T> &r, const ponto<T> &normal_plano) {
	ponto<T> v1 = q - p;
	ponto<T> v2 = r - p;
	ponto<T> cross_prod = prod_vetorial(v1, v2);
	T dot = prod_escalar(cross_prod, normal_plano);

	if(std::abs(dot) < EPS)
		return 0;                 // Colinear
	return (dot > 0) ? 1 : -1;  // Anti-horário ou Horário
}

//Verifica se um ponto p está sobre um segmento s, assumindo que são colineares.
template <typename T>
bool ponto_no_segmento(const ponto<T> &p, const segmento<T> &s) {
	return (p.x <= std::max(s.ini.x, s.fim.x) && p.x >= std::min(s.ini.x, s.fim.x) &&
	        p.y <= std::max(s.ini.y, s.fim.y) && p.y >= std::min(s.ini.y, s.fim.y) &&
	        p.z <= std::max(s.ini.z, s.fim.z) && p.z >= std::min(s.ini.z, s.fim.z));
}

//Verifica se dois segmentos de reta coplanares se intersectam.
template <typename T>
bool intersecta_segmentos_coplanares(const segmento<T> &s1, const segmento<T> &s2, const ponto<T> &normal_plano) {
	ponto<T> p1 = s1.ini, q1 = s1.fim;
	ponto<T> p2 = s2.ini, q2 = s2.fim;

	int o1 = orientacao_coplanar(p1, q1, p2, normal_plano);
	int o2 = orientacao_coplanar(p1, q1, q2, normal_plano);
	int o3 = orientacao_coplanar(p2, q2, p1, normal_plano);
	int o4 = orientacao_coplanar(p2, q2, q1, normal_plano);

	// Caso geral: os segmentos se cruzam
	if(o1 != o2 && o3 != o4)
		return true;

	// Casos especiais (colineares)
	if(o1 == 0 && ponto_no_segmento(p2, s1))
		return true;
	if(o2 == 0 && ponto_no_segmento(q2, s1))
		return true;
	if(o3 == 0 && ponto_no_segmento(p1, s2))
		return true;
	if(o4 == 0 && ponto_no_segmento(q1, s2))
		return true;

	return false;
}

// Verifica se um ponto está dentro de um triângulo, assumindo que são todos coplanares.
// Usa o teste "mesmo lado"
template <typename T>
bool ponto_dentro_triangulo_coplanar(const ponto<T> &p, const triangulo<T> &t, const ponto<T> &normal_plano) {
	int o1 = orientacao_coplanar(t.vertices[0], t.vertices[1], p, normal_plano);
	int o2 = orientacao_coplanar(t.vertices[1], t.vertices[2], p, normal_plano);
	int o3 = orientacao_coplanar(t.vertices[2], t.vertices[0], p, normal_plano);

	// O ponto está dentro se estiver sempre do mesmo lado (ou sobre) todas as arestas
	bool mesmo_lado_negativo = (o1 < 0 || o1 == 0) && (o2 < 0 || o2 == 0) && (o3 < 0 || o3 == 0);
	bool mesmo_lado_positivo = (o1 > 0 || o1 == 0) && (o2 > 0 || o2 == 0) && (o3 > 0 || o3 == 0);

	return mesmo_lado_negativo || mesmo_lado_positivo;
}

//Verifica se um segmento de reta intersecta um triângulo.
//Implementação baseada no algoritmo Moller-Trumbore.
template <typename T>
bool intersecta_segmento_triangulo(const segmento<T> &s, const triangulo<T> &t) {
	ponto<T> v0 = t.vertices[0];
	ponto<T> edge1 = t.vertices[1] - v0;
	ponto<T> edge2 = t.vertices[2] - v0;
	ponto<T> seg_dir = s.fim - s.ini;

	// Utilizacao Moller-Trumbore
	ponto<T> h = prod_vetorial(seg_dir, edge2);
	T a = prod_escalar(edge1, h);


	if(std::abs(a) < EPS) {

    // Se 'a' eh zero, o segmento eh paralelo ao plano do triangulo.
		// Precisamos verificar se eles sao coplanares e se ha sobreposicao.
		hiperplano<T> plano_triangulo(t);

		// Verifica se o segmento esta realmente no mesmo plano do triangulo
		if(std::abs(plano_triangulo.meioEspaco(s.ini)) > EPS) {
			return false;  // Paralelo, mas n coplanar
		}

		// Verifica se o segmento cruza alguma das arestas do triangulo
		segmento<T> aresta1 = {t.vertices[0], t.vertices[1]};
		segmento<T> aresta2 = {t.vertices[1], t.vertices[2]};
		segmento<T> aresta3 = {t.vertices[2], t.vertices[0]};

		if(intersecta_segmentos_coplanares(s, aresta1, plano_triangulo.normal) ||
		   intersecta_segmentos_coplanares(s, aresta2, plano_triangulo.normal) ||
		   intersecta_segmentos_coplanares(s, aresta3, plano_triangulo.normal)) {
			return true;
		}

		// Se não cruza arestas, verifica se o segmento esta inteiramente dentro do triangulo
		// (basta testar um dos pontos do segmento)
		if(ponto_dentro_triangulo_coplanar(s.ini, t, plano_triangulo.normal)) {
			return true;
		}

		return false;  // Coplanar, mas sem sobreposição
	}

	// --- FIM DO TRATAMENTO COPLANAR ---

	// Continuação da logica de Moller-Trumbore para o caso nao-coplanar
	T f = 1.0 / a;
	ponto<T> s_vec = s.ini - v0;
	T u = f * prod_escalar(s_vec, h);

	if(u < 0.0 || u > 1.0)
		return false;

	ponto<T> q = prod_vetorial(s_vec, edge1);
	T v = f * prod_escalar(seg_dir, q);

	if(v < 0.0 || u + v > 1.0)
		return false;

	T t_intersect = f * prod_escalar(edge2, q);

	// Verifica se a interseçao esta dentro do segmento [0, 1]
	if(t_intersect > EPS && t_intersect < 1.0 + EPS) {
		return true;
	}

	return false;
}


// Percorre a arvore BSP recursivamente para encontrar intersecoes com um segmento.
template <typename T>
void busca_intersecao(const BSPNode<T> *no, const segmento<T> &seg, std::vector<int> &ids_intersectados) {
	// Caso base: chegamos a uma regiao vazia do espaço
	if(no == nullptr) {
		return;
	}

	// testa intersecao com os triangulos coplanares no nó atual
	for(const auto &triangulo_no_plano : no->triangulosSobrePlano) {
		if(intersecta_segmento_triangulo(seg, triangulo_no_plano)) {
			ids_intersectados.push_back(triangulo_no_plano.id);
		}
	}

	// Se for uma folha, n tem mais para onde descer
	if(no->ehFolha()) {
		return;
	}

	// Decide para qual(is) lado(s) a busca deve continuar
	LadoPlano lado = no->planoDivisor.ladoSegmento(seg);

	switch(lado) {
	case LadoPlano::Positivo:
		busca_intersecao(no->direita, seg, ids_intersectados);
		break;
	case LadoPlano::Negativo:
		busca_intersecao(no->esquerda, seg, ids_intersectados);
		break;
	case LadoPlano::NoPlano:
		// Ja testamos os tringulos deste no, para aqui
		break;
	case LadoPlano::Intersecta:
		// O segmento cruza o plano, entao precisa ser testado em ambas as sub-arvores.
		busca_intersecao(no->esquerda, seg, ids_intersectados);
		busca_intersecao(no->direita, seg, ids_intersectados);
		break;
	}
}

#endif