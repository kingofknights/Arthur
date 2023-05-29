# Arthur [Trading App]

This document provide the structural idea, how any `request` and `response` are implemented.
`request` represent any packet sent from application `Arthur` sent to connected backend engine.
`response` represent following fields `Arthur` is expecting from backend engine

```cpp

#define JSON_PARAMS "params"
#define JSON_ID "id"
#define JSON_TOKEN "token"
#define JSON_PRICE "price"
#define JSON_QUANTITY "quantity"
#define JSON_CLIENT "client"
#define JSON_SIDE "side"
#define JSON_ORDER_ID "order_id"
#define JSON_ARGUMENTS "arguments"
#define JSON_PF_NUMBER "pf"
#define JSON_UNIQUE_ID "unique_id"
#define JSON_STRATEGY_NAME "name"
#define JSON_ORDER_TYPE "type"
#define JSON_TIME "time"
#define JSON_FILL_QUANTITY "fill_quantity"
#define JSON_FILL_PRICE "fill_price"
#define JSON_REMAINING "remaining"
#define JSON_MESSAGE "message"

enum SideType {
    Side_BUY = 0,
	Side_SELL
};

enum OrderType {
    OrderType_LIMIT = 0,
	OrderType_MARKET,
	OrderType_IOC,
	OrderType_SPREAD
};

enum RequestType {
    RequestType_LOGIN = 0,
	// ORDER
	RequestType_NEW,
	RequestType_MODIFY,
	RequestType_CANCEL,
	// STRATEGY
	RequestType_APPLY,
	RequestType_SUBSCRIBE,
	RequestType_UNSUBSCRIBE,
	RequestType_SUBSCRIBE_APPLY,
};

enum ResponseType {
    // ORDER
	ResponseType_PLACED = 0,
	ResponseType_NEW,
	ResponseType_REPLACED,
	ResponseType_CANCELLED,
	ResponseType_NEW_REJECT,
	ResponseType_REPLACE_REJECT,
	ResponseType_CANCEL_REJECT,
	ResponseType_FILLED,

    // STRATEGY
	ResponseType_PENDING,
	ResponseType_SUBCRIBED,
	ResponseType_APPLIED,
	ResponseType_UNSUBSCRIBED,
	ResponseType_TERMINATED,
	ResponseType_UPDATES,
	ResponseType_EXCHANGE_DISCONNECT,

    // TRACKER
	ResponseType_TRACKER,
};

using RequestInPackT = struct RequestInPackT {
    short				  TotalSize;
	uint8_t				  Type; // RequestType or ResponseType 
	uint64_t			  UserIdentifier;
	int				  CompressedMsgLen;
	std::array<char, 512> 	          Message;
};

xcdCompress
xcdDecompress

```

Note : Id in json just respentation of increamental serial number that is transfered to-and-fro
Note : Time should be represent in string format nano second
Note : Price should be decimal format 100.50 not 10050
- - -

### New Order Request

```json
{
  "id": 21,
  "params": {
    "token": 500112,
	"price": 350.45,
	"quantity": 2,
	"client": "pro",
	"side": SideType,
	"type": OrderType
  }
}
```

##### Example

```text
{"id":1,"params":{"client":"Pro","price":"1755296","quantity":50,"side":0,"token":35019,"type":0}}
{"id":2,"params":{"client":"ClientCode1","price":"1755296","quantity":50,"side":0,"token":35019,"type":0}}
```

- - - 

### Modify Order Request

 ```json
{
  "id": 23,
  "params": {
    "price": 355.45,
	"quantity": 3,
	"order_id": 1000000000011,
	"unique_id": "unique_identifier"
  }
}
```

##### Example

```text
{"id":1,"params":{"order_id":1624,"price":"1735049","quantity":7,"unique_id":5}}
{"id":2,"params":{"order_id":52,"price":"1732572","quantity":19,"unique_id":4}}
{"id":3,"params":{"order_id":4979,"price":"1731905","quantity":97,"unique_id":3}}
{"id":4,"params":{"order_id":7208,"price":"1734476","quantity":8,"unique_id":2}}
```

---

### Cancel Order Request

```json
{
  "id": 23,
  "params": {
    "order_id": 1000000000011,
	"unique_id": "unique_identifier"
  }
}
```

##### Example

```text
{"id":5,"params":{"order_id":679,"unique_id":6}}
{"id":6,"params":{"order_id":1624,"unique_id":5}}
{"id":7,"params":{"order_id":52,"unique_id":4}}
{"id":8,"params":{"order_id":4979,"unique_id":3}}
{"id":9,"params":{"order_id":7208,"unique_id":2}}
{"id":10,"params":{"order_id":210,"unique_id":1}}
```

---

---

## Strategy Request

## Subscribe and Apply

```json
{
  "id": 24,
  "params": {
    "PF": 1,
	"name": "BUTTERFLY",
	"arguments": {
	  "token": 500112,
	  "side": "BUY",
	  "other": "such variable"
	}
  }
}
```

#### Example

```text
{"id":11,"params":{"arguments":{"Lot":"0","Side1":"BUY","Side2":"BUY","Side3":"BUY","Token1":36694,"Token2":36690,"Token3":36691,"TotalLot":"10","UserGap":"100"},"name":"ButterFly","pf":1}}
```

---

## Unsubscribe

```json
{
  "id": 2,
  "params": {
    "name": "ButterFly",
	"pf": 1
  }
} 
```

---

## Response if parameter display value changes 

```json
{
  "id": 25,
  "params": {
    "pf": 1,
	"arguments": {
	  "token": 500112,
	  "side": "BUY",
	  "other": "such variable"
	}
  }
}

```

## Response if strategy status changes

```json
{
  "id": 25,
  "params": {
    "pf": 1
  }
}
```

## Response if strategy tracker is changes

```json
{
  "id": 25,
  "params": {
    "pf": 1,
	"message": "String Message"
  }
}
```

---
# Response

### Order Response
```json
{
    "id" : 1,
	"params" : {
	    "pf" : "",
		"unique_id" : "",
		"token" : "",
		"quantity" : "",
		"fill_quantity" : "",
		"remaining" : "",
		"order_id" : "",
		"price" : "",
		"fill_price" : "",
		"side" : "",
		"time" : "",
		"client" : "",
		"message" : "",
	}
}
```

## Features to be implemented

1. Pending Book Cancel All
2. Square off button in Symbol wise window
3. [RSS Feed](https://github.com/libcpr/cpr)
4. [Twitter API](https://github.com/a-n-t-h-o-n-y/Twitter-API-C-Library)
5. Custom Readable data format

