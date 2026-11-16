#ifndef TESTES_DOMINIOS_H_INCLUDED
#define TESTES_DOMINIOS_H_INCLUDED

#include "../include/dominios.h"

/**
 * @brief Classe base para execução de testes unitários dos domínios.
 */
class TUDominio {
protected:
    int estado;
    virtual void setUp() = 0;
    virtual void tearDown() = 0;
    virtual void testarCenarioSucesso() = 0;
    virtual void testarCenarioFalha() = 0;

public:
    static const int SUCESSO = 0;
    static const int FALHA = -1;
    int run();
    virtual ~TUDominio() = default;
};

class TUEmail : public TUDominio {
private:
    Email *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUEstado : public TUDominio {
private:
    Estado *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUIdentificador : public TUDominio {
private:
    Identificador *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TULimite : public TUDominio {
private:
    Limite *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUNome : public TUDominio {
private:
    Nome *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUPapel : public TUDominio {
private:
    Papel *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUPrioridade : public TUDominio {
private:
    Prioridade *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUSenha : public TUDominio {
private:
    Senha *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUTamanho : public TUDominio {
private:
    Tamanho *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUTexto : public TUDominio {
private:
    Texto *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

class TUTimestamp : public TUDominio {
private:
    Timestamp *dominio;
    void setUp() override;
    void tearDown() override;
    void testarCenarioSucesso() override;
    void testarCenarioFalha() override;
};

#endif // TESTES_DOMINIOS_H_INCLUDED
