import React from 'react';
import ReactDOM from 'react-dom';

import {createStore, applyMiddleware} from 'redux'
import {Provider} from 'react-redux'
import thunk from 'redux-thunk'
import {createLogger} from 'redux-logger'

import reducer from './reducers'

import {loadCookie} from "./Cookies";
import QuizChallenge from './containers/QuizChallenge'

const middleware = [thunk]
if (process.env.NODE_ENV !== 'production') {
  middleware.push(createLogger())
}

const store = createStore(
  reducer,
  applyMiddleware(...middleware)
)

/*
<script type="text/javascript">
const conf = {
    host: 'http://localhost:1337',
    collection: 'bb',
    setExpr: '10',
                // '': use challenges
                // "*", "...".split(","), "...".split("-")
                //  "*"
                //  "3-4"
                //  "3-"
                //  "-4"
                //  "2, 4"
                //  "2, 4, 10-17"
    option: {
        timeout: 30,
        sync: false
    }
}
document.cookie = 'conf=' + encodeURIComponent(JSON.stringify(conf)) + '; expires=Thu, 18 Dec 2023 12:00:00 UTC; path=/'
// saveCookie('conf', JSON.stringify(conf))
</script>
 */
const CONF_DEFAULT = {
    host: 'http://localhost:1337',
    collection: 'bb',
    setExpr: '5-10',
                // '': use challenges
                // "*", "...".split(","), "...".split("-")
                //  "*"
                //  "3-4"
                //  "3-"
                //  "-4"
                //  "2, 4"
                //  "2, 4, 10-17"
    option: {
        timeout: 30,
        sync: false
    }
}
const confStr = loadCookie('conf')

ReactDOM.render(
  <Provider store={store}>
    <QuizChallenge conf={confStr === '' ? CONF_DEFAULT : JSON.parse(confStr)}/>
  </Provider>,
  document.getElementById('root')
)
