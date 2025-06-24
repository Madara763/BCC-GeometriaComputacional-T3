import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from collections import namedtuple

# Define uma estrutura simples para pontos para facilitar o acesso
Ponto = namedtuple('Ponto', ['x', 'y', 'z'])

def ler_dados_entrada():
    """
    Lê e processa os dados da entrada padrão (stdin) no formato do trabalho.
    """
    linhas = sys.stdin.readlines()
    
    if not linhas:
        print("Erro: Entrada vazia.", file=sys.stderr)
        sys.exit(1)
        
    # Primeira linha: contagem de pontos, triângulos e segmentos
    try:
        n_pontos, n_tri, n_seg = map(int, linhas[0].strip().split())
    except (ValueError, IndexError):
        print("Erro: A primeira linha deve conter 3 números (n_pontos, n_tri, n_seg).", file=sys.stderr)
        sys.exit(1)

    pontos = []
    triangulos = []
    segmentos = []
    
    linha_atual = 1

    # Leitura dos pontos
    for i in range(n_pontos):
        try:
            x, y, z = map(float, linhas[linha_atual + i].strip().split())
            pontos.append(Ponto(x, y, z))
        except (ValueError, IndexError):
            print(f"Erro ao ler o ponto na linha {linha_atual + i + 1}", file=sys.stderr)
            sys.exit(1)
            
    linha_atual += n_pontos

    # Leitura dos triângulos (índices são 1-based)
    for i in range(n_tri):
        try:
            indices = map(int, linhas[linha_atual + i].strip().split())
            # Converte os índices 1-based para 0-based e pega os pontos
            verts = [pontos[idx - 1] for idx in indices]
            triangulos.append(verts)
        except (ValueError, IndexError):
            print(f"Erro ao ler o triângulo na linha {linha_atual + i + 1}", file=sys.stderr)
            sys.exit(1)

    linha_atual += n_tri
    
    # Leitura dos segmentos
    for i in range(n_seg):
        try:
            coords = list(map(float, linhas[linha_atual + i].strip().split()))
            p_ini = Ponto(coords[0], coords[1], coords[2])
            p_fim = Ponto(coords[3], coords[4], coords[5])
            segmentos.append((p_ini, p_fim))
        except (ValueError, IndexError):
            print(f"Erro ao ler o segmento na linha {linha_atual + i + 1}", file=sys.stderr)
            sys.exit(1)
            
    return pontos, triangulos, segmentos

def plotar_cena(pontos, triangulos, segmentos):
    """
    Cria um gráfico 3D interativo com os dados geométricos fornecidos.
    """
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')

    # 1. Plotar os Triângulos
    # Usamos Poly3DCollection para desenhar os polígonos em 3D
    poly_collection = Poly3DCollection(triangulos, alpha=0.3, linewidths=1)
    poly_collection.set_facecolor(['cyan', 'magenta', 'yellow', 'lime', 'orange', 'purple'] * (len(triangulos) // 6 + 1))
    poly_collection.set_edgecolor('black')
    ax.add_collection3d(poly_collection)
    
    # 2. Plotar os Segmentos
    for seg in segmentos:
        p_ini, p_fim = seg
        ax.plot([p_ini.x, p_fim.x], [p_ini.y, p_fim.y], [p_ini.z, p_fim.z], 
                color='red', marker='o', linewidth=3)

    # 3. Plotar os Pontos (opcional, mas bom para referência)
    if pontos:
        xs = [p.x for p in pontos]
        ys = [p.y for p in pontos]
        zs = [p.z for p in pontos]
        ax.scatter(xs, ys, zs, color='blue', s=20, label='Vértices')

    # Configurações do gráfico
    ax.set_xlabel('Eixo X')
    ax.set_ylabel('Eixo Y')
    ax.set_zlabel('Eixo Z')
    ax.set_title('Visualização 3D da Entrada Geométrica')
    ax.legend()
    
    # Ajusta os limites dos eixos para garantir que tudo seja visível
    todos_os_pontos = pontos
    for t in triangulos: todos_os_pontos.extend(t)
    for s in segmentos: todos_os_pontos.extend(s)
    
    if todos_os_pontos:
        min_x = min(p.x for p in todos_os_pontos)
        max_x = max(p.x for p in todos_os_pontos)
        min_y = min(p.y for p in todos_os_pontos)
        max_y = max(p.y for p in todos_os_pontos)
        min_z = min(p.z for p in todos_os_pontos)
        max_z = max(p.z for p in todos_os_pontos)
        ax.set_xlim(min_x - 10, max_x + 10)
        ax.set_ylim(min_y - 10, max_y + 10)
        ax.set_zlim(min_z - 10, max_z + 10)

    plt.show()

if __name__ == "__main__":
    print("Lendo dados da entrada padrão... (Pressione Ctrl+D ou Ctrl+Z e Enter para finalizar se estiver digitando)")
    pontos, triangulos, segmentos = ler_dados_entrada()
    print("Dados lidos. Gerando gráfico...")
    plotar_cena(pontos, triangulos, segmentos)