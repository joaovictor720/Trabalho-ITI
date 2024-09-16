#include <iostream>
#include <string>
#include "PPMc.h"  // Inclua o cabeçalho da sua classe PPMc
#include <cstring> // Para strcmp

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <arquivo de input> <-c/-d> <tamanho do contexto>" << std::endl;
        return 1;
    }

    // Pega o nome do arquivo de input
    std::string inputFile = argv[1];
    
    // Pega a flag de compressão ou descompressão
    std::string flag = argv[2];
    
    // Pega o tamanho do contexto
    int contextSize = std::stoi(argv[3]);
    
    // Verifica se a flag é para compressão (-c) ou descompressão (-d)
    if (flag == "-c") {
        // Criar o nome do arquivo de output para compressão
        std::string outputFile = inputFile + ".jvav" + std::to_string(contextSize);

        // Cria um objeto PPMc para compressão
        PPMc ppmc(contextSize);
        
        // Chama o método de compressão
        ppmc.compressFile(inputFile, outputFile);

        std::cout << "Compressão concluída! Arquivo de saída: " << outputFile << std::endl;

    } else if (flag == "-d") {
        // Checa se o arquivo de input tem a extensão correta para descompressão
        std::string expectedExtension = ".jvav" + std::to_string(contextSize);
        if (inputFile.size() < expectedExtension.size() ||
            inputFile.substr(inputFile.size() - expectedExtension.size()) != expectedExtension) {
            std::cerr << "Erro: o arquivo para descompressão deve ter a extensão " << expectedExtension << std::endl;
            return 1;
        }

        // Cria um nome de arquivo de saída para descompressão
        std::string outputFile = inputFile.substr(0, inputFile.size() - expectedExtension.size());

        // Cria um objeto PPMc para descompressão
        PPMc ppmc(contextSize);
        
        // Chama o método de descompressão
        ppmc.decompressFile(inputFile, outputFile);

        std::cout << "Descompressão concluída! Arquivo de saída: " << outputFile << std::endl;

    } else {
        std::cerr << "Erro: flag desconhecida. Use -c para compressão ou -d para descompressão." << std::endl;
        return 1;
    }

    return 0;
}
