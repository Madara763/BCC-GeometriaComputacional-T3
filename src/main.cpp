#include <iostream>
#include <sstream>

#include "poligono.hpp"

using namespace std;

int main(){

  //Leitura dos dados
  int nPontos, nTri, nSeg; //Quantidade de pontos, triangulos e segmentos respectivamente
  
  cin>>nPontos>>nTri>>nSeg;
  
  //Leitura dos pontos
  vector<ponto<t_ponto>> pontos;
  ponto<t_ponto> p;
  for(int i=0; i<nPontos; i++){
    cin>>p.x>>p.y>>p.z;
    pontos.push_back(p);
  }

  //Leitura dos triangulos
  vector<triangulo<t_ponto>> Triangulos;
  int ind;
  for(int i=0; i<nTri; i++){
    triangulo<t_ponto> t;
    //Le cada indice dos vertices
    for(int j=0; j<3; j++){
      cin>>ind;
      t.vertices.push_back(pontos[ind-1]);
    }
    Triangulos.push_back(t);    
  }

  //Le os segmentos
  vector<segmento<t_ponto>> Segmentos;
  segmento<t_ponto> s;
  for(int i=0; i<nSeg; i++){
    cin>>s.ini.x>>s.ini.y>>s.ini.z>>s.fim.x>>s.fim.y>>s.fim.z;
    Segmentos.push_back(s);
  }

  auto raiz = criaBSP<double>(Triangulos);

  #ifdef DEBUG
  //Impressao das estruturas lidas
  cout<<"Pontos:\n";
  for(ponto<t_ponto> p : pontos){
    cout<<p<<" \n";
  }

  int aux{1};
  cout<<"Triangulos:\n";
  for(triangulo<t_ponto> t: Triangulos){
    cout<<aux<<": "<<t.indVertices[0]<<", "<<t.indVertices[1]<<", "<<t.indVertices[2]<<"\n";
    aux++;
  }

  cout<<"Segmentos:\n";
  for(segmento<t_ponto> s : Segmentos){
    cout<<s.ini<<" -> "<<s.fim<<"\n";
  }
  
  printBSP(raiz);
  #endif

  delete(raiz);
  return 0;
}