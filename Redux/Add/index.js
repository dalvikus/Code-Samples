import React from 'react'
import ReactDOM from 'react-dom'

import {createStore} from 'redux'
import {Provider} from 'react-redux'
import {combineReducers} from 'redux'

import Add from './Add'

const texts = (state = [], action) => {
    switch (action.type) {
    case 'ADD_TEXT':
        return [
            action.text,
            ...state,
        ]
    default:
        return state
    }
}

const reducer = combineReducers({
    texts,
})
const store = createStore(reducer)

store.subscribe(() => {
    console.log('State: ', store.getState())
})

ReactDOM.render(
  <Provider store={store}>
    <Add/>
  </Provider>,
  document.getElementById('root')
)
