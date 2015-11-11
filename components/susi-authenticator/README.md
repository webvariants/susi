# Authenticator
This component handles user/role management and prevents arbitary users to reach processors they shouldn't.

## authenticator::login
login a user
#### Request Structure
```json
{
    "topic":"authenticator::login",
    "payload": {
        "username": "test",
        "password": "test"
    }
}
```
#### Response Structure
```json
{
    "topic":"authenticator::login",
    "payload": {
        "token": "abcdefghijklmnopqrstuvwxyz",
        "topics": ["guarded-topic-1","guarded-topic-2"]
    }
}
```

## authenticator::logout
logout a logged-in user
#### Request Structure
```json
{
    "topic":"authenticator::logout",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ]
}
```
#### Response Structure
```json
{
    "topic":"authenticator::logout",
    "payload": true
}
```

## authenticator::users::add
add a new user, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::users::add",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ],
    "payload": {
        "username": "test",
        "password": "test",
        "roles": ["role1","role2"]
    }
}
```
#### Response Structure
```json
{
    "topic":"authenticator::users::add",
    "payload": true
}
```

## authenticator::users::delete
delete a user, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::users::delete",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ],
    "payload": {
        "username": "test",
    }
}
```
#### Response Structure
```json
{
    "topic":"authenticator::users::delete",
    "payload": true
}
```

## authenticator::users::get
get the user-list, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::users::get",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ]
}
```
#### Response Structure
```json
{
    "topic":"authenticator::users::get",
    "payload": [
        {
            "username": "root",
            "roles": ["admin"]
        },{
            "username": "test",
            "roles": ["role1", "role2"]
        }
    ]
}

```

## authenticator::permissions::add
add a new permission, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::permissions::add",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ],
    "payload": {
        "pattern": {
            "topic": "guarded-topic",
            "payload": {
                "sample-key": "sample-value"
            }
        },
        "roles": ["role1","role2"]
    }
}
```
#### Response Structure
```json
{
    "topic":"authenticator::permissions::add",
    "payload": {
        "id": "abcdefg123"
    }
}
```

## authenticator::permissions::delete
delete a permission, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::permissions::delete",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ],
    "payload": {
        "id": "abcdefg123",
    }
}
```
#### Response Structure
```json
{
    "topic":"authenticator::permissions::delete",
    "payload": true
}
```

## authenticator::permissions::get
get the user-list, requires admin role in default setup
#### Request Structure
```json
{
    "topic":"authenticator::permissions::get",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ]
}
```
#### Response Structure
```json
{
    "topic":"authenticator::permissions::get",
    "payload": {
        "abcdefg123": {
            "pattern": {
                "topic": "guarded-topic",
                "payload": {
                    "sample-key": "sample-value"
                }
            },
            "roles": ["role1","role2"]
        }
    }
}
```
