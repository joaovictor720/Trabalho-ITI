import pandas as pd
import matplotlib.pyplot as plt

# Carrega os dados do arquivo CSV
data = pd.read_csv('compression_data.csv', header=None, names=['n', 'l(n)'])

# Assumindo que a primeira coluna é 'n' e a segunda coluna é 'l(n)'
n = data['n']
l_n = data['l(n)']

# Plota o gráfico
plt.figure(figsize=(10, 6))
plt.plot(n, l_n, marker='o')

# Adiciona título e rótulos aos eixos
plt.title('Gráfico l(n) x n')
plt.xlabel('n (Bytes processados)')
plt.ylabel('l(n) (Comprimento médio do arquivo comprimido)')

# Exibe o gráfico
plt.grid(True)
plt.show()
