import time

def lzw_comprimir_arquivo(caminho_arquivo, maximum_table_size):
    dicionario = {bytes([i]): i for i in range(256)}
    prefixo = b''
    codigo_binario = []
    bits_necessarios = 8

    with open(caminho_arquivo, 'rb') as arquivo:
        while True:
            caractere = arquivo.read(1)
            if not caractere:
                break

            novo_prefixo = prefixo + caractere
            if novo_prefixo in dicionario:
                prefixo = novo_prefixo
            else:
                indice_prefixo = dicionario[prefixo]
                codigo = format(indice_prefixo, f'0{bits_necessarios}b')
                codigo_binario.append(codigo)
                if len(dicionario) <= maximum_table_size:
                    dicionario[novo_prefixo] = len(dicionario)
                prefixo = caractere

                if 2 ** bits_necessarios < len(dicionario):
                    bits_necessarios += 1

    # Trata o último prefixo após a leitura completa do arquivo
    if prefixo:
        indice_prefixo = dicionario[prefixo]
        codigo = format(indice_prefixo, f'0{bits_necessarios}b')
        codigo_binario.append(codigo)

    # Gera a mensagem codificada final
    mensagem_codificada = ''.join(codigo_binario)

    buffer = bytearray()
    buffer.append(0)  # Placeholder para o número de bits ignorados

    for i in range(0, len(mensagem_codificada), 8):
        segmento = mensagem_codificada[i:i+8]
        if len(segmento) < 8:
            bits_ignorados = 8 - len(segmento)
            segmento = segmento.ljust(8, '0')
            buffer[0] = bits_ignorados  # Atualiza o byte de controle
        byte = int(segmento, 2)
        buffer.append(byte)

    return buffer

def salvar_dados_em_arquivo(dados, nome_arquivo):
    with open(nome_arquivo, 'wb') as arquivo:
        arquivo.write(dados)

if __name__ == "__main__":
    
    caminho_entrada = 'dickens' # Arquivo a ser comprimido
    caminho_saida = 'compressao.bin' # Arquivo comprimido de saída

    n = 8
    maximum_table_size = pow(2, int(n))

    start_time = time.time()
    dados_comprimidos = lzw_comprimir_arquivo(caminho_entrada, maximum_table_size)
    end_time = time.time()

    print(f"\nCompressão concluída em {end_time - start_time} segundos.\n")

    salvar_dados_em_arquivo(dados_comprimidos, caminho_saida)

