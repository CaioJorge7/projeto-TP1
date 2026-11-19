# Sistema de Gestão de Projetos Kanban — TP1

Sistema de gestão de projetos baseado no método Kanban desenvolvido em C++ (padrão C++11) para a disciplina **Técnicas de Programação 1 (TP1)**. O projeto segue arquitetura modular em quatro camadas (Domínios, Entidades, Serviços e Apresentação), com documentação no padrão **Doxygen**, modelagem **UML** e suítes de **testes unitários**.

---

## Estrutura do Repositório

```text
├── docs/                                    # Diagramas UML (.dot e PDF do modelo de arquitetura)
├── include/                                 # Cabeçalhos (.h) das camadas do sistema
│   ├── dominios.h                           # 11 classes de domínio e validações
│   ├── entidades.h                          # 4 entidades do sistema Kanban
│   ├── interfaces.h                         # Interfaces abstratas (contratos de serviço)
│   ├── containers.h                         # Contêineres em memória (std::map)
│   ├── sessao.h                             # Controle de usuário autenticado
│   ├── tempo.h                              # Conversão de Timestamp para minutos
│   ├── controladoras_pessoa.h               # Controladoras de autenticação e pessoa
│   ├── controladoras_projeto.h              # Controladora de projetos
│   ├── controladoras_quadro.h               # Controladora de quadros Kanban
│   └── controladoras_cartao.h               # Controladora de cartões, métricas e WIP
├── src/                                     # Implementações (.cpp)
│   ├── dominios.cpp                         # Regras de validação e métodos set/get dos domínios
│   └── entidades.cpp                        # Implementação dos construtores e métodos das entidades
├── tests/                                   # Suítes de testes unitários
│   ├── testes_dominios.h / .cpp             # Testes de sucesso e falha dos 11 domínios
│   ├── testes_entidades.h / .cpp            # Testes das entidades e interfaces
│   └── testes_servicos.h / .cpp             # 27 casos de teste (autenticação + 26 serviços)
├── main.cpp                                 # Ponto de entrada do sistema integrado (Apresentação)
├── main_teste_dominios.cpp                  # Executável isolado de testes de domínios
├── main_teste_entidades.cpp                 # Executável isolado de testes de entidades
├── main_teste_servicos.cpp                  # Executável isolado de testes de serviços
├── projeto_tp1.cbp                          # Arquivo de projeto para a IDE Code::Blocks
├── Doxyfile                                 # Configuração para geração da documentação HTML
├── EQUIPE.txt                               # Identificação dos membros e artefatos desenvolvidos
└── README.md                                # Documentação geral do projeto
```

---

## Arquitetura do Sistema

### 1. Camada de Domínios (`include/dominios.h`, `src/dominios.cpp`)
Implementação das classes básicas de representação de dados com validação de formato e lançamento de exceções (`std::invalid_argument`) via métodos públicos `setValor` e `getValor`:

1. **`Email`**: formato padrão de endereço eletrônico.
2. **`Estado`**: valores válidos `A FAZER`, `FAZENDO`, `FEITO`.
3. **`Identificador`**: código composto por 3 letras e 3 dígitos.
4. **`Limite`**: valor inteiro de `1` a `25` (limite de WIP do quadro).
5. **`Nome`**: texto com até 15 caracteres.
6. **`Papel`**: valores válidos `GESTOR` ou `DESENVOLVEDOR`.
7. **`Prioridade`**: valores válidos `ALTA`, `MEDIA` ou `BAIXA`.
8. **`Senha`**: 5 caracteres alfanuméricos contendo pelo menos 1 letra e 1 dígito.
9. **`Tamanho`**: valores válidos `GRANDE`, `MEDIO` ou `PEQUENO`.
10. **`Texto`**: até 30 caracteres, iniciando com letra maiúscula e terminando com ponto final (`.`).
11. **`Timestamp`**: formato `DIA-MES-ANO-HORARIO` com validação de anos bissextos.

---

### 2. Camada de Entidades e Interfaces (`include/entidades.h`, `src/entidades.cpp`, `include/interfaces.h`)
Classes que compõem os objetos centrais do sistema utilizando exclusivamente atributos do tipo domínio (preservando chaves primárias imutáveis após a construção) e contratos puramente virtuais para desacoplamento:

* **`Pessoa`**: `email: Email` (PK), `nome: Nome`, `senha: Senha`, `papel: Papel`.
* **`Projeto`**: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `inicio: Timestamp`, `termino: Timestamp`, `gestorEmail: Email` (referência a `Pessoa`).
* **`Quadro`**: `identificador: Identificador` (PK), `nome: Nome`, `limite: Limite`, `projetoId: Identificador` (referência a `Projeto`).
* **`CartaoAtividade`**: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `prioridade: Prioridade`, `estado: Estado`, `tamanho: Tamanho`, `entrada: Timestamp`, `inicio: Timestamp`, `termino: Timestamp`, `quadroId: Identificador` (referência a `Quadro`), `desenvolvedorEmail: Email` (referência a `Pessoa`).
  * *Ciclo de vida das datas:* na criação, apenas `entrada` é informada. `inicio` e `termino` começam vazios e são preenchidos nas transições de estado (`temInicio()` e `temTermino()` indicam sua disponibilidade).
* **Interfaces Abstratas (`interfaces.h`)**: `IServicoAutenticacao`, `IServicoPessoa`, `IServicoProjeto`, `IServicoQuadro` e `IServicoCartaoAtividade` (totalizando 26 funcionalidades de negócio além da autenticação).

---

### 3. Camada de Serviço (`include/containers.h`, `sessao.h`, `tempo.h`, `controladoras_*.h`)
Implementada no formato *header-only*, realiza as interfaces abstratas por meio de controladoras que gerenciam contêineres em memória (`std::map`) e validam regras de negócio e permissões via `Sessao` (lançando `std::logic_error` em caso de violação):

| Serviço | Permissão | Regras de Negócio Aplicadas |
| :--- | :--- | :--- |
| **Autenticar** | Qualquer usuário | Inicia sessão com e-mail/senha; em caso de falha, encerra a sessão anterior. |
| **Criar Pessoa** | Sem login | Impede cadastro de e-mails duplicados. |
| **Ler / Listar Pessoa** | Autenticado | Consulta dados cadastrais. |
| **Atualizar Pessoa** | Próprio usuário | Permite alteração de papel apenas se não houver projetos geridos ou cartões vinculados. |
| **Excluir Pessoa** | Próprio usuário | Bloqueado caso seja gestor de projeto ou desenvolvedor de cartão ativo; encerra a sessão. |
| **Criar Projeto** | `GESTOR` | Define automaticamente o usuário autenticado como gestor do projeto. |
| **Ler / Listar Projeto** | Autenticado | Consulta projetos cadastrados. |
| **Atualizar Projeto** | Gestor do projeto | Preserva identificador e gestor originais. |
| **Excluir Projeto** | Gestor do projeto | Bloqueado enquanto houver quadros vinculados ao projeto. |
| **Criar Quadro** | Gestor do projeto | Exige projeto existente. |
| **Ler / Listar Quadro** | Autenticado | Consulta quadros cadastrados. |
| **Atualizar Quadro** | Gestor do projeto | Novo limite não pode ser inferior ao WIP atual; preserva identificador e projeto. |
| **Excluir Quadro** | Gestor do projeto | Bloqueado enquanto houver cartões no quadro. |
| **Criar Cartão** | Gestor do projeto | Exige desenvolvedor válido com papel `DESENVOLVEDOR`; inicia em `A FAZER` com `entrada` obrigatória. |
| **Ler / Listar Cartão** | Autenticado | Consulta cartões cadastrados. |
| **Atualizar Cartão** | Gestor do projeto | Permite trocar desenvolvedor válido; estado, datas e quadro são alterados apenas via movimentação. |
| **Excluir Cartão** | Gestor do projeto | Remove o cartão do quadro. |
| **Mover Cartão** | Autenticado | Permite `A FAZER -> FAZENDO` (respeitando limite WIP e gravando `inicio`) e `FAZENDO -> FEITO` (gravando `termino`), respeitando ordem cronológica. |
| **Métricas e WIP** | Autenticado | **WIP**: total em `FAZENDO`. **Cycle Time** (`termino - inicio`) e **Lead Time** (`termino - entrada`) em minutos para cartões em `FEITO` (médias lançam exceção se não houver cartões concluídos). |

---

### 4. Camada de Apresentação (`main.cpp` e classes de interface)
Responsável pela interação com o usuário, leitura e pré-validação de dados de entrada, tratamento de exceções (`std::invalid_argument` e `std::logic_error`) e comunicação exclusiva com a camada de negócio por meio das interfaces abstratas (`IServico*`).

---

## Como Compilar e Executar

> **Atenção:** Cada executável possui sua própria função `main()`. Ao compilar via terminal ou configurar alvos no **Code::Blocks** (`projeto_tp1.cbp`), compile apenas um arquivo `main*.cpp` por vez.

### 1. Sistema Principal Integrado
```bash
g++ -std=c++11 -Wall -Wextra -Iinclude src/*.cpp main.cpp -o sistema_kanban
./sistema_kanban
```

### 2. Testes Unitários por Camada

* **Testes de Domínios:**
```bash
g++ -std=c++11 -Wall -Wextra -Iinclude -Itests src/dominios.cpp tests/testes_dominios.cpp main_teste_dominios.cpp -o teste_dominios
./teste_dominios
```

* **Testes de Entidades:**
```bash
g++ -std=c++11 -Wall -Wextra -Iinclude -Itests src/dominios.cpp src/entidades.cpp tests/testes_entidades.cpp main_teste_entidades.cpp -o teste_entidades
./teste_entidades
```

* **Testes de Serviços (27 cenários):**
```bash
g++ -std=c++11 -Wall -Wextra -Iinclude -Itests src/dominios.cpp src/entidades.cpp tests/testes_servicos.cpp main_teste_servicos.cpp -o teste_servicos
./teste_servicos
```

---

## Documentação e Diagramas

* **HTML (Doxygen):** Execute `doxygen Doxyfile` na raiz do projeto para gerar a documentação navegável em HTML.
* **Arquitetura UML:** Os diagramas de módulos, interfaces e entidades encontram-se na pasta `docs/`.