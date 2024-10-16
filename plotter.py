import sys
import numpy as np
import matplotlib.pyplot as plt

def plot_data(file_path, subsample_factor, graph_id):
    with open(file_path, 'r') as file:
        data = [float(line.strip()) for line in file]
    
    # Dividindo os data points pelo seu índice, já que os dados vêm como valor absolute bits escritos
    data = [value / (i + 1) for i, value in enumerate(data)]
    
    # Subamostrando
    subsampled_data = data[::subsample_factor]
    
    # Gerando os valores do eixo x (índices dos data points)
    x_values = np.arange(1, len(subsampled_data) + 1)
    
    # Plotando os data points originais
    plt.figure(figsize=(10, 5))
    plt.plot(x_values, subsampled_data, marker='o', linestyle='-', markersize=2, label=f"Fator de subamostragem: {subsample_factor}")
    
    # Plotando a assíntota
    mean_length = data[-1]
    plt.axhline(y=mean_length, color='green', linestyle='--', label=f"Comprimento médio: {mean_length:.3f}")
    
    plt.title(f"LZW - {graph_id})")
    plt.xlabel("Índice (subamostrado)")
    plt.ylabel("Comprimento médio (bits codificados / byte original)")
    plt.xscale('log')
    plt.grid(True)
    plt.legend()
    
    # Exibindo
    plt.savefig('plot_' + graph_id)
    # plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso: python plotter.py <arquivo de dados> <fator de subamostragem> <id do gráfico gerado>")
    else:
        file_path = sys.argv[1]
        subsample_factor = int(sys.argv[2])
        graph_id = sys.argv[3]
        plot_data(file_path, subsample_factor, graph_id)
