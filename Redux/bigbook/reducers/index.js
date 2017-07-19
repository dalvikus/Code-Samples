import {REQUEST_QUIZ_SET, RECEIVE_QUIZ_SET} from "../actions"
import {REQUEST_CHALLENGES, RECEIVE_CHALLENGES} from "../actions"
import {INIT_CHALLENGE, SET_CHALLENGE, SYNC_CHALLENGES} from "../actions"

/*
    {
        quizSet: {
            isFetching: Boolean,
            quizSet: // list of
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
