#ifndef LANCELOT_INCLUDE_STORE_PROCEDURES_HPP
#define LANCELOT_INCLUDE_STORE_PROCEDURES_HPP
#pragma once

inline constexpr char Createdb_[] =
    "CREATE TABLE IF NOT EXISTS ResultSet(Token INTEGER,"
    " Counter INTEGER PRIMARY KEY AUTOINCREMENT,"
    " StrikePrice INTEGER,"
    " InstType TEXT,"
    " Symbol TEXT,"
    " ExpiryDate INTEGER,"
    " Description TEXT,"
    " OptionType TEXT,"
    " Name TEXT,"
    " Exchange TEXT,"
    " Future INTEGER,"
    " LotSize INTEGER,"
    " TickSize INTEGER,"
    " Divisor INTEGER,"
    " Close INTEGER,"
    " HighDPR INTEGER,"
    " LowDPR INTEGER,"
    " FreezeQty INTEGER)";

inline constexpr char IsFuture_[] =
    "SELECT "
    " CASE "
    " WHEN R1.InstType='FUTSTK' THEN 1"
    " WHEN R1.InstType='FUTIDX' THEN 1"
    " WHEN R1.InstType='FUTCOM' THEN 1"
    " ELSE 0"
    " END"
    " FROM ResultSet R1"
    " WHERE R1.Token={}";

inline constexpr char IsOption_[] =
    "SELECT"
    " CASE"
    " WHEN R1.InstType='OPTSTK' THEN 1"
    " WHEN R1.InstType='OPTIDX' THEN 1"
    " WHEN R1.InstType='OPTCOM' THEN 1"
    " ELSE 0"
    " END"
    " FROM ResultSet R1"
    " WHERE R1.Token={}";

inline constexpr char IsEquity_[] =
    "SELECT"
    " CASE WHEN R1.InstType='Equity' THEN 1"
    " ELSE 0"
    " END"
    " FROM ResultSet R1"
    " WHERE R1.Token={}";

inline constexpr char GetNextStrike_[] =
    "SELECT T1.Token, T1.StrikePrice FROM ResultSet T1"
    " INNER JOIN"
    " ResultSet T2 ON"
    " T1.ExpiryDate=T2.ExpiryDate AND"
    " T1.OptionType=T2.OptionType AND"
    " T1.InstType=T2.InstType AND"
    " T1.Symbol=T2.Symbol"
    " WHERE T1.StrikePrice > T2.StrikePrice AND"
    " T2.Token= {} ORDER BY T1.StrikePrice ASC LIMIT 1;";

inline constexpr char GetPreviousStrike_[] =
    "SELECT T1.Token, T1.StrikePrice FROM ResultSet T1"
    " INNER JOIN"
    " ResultSet T2 ON"
    " T1.ExpiryDate=T2.ExpiryDate AND"
    " T1.OptionType=T2.OptionType AND"
    " T1.InstType=T2.InstType AND"
    " T1.Symbol=T2.Symbol"
    " WHERE T1.StrikePrice < T2.StrikePrice AND"
    " T2.Token= {} ORDER BY T1.StrikePrice DESC LIMIT 1;";

inline constexpr char IsCall_[] =
    "SELECT"
    " CASE WHEN R1.OptionType='CE' THEN 1"
    " ELSE 0"
    " END"
    " FROM ResultSet R1"
    " WHERE R1.Token={};";

inline constexpr char IsPut_[] =
    "SELECT"
    " CASE WHEN R1.OptionType='PE' THEN 1"
    " ELSE 0"
    " END"
    " FROM ResultSet R1"
    " WHERE R1.Token={};";

inline constexpr char GetNextExpiry_[] =
    "SELECT T1.Token FROM ResultSet T1"
    " INNER JOIN"
    " ResultSet T2 ON"
    " T1.InstType=T2.InstType AND"
    " T1.Symbol = T2.Symbol AND"
    " T1.OptionType=T2.OptionType AND"
    " T1.StrikePrice= T2.StrikePrice AND"
    " T1.Segment=T2.Segment AND"
    " T1.Exchange=T2.Exchange AND"
    " T1.ExpiryDate > T2.ExpiryDate"
    " WHERE T2.Token={}"
    " ORDER BY"
    " T1.ExpiryDate LIMIT 1;";

inline constexpr char GetCashForToken_[] =
    " SELECT T1.Token FROM ResultSet T1"
    " INNER JOIN"
    " ResultSet T2 ON"
    " T1.Symbol=T2.Symbol WHERE"
    " T2.Token={} AND"
    " T1.Segment='Cash' AND"
    " T1.InstType='Equity';";

inline constexpr char GetAltOptionType_[] =
    "SELECT T1.Token FROM ResultSet T1"
    " INNER JOIN ResultSet T2 ON"
    " T1.ExpiryDate = T2.ExpiryDate AND"
    " T1.OptionType <> T2.OptionType AND"
    " T1.InstType = T2.InstType AND"
    " T1.Symbol = T2.Symbol"
    " WHERE T1.StrikePrice = T2.StrikePrice AND"
    " T2.Token = {};";

inline constexpr char GetNoticeToken_[] =
    "SELECT T1.Token FROM ResultSet T1"
    " WHERE"
    " T1.ExpiryDate ={} AND"
    " T1.Symbol={} AND"
    " T1.StrikePrice={} AND"
    " T1.OptionType={}";

inline constexpr char StrikeRange[] =
    "SELECT T1.StrikePrice FROM ResultSet T1"
    " INNER JOIN"
    " ResultSet T2"
    " WHERE T1.Symbol = T2.Symbol"
    " AND T2.Token= {}"
    " AND T1.ExpiryDate = T2.ExpiryDate"
    " AND T1.StrikePrice BETWEEN {} AND {} ;";

inline constexpr char CreateStrategy_[] = "CREATE TABLE IF NOT EXISTS Strategy (name TEXT, data TEXT)";

inline constexpr char InsertStrategy_[]    = "INSERT INTO Strategy(name, data) values('{}', '{}')";
inline constexpr char RemoveStrategy_[]    = "DELETE FROM Strategy WHERE name = '{}'";
inline constexpr char GetStrategyID_[]     = "SELECT `ID` FROM Strategy WHERE Name = '{}'";
inline constexpr char GetStrategyParams_[] = "SELECT `ID`, Data FROM tblStrategyData WHERE StrategyName = '{}'";
inline constexpr char GetOptionChain_[] =
    "SELECT t1.Token, t2.Token, t1.StrikePrice"
    " FROM ResultSet t1, ResultSet t2"
    " WHERE t1.ExpiryDate == t2.ExpiryDate"
    " AND t1.StrikePrice == t2.StrikePrice"
    " AND t1.OptionType = 'CE'"
    " AND t2.OptionType = 'PE'"
    " AND t1.Token != t2.Token"
    " AND t1.Symbol = '{}'"
    " AND t2.Symbol = '{}'"
    " AND t1.ExpiryDate = {}"
    " AND t1.StrikePrice {} {}"
    " ORDER BY t1.StrikePrice {}"
    " LIMIT 10";

inline constexpr char GetFuture_[] =
    "SELECT T2.Token, T1.Token FROM ResultSet T1"
    " INNER JOIN ResultSet T2 ON T1.InstType="
    " (CASE "
    " WHEN T2.InstType ='OPTIDX' THEN 'FUTIDX'"
    " WHEN T2.InstType='OPTSTK' THEN 'FUTSTK'"
    " WHEN T2.InstType='OPTFUT' THEN 'FUTCOM'"
    " WHEN T2.InstType='OPTCUR' THEN 'FUTCUR'"
    " ELSE T2.InstType"
    " END)"
    " AND T1.Symbol = T2.Symbol"
    " AND T1.ExpiryDate >= T2.ExpiryDate"
    " ORDER BY T1.ExpiryDate";

inline constexpr char InsertResultSetRow_[] =
    "INSERT INTO ResultSet (Token, StrikePrice, InstType, Symbol, ExpiryDate, "
    "Description, OptionType, Name, Exchange, Future, LotSize, TickSize, Divisor, Close, HighDPR, LowDPR, FreezeQty) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

inline constexpr char GetClientCode_[]      = "SELECT Exchange, ClientCode FROM ManageClientCode WHERE  UserId = {}";
inline constexpr char GetStrategyList_[]    = "SELECT Name FROM Strategy";
inline constexpr char GetStrategyColumns_[] = "SELECT data FROM Strategy WHERE Name='{}'";
inline constexpr char GetResultSet_[]       = "SELECT * FROM ResultSet;";
inline constexpr char DeleteResultSet_[]    = "DELETE FROM ResultSet where Exchange=?";

#endif  // LANCELOT_INCLUDE_STORE_PROCEDURES_HPP
