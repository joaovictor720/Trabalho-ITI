#include "PPMc.h"

PPMc::PPMc(int maxContext, uint64_t maxBytes) 
: maxContext(maxContext), maxBytes(maxBytes), bytesAddedToModel(0), pastBitWrited(0)
{
    root = new TrieNode();
    initializeEquiprobableSymbols();
}

PPMc::~PPMc()
{
    delete root;
}

void PPMc::initializeEquiprobableSymbols() {
    for (uint16_t i = 0; i < 256; ++i) {
        equiprobableSymbols.push_back(std::make_pair(i, 1));
    }
    // EOF
    equiprobableSymbols.push_back(std::make_pair(257, 1));
}


void PPMc::encodeAndUpdateContexts(uint16_t symbol, ArithmeticEncoder& encoder, const std::vector<uint16_t>& context) {
    int contextSize = context.size();
    int maxExistentContext = findMaxExistentContext(context);
    std::vector<std::pair<uint16_t, int>> currentFrequencies;
    std::vector<std::pair<uint16_t, int>> rejectedFrequencies;
    TrieNode* currentNode = root;
    bool wasSymbolEncoded = false;

    


/*     if (bytesAddedToModel != 0)
    {
        if (bytesAddedToModel >= 1 && bytesAddedToModel < 10)
        {
            double l_n = static_cast<double>(encoder.getBitsWrited()) / bytesAddedToModel;
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
        }

        if (bytesAddedToModel == 10)
        {
            double l_n = static_cast<double>(encoder.getBitsWrited()) / bytesAddedToModel;
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
        }

        if (bytesAddedToModel == 100)
        {
            double l_n = static_cast<double>(encoder.getBitsWrited()) / bytesAddedToModel;
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
        }

        if (bytesAddedToModel == 500)
        {
            double l_n = static_cast<double>(encoder.getBitsWrited()) / bytesAddedToModel;
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
        }
        
        double l_n = static_cast<double>(encoder.getBitsWrited()) / bytesAddedToModel;

        if (bytesAddedToModel % 1000 == 0) {
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
        }
    } */

 
/*     if (bytesAddedToModel != 0)
    {

        if (bytesAddedToModel % 10000 == 0) {
            uint64_t last1000bits = encoder.getBitsWrited() - pastBitWrited;
            double l_n = static_cast<double>(last1000bits) / 10000;
            saveCompressionData(bytesAddedToModel, l_n, "compression_data.csv");
            pastBitWrited = encoder.getBitsWrited();
        }

    }  */
      



    // Percorremos os contextos do mais profundo existente até o mais raso tentando codificar o simbolo
    for (int depth = maxExistentContext; depth >= 0; --depth) {
        currentNode = root;

        // Define o subcontexto de acordo com o tamanho atual
        std::vector<uint16_t> subContext(context.end() - depth, context.end());

        // Percorre o subcontexto atual inteiro, descendo na árvore
        for (uint16_t ctxSymbol : subContext) {
            TrieNode* nextNode = currentNode->getChild(ctxSymbol);
            currentNode = nextNode; // Avança para o próximo nível
        }

        // Pega o vetor com as frequências 
        currentFrequencies = getSortedFrequencies(subContext);

        // Substrai os valores rejeitados de outras iterações
        subtractFrequenciesVector(currentFrequencies,rejectedFrequencies);

        // Verifica se o simbolo pode ser codificado
        if (currentNode->getChild(symbol) != nullptr)
        {
            // Codifica o simbolo
            encoder.encodeSymbol(symbol, currentFrequencies);
            updateAllTables(symbol, depth, context);

            return;
        }

        // Codigica o rho
        encoder.encodeSymbol(256, currentFrequencies);

        // Remove o rho da frequencia de contadores do contexto atual
        currentFrequencies.erase(
        std::remove_if(currentFrequencies.begin(), currentFrequencies.end(),
            [&](const std::pair<uint16_t, int>& freq) {
                return freq.first == 256;
            }),
        currentFrequencies.end());

        // Soma as frequeêncis rejeitadas atualmente com as frequências gerais rejeitas
        addFrequenciesVector(rejectedFrequencies, currentFrequencies);

    }

    // Caso percorra o for sem codificar o sybolo, ele nunca foi codificado antes
    // Codifica com equiprobabilidade
    encoder.encodeSymbol(symbol, equiprobableSymbols);

    if (maxBytes != -1 && bytesAddedToModel >= maxBytes) {
        // Se o limite de bytes foi atingido, não adiciona mais
        return;
    }

    // Retira o Symbolo após condifica-lo
    equiprobableSymbols.erase(
    std::remove_if(equiprobableSymbols.begin(), equiprobableSymbols.end(),
        [&](const std::pair<uint16_t, int>& freq) {
            return freq.first == symbol;
        }),
    equiprobableSymbols.end());

    updateAllTables(symbol, -1, context);



    return;
}

// Retorna os filhos e suas contagens (para codificação aritmética)
std::vector<std::pair<uint16_t, int>> PPMc::getSortedFrequencies(const std::vector<uint16_t>& context) {
    TrieNode* currentNode = root;
    for (uint16_t symbol : context) {
        currentNode = currentNode->getChild(symbol);
        if (currentNode == nullptr) {
            return {};
        }
    }

    std::vector<std::pair<uint16_t, TrieNode*>> nodes(currentNode->getChildren().begin(), currentNode->getChildren().end());
    std::sort(nodes.begin(), nodes.end(), [](const auto& a, const auto& b) {
        return a.second->getCount() > b.second->getCount();  // Ordena por contagem
    });

    std::vector<std::pair<uint16_t, int>> sortedFrequencies;
    for (auto& node : nodes) {
        sortedFrequencies.push_back({node.first, node.second->getCount()}); // Guarda a o simbolo e sua contagem
    }

    return sortedFrequencies;
}

void PPMc::compressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    ArithmeticEncoder encoder(output);
    std::vector<uint16_t> context;

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Erro ao abrir arquivos!" << std::endl;
        return;
    }

    // Lê o arquivo byte por byte e codifica
    char byte;
    while (input.get(byte)) {
        uint8_t symbol = static_cast<uint8_t>(byte);
        encodeAndUpdateContexts(symbol, encoder, context);

        if(maxContext != 0){
            // Atualiza o contexto
            if (context.size() >= maxContext) {
                // Remove o símbolo mais antigo
                context.erase(context.begin());
            }
            // Adiciona o novo símbolo ao contexto
            context.push_back(symbol);
        }
    }

    // EOF
    encodeAndUpdateContexts(257, encoder, context);



    // Finaliza a codificação
    encoder.finishEncoding();

    input.close();
    output.close();
    std::cout << "Compressão concluída!" << std::endl;
} 

void PPMc::subtractFrequenciesVector(std::vector<std::pair<uint16_t, int>>& currentFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies) {
    currentFrequencies.erase(
        std::remove_if(currentFrequencies.begin(), currentFrequencies.end(),
            [&](const std::pair<uint16_t, int>& current) {
                return std::any_of(rejectedFrequencies.begin(), rejectedFrequencies.end(),
                    [&](const std::pair<uint16_t, int>& rejected) {
                        return current.first == rejected.first;
                    });
            }),
        currentFrequencies.end());

    // Reordena por contador, do maior para o menor
    std::sort(currentFrequencies.begin(), currentFrequencies.end(),
        [](const std::pair<uint16_t, int>& a, const std::pair<uint16_t, int>& b) {
            return a.second > b.second;
        });
}

void PPMc::addFrequenciesVector(std::vector<std::pair<uint16_t, int>>& globalRejectedFrequencies, const std::vector<std::pair<uint16_t, int>>& rejectedFrequencies) {
    for (const auto& rejected : rejectedFrequencies) {
        auto it = std::find_if(globalRejectedFrequencies.begin(), globalRejectedFrequencies.end(),
            [&](const std::pair<uint16_t, int>& current) {
                return current.first == rejected.first;
            });

        if (it == globalRejectedFrequencies.end()) {
            globalRejectedFrequencies.push_back(rejected);
        }
    }

}

bool PPMc::decodeAndUpdateContexts(ArithmeticDecoder& decoder, std::vector<uint16_t>& context, std::ofstream& output) {
    bool wasSymbolDecoded = false;
    TrieNode* currentNode = root;

    int maxExistentContext = findMaxExistentContext(context);

    std::vector<std::pair<uint16_t, int>> currentFrequencies;
    std::vector<std::pair<uint16_t, int>> rejectedFrequencies;
    uint16_t trueDecodedSymbol;

    // Percorrer contextos do maior existente até o contexto 0
    for (int depth = maxExistentContext; depth >= 0; --depth) {
        currentNode = root;

        // Subcontexto para a profundidade atual
        std::vector<uint16_t> subContext(context.end() - depth, context.end());

        // Percorrer o subcontexto descendo a árvore
        for (uint16_t ctxSymbol : subContext) {
            TrieNode* nextNode = currentNode->getChild(ctxSymbol);
            currentNode = nextNode;
        }

        // Obter as frequências do contexto
        currentFrequencies = getSortedFrequencies(subContext);

        // Subtrair as frequências rejeitadas de outras iterações
        subtractFrequenciesVector(currentFrequencies, rejectedFrequencies);

        // Decodificar o símbolo atual usando o decodificador aritmético

        uint16_t decodedSymbol = decoder.decodeSymbol(currentFrequencies);

        if (decodedSymbol == 257) {
            return false; //Se for, para a decodificação
        }

        // Verificar se o símbolo decodificado é o especial (rho)
        if (decodedSymbol == 256) {
            // Se o símbolo é rho, devemos descer o contexto

            // Remove o rho da frequencia de contadores do contexto atual
            currentFrequencies.erase(
            std::remove_if(currentFrequencies.begin(), currentFrequencies.end(),
                [&](const std::pair<uint16_t, int>& freq) {
                    return freq.first == 256;
                }),
            currentFrequencies.end());

            addFrequenciesVector(rejectedFrequencies, currentFrequencies);
            continue; // Tentar um contexto menor
        }

        // Caso contrário, é o símbolo original
        output.put(static_cast<char>(decodedSymbol));

        trueDecodedSymbol = decodedSymbol;

        updateAllTables(trueDecodedSymbol, depth, context);

        if(maxContext != 0){
            // Atualiza o contexto
            if (context.size() >= maxContext) {
                // Remove o símbolo mais antigo
                context.erase(context.begin());
            }
            // Adiciona o novo símbolo ao contexto
            context.push_back(decodedSymbol);
        }
        wasSymbolDecoded = true;
        break;
    }

    if (!wasSymbolDecoded) {
        // Caso nenhum símbolo tenha sido decodificado, usar equiprobabilidade
        uint16_t symbol = decoder.decodeSymbol(equiprobableSymbols);

        trueDecodedSymbol = symbol;
        // Verificar se o símbolo decodificado é o EOF
        if (trueDecodedSymbol == 257) {
            return false; //Se for, para a decodificação
        }
        output.put(static_cast<char>(symbol));
    // Remover o símbolo da equiprobabilidade
    equiprobableSymbols.erase(
        std::remove_if(equiprobableSymbols.begin(), equiprobableSymbols.end(),
            [&](const std::pair<uint16_t, int>& freq) {
                return freq.first == symbol;
            }),
        equiprobableSymbols.end());

    updateAllTables(trueDecodedSymbol, -1, context);  
        
        if(maxContext != 0){
            // Atualiza o contexto
            if (context.size() >= maxContext) {
                // Remove o símbolo mais antigo
                context.erase(context.begin());
            }
            // Adiciona o novo símbolo ao contexto
            context.push_back(symbol);
        }

    if (maxBytes != -1 && bytesAddedToModel >= maxBytes) {
        // Se o limite de bytes foi atingido, não adiciona mais
        return true;
    }


    }
    

    return true;

}


void PPMc::decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);
    ArithmeticDecoder decoder(input);
    std::vector<uint16_t> context;

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Erro ao abrir arquivos!" << std::endl;
        return;
    }


    /// Que horroroso
    bool isNotTheEnd = true;
    //int temp = 5;
    while (isNotTheEnd) {
        isNotTheEnd = decodeAndUpdateContexts(decoder, context, output);
        
    }

    input.close();
    output.close();
    std::cout << "Descompressão concluída!" << std::endl;
}


int PPMc::findMaxExistentContext(const std::vector<uint16_t>& context) {
    TrieNode* currentNode = root;

    // O número de contextos presentes é dado pelo tamanho do vetor de contextos
    int currentContextSize = context.size();

    // Percorremos os contextos do mais profundo até o mais raso
    for (int depth = currentContextSize; depth >= 0; --depth) {
        currentNode = root;
        bool inexistentContext = false;

        // Define o subcontexto de acordo com o tamanho atual
        std::vector<uint16_t> subContext(context.end() - depth, context.end());

        

        // Percorre o subcontexto atual inteiro, descendo na árvore
        for (uint16_t ctxSymbol : subContext) {
            TrieNode* nextNode = currentNode->getChild(ctxSymbol);

            //Se o eu não consigo navegar pelo contexto inteiro, ele não existe
            if (nextNode == nullptr)
            {
                inexistentContext = true;
                break;
            }
            
            currentNode = nextNode; // Avança para o próximo nível
        }

        //
        if (inexistentContext)
        {
            continue;;
        }
        
        
        // Agora estamos no nó do contexto apropriado. Vamos verificar se o contexto já existe.
        if (!(currentNode->getChildren().empty())) {
            // Se o contexto foi encontrado, retornamos a profundidade do contexto
            return depth;
        }
    }

    // Se não encontramos o símbolo em nenhum contexto, retornamos -1
    return -1;
}

void PPMc::updateAllTables(uint16_t symbol, int levelKOfCodifiedSymbol, const std::vector<uint16_t>& context) {
    if (maxBytes != -1 && bytesAddedToModel >= maxBytes) {
        // Se o limite de bytes foi atingido, não adiciona mais
        return;
    }
    TrieNode* currentNode = root;
    
    // O número de contextos presentes é dado pelo tamanho do vetor de contextos
    int currentContextSize = context.size();

    // Percorremos os contextos do mais profundo até o mais raso
    for (int depth = currentContextSize; depth >= 0; --depth) {
        currentNode = root;
        // Define o subcontexto de acordo com o tamanho atual
        std::vector<uint16_t> subContext(context.end() - depth, context.end());

        // Percorre o subcontexto atual inteiro, descendo na árvore
        for (uint16_t ctxSymbol : subContext) {
            TrieNode* nextNode = currentNode->getChild(ctxSymbol);
            currentNode = nextNode; // Avança para o próximo nível
        }

        // Agora estamos no nó do contexto apropriado
        TrieNode* symbolNode = currentNode->getChild(symbol);
        if (symbolNode != nullptr) {
            // Se o símbolo já existe, incrementa o contador
            symbolNode->incrementCount();
        } else {
            // Caso contrário, cria e incrementa o contador
            currentNode->createChild(symbol)->incrementCount();
        }

        // Caso a profundidade atual seja maior do que a profundidade a qual o símbolo foi codificado, incrementa Rho
        if (depth > levelKOfCodifiedSymbol) {
            TrieNode* rhoNode = currentNode->getChild(256);
            if (rhoNode == nullptr) {
                currentNode->createChild(256)->incrementCount();
            } else {
                rhoNode->incrementCount();
            }
        }
    }

    bytesAddedToModel++;
    return;
}


void PPMc::serializeTrie(TrieNode* root, std::ofstream& outFile) {
    if (!root) return;
    
    // Salva o contador do nó
    int count = root->getCount();
    outFile.write(reinterpret_cast<const char*>(&count), sizeof(int));


    // Salva o número de filhos
    uint16_t numChildren = root->getChildren().size();
    outFile.write(reinterpret_cast<const char*>(&numChildren), sizeof(uint16_t));

    // Para cada filho, grava o símbolo e serializa recursivamente o nó filho
    for (const auto& [symbol, childNode] : root->getChildren()) {
        outFile.write(reinterpret_cast<const char*>(&symbol), sizeof(uint16_t));
        serializeTrie(childNode, outFile);
    }
}

TrieNode* PPMc::deserializeTrie(std::ifstream& inFile) {
    int count;
    inFile.read(reinterpret_cast<char*>(&count), sizeof(int));

    TrieNode* node = new TrieNode();
    node->setCount(count);

    uint16_t numChildren;
    inFile.read(reinterpret_cast<char*>(&numChildren), sizeof(uint16_t));

    for (uint16_t i = 0; i < numChildren; ++i) {
        uint16_t symbol;
        inFile.read(reinterpret_cast<char*>(&symbol), sizeof(uint16_t));

        TrieNode* childNode = deserializeTrie(inFile);
        node->adoptChild(symbol,childNode);
    }

    return node;
}

void PPMc::saveModel(const std::string& modelFile) {
    std::ofstream outFile(modelFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para salvar o modelo." << std::endl;
        return;
    }
    serializeModel(root, equiprobableSymbols, outFile);
    outFile.close();
}

void PPMc::loadModel(const std::string& modelFile) {
    std::ifstream inFile(modelFile, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para carregar o modelo." << std::endl;
        return;
    }
    deserializeModel(inFile);
    inFile.close();

}

void PPMc::serializeModel(TrieNode* root, const std::vector<std::pair<uint16_t, int>>& equiprobableSymbols, std::ofstream& outFile) {
    // Serializa a Trie (árvore)
    serializeTrie(root, outFile);

    // Serializa o vetor equiprobableSymbols
    uint16_t vectorSize = equiprobableSymbols.size();
    outFile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(uint16_t));

    for (const auto& [symbol, frequency] : equiprobableSymbols) {
        outFile.write(reinterpret_cast<const char*>(&symbol), sizeof(uint16_t));
        outFile.write(reinterpret_cast<const char*>(&frequency), sizeof(int));
    } 
}

void PPMc::deserializeModel(std::ifstream& inFile) {
    root = deserializeTrie(inFile);

    // Desserializa o vetor equiprobableSymbols
    uint16_t vectorSize;
    inFile.read(reinterpret_cast<char*>(&vectorSize), sizeof(uint16_t));

    equiprobableSymbols.clear();

    for (uint16_t i = 0; i < vectorSize; ++i) {
        uint16_t symbol;
        int frequency;
        inFile.read(reinterpret_cast<char*>(&symbol), sizeof(uint16_t));
        inFile.read(reinterpret_cast<char*>(&frequency), sizeof(int));
        equiprobableSymbols.emplace_back(symbol, frequency);
    }

}

void PPMc::saveCompressionData(uint64_t n, double l_n, const std::string& filename) {
    std::ofstream outFile;
    outFile.open(filename, std::ios_base::app);  // Abre o arquivo em modo append
    if (outFile.is_open()) {
        outFile << n << "," << l_n << "\n";  // Salva n e l(n) no formato CSV
        outFile.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo para salvar dados." << std::endl;
    }
}