# Authenticator
This component handles user management and prevents arbitrary users to access processors they shouldn't.
It works by granting access-tokens, which users can include into their event-headers to reach processors with restricted access.
To get this token, you need to publish an event with the topic "authenticator::login" and your credentials as payload. As response you will get an access token.

The authenticator will setup "guard"-processors for all permissions which are setup and maintained by using the events "authenticator::permissions::(add|delete|get)". Now, if a user publishes an event, the guard-processor handles the event as the first and checks if an access-token is provided in the event-header, and if the token is associated with a user which has the required role to access the guarded processor. Then the event is simply passed back to the susi-core and the guarded processors will proceed processing the event.

If no previous configuration has been done, the following default config will be added:
* user: "root"
* password: "toor"
* role: "admin"
* permissions: "authenticator::(users|permissions)::(.\*)" needs role "admin"

## authenticator::login
This event is used to obtain an access-token. You must specify your username and your password.
As a response you get an access-token and a list of events that need the access-token. You should NOT send your access-token to other events, or they could be captured by another (evil) consumer. The events in this list are guarded by the authenticator. Therefore the authenticator takes care of removing the access-tokens, so they can not be observed by another consumer.

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
If you are logged in, you must logout at the end of your session, in order to cleanup things.
Simply publish an event "authenticator::logout" with your access-token as header.

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
If you want to add a new user, publish an event with the topic "authenticator::users::add" and specify the user as shown below.

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
If you want to delete a new user, publish an event with the topic "authenticator::users::delete" and specify the user by name.

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
Get a list of all users and their roles.

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
If you want to add a new permission, publish an event with the topic "authenticator::permissions::add" and specify what to guard, and who can pass.
To specify what, you must provide a pattern. This pattern is basically the json representation of an event with topic and payload. As usual you can specify the topic as regular expression which is then matched, but you can also specify parts of the payload, which must exist within the event in question. Strings in the pattern payload can also be regular expressions.

As a result, you will get an id for this permission.

As an example, the sample request will guard the events with the topics "guarded-topic-1", "guarded-topic-2" and "guarded-topic-3", but only if the payload contains the field "key" with one of the values "sample-value-a", "sample-value-b" or  "sample-value-c".

With this approach you can for example disallow users from reading arbitrary values from one of the susi-states components (statefile or leveldb) without disallowing every use of this components.

#### Request Structure
```json
{
    "topic":"authenticator::permissions::add",
    "headers": [
        {"User-Token":"abcdefghijklmnopqrstuvwxyz"}
    ],
    "payload": {
        "pattern": {
            "topic": "guarded-topic-(1|2|3)",
            "payload": {
                "key": "sample-value-(a|b|c)"
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
If you want to delete a permission, publish an event with the topic "authenticator::permissions::delete" and specify the id of the permission.
You get this id when you register the permission, or from the permission list obtained via "authenticator::permissions::get".

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
If you want to get a list of all permissions, use this event.

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
