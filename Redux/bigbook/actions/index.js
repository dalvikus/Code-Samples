import axios from 'axios';
export const REQUEST_BIGBOOK = 'REQUEST_BIGBOOK'
export const RECEIVE_BIGBOOK = 'RECEIVE_BIGBOOK'

export const REQUEST_CHALLENGES = 'REQUEST_CHALLENGES'
export const RECEIVE_CHALLENGES = 'RECEIVE_CHALLENGES'

export const NEXT_QUIZ = 'NEXT_QUIZ'
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

export const nextQuiz = (index) => ({
  type: NEXT_QUIZ,
  index
})
export const setChallenge = (index) => ({
  type: SET_CHALLENGE,
  index
})

export const clearChallenge = (index) => ({
  type: CLEAR_CHALLENGE,
  index
})

export const updateAnswer = (index, answer) => ({
  type: UPDATE_ANSWER,
  index,
  answer
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

export const chooseAnswer = (challenges, challenge, start, intervalId) => dispatch => {
console.log(intervalId)
                    window.clearInterval(intervalId)

    console.log(challenge)
    const radios = document.getElementsByName('choose')
    let selected = -1
    for (let i = 0; i < radios.length; ++i) {
        if (radios[i].checked) {
            selected = i
            break
        }
    }
    const end = Date.now()
    console.log(selected)
console.log(start)
console.log(end)
console.log(end - start)
    console.log('selected = ' + selected)
    let index = challenges.map((e) => e.serial).indexOf(challenge.serial)
    console.log(index)
    ++index
    if (index !== challenges.length)
        dispatch(setChallenge(index))

    for (let i = 0; i < radios.length; ++i)
        radios[i].checked = false
    const bar = document.getElementById('bar')
    bar.style.width = '0%'
}
