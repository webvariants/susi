susi-mongodb
============

Description how to use the susi-mongodb component.<br>
This readme file explain the CRUD operation with Javascript and a mongodb as database.

---

### Create

```javascript
susi.publish({
		topic: 'mongodb::find',
		payload: {
			collection: 'susi',
			query: {
				bla: 'foobar'
			}
		}
	}, function(event) {
		console.log('Response: ', event);
	});
```

To create a document inside the mongodb, you must publish a event with the topic **mongodb::find**.<br>
Define as payload the **collection** in which the data should be saved and a **query** with the key:value pairs with the data.<br>
As response you receive in the payload the saved data with a ID which the database has created.

---

### Read

```javascript
    susi.publish({
        topic: 'mongodb::find',
        payload: {
            collection: 'susi',
            query: {
                bla: 'foobar'
            }
        }
    }, function(event) {
        console.log('Response: ', event);
    });
```

You can read your data from the database by using **mongodb::find** as topic.<br>
The Payload must have the **collection** key with the value and a **query** with the conditions to search or a empty query to read all data out of the database collection.

---

### Update

```javascript
    susi.publish({
		topic: 'mongodb::update',
		payload: {
			collection: 'susi',
			findQuery: {
				bla: 'foobar'
			},
			updateQuery: {
				$set: {
                    foo: 'bar'
                }
			}
		}
	}, function(event) {
		console.log('Response: ', event);
	});
```

When updating a document you need the value **mongodb::update** as topic key and the **collection** name.<br>
The rest of the payload must contain the the **findQuery** and the **updateQuery** keys with the conditions/values to search and update for.

---

### Delete

```javascript
    susi.publish({
		topic: 'mongodb::remove',
		payload: {
			collection: 'susi',
			removeQuery: {
				foo: 'bar'
			}
		}
	}, function(event) {
		console.log('Response: ', event);
	});
```

For deleting a document in the database you set the topic **mongodb::remove** and the **collection** name.<br>
The **removeQuery** contains the condition which documents should be removed.

---
