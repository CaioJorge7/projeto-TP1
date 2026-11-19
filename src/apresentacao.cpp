/**
 * @file apresentacao.cpp
 * @brief Implementacao da camada de Apresentacao do sistema Kanban (TP1).
 *
 * Dependencias desta unidade de compilacao:
 *   - apresentacao.h (e, por transitividade, interfaces.h, entidades.h, dominios.h)
 *   - Biblioteca padrao C++ (iostream, iomanip, limits, sstream, string, vector)
 *
 * NUNCA inclui containers.h, sessao.h ou controladoras_*.h: a camada de
 * apresentacao depende exclusivamente das 5 interfaces abstratas.
 */

#include "apresentacao.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>

// Para isatty() / _isatty()
#ifdef _WIN32
#  include <io.h>
/** @brief Macro que verifica se stdin e um terminal interativo no Windows. */
#  define STDIN_IS_TTY() (_isatty(_fileno(stdin)) != 0)
#else
#  include <unistd.h>
/** @brief Macro que verifica se stdin e um terminal interativo em ambiente POSIX/Linux. */
#  define STDIN_IS_TTY() (isatty(fileno(stdin)) != 0)
#endif

using std::cin;
using std::cout;
using std::string;
using std::vector;

// ===========================================================================
// Utilitarios estaticos (escopo de arquivo)
// ===========================================================================

/** Largura padrao das linhas separadoras. */
static const int LARGURA = 56;

/**
 * @brief Le uma linha completa do stdin, exibindo prompt antes.
 *
 * Nao deixa '\n' no buffer apos a leitura (std::getline consome o delimitador).
 */
static string lerLinha(const string& prompt) {
    if (!cin.good()) return "";
    cout << prompt;
    string s;
    std::getline(cin, s);
    return s;
}

/**
 * @brief Le um inteiro do stdin e descarta o restante da linha.
 *
 * Em caso de falha de extracao (entrada nao numerica), limpa o estado
 * de erro e retorna -1. Se stdin estiver em EOF/erro, retorna 0 (sair/voltar).
 */
static int lerOpcao(const string& prompt = "  Opcao: ") {
    if (!cin.good()) return 0;
    cout << prompt;
    int op;
    if (!(cin >> op)) {
        if (!cin.good()) return 0;
        cin.clear();
        op = -1;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return op;
}

/**
 * @brief Em modo interativo (TTY), exibe "Pressione Enter..." e aguarda.
 *
 * Quando stdin nao e um terminal (redirecionamento ou pipe), nao consome
 * nenhum byte extra do buffer para evitar desvio de entradas automatizadas.
 */
static void pausar() {
    if (STDIN_IS_TTY()) {
        cout << "\n  Pressione Enter para continuar...";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

/**
 * @brief Exibe um separador decorativo com titulo opcional.
 * @param titulo Titulo exibido apos a linha de separacao (vazio = apenas separador).
 */
static void exibirSeparador(const string& titulo = "") {
    cout << "\n" << string(LARGURA, '=') << "\n";
    if (!titulo.empty()) {
        cout << "  " << titulo << "\n" << string(LARGURA, '-') << "\n";
    }
}

/**
 * @brief Converte uma duracao em minutos para string legivel (ex.: "2h 15min").
 * @param minutos Duracao em minutos inteiros.
 * @return String formatada.
 */
static string formatarMinutos(long minutos) {
    if (minutos < 0) return "N/A";
    long h = minutos / 60;
    long m = minutos % 60;
    std::ostringstream oss;
    if (h > 0) oss << h << "h ";
    oss << m << "min  (" << minutos << " min no total)";
    return oss.str();
}

/**
 * @brief Template: le uma linha, tenta atribuir via setValor(string) e
 *        exibe mensagem de erro se a validacao falhar.
 *
 * Funciona para qualquer classe derivada de Dominio que use setValor(string).
 *
 * @tparam T    Classe de dominio com metodo setValor(string).
 * @param prompt Texto exibido antes da leitura.
 * @param out   [out] Recebe o valor validado.
 * @return true se a leitura e validacao foram bem-sucedidas; false caso contrario.
 */
template<typename T>
static bool lerDominio(const string& prompt, T& out) {
    string entrada = lerLinha(prompt);
    try {
        out.setValor(entrada);
        return true;
    } catch (const std::exception& e) {
        cout << "  [FORMATO INVALIDO] " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Le um inteiro e tenta atribuir via Limite::setValor(int).
 * @param prompt Texto exibido antes da leitura.
 * @param out   [out] Recebe o valor validado.
 * @return true se bem-sucedido; false caso contrario.
 */
static bool lerLimiteValor(const string& prompt, Limite& out) {
    if (!cin.good()) return false;
    cout << prompt;
    int v;
    if (!(cin >> v)) {
        if (!cin.good()) return false;
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "  [FORMATO INVALIDO] Informe um numero inteiro.\n";
        return false;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    try {
        out.setValor(v);
        return true;
    } catch (const std::exception& e) {
        cout << "  [FORMATO INVALIDO] " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief Exibe uma pergunta de confirmacao e retorna true se o usuario
 *        responder 's' ou 'S'.
 */
static bool confirmar(const string& mensagem) {
    if (!cin.good()) return false;
    cout << "\n  " << mensagem << " (s/n): ";
    string resp;
    std::getline(cin, resp);
    return (!resp.empty() && (resp[0] == 's' || resp[0] == 'S'));
}

// ===========================================================================
// Exibicao de entidades
// ===========================================================================

/**
 * @brief Exibe os dados formatados de uma Pessoa no terminal.
 * @param p Referencia para a Pessoa.
 */
static void exibirPessoa(const Pessoa& p) {
    cout << "  E-mail : " << p.getEmail().getValor() << "\n"
         << "  Nome   : " << p.getNome().getValor()  << "\n"
         << "  Papel  : " << p.getPapel().getValor() << "\n";
}

/**
 * @brief Exibe os dados formatados de um Projeto no terminal.
 * @param p Referencia para o Projeto.
 */
static void exibirProjeto(const Projeto& p) {
    cout << "  Identificador : " << p.getIdentificador().getValor() << "\n"
         << "  Nome          : " << p.getNome().getValor()          << "\n"
         << "  Descricao     : " << p.getDescricao().getValor()     << "\n"
         << "  Inicio        : " << p.getInicio().getValor()        << "\n"
         << "  Termino       : " << p.getTermino().getValor()       << "\n"
         << "  Gestor        : " << p.getGestorEmail().getValor()   << "\n";
}

/**
 * @brief Exibe os dados formatados de um Quadro no terminal.
 * @param q Referencia para o Quadro.
 */
static void exibirQuadro(const Quadro& q) {
    cout << "  Identificador : " << q.getIdentificador().getValor() << "\n"
         << "  Nome          : " << q.getNome().getValor()          << "\n"
         << "  Limite (WIP)  : " << q.getLimite().getValor()        << "\n"
         << "  Projeto       : " << q.getProjetoId().getValor()     << "\n";
}

/**
 * @brief Exibe os dados formatados de um CartaoAtividade no terminal.
 * @param c Referencia para o CartaoAtividade.
 */
static void exibirCartao(const CartaoAtividade& c) {
    cout << "  Identificador  : " << c.getIdentificador().getValor()      << "\n"
         << "  Nome           : " << c.getNome().getValor()               << "\n"
         << "  Descricao      : " << c.getDescricao().getValor()          << "\n"
         << "  Prioridade     : " << c.getPrioridade().getValor()         << "\n"
         << "  Estado         : " << c.getEstado().getValor()             << "\n"
         << "  Tamanho        : " << c.getTamanho().getValor()            << "\n"
         << "  Entrada        : " << c.getEntrada().getValor()            << "\n"
         << "  Inicio         : "
         << (c.temInicio()  ? c.getInicio().getValor()  : "(nao iniciado)")  << "\n"
         << "  Termino        : "
         << (c.temTermino() ? c.getTermino().getValor() : "(nao concluido)") << "\n"
         << "  Quadro         : " << c.getQuadroId().getValor()           << "\n"
         << "  Desenvolvedor  : " << c.getDesenvolvedorEmail().getValor() << "\n";
}

// ===========================================================================
// Fluxo compartilhado: criar Pessoa
// (usado tanto pelo menu inicial quanto pelo submenu autenticado de Pessoa)
// ===========================================================================

/**
 * @brief Coleta dados no terminal e cria uma Pessoa via servico.
 *
 * Esta funcao estatica e chamada tanto por Aplicacao::criarConta() (menu
 * inicial, sem autenticacao) quanto por ApresentacaoPessoa::criar()
 * (submenu autenticado), evitando duplicacao de codigo.
 *
 * @param servico Implementacao de IServicoPessoa a ser usada.
 */
static void fluxoCriarPessoa(IServicoPessoa* servico) {
    exibirSeparador("CRIAR PESSOA / CRIAR CONTA");
    cout << "  Preencha os dados da nova conta.\n\n";

    Email email;
    if (!lerDominio("  E-mail (ex.: usuario@dominio.com): ", email)) { pausar(); return; }

    Nome nome;
    if (!lerDominio("  Nome (ate 15 letras e espacos): ", nome)) { pausar(); return; }

    Senha senha;
    if (!lerDominio("  Senha (5 chars alfanum, min 1 letra e 1 digito, ex.: abc12): ", senha)) {
        pausar(); return;
    }

    Papel papel;
    if (!lerDominio("  Papel (GESTOR ou DESENVOLVEDOR): ", papel)) { pausar(); return; }

    try {
        Pessoa p(email, nome, senha, papel);
        servico->criar(p);
        cout << "\n  [OK] Conta criada com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

// ===========================================================================
// ApresentacaoPessoa — implementacao
// ===========================================================================

ApresentacaoPessoa::ApresentacaoPessoa(IServicoPessoa* servico)
    : servico(servico) {}

void ApresentacaoPessoa::criar() {
    fluxoCriarPessoa(servico);
}

void ApresentacaoPessoa::ler() {
    exibirSeparador("LER PESSOA");

    Email email;
    if (!lerDominio("  E-mail da pessoa: ", email)) { pausar(); return; }

    try {
        Pessoa p = servico->ler(email);
        cout << "\n";
        exibirPessoa(p);
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoPessoa::atualizar() {
    exibirSeparador("ATUALIZAR PESSOA");
    cout << "  NOTA: Somente voce pode atualizar seus proprios dados.\n\n";

    Email email;
    if (!lerDominio("  Seu e-mail (chave, nao sera alterado): ", email)) { pausar(); return; }

    // Busca dados atuais para exibir ao usuario
    try {
        Pessoa atual = servico->ler(email);
        cout << "\n  Dados atuais:\n";
        exibirPessoa(atual);
        cout << "\n  Informe os novos valores:\n";

        Nome nome;
        if (!lerDominio("  Novo nome: ", nome)) { pausar(); return; }

        Senha senha;
        if (!lerDominio("  Nova senha (5 chars alfanum, min 1 letra e 1 digito): ", senha)) {
            pausar(); return;
        }

        Papel papel;
        if (!lerDominio("  Novo papel (GESTOR ou DESENVOLVEDOR): ", papel)) { pausar(); return; }

        // Monta objeto com mesma chave primaria e campos atualizados
        Pessoa novo = atual;
        novo.setNome(nome);
        novo.setSenha(senha);
        novo.setPapel(papel);

        servico->atualizar(novo);
        cout << "\n  [OK] Dados atualizados com sucesso!\n";

    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoPessoa::excluir() {
    exibirSeparador("EXCLUIR PESSOA");
    cout << "  NOTA: Somente voce pode excluir sua propria conta.\n"
         << "        A conta nao pode ser excluida se houver projetos ou\n"
         << "        cartoes associados a ela.\n\n";

    Email email;
    if (!lerDominio("  Seu e-mail: ", email)) { pausar(); return; }

    if (!confirmar("Deseja realmente excluir sua conta? Esta acao e irreversivel.")) {
        cout << "\n  Operacao cancelada.\n";
        pausar();
        return;
    }

    try {
        servico->excluir(email);
        cout << "\n  [OK] Conta excluida. Voce sera desconectado.\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoPessoa::listar() {
    exibirSeparador("LISTAR PESSOAS");

    try {
        vector<Pessoa> lista = servico->listar();
        if (lista.empty()) {
            cout << "  Nenhuma pessoa cadastrada.\n";
        } else {
            cout << "  Total: " << lista.size() << " pessoa(s)\n";
            for (size_t i = 0; i < lista.size(); ++i) {
                cout << "\n  [" << (i + 1) << "]\n";
                exibirPessoa(lista[i]);
            }
        }
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoPessoa::exibirMenu() {
    while (true) {
        exibirSeparador("MENU - PESSOAS");
        cout << "  1) Criar pessoa\n"
             << "  2) Ler pessoa\n"
             << "  3) Atualizar pessoa\n"
             << "  4) Excluir pessoa\n"
             << "  5) Listar pessoas\n"
             << "  0) Voltar\n";
        int op = lerOpcao();
        switch (op) {
            case 1: criar();    break;
            case 2: ler();      break;
            case 3: atualizar(); break;
            case 4: excluir();  break;
            case 5: listar();   break;
            case 0: return;
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

// ===========================================================================
// ApresentacaoProjeto — implementacao
// ===========================================================================

ApresentacaoProjeto::ApresentacaoProjeto(IServicoProjeto* servico)
    : servico(servico) {}

void ApresentacaoProjeto::criar() {
    exibirSeparador("CRIAR PROJETO");
    cout << "  NOTA: O gestor do projeto sera o usuario autenticado.\n"
         << "        Apenas usuarios com papel GESTOR podem criar projetos.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador (3 letras + 3 digitos, ex.: ABC123): ", id)) {
        pausar(); return;
    }

    Nome nome;
    if (!lerDominio("  Nome do projeto: ", nome)) { pausar(); return; }

    Texto descricao;
    if (!lerDominio(
            "  Descricao (comeca c/ maiusc., termina c/ '.', ate 30 chars): ",
            descricao)) {
        pausar(); return;
    }

    Timestamp inicio;
    if (!lerDominio("  Data de inicio (ex.: 01-JAN-2024-08:00): ", inicio)) {
        pausar(); return;
    }

    Timestamp termino;
    if (!lerDominio("  Data de termino (ex.: 31-DEZ-2024-18:00): ", termino)) {
        pausar(); return;
    }

    // gestorEmail e substituido pelo servico; usamos placeholder valido
    Email gestorPlaceholder;
    gestorPlaceholder.setValor("gestor@placeholder.com");

    try {
        Projeto p(id, nome, descricao, inicio, termino, gestorPlaceholder);
        servico->criar(p);
        cout << "\n  [OK] Projeto criado com sucesso! Voce e o gestor.\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoProjeto::ler() {
    exibirSeparador("LER PROJETO");

    Identificador id;
    if (!lerDominio("  Identificador do projeto: ", id)) { pausar(); return; }

    try {
        Projeto p = servico->ler(id);
        cout << "\n";
        exibirProjeto(p);
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoProjeto::atualizar() {
    exibirSeparador("ATUALIZAR PROJETO");
    cout << "  NOTA: Somente o gestor do projeto pode atualizá-lo.\n"
         << "        O identificador e o gestor nao podem ser alterados.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do projeto: ", id)) { pausar(); return; }

    try {
        Projeto atual = servico->ler(id);
        cout << "\n  Dados atuais:\n";
        exibirProjeto(atual);
        cout << "\n  Informe os novos valores:\n";

        Nome nome;
        if (!lerDominio("  Novo nome: ", nome)) { pausar(); return; }

        Texto descricao;
        if (!lerDominio(
                "  Nova descricao (comeca c/ maiusc., termina c/ '.'): ",
                descricao)) {
            pausar(); return;
        }

        Timestamp inicio;
        if (!lerDominio("  Nova data de inicio (ex.: 01-JAN-2024-08:00): ", inicio)) {
            pausar(); return;
        }

        Timestamp termino;
        if (!lerDominio("  Nova data de termino (ex.: 31-DEZ-2024-18:00): ", termino)) {
            pausar(); return;
        }

        Projeto novo = atual;
        novo.setNome(nome);
        novo.setDescricao(descricao);
        novo.setInicio(inicio);
        novo.setTermino(termino);
        // gestorEmail preservado (servico tambem sobrescreve com o valor armazenado)

        servico->atualizar(novo);
        cout << "\n  [OK] Projeto atualizado com sucesso!\n";

    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoProjeto::excluir() {
    exibirSeparador("EXCLUIR PROJETO");
    cout << "  NOTA: Somente o gestor do projeto pode excluí-lo.\n"
         << "        O projeto nao pode ser excluido se ainda houver quadros.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do projeto: ", id)) { pausar(); return; }

    if (!confirmar("Deseja realmente excluir este projeto?")) {
        cout << "\n  Operacao cancelada.\n";
        pausar();
        return;
    }

    try {
        servico->excluir(id);
        cout << "\n  [OK] Projeto excluido com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoProjeto::listar() {
    exibirSeparador("LISTAR PROJETOS");

    try {
        vector<Projeto> lista = servico->listar();
        if (lista.empty()) {
            cout << "  Nenhum projeto cadastrado.\n";
        } else {
            cout << "  Total: " << lista.size() << " projeto(s)\n";
            for (size_t i = 0; i < lista.size(); ++i) {
                cout << "\n  [" << (i + 1) << "]\n";
                exibirProjeto(lista[i]);
            }
        }
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoProjeto::exibirMenu() {
    while (true) {
        exibirSeparador("MENU - PROJETOS");
        cout << "  1) Criar projeto\n"
             << "  2) Ler projeto\n"
             << "  3) Atualizar projeto\n"
             << "  4) Excluir projeto\n"
             << "  5) Listar projetos\n"
             << "  0) Voltar\n";
        int op = lerOpcao();
        switch (op) {
            case 1: criar();     break;
            case 2: ler();       break;
            case 3: atualizar(); break;
            case 4: excluir();   break;
            case 5: listar();    break;
            case 0: return;
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

// ===========================================================================
// ApresentacaoQuadro — implementacao
// ===========================================================================

ApresentacaoQuadro::ApresentacaoQuadro(IServicoQuadro* servico)
    : servico(servico) {}

void ApresentacaoQuadro::criar() {
    exibirSeparador("CRIAR QUADRO");
    cout << "  NOTA: Somente o gestor do projeto pode criar um quadro nele.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do quadro (ex.: QUA001): ", id)) {
        pausar(); return;
    }

    Nome nome;
    if (!lerDominio("  Nome do quadro: ", nome)) { pausar(); return; }

    Limite limite;
    if (!lerLimiteValor("  Limite de WIP (1 a 25): ", limite)) { pausar(); return; }

    Identificador projetoId;
    if (!lerDominio("  Identificador do projeto ao qual pertence: ", projetoId)) {
        pausar(); return;
    }

    try {
        Quadro q(id, nome, limite, projetoId);
        servico->criar(q);
        cout << "\n  [OK] Quadro criado com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoQuadro::ler() {
    exibirSeparador("LER QUADRO");

    Identificador id;
    if (!lerDominio("  Identificador do quadro: ", id)) { pausar(); return; }

    try {
        Quadro q = servico->ler(id);
        cout << "\n";
        exibirQuadro(q);
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoQuadro::atualizar() {
    exibirSeparador("ATUALIZAR QUADRO");
    cout << "  NOTA: Somente o gestor do projeto pode atualizar o quadro.\n"
         << "        O identificador e o projeto nao podem ser alterados.\n"
         << "        O novo limite nao pode ser menor que o WIP atual.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do quadro: ", id)) { pausar(); return; }

    try {
        Quadro atual = servico->ler(id);
        cout << "\n  Dados atuais:\n";
        exibirQuadro(atual);
        cout << "\n  Informe os novos valores:\n";

        Nome nome;
        if (!lerDominio("  Novo nome: ", nome)) { pausar(); return; }

        Limite limite;
        if (!lerLimiteValor("  Novo limite de WIP (1 a 25): ", limite)) {
            pausar(); return;
        }

        Quadro novo = atual;
        novo.setNome(nome);
        novo.setLimite(limite);
        // projetoId preservado (servico tambem sobrescreve)

        servico->atualizar(novo);
        cout << "\n  [OK] Quadro atualizado com sucesso!\n";

    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoQuadro::excluir() {
    exibirSeparador("EXCLUIR QUADRO");
    cout << "  NOTA: Somente o gestor do projeto pode excluir o quadro.\n"
         << "        O quadro nao pode ser excluido se ainda houver cartoes.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do quadro: ", id)) { pausar(); return; }

    if (!confirmar("Deseja realmente excluir este quadro?")) {
        cout << "\n  Operacao cancelada.\n";
        pausar();
        return;
    }

    try {
        servico->excluir(id);
        cout << "\n  [OK] Quadro excluido com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoQuadro::listar() {
    exibirSeparador("LISTAR QUADROS");

    try {
        vector<Quadro> lista = servico->listar();
        if (lista.empty()) {
            cout << "  Nenhum quadro cadastrado.\n";
        } else {
            cout << "  Total: " << lista.size() << " quadro(s)\n";
            for (size_t i = 0; i < lista.size(); ++i) {
                cout << "\n  [" << (i + 1) << "]\n";
                exibirQuadro(lista[i]);
            }
        }
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoQuadro::exibirMenu() {
    while (true) {
        exibirSeparador("MENU - QUADROS");
        cout << "  1) Criar quadro\n"
             << "  2) Ler quadro\n"
             << "  3) Atualizar quadro\n"
             << "  4) Excluir quadro\n"
             << "  5) Listar quadros\n"
             << "  0) Voltar\n";
        int op = lerOpcao();
        switch (op) {
            case 1: criar();     break;
            case 2: ler();       break;
            case 3: atualizar(); break;
            case 4: excluir();   break;
            case 5: listar();    break;
            case 0: return;
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

// ===========================================================================
// ApresentacaoCartao — implementacao
// ===========================================================================

ApresentacaoCartao::ApresentacaoCartao(IServicoCartaoAtividade* servico)
    : servico(servico) {}

void ApresentacaoCartao::criar() {
    exibirSeparador("CRIAR CARTAO DE ATIVIDADE");
    cout << "  NOTA: Somente o gestor do projeto do quadro pode criar cartoes.\n"
         << "        O cartao entra automaticamente em estado 'A FAZER'.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao (ex.: CAR001): ", id)) {
        pausar(); return;
    }

    Nome nome;
    if (!lerDominio("  Nome do cartao: ", nome)) { pausar(); return; }

    Texto descricao;
    if (!lerDominio(
            "  Descricao (comeca c/ maiusc., termina c/ '.', ate 30 chars): ",
            descricao)) {
        pausar(); return;
    }

    Prioridade prioridade;
    if (!lerDominio("  Prioridade (ALTA, MEDIA ou BAIXA): ", prioridade)) {
        pausar(); return;
    }

    Tamanho tamanho;
    if (!lerDominio("  Tamanho (GRANDE, MEDIO ou PEQUENO): ", tamanho)) {
        pausar(); return;
    }

    Timestamp entrada;
    if (!lerDominio("  Data de entrada em A FAZER (ex.: 01-JAN-2024-09:00): ", entrada)) {
        pausar(); return;
    }

    Identificador quadroId;
    if (!lerDominio("  Identificador do quadro: ", quadroId)) { pausar(); return; }

    Email devEmail;
    if (!lerDominio("  E-mail do desenvolvedor responsavel: ", devEmail)) {
        pausar(); return;
    }

    // Estado e forcado para A FAZER pelo servico; usamos A FAZER aqui tambem
    Estado estadoInicial;
    estadoInicial.setValor("A FAZER");

    try {
        CartaoAtividade c(id, nome, descricao, prioridade, estadoInicial,
                          tamanho, entrada, quadroId, devEmail);
        servico->criar(c);
        cout << "\n  [OK] Cartao criado com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::ler() {
    exibirSeparador("LER CARTAO DE ATIVIDADE");

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    try {
        CartaoAtividade c = servico->ler(id);
        cout << "\n";
        exibirCartao(c);
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::atualizar() {
    exibirSeparador("ATUALIZAR CARTAO DE ATIVIDADE");
    cout << "  NOTA: Somente o gestor do projeto pode atualizar o cartao.\n"
         << "        Estado, datas e quadro so mudam via opcao 'Mover'.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    try {
        CartaoAtividade atual = servico->ler(id);
        cout << "\n  Dados atuais:\n";
        exibirCartao(atual);
        cout << "\n  Informe os novos valores (apenas campos editaveis):\n";

        Nome nome;
        if (!lerDominio("  Novo nome: ", nome)) { pausar(); return; }

        Texto descricao;
        if (!lerDominio(
                "  Nova descricao (comeca c/ maiusc., termina c/ '.'): ",
                descricao)) {
            pausar(); return;
        }

        Prioridade prioridade;
        if (!lerDominio("  Nova prioridade (ALTA, MEDIA ou BAIXA): ", prioridade)) {
            pausar(); return;
        }

        Tamanho tamanho;
        if (!lerDominio("  Novo tamanho (GRANDE, MEDIO ou PEQUENO): ", tamanho)) {
            pausar(); return;
        }

        Email devEmail;
        if (!lerDominio("  E-mail do novo desenvolvedor responsavel: ", devEmail)) {
            pausar(); return;
        }

        // Copia o cartao atual e atualiza apenas os campos editaveis;
        // estado, entrada, inicio, termino e quadroId serao preservados pelo servico
        CartaoAtividade novo = atual;
        novo.setNome(nome);
        novo.setDescricao(descricao);
        novo.setPrioridade(prioridade);
        novo.setTamanho(tamanho);
        novo.setDesenvolvedorEmail(devEmail);

        servico->atualizar(novo);
        cout << "\n  [OK] Cartao atualizado com sucesso!\n";

    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::excluir() {
    exibirSeparador("EXCLUIR CARTAO DE ATIVIDADE");
    cout << "  NOTA: Somente o gestor do projeto pode excluir cartoes.\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    if (!confirmar("Deseja realmente excluir este cartao?")) {
        cout << "\n  Operacao cancelada.\n";
        pausar();
        return;
    }

    try {
        servico->excluir(id);
        cout << "\n  [OK] Cartao excluido com sucesso!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::listar() {
    exibirSeparador("LISTAR CARTOES DE ATIVIDADE");

    try {
        vector<CartaoAtividade> lista = servico->listar();
        if (lista.empty()) {
            cout << "  Nenhum cartao cadastrado.\n";
        } else {
            cout << "  Total: " << lista.size() << " cartao(es)\n";
            for (size_t i = 0; i < lista.size(); ++i) {
                cout << "\n  [" << (i + 1) << "]\n";
                exibirCartao(lista[i]);
            }
        }
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::mover() {
    exibirSeparador("MOVER CARTAO DE ATIVIDADE");
    cout << "  Transicoes validas: A FAZER -> FAZENDO  ou  FAZENDO -> FEITO\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    // Exibe estado atual para ajudar o usuario
    try {
        CartaoAtividade atual = servico->ler(id);
        cout << "  Estado atual: " << atual.getEstado().getValor() << "\n\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
        pausar();
        return;
    }

    cout << "  Para qual estado deseja mover?\n"
         << "    1) FAZENDO  (A FAZER -> FAZENDO)\n"
         << "    2) FEITO    (FAZENDO -> FEITO)\n"
         << "    0) Cancelar\n";
    int opcao = lerOpcao();
    if (opcao == 0) {
        cout << "\n  Operacao cancelada.\n";
        pausar();
        return;
    }
    if (opcao != 1 && opcao != 2) {
        cout << "\n  Opcao invalida.\n";
        pausar();
        return;
    }

    Estado novoEstado;
    novoEstado.setValor(opcao == 1 ? "FAZENDO" : "FEITO");

    Timestamp instante;
    if (!lerDominio("  Instante da mudanca (ex.: 15-JAN-2024-10:30): ", instante)) {
        pausar(); return;
    }

    try {
        servico->mover(id, novoEstado, instante);
        cout << "\n  [OK] Cartao movido para " << novoEstado.getValor() << "!\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::lerTempoCiclo() {
    exibirSeparador("TEMPO DE CICLO DO CARTAO");
    cout << "  Cycle Time = termino - inicio  (apenas cartoes em FEITO)\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    try {
        long min = servico->lerTempoCiclo(id);
        cout << "\n  Tempo de Ciclo: " << formatarMinutos(min) << "\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::lerTempoCicloMedio() {
    exibirSeparador("TEMPO DE CICLO MEDIO DO QUADRO");
    cout << "  Media dos Cycle Times de todos os cartoes em FEITO do quadro.\n\n";

    Identificador idQuadro;
    if (!lerDominio("  Identificador do quadro: ", idQuadro)) { pausar(); return; }

    try {
        double media = servico->lerTempoCicloMedio(idQuadro);
        cout << "\n  Tempo de Ciclo Medio: "
             << formatarMinutos(static_cast<long>(media))
             << "\n  (valor exato: " << std::fixed << std::setprecision(1)
             << media << " min)\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::lerLeadTime() {
    exibirSeparador("LEAD TIME DO CARTAO");
    cout << "  Lead Time = termino - entrada  (apenas cartoes em FEITO)\n\n";

    Identificador id;
    if (!lerDominio("  Identificador do cartao: ", id)) { pausar(); return; }

    try {
        long min = servico->lerLeadTime(id);
        cout << "\n  Lead Time: " << formatarMinutos(min) << "\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::lerLeadTimeMedio() {
    exibirSeparador("LEAD TIME MEDIO DO QUADRO");
    cout << "  Media dos Lead Times de todos os cartoes em FEITO do quadro.\n\n";

    Identificador idQuadro;
    if (!lerDominio("  Identificador do quadro: ", idQuadro)) { pausar(); return; }

    try {
        double media = servico->lerLeadTimeMedio(idQuadro);
        cout << "\n  Lead Time Medio: "
             << formatarMinutos(static_cast<long>(media))
             << "\n  (valor exato: " << std::fixed << std::setprecision(1)
             << media << " min)\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::lerWip() {
    exibirSeparador("WIP DO QUADRO");
    cout << "  WIP = numero de cartoes em estado FAZENDO no quadro.\n\n";

    Identificador idQuadro;
    if (!lerDominio("  Identificador do quadro: ", idQuadro)) { pausar(); return; }

    try {
        unsigned int wip = servico->lerWip(idQuadro);
        cout << "\n  WIP atual do quadro: " << wip << " cartao(es) em FAZENDO.\n";
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void ApresentacaoCartao::exibirMenu() {
    while (true) {
        exibirSeparador("MENU - CARTOES DE ATIVIDADE");
        cout << "  --- CRUD ---\n"
             << "  1)  Criar cartao\n"
             << "  2)  Ler cartao\n"
             << "  3)  Atualizar cartao\n"
             << "  4)  Excluir cartao\n"
             << "  5)  Listar cartoes\n"
             << "  --- Kanban ---\n"
             << "  6)  Mover cartao (A FAZER->FAZENDO->FEITO)\n"
             << "  --- Metricas ---\n"
             << "  7)  Ler Tempo de Ciclo\n"
             << "  8)  Ler Tempo de Ciclo Medio (quadro)\n"
             << "  9)  Ler Lead Time\n"
             << "  10) Ler Lead Time Medio (quadro)\n"
             << "  11) Ler WIP (quadro)\n"
             << "  0)  Voltar\n";
        int op = lerOpcao();
        switch (op) {
            case 1:  criar();              break;
            case 2:  ler();               break;
            case 3:  atualizar();         break;
            case 4:  excluir();           break;
            case 5:  listar();            break;
            case 6:  mover();             break;
            case 7:  lerTempoCiclo();     break;
            case 8:  lerTempoCicloMedio(); break;
            case 9:  lerLeadTime();       break;
            case 10: lerLeadTimeMedio();  break;
            case 11: lerWip();            break;
            case 0: return;
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

// ===========================================================================
// Aplicacao — implementacao
// ===========================================================================

Aplicacao::Aplicacao(IServicoAutenticacao*    sa,
                     IServicoPessoa*          sp,
                     IServicoProjeto*         spj,
                     IServicoQuadro*          sq,
                     IServicoCartaoAtividade* sc,
                     std::function<void()>    logoutCallback)
    : sAuth(sa),
      sPessoa(sp),
      apPessoa(sp),
      apProjeto(spj),
      apQuadro(sq),
      apCartao(sc),
      fnLogout(logoutCallback),
      autenticado(false)
{}

void Aplicacao::criarConta() {
    // Delega ao fluxo compartilhado; nao exige autenticacao
    fluxoCriarPessoa(sPessoa);
}

void Aplicacao::autenticar() {
    exibirSeparador("LOGIN");

    Email email;
    if (!lerDominio("  E-mail: ", email)) { pausar(); return; }

    Senha senha;
    if (!lerDominio("  Senha: ", senha)) { pausar(); return; }

    try {
        bool ok = sAuth->autenticar(email, senha);
        if (ok) {
            cout << "\n  [OK] Login efetuado com sucesso! Bem-vindo(a).\n";
            autenticado = true;
        } else {
            cout << "\n  [FALHA] E-mail ou senha incorretos.\n";
        }
    } catch (const std::exception& e) {
        cout << "\n  [ERRO] " << e.what() << "\n";
    }
    pausar();
}

void Aplicacao::logout() {
    fnLogout();          // chama CSAutenticacao::encerrarSessao() via callback
    autenticado = false;
    cout << "\n  Sessao encerrada. Ate logo!\n";
    pausar();
}

void Aplicacao::menuInicial() {
    while (!autenticado) {
        exibirSeparador("SISTEMA KANBAN - TP1");
        cout << "  1) Criar conta\n"
             << "  2) Autenticar (Login)\n"
             << "  0) Sair\n";
        int op = lerOpcao();
        switch (op) {
            case 1: criarConta();  break;
            case 2: autenticar();  break;  // define autenticado=true se OK
            case 0: return;               // sair: autenticado permanece false
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

void Aplicacao::menuPrincipal() {
    while (autenticado) {
        exibirSeparador("MENU PRINCIPAL");
        cout << "  1) Pessoas\n"
             << "  2) Projetos\n"
             << "  3) Quadros\n"
             << "  4) Cartoes de Atividade\n"
             << "  0) Logout\n";
        int op = lerOpcao();
        switch (op) {
            case 1: apPessoa.exibirMenu();  break;
            case 2: apProjeto.exibirMenu(); break;
            case 3: apQuadro.exibirMenu();  break;
            case 4: apCartao.exibirMenu();  break;
            case 0: logout();               break;  // define autenticado=false
            default:
                cout << "\n  Opcao invalida. Tente novamente.\n";
                pausar();
        }
    }
}

void Aplicacao::executar() {
    cout << "\n" << string(LARGURA, '*') << "\n"
         << "  Sistema de Gestao de Projetos Kanban - TP1\n"
         << "  Tecnicas de Programacao 1 - UnB\n"
         << string(LARGURA, '*') << "\n";

    bool rodando = true;
    while (rodando) {
        menuInicial();

        if (autenticado) {
            // Usuario fez login: exibe menu principal ate logout
            menuPrincipal();
            // Apos logout, volta ao menuInicial (autenticado == false)
        } else {
            // Usuario escolheu Sair no menu inicial
            rodando = false;
        }
    }

    cout << "\n  Encerrando o sistema. Obrigado!\n"
         << string(LARGURA, '*') << "\n\n";
}
