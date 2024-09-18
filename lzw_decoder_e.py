from bitarray import bitarray
import time

def read_bits_from_file(file_path):
    bits = bitarray()
    with open(file_path, 'rb') as arquivo_binario:
        bits.fromfile(arquivo_binario)
    return bits

def descomprimir_arquivo(file_path, maximum_table_size):
    dicionario = {i: bytes([i]) for i in range(256)}
    mensagem_decodificada = bytearray()

    # Lê o arquivo comprimido e obtém os bits
    bits = read_bits_from_file(file_path)

    # Extrai o primeiro byte que contém a quantidade de bits ignorados
    bits_ignorados = bits[:8].tobytes()[0]
    bits = bits[8:]  # Remove o byte de controle

    # Remove os bits ignorados do final
    if bits_ignorados > 0:
        bits = bits[:-bits_ignorados]

    # Calcula o número inicial de bits necessários
    bits_necessarios = 8  # Começa com 8 bits para códigos iniciais (0-255)

    posicao = 0
    codigo_anterior = None

    while posicao + bits_necessarios <= len(bits):
        # Lê o número necessário de bits
        codigo_binario = bits[posicao:posicao + bits_necessarios]
        posicao += bits_necessarios

        # Converte o código binário para índice
        indice = int(codigo_binario.to01(), 2)

        if indice in dicionario:
            entrada_atual = dicionario[indice]
        elif indice == len(dicionario):
            # Caso especial quando o índice não está no dicionário
            entrada_atual = codigo_anterior + codigo_anterior[:1]
        else:
            raise ValueError("Índice inválido: {}".format(indice))

        mensagem_decodificada.extend(entrada_atual)

        if codigo_anterior is not None:
            # Adiciona nova entrada ao dicionário
            nova_entrada = codigo_anterior + entrada_atual[:1]
            if len(dicionario) <= maximum_table_size:
                dicionario[len(dicionario)] = nova_entrada

        codigo_anterior = entrada_atual

        # Atualiza o número de bits necessários se o dicionário tiver crescido
        if len(dicionario) >= 2**bits_necessarios:
            bits_necessarios += 1
            
    return mensagem_decodificada

def salvar_mensagem_em_arquivo(mensagem, nome_arquivo):
    with open(nome_arquivo, 'wb') as arquivo:
        arquivo.write(mensagem)

if __name__ == "__main__":

    n = 8
    maximum_table_size = pow(2, int(n))

    start_time = time.time()
    mensagem_decodificada = descomprimir_arquivo('compressao.bin', maximum_table_size)
    end_time = time.time()

    print(f"\nTempo de descompressão: {end_time - start_time} segundos\n")

    salvar_mensagem_em_arquivo(mensagem_decodificada, 'descompressao.txt')
