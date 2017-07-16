import {combineReducers} from 'redux'

import {ADD_BOOKMARK, DELETE_BOOKMARK} from "../actions"
import {FETCH_SHOW, INVALIDATE_SHOW, REQUEST_EPISODES, RECEIVE_EPISODES} from "../actions"

import {Show} from '../Show'

const defaultShow = new Show('current', '')
const fetchedShow = (state = defaultShow, action) => {
    switch (action.type) {
    case FETCH_SHOW:
        if (state.mid !== action.show.mid || state.title !== action.show.title) {
            return action.show
        } else {
            return state
        }
    default:
        return state
    }
}

function showInfo(
    state = {
        isFetching: false,
        invalidated: false,
        episodes: [],
        pages: []
    },
    action
) {
    switch (action.type) {
    case INVALIDATE_SHOW:
        return Object.assign({}, state, {
            invalidated: true
        })
    case REQUEST_EPISODES:
        return Object.assign({}, state, {
            isFetching: true,
            invalidated: false
        })
    case RECEIVE_EPISODES:
        return Object.assign({}, state, {
            isFetching: false,
            invalidated: false,
            episodes: action.episodes,
            pages: action.pages
        })
    default:
        return state
    }
}

const episodesByShow = (state = {}, action) => {
    switch (action.type) {
    case INVALIDATE_SHOW:
    case REQUEST_EPISODES:
    case RECEIVE_EPISODES:
        return Object.assign({}, state, {
            [action.show.toString()]: showInfo(state[action.show.toString()], action)
        })
    default:
        return state;
    }
}

const bookmarks = (state = [], action) => {
    switch (action.type) {
    case ADD_BOOKMARK:
        return [
            ...state,
            {
                id: action.id,
                show: action.show
            }
        ]
    case DELETE_BOOKMARK:
        return state.filter((bookmark) => {return bookmark.id !== action.id})
    default:
        return state
    }
}

const reducer = combineReducers({
    fetchedShow,
    bookmarks,
    episodesByShow,
})

export default reducer
