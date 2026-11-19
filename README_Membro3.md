# TP1 - Membro 3: Camada de Serviço

Controladoras, contêineres e regras Kanban do sistema de gestão de projetos (TP1 - Técnicas de Programação 1). A camada realiza as interfaces abstratas do Membro 2 (`include/interfaces.h`), guarda as entidades dele (`include/entidades.h`) e usa os domínios do Membro 1 (`include/dominios.h`).

## Conteúdo entregue

```text
├── include/
│   ├── containers.h              # Contêineres em memória (Pessoas, Projetos, Quadros, Cartões)
│   ├── sessao.h                  # Usuário autenticado, compartilhado entre as controladoras
│   ├── tempo.h                   # Conversão de Timestamp em minutos
│   ├── controladoras_pessoa.h    # CSAutenticacao e CSPessoa
│   ├── controladoras_projeto.h   # CSProjeto
│   ├── controladoras_quadro.h    # CSQuadro
│   └── controladoras_cartao.h    # CSCartao (CRUD, mover, métricas e WIP)
├── tests/
│   ├── testes_servicos.h         # Declaração dos testes da camada de serviço
│   └── testes_servicos.cpp       # 27 casos: autenticação + 26 serviços
└── main_teste_servicos.cpp       # Executável dos testes da camada de serviço
```

A camada de serviço é feita só de cabeçalhos (*header-only*): não há `.cpp` dela para adicionar ao projeto do Code::Blocks. Basta o diretório `include/` estar no caminho de busca.

## Como a camada funciona

- Cada controladora realiza uma interface do Membro 2: `CSAutenticacao` (`IServicoAutenticacao`), `CSPessoa` (`IServicoPessoa`), `CSProjeto` (`IServicoProjeto`), `CSQuadro` (`IServicoQuadro`) e `CSCartao` (`IServicoCartaoAtividade`).
- Os dados ficam em memória, em quatro contêineres (`std::map`), um por entidade. Os contêineres só guardam e buscam; as regras de negócio ficam nas controladoras.
- As interfaces não recebem "quem está logado". Por isso existe a `Sessao`: `autenticar()` guarda o usuário e os demais serviços consultam a sessão para conferir o papel.
- Falhas (sem login, papel sem direito, duplicidade, não encontrado, regra violada) são avisadas por `std::logic_error` com uma mensagem. Falhas de formato continuam sendo `std::invalid_argument`, lançadas pelos domínios (que também é `std::logic_error`).
- Os tempos das métricas são em minutos, calculados a partir dos `Timestamp` dos cartões.

## Regras implementadas

| Serviço | Quem pode | Regras |
|---|---|---|
| Autenticar | qualquer pessoa | Se falhar, a sessão anterior também é encerrada. |
| Criar pessoa | sem login | E-mail não pode repetir. |
| Ler / listar pessoa | autenticado | |
| Atualizar pessoa | a própria pessoa | O papel só muda se a pessoa não for gestora de projeto nem desenvolvedora de cartão. |
| Excluir pessoa | a própria pessoa | Bloqueado se for gestora de projeto ou desenvolvedora de cartão. Encerra a sessão. |
| Criar projeto | GESTOR | Quem cria vira o gestor (o `gestorEmail` recebido é substituído). |
| Ler / listar projeto | autenticado | |
| Atualizar projeto | gestor do projeto | Identificador e gestor não mudam. |
| Excluir projeto | gestor do projeto | Bloqueado se tiver quadros. |
| Criar quadro | gestor do projeto | O projeto precisa existir. |
| Ler / listar quadro | autenticado | |
| Atualizar quadro | gestor do projeto | Novo limite não pode ser menor que o WIP atual. Identificador e projeto não mudam. |
| Excluir quadro | gestor do projeto | Bloqueado se tiver cartões. |
| Criar cartão | gestor do projeto | O desenvolvedor precisa existir e ter papel DESENVOLVEDOR. O cartão entra em A FAZER e a `entrada` é obrigatória. |
| Ler / listar cartão | autenticado | |
| Atualizar cartão | gestor do projeto | Estado, datas e quadro não mudam (só o `mover` mexe neles). O desenvolvedor pode ser trocado por outro válido. |
| Excluir cartão | gestor do projeto | |
| Mover cartão | autenticado | Só A FAZER → FAZENDO (respeita o limite de WIP e grava o início) e FAZENDO → FEITO (grava o término). O instante não pode ser anterior ao registro anterior do cartão. |
| Tempo de ciclo / lead time | autenticado | Só para cartão em FEITO. Ciclo = término − início; lead time = término − entrada. |
| Tempo de ciclo médio / lead time médio | autenticado | Média dos cartões em FEITO do quadro. Sem nenhum concluído, lança exceção. |
| WIP | autenticado | Quantidade de cartões do quadro em FAZENDO. |

## Como ligar a camada (para a apresentação)

Os contêineres e a sessão precisam existir enquanto o programa roda e ser criados antes das controladoras:

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

// A apresentação deve depender só das interfaces:
IServicoAutenticacao* sAutenticacao = &autenticacao;
IServicoPessoa* sPessoa = &servicoPessoa;
IServicoProjeto* sProjeto = &servicoProjeto;
IServicoQuadro* sQuadro = &servicoQuadro;
IServicoCartaoAtividade* sCartao = &servicoCartao;
```

Uso:
- Criar a conta (`sPessoa->criar`) não exige login. Os demais serviços exigem `sAutenticacao->autenticar(email, senha)` antes.
- Para sair da conta, use `autenticacao.encerrarSessao()` (hoje só existe na classe concreta, não na interface).
- Envolva as chamadas em `try { ... } catch (const std::logic_error& e) { ... }` e mostre `e.what()` ao usuário.

## Compilação e testes (GCC C++11)

```bash
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude \
  src/dominios.cpp src/entidades.cpp tests/testes_servicos.cpp \
  main_teste_servicos.cpp -o teste_servicos
./teste_servicos
```

No Code::Blocks, os testes são um executável à parte: não compile `main_teste_servicos.cpp` junto com o `main.cpp` final, pois os dois têm uma função `main`.

## Decisões e pontos em aberto

As decisões da camada de serviço estão registradas nos comentários do código como `ESCOLHA(Membro3)`. O que depende de definição da equipe está marcado como `TODO(Membro2)`. Para listar tudo:

```bash
grep -rn "ESCOLHA(Membro3)" include tests
grep -rn "TODO(Membro2)" include tests
```
