import {REQUEST_QUIZ_SET, RECEIVE_QUIZ_SET} from "../actions"
import {REQUEST_CHALLENGES, RECEIVE_CHALLENGES} from "../actions"
import {INIT_CHALLENGE, SET_CHALLENGE, SYNC_CHALLENGES} from "../actions"


const INIT_STATE = {
    collection: '',

    isQuizSetFetching: false,
    quizSet: [],
    quizSetStatus: '',

    areChallengesFetching: false,
    challenges: [],
    challengesStatus: '',

    challenge: INIT_CHALLENGE
}

const reducer = (state = INIT_STATE, action) => {
    switch (action.type) {
    case REQUEST_QUIZ_SET:
        return Object.assign({}, state, {
            isQuizSetFetching: true
        });
    case RECEIVE_QUIZ_SET:
        return Object.assign({}, state, {
            isQuizSetFetching: false,
            quizSet: action.quizSet,
            quizSetStatus: action.quizSetStatus
        })
    case REQUEST_CHALLENGES:
        return Object.assign({}, state, {
            areChallengesFetching: true
        })
    case RECEIVE_CHALLENGES:
        return Object.assign({}, state, {
            areChallengesFetching: false,
            challenges: action.challenges,
            challengesStatus: action.challengesStatus
        })
    case SET_CHALLENGE:
        return Object.assign({}, state, {
            challenge: action.challenge
        })
    case SYNC_CHALLENGES:
        return Object.assign({}, state, {
            challenge: action.challenge,
            challenges: state.challenges.map((challenge, index) => {
                if (index === action.answer.index) {
                    let answers = challenge.answers.slice(0, challenge.answers.length)
                    answers.push(action.answer.answer)
                    return Object.assign({}, challenge, {
                        answers: answers
                    })
                } else
                    return challenge
            }).filter((challenge, index) => {
                const answers = challenge.answers
                const len = answers.length
                return !action.clear || len === 0 || answers[len - 1]['result'] === false
            })
        })
    default:
        return state
    }
}

export default reducer
