#ifndef ARTHUR_INCLUDE_ENUMS_HPP
#define ARTHUR_INCLUDE_ENUMS_HPP
#pragma once

enum VisualTheme : int {
    VisualTheme_MATERIAL_LIGHT = 0,
    VisualTheme_MATERIAL_DARK,
    VisualTheme_ONE_LIGHT,
    VisualTheme_ONE_DARK,
    VisualTheme_ARTHUR_LIGHT,
    VisualTheme_ARTHUR_DARK,
    VisualTheme_IMGUI_LIGHT,
    VisualTheme_IMGUI_DARK,
    VisualTheme_END
};
enum ColumnGeneratorColumnIndex {
    ColumnGeneratorColumnIndex_NAME = 0,
    ColumnGeneratorColumnIndex_TYPE,
    ColumnGeneratorColumnIndex_VALUE,
    ColumnGeneratorColumnIndex_END
};

enum MarketWatchColumnIndex {
    MarketWatchColumnIndex_CONTACT_NAME = 0,
    MarketWatchColumnIndex_ATP,
    MarketWatchColumnIndex_LTP,
    MarketWatchColumnIndex_LTQ,
    MarketWatchColumnIndex_LTT,
    MarketWatchColumnIndex_TOP_BID,
    MarketWatchColumnIndex_TOP_ASK,
    MarketWatchColumnIndex_OPEN,
    MarketWatchColumnIndex_HIGH,
    MarketWatchColumnIndex_LOW,
    MarketWatchColumnIndex_CLOSE,
    MarketWatchColumnIndex_LOWDPR,
    MarketWatchColumnIndex_HIGHDPR,
    MarketWatchColumnIndex_TOTAL_BUY_QUANTITY,
    MarketWatchColumnIndex_TOTAL_SELL_QUANTITY,
    MarketWatchColumnIndex_VOLUME_TRADED_TODAY,
    MarketWatchColumnIndex_OPEN_INTEREST,
    MarketWatchColumnIndex_END
};

enum MarketWatchToolTipColumnIndex {
    MarketWatchToolTipColumnIndex_BUY_ORDER = 0,
    MarketWatchToolTipColumnIndex_BUY_QUANTITY,
    MarketWatchToolTipColumnIndex_BUY_PRICE,
    MarketWatchToolTipColumnIndex_ASK_PRICE,
    MarketWatchToolTipColumnIndex_ASK_QUANTITY,
    MarketWatchToolTipColumnIndex_ASK_ORDER,
    MarketWatchToolTipColumnIndex_END
};
enum DataType : int {
    DataType_CLIENT = 0,
    DataType_COMBO,
    DataType_CONTRACT,
    DataType_FLOAT,
    DataType_INT,
    DataType_TEXT,
    DataType_RADIO,
    DataType_UPDATES,
    DataType_END
};

enum BooksColumnIndex {
    BooksColumnIndex_PF = 0,
    BooksColumnIndex_CONTRACT,
    BooksColumnIndex_PRICE,
    BooksColumnIndex_QUANTITY,
    BooksColumnIndex_FILLPRICE,
    BooksColumnIndex_FILLQUANTITY,
    BooksColumnIndex_REMAINING_QTY,
    BooksColumnIndex_CLIENT,
    BooksColumnIndex_STATUS,
    BooksColumnIndex_TIME,
    BooksColumnIndex_GATEWAY,
    BooksColumnIndex_ORDERNUMBER,
    BooksColumnIndex_MESSAGE,
    BooksColumnIndex_END
};

enum SymbolWiseNetBookColumnIndex {
    SymbolWiseNetBookColumnIndex_CONTRACT,
    SymbolWiseNetBookColumnIndex_AVGBID,
    SymbolWiseNetBookColumnIndex_BUYQTY,
    SymbolWiseNetBookColumnIndex_SELLQTY,
    SymbolWiseNetBookColumnIndex_AVGSELL,
    SymbolWiseNetBookColumnIndex_TOTAL,
    SymbolWiseNetBookColumnIndex_NETINVEST,
    SymbolWiseNetBookColumnIndex_MTM,
    SymbolWiseNetBookColumnIndex_LTP,
    SymbolWiseNetBookColumnIndex_PNL,
    SymbolWiseNetBookColumnIndex_END
};

enum PFWiseNetBookColumnIndex {
    PFWiseNetBookColumnIndex_PF,
    PFWiseNetBookColumnIndex_CONTRACT,
    PFWiseNetBookColumnIndex_AVGBID,
    PFWiseNetBookColumnIndex_BUYQTY,
    PFWiseNetBookColumnIndex_SELLQTY,
    PFWiseNetBookColumnIndex_AVGSELL,
    PFWiseNetBookColumnIndex_TOTAL,
    PFWiseNetBookColumnIndex_NETINVEST,
    PFWiseNetBookColumnIndex_MTM,
    PFWiseNetBookColumnIndex_LTP,
    PFWiseNetBookColumnIndex_PNL,
    PFWiseNetBookColumnIndex_END
};

enum GreekBookColumnIndex {
    GreekBookColumnIndex_SYMBOL,
    GreekBookColumnIndex_DELTA,
    GreekBookColumnIndex_GAMMA,
    GreekBookColumnIndex_VEGA,
    GreekBookColumnIndex_THETA,
    GreekBookColumnIndex_MTM,
    GreekBookColumnIndex_MARKETRATE,
    GreekBookColumnIndex_VALUE,
    GreekBookColumnIndex_END,
};

enum OptionChainColumnIndex {
    OptionChainColumnIndex_CALL_OI,
    OptionChainColumnIndex_CALL_VOLUME,
    OptionChainColumnIndex_CALL_DELTA,
    OptionChainColumnIndex_CALL_GAMMA,
    OptionChainColumnIndex_CALL_VEGA,
    OptionChainColumnIndex_CALL_THETA,
    OptionChainColumnIndex_CALL_IV,
    OptionChainColumnIndex_CALL_LTP,
    OptionChainColumnIndex_CALL_CHANGE,
    OptionChainColumnIndex_CALL_BID_QTY,
    OptionChainColumnIndex_CALL_BID_PRICE,
    OptionChainColumnIndex_CALL_ASK_PRICE,
    OptionChainColumnIndex_CALL_ASK_QTY,
    OptionChainColumnIndex_STRIKE_PRICE,
    OptionChainColumnIndex_PUT_BID_QTY,
    OptionChainColumnIndex_PUT_BID_PRICE,
    OptionChainColumnIndex_PUT_ASK_PRICE,
    OptionChainColumnIndex_PUT_ASK_QTY,
    OptionChainColumnIndex_PUT_CHANGE,
    OptionChainColumnIndex_PUT_LTP,
    OptionChainColumnIndex_PUT_IV,
    OptionChainColumnIndex_PUT_THETA,
    OptionChainColumnIndex_PUT_VEGA,
    OptionChainColumnIndex_PUT_GAMMA,
    OptionChainColumnIndex_PUT_DELTA,
    OptionChainColumnIndex_PUT_VOLUME,
    OptionChainColumnIndex_PUT_OI,
    OptionChainColumnIndex_END
};

enum ScannerFunctionColumnIndex {
    ScannerFunctionColumnIndex_NAME,
    ScannerFunctionColumnIndex_VARIABLE,
    ScannerFunctionColumnIndex_END
};

enum ScannerSavedColumnIndex {
    ScannerSavedColumnIndex_NUMBER,
    ScannerSavedColumnIndex_NAME,
    ScannerSavedColumnIndex_OPERATIONS,
    ScannerSavedColumnIndex_END
};

enum ExportImport : int {
    ExportImport_EXPORT,
    ExportImport_IMPORT,
    ExportImport_NONE
};

enum ExcelDataColumn {
    ExcelDataColumn_BID_ORDER = 1,
    ExcelDataColumn_BID_QUANTITY,
    ExcelDataColumn_BID_PRICE,
    ExcelDataColumn_ASK_ORDER,
    ExcelDataColumn_ASK_QUANTITY,
    ExcelDataColumn_ASK_PRICE,
};

enum ExcelSecondColumn {
    ExcelSecondColumn_LTP = 1,
    ExcelSecondColumn_ATP,
    ExcelSecondColumn_OPEN,
    ExcelSecondColumn_HIGH,
    ExcelSecondColumn_LOW,
    ExcelSecondColumn_CLOSE,
};

enum ExcelThirdColumn {
    ExcelThirdColumn_LOW_DPR = 1,
    ExcelThirdColumn_HIGH_DPR,
    ExcelThirdColumn_TOTAL_BUY_QUANTITY,
    ExcelThirdColumn_TOTAL_SELL_QUANTITY,
    ExcelThirdColumn_VOLUME_TRADED_TODAY,
    ExcelThirdColumn_OPEN_INTEREST
};

enum ConfigFile {
    ConfigFile_DEMO = 0,
    ConfigFile_EXCEL_WINDOW,
    ConfigFile_GREEK_BOOK,
    ConfigFile_THEME,
    ConfigFile_MARKET_LADDER,
    ConfigFile_MARKET_WATCH,
    ConfigFile_PENDING_BOOK,
    ConfigFile_STRATEGY_WORKSPACE,
    ConfigFile_TRADE_BOOK,
    ConfigFile_ORDER_ALL_BOOK,
    ConfigFile_REJECT_BOOK,
    ConfigFile_OPTION_CHAIN,
    ConfigFile_END
};

enum TradeTrackerColumn {
    TradeTrackerColumn_ID,
    TradeTrackerColumn_NAME,
    TradeTrackerColumn_DESCRIPTIONS,
    TradeTrackerColumn_END
};

enum ClientIndex {
    ClientIndex_EXCHANGE = 0,
    ClientIndex_CLIENTCODE
};

enum Spot {
    Spot_BANKNIFTY = 0,
    Spot_NIFTY,
    Spot_VIX
};

enum NetBookCalculation {
    NetBookCalculation_SYMBOL,
    NetBookCalculation_PF,
    NetBookCalculation_GREEK
};

#endif// ARTHUR_INCLUDE_ENUMS_HPP
