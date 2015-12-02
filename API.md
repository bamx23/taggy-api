# Описание API

## Краткое описание

Сервис предоставляет доступ к истории курса валют. У модераторов есть возможность обновлять курсы в течении текущего часа. Историю за прошедшие часы менять нельзя. Получать историю можно с интервалом не короче часа. Есть ограничение на максимальное число записей за запрос.

## Ресурсы API

Ресурсом API является список курсов валют в определенное время. Такой список может быть представлен следующей структурой:
```
{
    "currency": [
        {
            "name": "BYR",
            "value": 15700.0
        },
        {
            "name": "RUB",
            "value": 65.0
        }
    ],
    "time": "20151025T183142+03"
}
```

Данный ресурс может передаваться как одиночно, так и списком.

## Запросы

#### GET /api/v1/currency

Текущие курсы валют.

```
{
    "currency": [
        {
            "name": "BYR",
            "value": 15700.0
        },
        {
            "name": "RUB",
            "value": 65.0
        }
    ],
    "id": "20151025T000300",
    "time": "20151025T000300+03",
    "updated": "20151025T190300+03"
}
```

#### GET /api/v1/history/

```
{
    "final": false,
    "id": "20151026",
    "list": [
        {
            "currency": [
                {
                    "name": "BYR",
                    "value": 15700.0
                },
                {
                    "name": "RUB",
                    "value": 65.0
                }
            ],
            "time": "20151025T000300+03"
        },
        {
            "currency": [
                {
                    "name": "BYR",
                    "value": 15700.0
                },
                {
                    "name": "RUB",
                    "value": 65.0
                }
            ],
            "time": "20151025T010300+03"
        }
    ],
    "next": "http://api.taggy.by/api/v1/history/20151025",
    "self": "http://api.taggy.by/api/v1/history/20151026",
    "updated": "20151026T230300+03"
}
```

#### GET /api/v1/history/20151025

```
{
    "final": true,
    "id": "20151025",
    "list": [
        {
            "currency": [
                {
                    "name": "BYR",
                    "value": 15700.0
                },
                {
                    "name": "RUB",
                    "value": 65.0
                }
            ],
            "time": "20151025T000300+03"
        },
        {
            "currency": [
                {
                    "name": "BYR",
                    "value": 15700.0
                },
                {
                    "name": "RUB",
                    "value": 65.0
                }
            ],
            "time": "20151025T010300+03"
        }
    ],
    "next": "http://api.taggy.by/api/v1/history/20151024",
    "prev": "http://api.taggy.by/api/v1/history/20151026",
    "self": "http://api.taggy.by/api/v1/history/20151025",
    "updated": "20151025T190300+03"
}
```

## Internal API

Запросы ниже отделяются на уровне NGINX от публичного API, описанного выше.
Данное API предназначено роботов, предоставляющих курсы валют в систему.

#### POST /internal-api/v1/latest

*Request:*

```
{
    "currency": [
        {
            "name": "BYR",
            "value": 15700.0
        },
        {
            "name": "RUB",
            "value": 65.0
        }
    ]
}
```

*Response:*

```
{
    "status": "ERROR",
    "message": "Something went wrong"
}
```

## Замечания

* Формат дат может быть изменен