/*
  Biblioteca define funcoes e estruturas de dados
  para identificacao de poligonos e pontos
  Criado por Davi Garcia Lazzarin e Matheus Telles Batista
  Data: 22/06/25
*/

#include <vector>
#include <iostream>

#ifndef _POLIGONO_
#define _POLIGONO_

// ====================================== Definicoes ======================================

//Alterar o tipo confome implementacao
#define t_ponto int 
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
};

template <typename T> struct segmento{
  ponto<T> ini, fim; 
};

template <typename T> struct triangulo{
  std::vector<t_ponto> indVertices;
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

#endif