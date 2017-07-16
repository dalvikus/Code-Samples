import React from 'react'
import ReactDOM from 'react-dom'

import {createStore, applyMiddleware} from 'redux'
import {Provider} from 'react-redux'
import thunk from 'redux-thunk'
import {createLogger} from 'redux-logger'

import reducer from './reducers'

import BayKoreans from './components/BayKoreans'
import {addBookmark, deleteBookmark, fetchShow} from './actions'

import {Show} from './Show'

const middleware = [ thunk ]
if (process.env.NODE_ENV !== 'production') {
  middleware.push(createLogger())
}
/*
store.subscribe(() => {
    let state = store.getState()
    console.log('store.subscribe: State: ', state)
})
 */


const store = createStore(
  reducer,
  applyMiddleware(...middleware)
)

store.dispatch(addBookmark(new Show('current', 'kbs')));
store.dispatch(addBookmark(new Show('current', 'kbs')));
store.dispatch(deleteBookmark(1));
store.dispatch(addBookmark(new Show('current', '이야기 Y')));
store.dispatch(fetchShow(new Show('drama', '봉순')));

ReactDOM.render(
  <Provider store={store}>
    <BayKoreans/>
  </Provider>,
  document.getElementById('root')
)
