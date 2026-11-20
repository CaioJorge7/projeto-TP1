# Sistema de Gestão de Projetos Kanban — TP1

Sistema de gestão de projetos baseado no método Kanban desenvolvido em C++ (padrão C++11) para a disciplina **Técnicas de Programação 1 (TP1)**, com projeto compatível com **Code::Blocks**. O sistema organiza o fluxo em quadros com as colunas **A FAZER**, **FAZENDO** e **FEITO**, papéis de **GESTOR** e **DESENVOLVEDOR**, autenticação de usuários e métricas de acompanhamento Kanban.

---

## Visão Geral e Arquitetura

A arquitetura segue o estilo em camadas e módulos com interfaces abstratas (classes abstratas com métodos virtuais puros), documentação em **Doxygen** (HTML), modelagem **UML** (diagrama de classes, módulos, interfaces e relacionamentos, incluindo artefatos em `.dot` e `.pdf`) e suítes de testes unitários.

* **Camada de Apresentação**: responsável pela interface com o usuário (incluindo classes controladoras da apresentação), leitura e pré-validação de dados de entrada, tratamento de exceções e comunicação com a camada de negócio exclusivamente por meio das interfaces abstratas (`IServico*`).
* **Camada de Serviço**: responsável pela lógica de negócio, controle de sessão/permissões, regras Kanban, cálculo de métricas e armazenamento de dados por meio de classes controladoras e contêineres (armazenamento em memória com `std::map`).
* **Camadas de Dados e Contratos**: classes de **Domínio** (com validação e exceção no `set`), classes de **Entidade** (com atributos do tipo domínio e chaves primárias imutáveis após a construção) e **Interfaces** abstratas que desacoplam os módulos.

---

## Estrutura do Repositório

```text
├── docs/                                    # Modelo de arquitetura (PDF) e fontes .dot dos diagramas UML
├── include/                                 # Cabeçalhos (.h) das camadas e utilitários
│   ├── dominios.h                           # Declaração das 11 classes de domínio (Doxygen)
│   ├── entidades.h                          # Declaração das 4 entidades do Kanban (Doxygen)
│   ├── interfaces.h                         # Interfaces abstratas (métodos virtuais puros)
│   ├── containers.h                         # Contêineres em memória (std::map)
│   ├── sessao.h                             # Controle de usuário autenticado compartilhado
│   ├── tempo.h                              # Conversão de Timestamp em minutos
│   ├── controladoras_pessoa.h               # CSAutenticacao e CSPessoa
│   ├── controladoras_projeto.h              # CSProjeto
│   ├── controladoras_quadro.h               # CSQuadro
│   └── controladoras_cartao.h               # CSCartao (CRUD, mover, métricas e WIP)
├── src/                                     # Implementações (.cpp)
│   ├── dominios.cpp                         # Regras de validação e métodos set/get dos domínios
│   └── entidades.cpp                        # Construtores e métodos set/get das entidades
├── tests/                                   # Suítes de testes unitários
│   ├── testes_dominios.h / .cpp             # Testes de sucesso e falha dos 11 domínios
│   ├── testes_entidades.h / .cpp            # Testes de entidades e interfaces abstratas
│   └── testes_servicos.h / .cpp             # 27 casos de teste (autenticação + 26 serviços)
├── main.cpp                                 # Ponto de entrada do sistema integrado (Apresentação)
├── main_teste_dominios.cpp                  # Executável isolado de testes de domínios
├── main_teste_entidades.cpp                 # Executável isolado de testes de entidades
├── main_teste_servicos.cpp                  # Executável isolado de testes de serviços
├── projeto_tp1.cbp                          # Projeto para a IDE Code::Blocks
├── EQUIPE.txt                               # Identificação dos membros e artefatos desenvolvidos
└── README.md                                # Documentação geral do projeto
```


---

## Especificação por Camada

### 1. Domínios (`include/dominios.h`, `src/dominios.cpp`)
Cada domínio é uma classe com atributo de tipo suportado pela linguagem e acesso via métodos públicos `setValor` e `getValor`. O método `setValor` valida o formato e lança `std::invalid_argument` (que deriva de `std::logic_error`) em caso de valor inválido. Conforme a especificação, as validações não precisam considerar acentuação nem a letra `ç`.

1. **`Email`**: `parte-local@domínio`. Parte local (até 64 caracteres) pode ter `a-z`, `0-9`, `.` ou `-`, sem começar ou terminar com `.` ou `-`, e `.` ou `-` deve ser seguido por letra(s) ou dígito(s). Domínio (até 255 caracteres) tem partes separadas por `.` com `a-z`, `0-9` ou `-`, sem iniciar ou terminar parte com `-`.
2. **`Estado`**: valores válidos `A FAZER`, `FAZENDO` ou `FEITO`.
3. **`Identificador`**: 6 caracteres, sendo os 3 primeiros letras (`a-z` ou `A-Z`) e os 3 últimos dígitos (`0-9`).
4. **`Limite`**: valor inteiro de `1` a `25` (define o limite máximo de itens simultâneos em `FAZENDO` / WIP do quadro).
5. **`Nome`**: até 15 caracteres (`A-Z`, `a-z` ou espaço em branco); não começa nem termina com espaço em branco, e espaço em branco é seguido por letra.
6. **`Papel`**: valores válidos `GESTOR` ou `DESENVOLVEDOR`.
7. **`Prioridade`**: valores válidos `ALTA`, `MEDIA` ou `BAIXA`.
8. **`Senha`**: 5 caracteres alfanuméricos (`a-z`, `A-Z`, `0-9`), contendo ao menos 1 letra e ao menos 1 dígito.
9. **`Tamanho`**: valores válidos `GRANDE`, `MEDIO` ou `PEQUENO`.
10. **`Texto`**: até 30 caracteres (`a-z`, `A-Z`, `0-9`, espaço em branco ou pontuação `,`, `;`, `.`, `:`, `?`, `!`); inicia com letra maiúscula, termina com ponto final (`.`) e não tem dois sinais de pontuação em sequência.
11. **`Timestamp`**: formato `DIA-MÊS-ANO-HORÁRIO`, com `DIA` de `1` a `31`, `MÊS` em `JAN`, `FEV`, `MAR`, `ABR`, `MAI`, `JUN`, `JUL`, `AGO`, `SET`, `OUT`, `NOV`, `DEZ`, `ANO` de `2000` a `2099` e `HORÁRIO` de `00:00` a `23:59`, validando a data inclusive em anos bissextos.

---

### 2. Entidades e Interfaces (`include/entidades.h`, `src/entidades.cpp`, `include/interfaces.h`)
As entidades utilizam exclusivamente atributos do tipo domínio com métodos públicos `set` e `get`, preservando as chaves primárias imutáveis após a construção e mantendo referências entre entidades por atributos de domínio (sem guardar ponteiros ou objetos de entidade dentro de outra entidade), garantindo multiplicidades e evitando inconsistências.

* **`Pessoa`**: `email: Email` (PK), `nome: Nome`, `senha: Senha`, `papel: Papel`.
* **`Projeto`**: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `inicio: Timestamp`, `termino: Timestamp`, `gestorEmail: Email` (referencia `Pessoa` gestora).
* **`Quadro`**: `identificador: Identificador` (PK), `nome: Nome`, `limite: Limite`, `projetoId: Identificador` (referencia `Projeto`).
* **`CartaoAtividade`**: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `prioridade: Prioridade`, `estado: Estado`, `tamanho: Tamanho`, `entrada: Timestamp`, `inicio: Timestamp`, `termino: Timestamp`, `quadroId: Identificador` (referencia `Quadro`), `desenvolvedorEmail: Email` (referencia `Pessoa` desenvolvedora).
  * **Ciclo de vida das datas**: na criação, apenas `entrada` é informada. `inicio` e `termino` começam vazios e são preenchidos quando o serviço realiza as transições; use `temInicio()` e `temTermino()` antes de acessar os valores.
* **Interfaces Abstratas (`include/interfaces.h`)**: contratos com métodos virtuais puros (`IServicoAutenticacao`, `IServicoPessoa`, `IServicoProjeto`, `IServicoQuadro` e `IServicoCartaoAtividade`), cobrindo autenticação e as 26 funcionalidades dos requisitos funcionais.

---

### 3. Camada de Serviço (`include/containers.h`, `sessao.h`, `tempo.h`, `controladoras_*.h`)
Implementada no formato *header-only* (sem `.cpp` próprio; basta incluir `include/` no caminho de busca). As controladoras realizam as interfaces abstratas, operam sobre quatro contêineres em memória (`std::map`) e verificam sessão e regras de negócio via `Sessao`.

* **Exceções**: violações de regra de negócio, permissão, autenticação, duplicidade, referência inexistente ou estado inválido lançam `std::logic_error`; falhas de formato lançam `std::invalid_argument`.
* **Unidade de tempo**: todos os tempos do sistema (tempo de ciclo, lead time e suas médias) são expressos em **minutos**, calculados a partir dos `Timestamp`.

| Serviço / Funcionalidade | Permissão / Quem pode | Regras Aplicadas |
| :--- | :--- | :--- |
| **Autenticar** | Qualquer usuário | Inicia sessão com e-mail e senha; se falhar, encerra também a sessão anterior. |
| **Criar Pessoa** | Sem login | Impede e-mail duplicado; demais serviços exigem usuário autenticado. |
| **Ler / Listar Pessoa** | Autenticado | Leitura exibe os dados da pessoa (sem a senha); listagem exibe o e-mail (identificador) de cada pessoa. |
| **Atualizar Pessoa** | Próprio usuário | Chave primária (`email`) não muda; troca de `papel` só ocorre se não houver projetos geridos nem cartões vinculados. |
| **Excluir Pessoa** | Próprio usuário | Bloqueado se for gestor de projeto ou desenvolvedor de cartão; encerra a sessão. |
| **Criar Projeto** | `GESTOR` | Quem cria passa a ser o gestor do projeto (`gestorEmail` é definido pelo usuário autenticado); término não pode ser anterior ao início. |
| **Ler / Listar Projeto** | Autenticado | Leitura exibe os dados do projeto; listagem exibe o identificador de cada projeto. |
| **Atualizar Projeto** | Gestor do projeto | Identificador e gestor originais não mudam; término não pode ser anterior ao início. |
| **Excluir Projeto** | Gestor do projeto | Bloqueado enquanto houver quadros vinculados ao projeto. |
| **Criar Quadro** | Gestor do projeto | Exige projeto existente. |
| **Ler / Listar Quadro** | Autenticado | Leitura exibe os dados do quadro; listagem exibe o identificador de cada quadro. |
| **Atualizar Quadro** | Gestor do projeto | Novo `limite` não pode ser menor que o WIP atual; identificador e `projetoId` não mudam. |
| **Excluir Quadro** | Gestor do projeto | Bloqueado enquanto houver cartões no quadro. |
| **Criar Cartão** | Gestor do projeto | Exige desenvolvedor existente com papel `DESENVOLVEDOR`; inicia em `A FAZER` com `entrada` obrigatória. |
| **Ler / Listar Cartão** | Autenticado | Leitura exibe os dados do cartão; listagem exibe o identificador de cada cartão. |
| **Atualizar Cartão** | Gestor do projeto | Permite trocar desenvolvedor válido; estado, datas e quadro só mudam via movimentação. |
| **Excluir Cartão** | Gestor do projeto | Remove o cartão sem deixar referências inconsistentes. |
| **Mover Cartão** | Autenticado | Permite apenas `A FAZER -> FAZENDO` (respeitando limite WIP e registrando `inicio`) e `FAZENDO -> FEITO` (registrando `termino`), respeitando a ordem cronológica. |
| **Ler WIP** | Autenticado | Quantidade de cartões do quadro atualmente em `FAZENDO`. |
| **Ler Tempo de Ciclo / Medio** | Autenticado | Para cartões em `FEITO`: `termino - inicio` (em minutos). Média sobre concluídos do quadro (lança exceção se não houver concluídos). |
| **Ler Lead Time / Medio** | Autenticado | Para cartões em `FEITO`: `termino - entrada` (em minutos). Média sobre concluídos do quadro (lança exceção se não houver concluídos). |

---

### 4. Camada de Apresentação (`main.cpp` e controladoras de apresentação)
Responsável pela interação com o usuário, validação de entrada, tratamento de exceções e uso exclusivo das interfaces abstratas (`IServico*`), sem acoplar diretamente às classes concretas de serviço.

#### Exemplo de instanciação e ligação das camadas

```cpp
#include "containers.h"
#include "sessao.h"
#include "controladoras_pessoa.h"
#include "controladoras_projeto.h"
#include "controladoras_quadro.h"
#include "controladoras_cartao.h"

ContainerPessoas pessoas;
ContainerProjetos projetos;
ContainerQuadros quadros;
ContainerCartoes cartoes;
Sessao sessao;

CSAutenticacao autenticacao(&pessoas, &sessao);
CSPessoa servicoPessoa(&pessoas, &projetos, &cartoes, &sessao);
CSProjeto servicoProjeto(&pessoas, &projetos, &quadros, &sessao);
CSQuadro servicoQuadro(&pessoas, &projetos, &quadros, &cartoes, &sessao);
CSCartao servicoCartao(&pessoas, &projetos, &quadros, &cartoes, &sessao);

// A apresentação deve depender apenas das interfaces:
IServicoAutenticacao* sAutenticacao = &autenticacao;
IServicoPessoa* sPessoa = &servicoPessoa;
IServicoProjeto* sProjeto = &servicoProjeto;
IServicoQuadro* sQuadro = &servicoQuadro;
IServicoCartaoAtividade* sCartao = &servicoCartao;
```

* **Fluxo de sessão**: `sPessoa->criar` pode ser chamado sem login; os demais serviços exigem `sAutenticacao->autenticar(...)` antes. Para encerrar a sessão (logout), utilize `sAutenticacao->encerrarSessao()`, que faz parte da interface.
* **Tratamento de erros**: envolva chamadas em blocos `try / catch` tratando `std::logic_error` (e `std::invalid_argument`) e exibindo `e.what()`.

---

## Compilação, Testes e Documentação

> **Atenção:** Cada arquivo `main*.cpp` possui sua própria função `main()`. Não compile mais de um arquivo com `main()` no mesmo executável.

### Code::Blocks
Abra `projeto_tp1.cbp` e escolha o alvo (*Build target*) na barra de ferramentas. Cada alvo já tem os arquivos certos associados:

| Alvo | Executável gerado | Conteúdo |
| :--- | :--- | :--- |
| `Debug` / `Release` | `bin/Debug/projeto_tp1` / `bin/Release/projeto_tp1` | Sistema integrado (menus em modo texto) |
| `Teste_Dominios` | `bin/Testes/teste_dominios` | Testes unitários dos 11 domínios |
| `Teste_Entidades` | `bin/Testes/teste_entidades` | Testes das entidades e das interfaces abstratas |
| `Teste_Servicos` | `bin/Testes/teste_servicos` | 27 casos de teste da camada de serviço |

Use *Build → Build and run* (F9). No Windows, os executáveis recebem a extensão `.exe`.

### Linha de comando (g++)

#### Sistema integrado
```bash
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude src/*.cpp main.cpp -o sistema_kanban
./sistema_kanban
```

#### Testes unitários por camada
* **Domínios**:
  ```bash
  g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude -Itests src/dominios.cpp tests/testes_dominios.cpp main_teste_dominios.cpp -o teste_dominios
  ./teste_dominios
  ```
* **Entidades**:
  ```bash
  g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude -Itests src/dominios.cpp src/entidades.cpp tests/testes_entidades.cpp main_teste_entidades.cpp -o teste_entidades
  ./teste_entidades
  ```
* **Serviços (27 casos: autenticação + 26 serviços)**:
  ```bash
  g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude -Itests src/dominios.cpp src/entidades.cpp tests/testes_servicos.cpp main_teste_servicos.cpp -o teste_servicos
  ./teste_servicos
  ```

### Documentação Doxygen

A documentação do código-fonte é gerada utilizando o Doxygen. Para gerar ou atualizar a documentação, execute na raiz do projeto:

```bash
doxygen Doxyfile
```

O arquivo de configuração `Doxyfile` define:
- `OUTPUT_DIRECTORY = docs/doxygen`: diretório de saída da documentação.
- `OUTPUT_LANGUAGE = Brazilian`: idioma da documentação em português.
- `HAVE_DOT = NO`: desativa a integração com Graphviz.

Após a geração, abra `docs/doxygen/html/index.html` em um navegador para consultar a documentação.

### Modelo de arquitetura
* **Modelo de arquitetura**: `docs/modelo_de_arquitetura.pdf` (diagrama UML de módulos, interfaces e relacionamentos, responsabilidades de cada módulo, interfaces, modelo de entidades e decisões de projeto).
* **Fontes dos diagramas**: `docs/uml_modulos.dot` e `docs/uml_entidades.dot` (Graphviz; gere a imagem com `dot -Tpng arquivo.dot -o arquivo.png`).

---

## Decisões de Projeto, Entrega e Referências

* **Decisões de projeto**: as regras adotadas onde a especificação deixa margem (unidade de tempo, permissões, exclusão bloqueada em vez de cascata, atributos de referência entre entidades, listagem completa) estão descritas na seção 7 do modelo de arquitetura e comentadas no código como `Decisao de projeto:`.
* **Entrega e teste fumaça**: identificação dos membros e artefatos em `EQUIPE.txt`, pacote ZIP no formato `T1-TP1-X-Y-Z.ZIP` (com as matrículas), código-fonte e executáveis, e vídeo de *smoke test* cobrindo um cenário de sucesso para cada funcionalidade do sistema integrado.
* **Referências**: Especificação do Trabalho Prático de Técnicas de Programação 1 (`TP1-TRABALHO`) e repositório da equipe `CaioJorge7/projeto-TP1`.
