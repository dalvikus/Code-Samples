import React from 'react';
import ReactDOM from 'react-dom';

import {createStore, applyMiddleware} from 'redux'
import {Provider} from 'react-redux'
import thunk from 'redux-thunk'
import {createLogger} from 'redux-logger'

import reducer from './reducers'

import BigBook from './containers/BigBook';

const middleware = [thunk]
if (process.env.NODE_ENV !== 'production') {
  middleware.push(createLogger())
}

const store = createStore(
  reducer,
  applyMiddleware(...middleware)
)

ReactDOM.render(
  <Provider store={store}>
    <BigBook/>
  </Provider>,
  document.getElementById('root')
)
