# Objetivos e ideias

### Compressão

##### Geral
- Flag para não atualizar o modelo:
	- Possivelmente passar um caminho pra um modelo existente;
		- Isso precisaria de um formato de arquivo pra especificar os dados de cada tipo de modelo;
- Flag para medir o tempo de execução;
- Flag para	registrar o comprimento médio do arquivo até o byte atual (n-ésimo byte);

##### PPMc:
- Parâmetro do tamanho de níveis de memória probabilística;
- Implementar o arimético provavelmente com concatenação de inteiros para formar as casas decimais;

##### LZW:
- Flag para manter estático ou reiniciar o dicionário;
- Parâmetro para o tamanho máximo do dicionário (em bytes);

##### Sempressão
- Definir o cabeçalho;
- Ler o modelo usado a partir do cabeçalho;
- Flag para medir o tempo de execução;