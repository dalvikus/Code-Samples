import {combineReducers} from 'redux'

import {ADD_BOOKMARK, DELETE_BOOKMARK} from "../actions"
import {FETCH_SHOW, INVALIDATE_SHOW, REQUEST_EPISODES, RECEIVE_EPISODES} from "../actions"
import {TOGGLE_HIDE_CLIPS, REQUEST_CLIPS, RECEIVE_CLIPS} from "../actions"

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

/*
Show.toString(): {
    isFetching: Boolean,
    invalidated: Boolean,
    episodes: [
        {
            text: String
            href: String

            areClipsFetching: Boolean,
            hideClips: Boolean
            clips: []
        }
    ],
    pages: []
}
 */
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
            episodes: action.episodes.map((e) => ({
                href: e.href,
                text: e.text,

                areClipsFetching: false,
                hideClips: true,
                clips: []
            })),
            pages: action.pages
        })
    case TOGGLE_HIDE_CLIPS:
        return Object.assign({}, state, {
            episodes: state.episodes.map((episode) => {
                if (episode.href === action.href) {
                    return Object.assign({}, episode, {
                        hideClips: !episode.hideClips
                    })
                }
                return episode
            })
        });
    case REQUEST_CLIPS:
        return Object.assign({}, state, {
            episodes: state.episodes.map((episode) => {
                if (episode.href === action.href) {
                    return Object.assign({}, episode, {
                        areClipsFetching: true
                    })
                }
                return episode
            })
        });
    case RECEIVE_CLIPS:
        return Object.assign({}, state, {
            episodes: state.episodes.map((episode) => {
                if (episode.href === action.href) {
                    return Object.assign({}, episode, {
                        areClipsFetching: false,
                        hideClips: false,
                        clips: action.clips
                    })
                }
                return episode
            })
        });
    default:
        return state
    }
}

const episodesByShow = (state = {}, action) => {
    switch (action.type) {
    case INVALIDATE_SHOW:
    case REQUEST_EPISODES:
    case RECEIVE_EPISODES:
    case TOGGLE_HIDE_CLIPS:
    case REQUEST_CLIPS:
    case RECEIVE_CLIPS:
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
            action.show
        ]
    case DELETE_BOOKMARK:
        return state.filter((bookmark, index) => {return index !== action.index})
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
