import os
import argparse

def concatenate_files(output_file, input_files):
    """
    Concatena vários arquivos de entrada em um único arquivo de saída.
    
    :param output_file: Nome do arquivo de saída.
    :param input_files: Lista de nomes dos arquivos de entrada a serem concatenados.
    """
    with open(output_file, 'wb') as outfile:
        for input_file in input_files:
            if os.path.isfile(input_file):
                with open(input_file, 'rb') as infile:
                    # Lê o conteúdo do arquivo de entrada e escreve no arquivo de saída
                    outfile.write(infile.read())
                print(f"Arquivo '{input_file}' adicionado.")
            else:
                print(f"Arquivo '{input_file}' não encontrado.")

def main():
    parser = argparse.ArgumentParser(description="Concatena vários arquivos em um único arquivo.")
    parser.add_argument('output_file', type=str, help="Nome do arquivo de saída.")
    parser.add_argument('input_files', nargs='+', type=str, help="Arquivos de entrada a serem concatenados.")
    
    args = parser.parse_args()
    
    concatenate_files(args.output_file, args.input_files)
    print(f"Arquivos concatenados em '{args.output_file}'.")

if __name__ == "__main__":
    main()
