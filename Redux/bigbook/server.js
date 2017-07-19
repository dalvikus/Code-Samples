const port = 1337;
const express = require('express');
const app = express();
const MongoClient = require('mongodb').MongoClient

const bodyParser = require('body-parser')
app.use(bodyParser.json());       // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({     // to support URL-encoded bodies
  extended: true
})); 

app.use('/quiz', (req, res) => {
    console.log('/quiz')
    console.log(req.query)
    res.set('Access-Control-Allow-Origin', '*')
    const collection = req.query['collection']
    if (collection === undefined) {
        res.json({'status': 'oops', 'msg': 'no collection'})
    } else {
    MongoClient.connect(
//      'mongodb://localhost:27017/gre',
        'mongodb://bb:bb@ds127391.mlab.com:27391/gre',
        (err, db) => {
            if (err) throw err

            db.collection(collection).find().toArray(
                (err, result) => {
                    if (err) throw err

                    res.json({'status': 'ok', 'quiz-set': result})
                }
            )
        }
    )
    }
});

app.options("/*", (req, res, next) => {
    console.log('options: /*')
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,OPTIONS');
    res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization, Content-Length, X-Requested-With');
    res.sendStatus(200);
})

app.post('/sync', (req, res) => {
    console.log('/sync')
    console.log(req.query)
    res.set('Access-Control-Allow-Origin', '*')
    const collection = req.query['collection']
    if (collection === undefined) {
        res.json({'status': 'oops', 'msg': 'no collection'})
    } else {
    const challengeCollection = collection + '_challenges'
    MongoClient.connect(
//      'mongodb://localhost:27017/gre',
        'mongodb://bb:bb@ds127391.mlab.com:27391/gre',
        (err, db) => {
            if (err) throw err

            db.collection(challengeCollection).insertOne(
                {
                    date: Date.now(),
                    challenges: req.body
                },
                (err, result) => {
                    if (err) throw err

                    res.json({'status': 'ok'})
                }
            )
        }
    )
    }
});

app.use('/challenge', (req, res) => {
    console.log('/challenge')
    console.log(req.query)
    res.set('Access-Control-Allow-Origin', '*')
    const collection = req.query['collection']
    if (collection === undefined) {
        res.json({'status': 'oops', 'msg': 'no collection'})
    } else {
    const challengeCollection = collection + '_challenges'
    MongoClient.connect(
//      'mongodb://localhost:27017/gre',
        'mongodb://bb:bb@ds127391.mlab.com:27391/gre',
        (err, db) => {
            if (err) throw err

            db.collection(challengeCollection).find({}, {'_id': 0, 'date': 0}).sort({'date': -1}).limit(1).toArray(
                (err, result) => {
                    if (err) throw err

                    res.json({'status': 'ok', 'challenges': (result.length === 0 ? [] : result[0]['challenges'])})
                }
            )
        }
    )
    }
});

app.use(
    '/about',
    (req, res) => {
        setTimeout(
            () => {
                res.send('hello, world')
            },
            3000
        )
    }
)

app.listen(port);

console.log('Server running at http://localhost:' + port);
