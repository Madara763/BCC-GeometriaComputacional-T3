#include <algorithm>  // Para std::sort e std::unique
#include <iostream>
#include <sstream>
#include <vector>

#include "poligono.hpp"

using namespace std;

int main() {
	// Leitura dos dados
	int nPontos, nTri, nSeg;
	cin >> nPontos >> nTri >> nSeg;

	// Leitura dos pontos
	vector<ponto<t_ponto>> pontos;
	ponto<t_ponto> p;
	for(int i = 0; i < nPontos; i++) {
		cin >> p.x >> p.y >> p.z;
		pontos.push_back(p);
	}

	// Leitura dos triangulos
	vector<triangulo<t_ponto>> Triangulos;
	int ind;
	for(int i = 0; i < nTri; i++) {
		triangulo<t_ponto> t;
		t.id = i + 1;  // Atribui o ID original do triângulo (1-based)
		for(int j = 0; j < 3; j++) {
			cin >> ind;
			t.vertices.push_back(pontos[ind - 1]);
		}
		Triangulos.push_back(t);
	}

	// Le os segmentos
	vector<segmento<t_ponto>> Segmentos;
	segmento<t_ponto> s;
	for(int i = 0; i < nSeg; i++) {
		cin >> s.ini.x >> s.ini.y >> s.ini.z >> s.fim.x >> s.fim.y >> s.fim.z;
		Segmentos.push_back(s);
	}

	//Constrói a árvore BSP
	BSPNode<t_ponto>* raiz = criaBSP<t_ponto>(Triangulos);

	//Para cada segmento, busca as intersecoes e imprime a saida
	for(const auto& seg : Segmentos) {
		vector<int> ids_encontrados;
		busca_intersecao(raiz, seg, ids_encontrados);

		// processamento dos resultados
		// Ordena os IDs para que a função unique funcione corretamente
		sort(ids_encontrados.begin(), ids_encontrados.end());

		// Remove IDs duplicados
		auto ultimo_unico = unique(ids_encontrados.begin(), ids_encontrados.end());
		ids_encontrados.erase(ultimo_unico, ids_encontrados.end());

		//Imprime no formato exigido
		cout << ids_encontrados.size();
		for(int id : ids_encontrados) {
			cout << " " << id;
		}
		cout << endl;
	}

	// libera memoria
	delete raiz;
	return 0;
}