#ifndef DOMINIOS_H_INCLUDED
#define DOMINIOS_H_INCLUDED

#include <string>
#include <stdexcept>

using namespace std;

/**
 * @brief Classe abstrata base para todos os domínios do sistema.
 * 
 * Define a interface padrão com métodos set e get, além de centralizar
 * o mecanismo de validação que lança exceções do tipo invalid_argument.
 */
class Dominio {
protected:
    /** @brief Valor textual armazenado no dominio. */
    string valor;
    /**
     * @brief Método virtual puro responsável por validar o formato do dado.
     * @param valor String a ser validada.
     * @throw invalid_argument Caso o valor não atenda às regras de negócio.
     */
    virtual void validar(string valor) = 0;

public:
    /**
     * @brief Define o valor do domínio após validação.
     * @param valor Novo valor a ser atribuído.
     * @throw invalid_argument Caso o formato seja inválido.
     */
    void setValor(string valor);

    /**
     * @brief Retorna o valor armazenado no domínio.
     * @return string Valor atual do atributo.
     */
    string getValor() const;

    /** @brief Destrutor virtual padrão. */
    virtual ~Dominio() = default;
};

inline string Dominio::getValor() const {
    return valor;
}

/**
 * @brief Representa um endereço de e-mail válido.
 * 
 * Formato: parte-local@@dominio.
 * Parte local: até 64 caracteres (a-z, 0-9, '.', '-'), não inicia/termina com '.' ou '-',
 * e '.' ou '-' deve ser seguido por letra ou dígito.
 * Domínio: partes separadas por '.', até 255 caracteres (a-z, 0-9, '-'), não inicia/termina com '-'.
 */
class Email : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o estado de um cartão de atividade no quadro Kanban.
 * 
 * Valores válidos: "A FAZER", "FAZENDO", "FEITO".
 */
class Estado : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o código identificador único de entidades.
 * 
 * Formato: 6 caracteres, sendo os 3 primeiros letras (a-z, A-Z) e os 3 últimos dígitos (0-9).
 */
class Identificador : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o limite máximo de cartões simultâneos (WIP) na coluna FAZENDO.
 * 
 * Valores válidos: inteiros de 1 a 25.
 */
class Limite {
private:
    /** @brief Valor numerico do limite armazenado. */
    int valor;
    /**
     * @brief Valida se o limite esta entre 1 e 25.
     * @param valor Inteiro a validar.
     * @throw invalid_argument Caso fora do intervalo.
     */
    void validar(int valor);

public:
    /**
     * @brief Define o valor numérico do limite após validação.
     * @param valor Inteiro entre 1 e 25.
     * @throw invalid_argument Caso esteja fora do intervalo permitido.
     */
    void setValor(int valor);

    /**
     * @brief Retorna o valor numérico do limite.
     * @return int Valor atual do limite.
     */
    int getValor() const;
};

inline int Limite::getValor() const {
    return valor;
}

/**
 * @brief Representa um nome próprio no sistema.
 * 
 * Formato: até 15 caracteres (letras A-Z, a-z ou espaço), espaço seguido por letra,
 * sem espaços no início ou no fim.
 */
class Nome : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o papel de um usuário no sistema.
 * 
 * Valores válidos: "GESTOR", "DESENVOLVEDOR".
 */
class Papel : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o nível de prioridade de um cartão de atividade.
 * 
 * Valores válidos: "ALTA", "MEDIA", "BAIXA".
 */
class Prioridade : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa a senha de autenticação de um usuário.
 * 
 * Formato: exatamente 5 caracteres alfanuméricos (a-z, A-Z, 0-9),
 * contendo pelo menos uma letra e um dígito.
 */
class Senha : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa o tamanho estimado de uma atividade.
 * 
 * Valores válidos: "GRANDE", "MEDIO", "PEQUENO" (sem acento, conforme o requisito
 * nao funcional 12, que dispensa acentuacao nas validacoes).
 */
class Tamanho : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa um texto descritivo curto.
 * 
 * Formato: até 30 caracteres (letras, dígitos, espaços e pontuação , ; . : ? !),
 * pontuação não seguida por pontuação, inicia com letra maiúscula e termina com ponto ('.').
 */
class Texto : public Dominio {
private:
    void validar(string valor) override;
};

/**
 * @brief Representa uma marcação de data e hora válida.
 * 
 * Formato: DIA-MES-ANO-HORARIO (ex: 29-FEV-2024-14:30).
 * DIA: 1 a 31 (validado conforme mês e ano bissexto).
 * MES: JAN, FEV, MAR, ABR, MAI, JUN, JUL, AGO, SET, OUT, NOV, DEZ.
 * ANO: 2000 a 2099.
 * HORARIO: 00:00 a 23:59.
 */
class Timestamp : public Dominio {
private:
    void validar(string valor) override;
    /**
     * @brief Verifica se um ano e bissexto.
     * @param ano Ano a verificar.
     * @return true se o ano e bissexto; false caso contrario.
     */
    bool ehBissexto(int ano);
};

#endif // DOMINIOS_H_INCLUDED
