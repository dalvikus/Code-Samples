import React from 'react';
import ReactDOM from 'react-dom';

import {createStore} from 'redux'
import {Provider} from 'react-redux'
import {combineReducers} from 'redux'

import './index.css';
import App from './App';

const defaultShow = {mid: 'current', title: ''};
const show = (state = defaultShow, action) => {
console.log('reducer: show; ', action);
    switch (action.type) {
    case 'FETCH':
        if (state.mid !== action.show.mid || state.title !== action.show.title) {
            return action.show;
        } else {
            return state;
        }
    default:
        return state;
    }
};
const bookmarks = (state = [], action) => {
    switch (action.type) {
    case 'ADD':
        return [
            ...state,
            {
                id: action.id,
                show: action.show
            }
        ];
    case 'DELETE':
        return state.filter((bookmark) => {return bookmark.id !== action.id});
    default:
        return state;
    }
};

const reducer = combineReducers({
  show,
  bookmarks
})
const store = createStore(reducer)


store.subscribe(() => {
    let state = store.getState();
    console.log('State: ', state);
});

console.log(store.getState());

let bookmark_id = 0;
store.dispatch(
    {
        type: 'ADD',
        id: bookmark_id++,
        show: {mid: 'current', title: 'kbs'}
    }
);
store.dispatch(
    {
        type: 'ADD',
        id: bookmark_id++,
        show: {mid: 'current', title: 'kbs'}
    }
);
store.dispatch(
    {
        type: 'DELETE',
        id: 1,
    }
);
store.dispatch(
    {
        type: 'ADD',
        id: bookmark_id++,
        show: {mid: 'current', title: 'kbs'}
    }
);
store.dispatch(
    {
        type: 'FETCH',
        show: {mid: 'drama', title: '봉순'}
    }
);

console.log(store.getState());

ReactDOM.render(
  <Provider store={store}>
    <App />
  </Provider>,
  document.getElementById('root')
)
