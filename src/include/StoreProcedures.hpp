#pragma once

inline constexpr char createdb_[] =
	"CREATE TABLE ResultSet (Token INTEGER, Counter INTEGER PRIMARY KEY "
	"AUTOINCREMENT, StrikePrice INTEGER, InstType TEXT, Symbol TEXT, "
	"ExpiryDate INTEGER Description TEXT, "
	"OptionType TEXT, Name TEXT, Exchange TEXT, Segment TEXT)";

inline constexpr char IsFuture_[] =
	"SELECT case when R1.InstType='FUTSTK' THEN 1 WHEN R1.InstType='FUTIDX' "
	"then 1 when R1.InstType='FUTCOM' then 1 else 0 end from ResultSet R1 "
	"where R1.Token=?";

inline constexpr char IsOption_[] =
	"SELECT case when R1.InstType='OPTSTK' THEN 1 WHEN R1.InstType='OPTIDX' "
	"then 1 when R1.InstType='OPTCOM' then 1 else 0 end from ResultSet R1 "
	"where R1.Token=?";

inline constexpr char IsEquity_[] =
	"SELECT case when R1.InstType='Equity' THEN 1 "
	"else 0 end from ResultSet R1 where R1.Token=?";

inline constexpr char GetNextStrike_[] =
	"SELECT T1.Token, T1.StrikePrice from ResultSet T1 inner join ResultSet T2 "
	"on T1.ExpiryDate = T2.ExpiryDate and T1.OptionType = T2.OptionType and "
	"T1.InstType = "
	"T2.InstType and T1.Symbol = T2.Symbol where T1.StrikePrice > "
	"T2.StrikePrice and T2.Token = ? order by T1.StrikePrice asc limit 1;";

inline constexpr char GetPreviousStrike_[] =
	"SELECT T1.Token, T1.StrikePrice from ResultSet T1 inner join ResultSet T2 "
	"on T1.ExpiryDate = T2.ExpiryDate and T1.OptionType = T2.OptionType and "
	"T1.InstType = "
	"T2.InstType and T1.Symbol = T2.Symbol where T1.StrikePrice < "
	"T2.StrikePrice and T2.Token = ? order by T1.StrikePrice desc limit 1;";

inline constexpr char IsCall_[] =
	"SELECT case when R1.OptionType='CE' then 1 else 0 "
	"end from ResultSet R1 where R1.Token = ?;";

inline constexpr char IsPut_[] =
	"SELECT case when R1.OptionType='PE' then 1 else 0 "
	"end from ResultSet R1 where R1.Token = ?;";

inline constexpr char GetTokenFromName_[] = "SELECT T1.Token from ResultSet T1 where T1.Name=?";

inline constexpr char GetNextExpiry_[] =
	"SELECT T1.Token from ResultSet T1 inner join ResultSet T2 on T1.InstType= "
	"T2.InstType \n"
	"and T1.Symbol = T2.Symbol and T1.OptionType=T2.OptionType and "
	"T1.StrikePrice= T2.StrikePrice\n"
	"and T1.Segment=T2.Segment and T1.Exchange=T2.Exchange\n"
	"and T1.ExpiryDate > T2.ExpiryDate where T2.Token =? order by "
	"T1.ExpiryDate limit 1; \n";

inline constexpr char GetCashForToken_[] =
	" select T1.Token from ResultSet T1 inner join ResultSet T2 \n"
	" on  T1.Symbol=T2.Symbol where T2.Token =? and T1.Segment = 'Cash' and "
	"T1.InstType='Equity' ;";

inline constexpr char GetAltOptionType_[] =
	"SELECT T1.Token from ResultSet T1 inner join ResultSet T2 on "
	"T1.ExpiryDate = T2.ExpiryDate and T1.OptionType <> T2.OptionType and "
	"T1.InstType = T2.InstType and\n"
	"T1.Symbol = T2.Symbol where T1.StrikePrice = T2.StrikePrice and T2.Token "
	"=? ;";

inline constexpr char GetNoticeToken_[] =
	"SELECT T1.Token from ResultSet T1 where T1.ExpiryDate =? and T1.Symbol=? "
	"and T1.StrikePrice=? and T1.OptionType=?";

inline constexpr char StrikeRange[] =
	"SELECT T1.StrikePrice FROM ResultSet T1"
	" INNER JOIN"
	" ResultSet T2"
	" WHERE T1.Symbol = T2.Symbol"
	" AND T2.Token= {}"
	" AND T1.ExpiryDate = T2.ExpiryDate"
	" AND T1.StrikePrice BETWEEN {} AND {} ;";

inline constexpr char InsertStrategy_[]	   = "INSERT INTO Strategy(name, data) values('{}', '{}')";
inline constexpr char GetStrategyID_[]	   = "SELECT `ID` FROM Strategy WHERE Name = '{}'";
inline constexpr char GetStrategyParams_[] = "SELECT `ID`, Data FROM tblStrategyData WHERE StrategyName = '{}'";
inline constexpr char GetOptionChain_[] =
	"select t1.Token, t2.Token, t1.StrikePrice"
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

inline constexpr char GetClientCode_[]		= "SELECT Exchange, ClientCode FROM ManageClientCode WHERE  UserId = {}";
inline constexpr char GetStrategyList_[]	= "SELECT Name FROM Strategy";
inline constexpr char GetStrategyColumns_[] = "SELECT data FROM Strategy WHERE Name='{}'";
