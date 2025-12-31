# Projeto Dinno Meteor Versão 02

Jogo em linguagem C e Allegro 5, sobre o dinossauro de come meteoros.


## Stack utilizada

**Linguagem C** : Desenvolvimento de Struct, Estados, Variaveis e constantes.

**Allegro 5** : Desenvolvimento de Cores, sprites, imagens, frames, sons e outros.

## Autores

- [Hudson Sena](https://github.com/HudsonSena)
- Filipe Holanda
- Pedro Henrique

## Observações: 
O jogo ainda não está terminado, mas esta dentro dos requisitos para avaliação. Conforme o tempo, vamos adicionar imagens e sprites para melhorar a visualização e experiência do jogo.

## -----------------------------------------------

# Guia Completo para Compilar Allegro 5 com GCC no Windows

Este é um tutorial definitivo que ensina o método mais robusto e livre de erros para configurar um ambiente de desenvolvimento C/C++ com a biblioteca Allegro 5 no Windows, utilizando o MSYS2 e o gerenciador de pacotes `pacman`.

---

Allegro é uma biblioteca multiplataforma voltada principalmente para programação de jogos e multimídia. Ela lida com tarefas comuns e de baixo nível, como criar janelas, receber entrada do usuário, carregar dados, desenhar imagens, reproduzir sons, entre outras, abstraindo a complexidade da plataforma subjacente. No entanto, Allegro não é uma engine de jogos: você tem liberdade para projetar e estruturar seu programa como preferir.

- [Fonte: Site Oficial do Allegro](https://liballeg.org/)

### Parte 1: Configuração Única do Ambiente

Esta é a base do nosso ambiente. Você só precisa fazer isso uma vez. A ferramenta principal aqui é o **MSYS2**.

passos de instalação

  1. **Instalar o MSYS2 e Atualizar**
    Vá para no [Site Oficial do MSyS2](https://www.msys2.org/) e baixe o instalador.
    Instale seguindo as instruções padrão.
    Ao final, abra o terminal MSYS2 MSYS (geralmente com ícone roxo). Use este terminal apenas para a atualização inicial do sistema.
    Execute o comando:
    <code>pacman -Syu<code>
    
  AVISO IMPORTANTE: Durante o comando <code>pacman -Syu</code>, é normal que o terminal peça para ser fechado no meio do processo. Se isso acontecer, feche a janela, abra o mesmo terminal MSYS2 MSYS novamente e execute o comando <code>pacman -Syu</code> mais uma vezpara garantir que a atualização seja concluída.
    

  2. **Instalar o Compilador GCC e o Allegro 5**
    A partir de agora, para todo o resto, use sempre o terminal MSYS2 MINGW64 (geralmente com ícone azul). Ele é a sua bancada de trabalho principal para programar.
    
  Execute o comando abaixo para instalar de uma só vez o compilador, as ferramentas e a biblioteca Allegro 5:
    <pre><code>pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-pkg-config mingw-w64-x86_64-allegro</code></pre>
    O <code>pacman</code> cuidará de todas as dependências automaticamente.

Com o ambiente configurado, você está pronto para compilar qualquer projeto Allegro.

---

### Parte 2: Compilando e Executando seu Projeto

Este é o fluxo de trabalho que você usará toda vez que for compilar.

1.  **Abra o Terminal Correto:** Sempre use o terminal **MSYS2 MINGW64** (Azul).

2.  **Navegue até a Pasta do Projeto:** Use o comando `cd` com o formato de caminho do Linux.
    ```bash
    # Exemplo para uma pasta na Área de Trabalho
    cd /c/Users/SeuNome/Desktop/MeuProjeto
    ```

3.  **Compile o Código:** Use o `gcc` em conjunto com o `pkg-config`. O `pkg-config` é uma ferramenta que informa ao `gcc` exatamente quais flags usar para encontrar e linkar as bibliotecas do Allegro.
    
    O jogo precisa das seguintes libs, o comando fica assim:
    ```bash
    gcc main.c dinossauro.c meteoro.c item.c score.c -o dinnoMeteor_demo -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image

    ```

4.  **Execute o Programa:** Se o comando acima não retornou erros, o arquivo `dinnoMeteor_demo` foi criado. Para rodá-lo:
    ```bash
    ./dinnoMeteor_demo.exe
    ```