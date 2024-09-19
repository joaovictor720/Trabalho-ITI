#include <iostream>
#include <string>
#include <cstdlib>  // Para exit()
#include "PPMc.h"  // Inclua o cabeçalho da sua classe PPMc
#include <chrono>  // Para medir o tempo

void printUsage(const std::string& programName) {
    std::cerr << "Uso: " << programName << " <arquivo de input> <-c/-d> <tamanho do contexto> "
              << "[-m <máximo de bytes adicionados ao modelo>] "
              << "[-sm <salvar modelo>] "
              << "[-um <usar modelo específico> <arquivo do modelo>]" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2 || std::string(argv[1]) == "-h") {
        printUsage(argv[0]);
        return 1;
    }

    // Pega o nome do arquivo de input
    if (argc < 4) {
        std::cerr << "Erro: parâmetros insuficientes." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile = argv[1];
    
    // Pega a flag de compressão ou descompressão
    std::string flag = argv[2];
    
    // Pega o tamanho do contexto
    int contextSize = std::stoi(argv[3]);

    // Inicializa as variáveis para salvar e carregar o modelo
    uint64_t maxBytes = -1;  // -1 indica que não há limite
    bool saveModel = false;
    std::string modelFile = "";
    bool useModel = false;

    // Processa os argumentos adicionais
    for (int i = 4; i < argc; ++i) {
        if (std::string(argv[i]) == "-m" && i + 1 < argc) {
            maxBytes = std::stoull(argv[++i]);
        } else if (std::string(argv[i]) == "-sm") {
            saveModel = true;
        } else if (std::string(argv[i]) == "-um" && i + 1 < argc) {
            useModel = true;
            modelFile = argv[++i];
        } else {
            std::cerr << "Erro: parâmetro desconhecido " << argv[i] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Verifica se a flag é para compressão (-c) ou descompressão (-d)
    if (flag == "-c") {
        // Criar o nome do arquivo de output para compressão
        std::string outputFile = inputFile + ".jvav" + std::to_string(contextSize);

        // Cria um objeto PPMc para compressão, com ou sem limite de bytes
        PPMc ppmc(contextSize, maxBytes);

        // Verifica se o usuário pediu para usar um modelo específico
        if (useModel) {
            ppmc.loadModel(modelFile);
            // Quando um modelo é carregado, o limite de bytes não se aplica
            maxBytes = 0;
        }

        auto start = std::chrono::high_resolution_clock::now();

        // Chama o método de compressão
        ppmc.compressFile(inputFile, outputFile);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Compressão concluída! Arquivo de saída: " << outputFile << std::endl;
        std::cout << "Tempo de compressão: " << elapsed.count() << " segundos" << std::endl;


        // Salva o modelo, se solicitado
        if (saveModel) {
            std::string modelOutput = inputFile + ".model";
            ppmc.saveModel(modelOutput);
            std::cout << "Modelo treinado com " << ppmc.bytesAddedToModel << " bytes e salvo em: " << modelOutput << std::endl;
        }

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
        PPMc ppmc(contextSize, maxBytes);

        // Verifica se o usuário pediu para usar um modelo específico
        if (useModel) {
            ppmc.loadModel(modelFile);
            // Quando um modelo é carregado, o limite de bytes não se aplica
            maxBytes = 0;
        }

        // Medir o tempo de descompressão
        auto start = std::chrono::high_resolution_clock::now();
        
        ppmc.decompressFile(inputFile, outputFile);
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "Descompressão concluída! Arquivo de saída: " << outputFile << std::endl;
        std::cout << "Tempo de descompressão: " << elapsed.count() << " segundos" << std::endl;

    } else {
        std::cerr << "Erro: flag desconhecida. Use -c para compressão ou -d para descompressão." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
