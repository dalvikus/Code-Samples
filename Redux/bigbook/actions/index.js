import axios from 'axios'
import {Answer} from '../Answer'

export const TIMEOUT = 30000    // in milliseconds; multiple of 1000

export const INIT_CHALLENGE = {
    indexToChallenges: -1,
    quiz: {
        'question': '',
        'choices': [],
        'answer': -1
    }
}

export const REQUEST_BIGBOOK = 'REQUEST_BIGBOOK'
export const RECEIVE_BIGBOOK = 'RECEIVE_BIGBOOK'

export const REQUEST_CHALLENGES = 'REQUEST_CHALLENGES'
export const RECEIVE_CHALLENGES = 'RECEIVE_CHALLENGES'

export const UPDATE_ANSWER = 'UPDATE_ANSWER'
export const SET_CHALLENGE = 'SET_CHALLENGE'
export const CLEAR_CHALLENGE = 'CLEAR_CHALLENGE'

export const requestBigBook = () => ({
  type: REQUEST_BIGBOOK
})

export const receiveBigBook = (bigbook) => ({
  type: RECEIVE_BIGBOOK,
  bigbook
})

export const requestChallenges = () => ({
  type: REQUEST_CHALLENGES
})

export const receiveChallenges = (challenges) => ({
  type: RECEIVE_CHALLENGES,
  challenges
})

export const setChallenge = (challenge) => ({
  type: SET_CHALLENGE,
  challenge
})

export const clearChallenge = (index) => ({
  type: CLEAR_CHALLENGE,
  index
})

export const updateAnswer = (answer, challenge) => ({
  type: UPDATE_ANSWER,
  answer,
  challenge
})

export const fetchBigBook = (url) => dispatch => {
    dispatch(requestBigBook())

    const request = axios.get(url);
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status);
        console.error(response.statusText);
        console.error(response.headers);
      } else {
        dispatch(receiveBigBook(response.data))
        dispatch(requestChallenges())
      }
    });
/*
console.log(url)
    fetch(url, {mode: 'no-cors'})
    .then(function(response) {
console.log(response)
console.log(response.statue)
            return response.blob();
    })
    .then(function(bigbook_json) {
        dispatch(receiveBigBook(bigbook_json))
    })
.catch(function(error) {
  console.log('There has been a problem with your fetch operation: ' + error.message);
})
 */
}

export const fetchChallenges = () => dispatch => {
    let challenges = [
        {'serial': 377, 'answers': []},
        {'serial': 2, 'answers': []},
        {'serial': 378, 'answers': []},
        {'serial': 37, 'answers': []},
        {'serial': 22, 'answers': []},
        {'serial': 78, 'answers': []},
    ]
    dispatch(receiveChallenges(challenges))
/*
    const request = axios.get(url);
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status);
        console.error(response.statusText);
        console.error(response.headers);
      } else {
        dispatch(receiveBigBook(response.data))
        dispatch(requestChallenges())
      }
    });
 */
}

// The maximum is exclusive and the minimum is inclusive
function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min)) + min;
}

// a[]: 0, 1, ..., a.length - 1
function shuffle(a) {
    const len = a.length
    let k = 0
    while (k + 1 < len) {
        let i = getRandomInt(k, len)
        // swap a[k] and a[i]
        if (i !== k) {
            let ii = a[k]
            a[k] = a[i]
            a[i] = ii
        }
        // next
        ++k
    }
    return a
}

export const haveChallengeReady = (bigbook, challenges, indexToChallenges, answer) => dispatch => {
    if (challenges.length === 0)
        throw new Error('no challenges')
    if (indexToChallenges < 0 || indexToChallenges >= challenges.length)
        throw new RangeError('indexToChallenges(' + indexToChallenges + '): out of range; # of challenges = ' + challenges.length)

    let quiz = bigbook[challenges[indexToChallenges].serial - 1]
    console.log(quiz.choices[quiz.answer])

    let a = quiz.choices.map((e, index) => index)
    a = shuffle(a)
    let choices = []
    for (let i = 0; i < a.length; ++i)
        choices.push(quiz.choices[a[i]])
    let ai = a.indexOf(quiz.answer)
    console.log(choices[ai])

    let challenge = {
        indexToChallenges,
        quiz: {
            question: quiz.question,
            choices: choices,
            answer: ai
        }
    }
    if (answer === null)
        dispatch(setChallenge(challenge))
    else
        dispatch(updateAnswer(answer, challenge))
}

export const chooseAnswer = (bigbook, challenges, challenge, start, intervalId) => dispatch => {
    document.getElementById('bar').style.width = '0%'

    window.clearInterval(intervalId)

    const end = Date.now()
    console.log(end - start)

    const confidenceLevels = document.getElementsByName('confidence-level')
    let confidenceLevel = 5
    for (let i = 0; i < 5; ++i) {
        if (confidenceLevels[i].checked) {
            confidenceLevels[i].checked = false
            confidenceLevel = 1 + i
            break
        }
    }
    console.log('confidenceLevel = ' + confidenceLevel)

    const choices = document.getElementsByName('choice')
    let selected = -1
    for (let i = 0; i < choices.length; ++i) {
        if (choices[i].checked) {
            choices[i].checked = false
            selected = i
            break
        }
    }
//  console.log('selected = ' + selected)
    let quiz = challenge.quiz
    let selectedAnswer = ''
    if (selected !== -1)
        selectedAnswer = quiz.choices[selected]
    console.log('selected answer: ', selectedAnswer)
    console.log('correct answer: ', quiz.choices[quiz.answer])
    let result = selected !== -1 && selected === quiz.answer
    console.log('result? ' + result)

    let indexToChallenges = challenge.indexToChallenges
    let answer = {
        index: indexToChallenges,
        answer: new Answer(TIMEOUT / 1000 /* in seconds */, start, end, result, selectedAnswer, confidenceLevel)
    }

    ++indexToChallenges
    if (indexToChallenges !== challenges.length) {
        dispatch(haveChallengeReady(bigbook, challenges, indexToChallenges, answer))
    } else {
        dispatch(updateAnswer(answer, INIT_CHALLENGE))
    }
}
