#ifndef TESTES_SERVICOS_H_INCLUDED
#define TESTES_SERVICOS_H_INCLUDED

#include <string>

/**
 * @brief Testes de unidade da camada de servico.
 *
 * Cada servico do sistema (autenticacao mais os 26 servicos da tabela do
 * enunciado) tem um caso de teste com cenario de sucesso. Alguns casos tambem
 * conferem uma falha tipica (por exemplo, papel sem direito ao servico).
 */
class TesteServicos {
private:
    int falhas;

    void registrar(const std::string& nome, bool passou);

    bool testarAutenticar();

    bool testarCriarPessoa();
    bool testarLerPessoa();
    bool testarAtualizarPessoa();
    bool testarExcluirPessoa();
    bool testarListarPessoa();

    bool testarCriarProjeto();
    bool testarLerProjeto();
    bool testarAtualizarProjeto();
    bool testarExcluirProjeto();
    bool testarListarProjeto();

    bool testarCriarQuadro();
    bool testarLerQuadro();
    bool testarAtualizarQuadro();
    bool testarExcluirQuadro();
    bool testarListarQuadro();

    bool testarCriarCartao();
    bool testarLerCartao();
    bool testarAtualizarCartao();
    bool testarExcluirCartao();
    bool testarListarCartao();
    bool testarMoverCartao();
    bool testarLerTempoCiclo();
    bool testarLerTempoCicloMedio();
    bool testarLerLeadTime();
    bool testarLerLeadTimeMedio();
    bool testarLerWip();

public:
    TesteServicos() : falhas(0) {}

    /**
     * @brief Executa todos os casos de teste e imprime o resultado de cada um.
     * @return Quantidade de casos de teste que falharam (0 = todos passaram).
     */
    int executar();
};

#endif // TESTES_SERVICOS_H_INCLUDED
