Projeto 1 (Proj1) - Processamento de Imagens com SDL3, SDL_image e SDL_ttf
Integrantes do grupo

- Gabriel Alves de Freitas Spinola Sucupira - RA: 10418133
- Henrique Pena Ribeiro - RA: 10417975
- Luiz Henrique Ribeiro Pulga - RA: 10409246

Disciplina

Computação Visual

Universidade Presbiteriana Mackenzie

Professor: André Kishimoto
Descrição do projeto

Este projeto consiste em uma aplicação desenvolvida em linguagem C para o processamento e análise estatística de imagens. Utilizando a biblioteca SDL3 para interface e renderização, o software permite visualizar uma imagem, analisar seu histograma em tempo real e aplicar a técnica de equalização de histograma para melhoria de contraste.

O programa opera com um sistema de duas janelas: uma dedicada à exibição da imagem (original ou processada) e outra técnica, que apresenta o histograma de intensidades, métricas de brilho/contraste e controles de interação.
Funcionalidades implementadas

    Carregamento de imagem

        Utiliza a biblioteca SDL_image para suporte a múltiplos formatos (PNG, JPG, BMP).

        Conversão automática para o formato SDL_PIXELFORMAT_RGBA32 para garantir consistência no acesso aos pixels.

    Análise e conversão para escala de cinza

        O programa detecta se a imagem já é monocromática.

        Caso seja colorida, realiza a conversão para tons de cinza utilizando a fórmula de luminosidade:
        Y=0.2125⋅R+0.7154⋅G+0.0721⋅B

    GUI com duas janelas

        Janela Principal: Exibe a imagem atual em suas dimensões originais.

        Janela de Histograma: Exibe o gráfico de frequências e informações estatísticas em uma janela de 460×620 pixels.

    Análise e exibição do histograma

        Gráfico dinâmico com 256 níveis de intensidade.

        Métricas calculadas:

            Média de intensidade (Brilho).

            Desvio padrão (Contraste).

        Classificações automáticas:

            Brilho: Escura (meˊdia<85), Média ou Clara (meˊdia≥170).

            Contraste: Baixo (desvio<40), Médio ou Alto (desvio≥80).

    Equalização do histograma

        Implementação via Função de Distribuição Acumulada (CDF).

        Botão interativo que alterna instantaneamente entre a versão original em cinza e a versão equalizada.

    Salvar imagem

        Tecla S: Exporta a imagem que está sendo visualizada no momento para um arquivo chamado output_image.png.

Organização do código
Plaintext

proj1_final/
├── CMakeLists.txt
├── README.md
├── assets/
│   └── image1.jpg
│   └── image2.jpg
└── src/
    ├── main.c
    ├── app.c
    ├── app.h
    ├── image_processing.c
    ├── image_processing.h
    ├── ui.c
    ├── ui.h
    ├── font_utils.c
    └── font_utils.h

Bibliotecas utilizadas

    SDL3: Core da aplicação e renderização.

    SDL3_image: Carregamento e salvamento de arquivos de imagem.

    SDL3_ttf: Renderização de textos e fontes TrueType.

Dependências
Linux (Ubuntu/Debian)
Bash

sudo apt-get update
sudo apt-get install build-essential cmake libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev

Windows com vcpkg
Bash

vcpkg install sdl3:x64-windows sdl3-image:x64-windows sdl3-ttf:x64-windows

Compilação
Linux / macOS
Bash

cmake -S . -B build
cmake --build build

Windows
Bash

cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release

Execução
Bash

./build/bin/proj1 caminho_da_imagem.png

Decisões de implementação

    Gerenciamento de Estado: Criamos uma estrutura ImageState que mantém as superfícies original e equalizada em memória, evitando processamento redundante ou releitura de disco ao alternar modos.

    Interface Desacoplada: A lógica de desenho da UI (ui.c) foi separada do processamento de imagem (image_processing.c) para facilitar a manutenção.

    Robustez de Fontes: Implementamos o font_utils.c para buscar fontes em diretórios comuns de diversos sistemas operacionais (Windows, Linux, macOS), garantindo que o programa execute mesmo sem o diretório assets configurado.

Contribuições de cada integrante

    Gabriel: Estrutura da aplicação (app.c), gerenciamento de janelas e loop de eventos principal.

    Henrique: Lógica de processamento de imagem (image_processing.c), incluindo conversão de escala de cinza e cálculo de histograma/estatísticas.

    Gabriel e Henrique: Implementação do algoritmo de equalização por CDF e funcionalidade de salvamento de arquivos.

    Luiz: Desenvolvimento da interface gráfica (ui.c), renderização do gráfico do histograma e sistema de botões interativos.

Testes realizados

    Carregamento de imagens em alta resolução.

    Teste de redimensionamento implícito e posicionamento de janelas.

    Verificação da precisão estatística da média e desvio padrão.

    Teste de interatividade do botão (hover e press states).

    Validação da gravação do arquivo output_image.png após equalização.
