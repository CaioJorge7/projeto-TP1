# TP1 - Membro 2: Entidades, interfaces e arquitetura

## Conteudo entregue

- `include/entidades.h`: declaracoes das quatro entidades do Kanban, documentadas em Doxygen.
- `src/entidades.cpp`: implementacao dos construtores e metodos publicos `set/get`.
- `include/interfaces.h`: contratos abstratos com metodos virtuais puros para a camada de servico.
- `tests/testes_entidades.h` e `.cpp`: testes de entidades e interfaces abstratas.
- `main_teste_entidades.cpp`: executavel dos testes, separado do `main.cpp` final.
- `docs/Modelo_Arquitetura_TP1_Membro2.pdf`: UML de classes e componentes, com explicacoes.
- `docs/uml_entidades.dot`, `docs/uml_modulos.dot`: codigo-fonte dos diagramas (Graphviz).

## Compatibilidade com o repositorio da equipe

Os arquivos foram feitos para conviver com a implementacao preexistente em:

- `include/dominios.h`
- `src/dominios.cpp`

O pacote **nao substitui** esses arquivos. A API dos dominios usada aqui e `setValor` / `getValor`, inclusive `Limite` (inteiro), conforme a branch `feature/dominios` do repositorio `CaioJorge7/projeto-TP1` consultada em 17/11/2026.

## Compilacao e testes (GCC C++11)

Coloque este conteudo na raiz do projeto, preservando as pastas ja existentes. Execute:

```bash
g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude \
  -Itests src/dominios.cpp src/entidades.cpp tests/testes_entidades.cpp \
  main_teste_entidades.cpp -o teste_entidades
./teste_entidades
```

Para o Code::Blocks, adicione `src/entidades.cpp` ao projeto, e os cabecalhos `include/entidades.h` e `include/interfaces.h` como arquivos de apoio. **Nao compile** `main_teste_entidades.cpp` junto do `main.cpp` final, pois ambos conteriam uma funcao `main`.

## Entidades e atributos

- `Pessoa`: `email: Email` (PK), `nome: Nome`, `senha: Senha`, `papel: Papel`.
- `Projeto`: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `inicio: Timestamp`, `termino: Timestamp`, `gestorEmail: Email` (referencia Pessoa).
- `Quadro`: `identificador: Identificador` (PK), `nome: Nome`, `limite: Limite`, `projetoId: Identificador` (referencia Projeto).
- `CartaoAtividade`: `identificador: Identificador` (PK), `nome: Nome`, `descricao: Texto`, `prioridade: Prioridade`, `estado: Estado`, `tamanho: Tamanho`, `entrada: Timestamp`, `inicio: Timestamp`, `termino: Timestamp`, `quadroId: Identificador`, `desenvolvedorEmail: Email`.

Os campos de referencia foram adicionados para permitir implementar as multiplicidades exigidas pela especificacao sem guardar ponteiros ou objetos de entidade em outra entidade. Sao **atributos do tipo dominio**, conforme a exigencia da especificacao. As chaves primarias nao podem ser alteradas depois do construtor.

**Datas dos cartoes:** no instante de criacao, somente `entrada` e informada. `inicio` e `termino` comecam vazios e recebem um `Timestamp` validado quando o servico efetiva as respectivas transicoes. Consulte `temInicio()` e `temTermino()` antes de utilizar os valores. O servico deve garantir a ordem cronologica e as regras Kanban.

## Interfaces e 26 funcionalidades

Os quatro contratos CRUD somam 20 metodos (criar, ler, atualizar, excluir, listar). `IServicoCartaoAtividade` acrescenta `mover`, `lerTempoCiclo`, `lerTempoCicloMedio`, `lerLeadTime`, `lerLeadTimeMedio` e `lerWip`: **26 funcionalidades**. `IServicoAutenticacao` acrescenta a autenticacao necessaria para acesso; esta nao aparece como item numerado da tabela de 26 servicos.

Os contratos especificam comportamento, mas **nao implementam** CRUD, armazenamento, permissao de papel, movimento, WIP ou calculos: essas tarefas pertencem ao Membro 3 (camada de servico).

## Regras de integracao para o Membro 3

1. Um Projeto faz referencia a uma Pessoa gestora existente; um Quadro a um Projeto existente; um Cartao a um Quadro e a uma Pessoa desenvolvedora existente.
2. As referencias nao devem ficar orfas apos exclusoes; verificar cascatas ou impedir exclusao quando existem dependencias.
3. Nao permitir edicao das chaves primarias; verificar papel `GESTOR` / `DESENVOLVEDOR` e autorizacao em cada servico.
4. Movimento permitido: `A FAZER -> FAZENDO -> FEITO`, respeitando `Quadro.limite`.
5. Cycle Time: `termino - inicio`, Lead Time: `termino - entrada`, calculados apenas para atividades concluidas. Medias sobre atividades concluidas do quadro; decidir como sinalizar conjunto vazio.
6. Convencao adotada pela interface para tempos: **minutos** (a especificacao nao determina uma unidade). Validar essa convencao com a equipe antes da implementacao.

## Como enviar ao GitHub da equipe

```bash
git clone -b feature/dominios https://github.com/CaioJorge7/projeto-TP1.git
cd projeto-TP1
git switch -c feature/entidades
# Copie o conteudo das pastas include/, src/, tests/ e docs/ deste pacote.
git add include/entidades.h include/interfaces.h src/entidades.cpp \
  tests/testes_entidades.h tests/testes_entidades.cpp \
  main_teste_entidades.cpp docs/ README_Membro2.md
git commit -m "Implementa entidades, interfaces e diagrama UML do TP1"
git push -u origin feature/entidades
```

Crie depois o Pull Request para integrar `feature/entidades` na branch definida pela equipe, preferencialmente depois da integracao de `feature/dominios`.

## Referencias consultadas

- Especificacao do Trabalho Pratico, PDF enviado (particularmente paginas 1-5).
- Plano de Divisao de Tarefas, PDF enviado (Membro 2, pagina 2).
- Repositorio da equipe: https://github.com/CaioJorge7/projeto-TP1
- Referencia de organizacao: https://github.com/Eduardo734-ai/Trabalho_de_Tecnicas_de_Programacao_1

Nao foram copiados codigos de implementacao do repositorio de exemplo; ele foi utilizado como referencia de organizacao modular e Doxygen.
