import {REQUEST_BIGBOOK, RECEIVE_BIGBOOK} from "../actions"
import {REQUEST_CHALLENGES, RECEIVE_CHALLENGES} from "../actions"
import {NEXT_QUIZ, SET_CHALLENGE, CLEAR_CHALLENGE, UPDATE_ANSWER} from "../actions"

import {Answer} from '../Answer'

/*
    {
        bigbook: {
            isFetching: Boolean,
            bigbook: // list of
                     // {
                     //     'serial': 1,
                     //     'note': '1. GRE Big Book Test 1 Section 1 no.1',
                     //     'question': 'Nonviolent demonstrations often create such tensions that a community that has constantly refused to ________ its injustices is forced to correct them: the injustices can no longer be ________.',
                     //     'choices': ['acknowledge ... ignored', 'decrease ... verified', 'tolerate ... accepted', 'address ... eliminated', 'explain ... discussed']
                     //     'answer': 0,
                     // }
                     //
        },
        challenges: // list of challenge
                    //  {
                    //      bigbook_id: ###,
                    //      // list of Answers
                    //  }
                    //

        challenge_id: # current id of challenge
        challenge: # challenge
    }
 */

const reducer =
(
    state = {
        isBigBookFetching: false,
        bigbook: [],

        areChallengesFetching: false,
        challenges: [],

        challenge: {
            'serial': -1,
            'note': '',
            'question': '',
            'choices': [],
            'answer': -1
        }
    },
    action
) => {
    switch (action.type) {
    case REQUEST_BIGBOOK:
        return Object.assign({}, state, {
            isBigBookFetching: true
        });
    case RECEIVE_BIGBOOK:
        return Object.assign({}, state, {
            isBigBookFetching: false,
            bigbook: action.bigbook
        })
    case REQUEST_CHALLENGES:
        return Object.assign({}, state, {
            areChallengesFetching: true
        })
    case RECEIVE_CHALLENGES:
        return Object.assign({}, state, {
            areChallengesFetching: false,
            challenges: action.challenges
        })
    case SET_CHALLENGE:
        if (action.index >= 0 && action.index < state.challenges.length)
            return Object.assign({}, state, {
                challenge: state.bigbook[state.challenges[action.index].serial - 1]
            })
        else
            return state
    case CLEAR_CHALLENGE:
        return Object.assign({}, state, {
            challenges: state.challenges.filter((challenge, index) => {return index !== action.index})
        })
    case UPDATE_ANSWER:
        return Object.assign({}, state, {
            challenges: state.challenges.map((challenge, index) => {
                if (index === action.index) {
                    return Object.assign({}, challenge, {
                        answers: challenge.answers.push(action.answer)
                    })
                } else
                    return challenge
            })
        })
    default:
        return state
    }
}

export default reducer
