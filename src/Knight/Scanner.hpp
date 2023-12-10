#pragma once

#include "MathParser.hpp"

/**
 * @details class to handle the custom scanner query from kafka
 * */
class Scanner : public MathParser {
  public:
    /**
     * @details constructor
     * */
    explicit Scanner();

    ~Scanner() override = default;

    /**
     * @details function to process new infromation of data to scanner query
     * */
    void Process(const int token);

    /**
     * @details function to get instance of scanner class
     * */
    static Scanner& GetInstance();

    /**
     * @details function to unsubscribe the given query
     * */
    void ScannerUnsubscribe(uint64_t key);

    /**
     * @details function to execute and process the integrity of scanner query
     * */
    bool EvaluateExp(uint64_t key, const std::string& formula, bool OneShot);

  protected:
  private:
    /**
     * @details function to invoke event to all given scanner query
     * */
    void Transfer(int Token, const ExpressionContainerT::value_type& value);

    /**
     * @details function to check the scanner is not returning same output
     * */
};
