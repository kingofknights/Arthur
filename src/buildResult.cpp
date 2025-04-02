#include "Lancelot.hpp"
#include "Logger.hpp"
#include "Structure.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
namespace Lancelot {
    static void Initialize(const std::string& name_) {
        std::fstream file(name_, std::ios::in);
        if (not file.is_open()) {
            LOG(INFO, "unable to open contract file {}", name_);
            return;
        }
        TableWithColumnIndexT table;

        while (not file.eof()) {
            std::string data;
            std::getline(file, data);

            if (data.empty() or std::isalpha(data[0]) != 0) {
                continue;
            }

            RowWithColumnIndexT result;
            boost::split(result, data, boost::is_any_of(" ,"));

            auto* resultSetPtr = new ResultSetT;

            resultSetPtr->_token       = boost::lexical_cast<uint32_t>(result[TOKEN]);
            resultSetPtr->_futureToken = boost::lexical_cast<uint32_t>(result[FUTURE_TOKEN]);
            resultSetPtr->_expiryDate  = boost::lexical_cast<uint32_t>(result[EXPIRY]);
            resultSetPtr->_lotSize     = boost::lexical_cast<uint32_t>(result[LOT_SIZE]);
            resultSetPtr->_tickSize    = boost::lexical_cast<uint32_t>(result[TICK_SIZE]);
            resultSetPtr->_divisor     = 100;
            resultSetPtr->_instType    = ContractInfo::GetInstrumentType(result[INSTRUMENT_TYPE]);
            resultSetPtr->_option      = ContractInfo::GetOptionType(result[OPTION_TYPE]);
            resultSetPtr->_exchange    = Exchange_NSE_FUTURE;
            resultSetPtr->_strikePrice = boost::lexical_cast<float>(result[STRIKE]);
            resultSetPtr->_symbol      = result[SYMBOL];
            resultSetPtr->_segment     = "F&O";

            resultSetPtr->_name = result[DESCRIPTION];

            {
                std::stringstream ss;
                ss << (resultSetPtr->_strikePrice < 0 ? "FUT" : "OPT");
                ss << ' ' << resultSetPtr->_symbol.data();
                if (resultSetPtr->_strikePrice > 0) ss << ' ' << (resultSetPtr->_strikePrice) << ' ' << (resultSetPtr->_option == Lancelot::OptionType_CALL ? "CE" : "PE");
                ss << ' ' << FORMAT("{:%d%b}", fmt::localtime(resultSetPtr->_expiryDate));
                auto description           = boost::to_upper_copy(ss.str());
                resultSetPtr->_description = description;
            }

            table.push_back(result);
        }
        auto* contractFetcher = new ContractFetcher("ResultSet.db3");
        contractFetcher->Insert(table);
    }
}  // namespace Lancelot
auto main(int argc, const char** argv) -> int {
    if (argc < 2) {
        LOG(ERROR, "usage : {} <filename>", argv[0]);
        return 0;
    }
    Lancelot::Initialize(argv[1]);
}
