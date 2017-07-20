import axios from 'axios'
import {Answer} from '../Answer'

export const FETCH_QUIZ_SET_URL = 'http://localhost:1337/quiz'
export const FETCH_CHALLENGES_URL = 'http://localhost:1337/challenge'
export const SYNC_CHALLENGES_URL = 'http://localhost:1337/sync'

export const INIT_CHALLENGE = {
    indexToChallenges: -1,
    quiz: {
        'serial': 0,
        'question': '',
        'choices': [],
        'answer': -1
    }
}

export const REQUEST_QUIZ_SET = 'REQUEST_QUIZ_SET'
export const RECEIVE_QUIZ_SET = 'RECEIVE_QUIZ_SET'

export const REQUEST_CHALLENGES = 'REQUEST_CHALLENGES'
export const RECEIVE_CHALLENGES = 'RECEIVE_CHALLENGES'

export const SYNC_CHALLENGES = 'SYNC_CHALLENGES'
export const SET_CHALLENGE = 'SET_CHALLENGE'

export const requestQuizSet = (collection) => ({
    type: REQUEST_QUIZ_SET
})

export const receiveQuizSet = (quizSet, quizSetStatus) => ({
    type: RECEIVE_QUIZ_SET,
    quizSet,
    quizSetStatus
})

export const requestChallenges = (collection) => ({
    type: REQUEST_CHALLENGES
})

export const receiveChallenges = (challenges, challengesStatus) => ({
    type: RECEIVE_CHALLENGES,
    challenges,
    challengesStatus
})

export const setChallenge = (challenge) => ({
  type: SET_CHALLENGE,
  challenge
})

export const syncChallenges = (answer, challenge, clear) => ({
  type: SYNC_CHALLENGES,
  answer,
  challenge,
  clear
})

const haveQuizSetReady = (data, collection) => dispatch => {
    const failure =
        data['status'] === undefined ||
        data['status'] !== 'ok' ||
        data['quiz-set'] === undefined
    const quizSet       = failure ? [] : data['quiz-set']
    const quizSetStatus = failure ? 'Oops' : 'Ok'
    dispatch(receiveQuizSet(quizSet, quizSetStatus))
    if (!failure)
        dispatch(requestChallenges(collection))
}

export const fetchQuizSet = (conf) => dispatch => {
    dispatch(requestQuizSet())

    const url = FETCH_QUIZ_SET_URL + '?collection=' + encodeURIComponent(conf.collection)
    const request = axios.get(url)
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status)
        console.error(response.statusText)
        console.error(response.headers)
      } else {
        dispatch(haveQuizSetReady(response.data))
      }
    })
}

const haveChallengesReady = (data) => dispatch => {
    const failure =
        data['status'] === undefined ||
        data['status'] !== 'ok' ||
        data['challenges'] === undefined
    const challenges       = failure ? [] : data['challenges']
    const challengesStatus = failure ? 'Oops' : 'Ok'
    dispatch(receiveChallenges(
        challenges.filter((challenge) => {
            const answers = challenge.answers
            const len = answers.length
            return len === 0 || answers[len - 1]['result'] === false
        })
        , challengesStatus))
}

export const fetchChallenges = (conf, quizSet) => dispatch => {
/*
    let challenges = [
        {'serial': 377, 'answers': []},
        {'serial': 2, 'answers': []},
        {'serial': 378, 'answers': []},
        {'serial': 37, 'answers': []},
        {'serial': 22, 'answers': []},
        {'serial': 78, 'answers': []},
    ]
    dispatch(haveChallengesReady({'status': 'ok', 'challenges': challenges}))
 */
  const setExpr = conf.setExpr.trim()
  if (setExpr === '') {
    const url = FETCH_CHALLENGES_URL + '?collection=' + encodeURIComponent(conf.collection)
    const request = axios.get(url)
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status)
        console.error(response.statusText)
        console.error(response.headers)
      } else {
        dispatch(haveChallengesReady(response.data))
      }
    })
  } else {
    const serials = quizSet.map(e => e.serial).sort((a, b) => {return a - b})
    let serialsSet = new Set()
    if (setExpr === '*') {
        serialsSet = new Set(serials)
    } else {
        setExpr.split(',').forEach((e) => {
            const a = e.trim().split('-').map(e => e.trim())
            if (a.length > 2) {
                console.error('"' + e + '": invalid range expression')
            } else {
                if (a.length === 2) {
                    if (a[0] === '' && a[1] === '') {
                        for (let i = 0; i < serials.length; ++i)
                            serialsSet.add(serials[i])
                    } else if (a[0] === '') {
                        const n = parseInt(a[1], 10)
                        if (isNaN(n)) {
                            console.error('"' + a[1] + '": not a number')
                        } else {
                            const i2 = serials.indexOf(n)
                            for (let i = 0; i <= i2; ++i)
                                serialsSet.add(serials[i])
                        }
                    } else if (a[1] === '') {
                        const n = parseInt(a[0], 10)
                        if (isNaN(n)) {
                            console.error('"' + a[0] + '": not a number')
                        } else {
                            const i1 = serials.indexOf(n)
                            if (i1 >= 0) {
                                for (let i = i1; i < serials.length; ++i)
                                    serialsSet.add(serials[i])
                            }
                        }
                    } else {
                        const n1 = parseInt(a[0], 10)
                        if (isNaN(n1)) {
                            console.error('"' + a[0] + '": not a number')
                        } else {
                            const i1 = serials.indexOf(n1)
                            if (i1 >= 0) {
                                const n2 = parseInt(a[1], 10)
                                if (isNaN(n2)) {
                                    console.error('"' + a[1] + '": not a number')
                                } else {
                                    const i2 = serials.indexOf(n2)
                                    for (let i = i1; i <= i2; ++i)
                                        serialsSet.add(serials[i])
                                }
                            }
                        }
                    }
                } else {
                    const n = parseInt(a[0], 10)
                    if (isNaN(n)) {
                        console.error('"' + a[0] + '": not a number')
                    } else {
                        serialsSet.add(n)
                    }
                }
            }
        })
    }
    // serialsSet
    let challenges = []
    serialsSet.forEach(serial => {
        challenges.push({'serial': serial, 'answers': []})
    })
    dispatch(haveChallengesReady({'status': 'ok', 'challenges': challenges}))
  }
}

// The maximum is exclusive and the minimum is inclusive
function getRandomInt(min, max) {
  min = Math.ceil(min)
  max = Math.floor(max)
  return Math.floor(Math.random() * (max - min)) + min
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

export const haveChallengeReady = (quizSet, challenges, indexToChallenges, answer) => dispatch => {
    if (challenges.length === 0)
        throw new Error('no challenges')
    if (indexToChallenges < 0 || indexToChallenges >= challenges.length)
        throw new RangeError('indexToChallenges(' + indexToChallenges + '): out of range; # of challenges = ' + challenges.length)

    let quiz = quizSet[challenges[indexToChallenges].serial - 1]
////console.log(quiz.choices[quiz.answer])

    let a = quiz.choices.map((e, index) => index)
    a = shuffle(a)
    let choices = []
    for (let i = 0; i < a.length; ++i)
        choices.push(quiz.choices[a[i]])
    let ai = a.indexOf(quiz.answer)
////console.log(choices[ai])

    let challenge = {
        indexToChallenges,
        quiz: {
            serial: quiz.serial,
            question: quiz.question,
            choices: choices,
            answer: ai
        }
    }
    if (answer === null)
        dispatch(setChallenge(challenge))
    else
        dispatch(syncChallenges(answer, challenge, false))
}

export const chooseAnswer = (conf, quizSet, challenges, challenge, start, intervalId) => dispatch => {
    document.getElementById('bar').style.width = '0%'

    window.clearInterval(intervalId)

    const end = Date.now()

    const confidenceLevels = document.getElementsByName('confidence-level')
    let confidenceLevel = 5
    for (let i = 0; i < 5; ++i) {
        if (confidenceLevels[i].checked) {
            confidenceLevels[i].checked = false
            confidenceLevel = 1 + i
            break
        }
    }

    const choices = document.getElementsByName('choice')
    let selected = -1
    for (let i = 0; i < choices.length; ++i) {
        if (choices[i].checked) {
            choices[i].checked = false
            selected = i
            break
        }
    }
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
        answer: new Answer(conf.option.timeout /* in seconds */, start, end, result, selectedAnswer, confidenceLevel)
    }

    ++indexToChallenges
    if (indexToChallenges !== challenges.length) {
        dispatch(haveChallengeReady(quizSet, challenges, indexToChallenges, answer))
    } else {
        if (conf.option.sync)
            dispatch(haveSyncChallengesReady(conf.collection, challenges, answer))
        dispatch(syncChallenges(answer, INIT_CHALLENGE, true))
    }
}

export const haveSyncChallengesReady = (collection, challenges, answer) => dispatch => {
    const updatedChallenges = challenges.map((challenge, index) => {
        if (index === answer.index) {
            let answers = challenge.answers.slice(0, challenge.answers.length)
            answers.push(answer.answer)
            return Object.assign({}, challenge, {
                answers: answers
            })
        } else
            return challenge
    })

    const headers = new Headers({
        'Content-Type': 'application/json'
    })
    const init = {
        method: 'POST',
        headers: headers,
        body: JSON.stringify(updatedChallenges),
    }
    const url = SYNC_CHALLENGES_URL + '?collection=' + encodeURIComponent(collection)
    const request = new Request(url, init)
    fetch(request).then((resonse) => {
        return resonse.json()
    }).then((json) => {
        const failure = json['status'] === undefined || json['status'] !== 'ok'
        if (failure)
            console.error('ERR: failed to sync')
        else
            console.log('INFO: sync done successfully')
    }).catch((error) => {
        console.error('ERR: ' + error.message)
    })
/*
    const request = axios.post(SYNC_CHALLENGES_URL, challenges)
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status)
        console.error(response.statusText)
        console.error(response.headers)
      } else {
        dispatch(syncChallenges(answer, INIT_CHALLENGE, true))
      }
    })
 */
}
