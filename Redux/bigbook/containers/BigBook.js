import React from 'react'
import {connect} from 'react-redux'
import '../index.css'

import {TIMEOUT} from '../actions'
import {fetchBigBook, fetchChallenges, haveChallengeReady, chooseAnswer} from '../actions'

var start
var intervalId = -1

class BigBook extends React.Component {

    componentWillReceiveProps(nextProps) {
        const {dispatch, areChallengesFetching} = this.props
        if (nextProps.areChallengesFetching && areChallengesFetching !== nextProps.areChallengesFetching) {
            dispatch(fetchChallenges())
        }
    }

    componentDidMount() {
        this.props.dispatch(fetchBigBook())
    }

    componentDidUpdate() {
        const {dispatch, isBigBookFetching, bigbook, challenges, challenge} = this.props
        const bar = document.getElementById('bar')
        const confidenceLevels = document.getElementsByName('confidence-level')
        if (!isBigBookFetching && bigbook.length > 0 && challenges.length > 0 && challenge.indexToChallenges >= 0 && challenge.indexToChallenges < challenges.length) {
            start = Date.now()
            let n = 0
            intervalId = window.setInterval(() => {
                if (n < 100 && n % 20 === 0) {
                    confidenceLevels.value = 5 - n / 20
                    confidenceLevels[4 - n / 20].checked = true
                }

                ++n
                if (n <= 100)
                bar.style.width = n + '%'

                if (n === 100) {
                    dispatch(chooseAnswer(bigbook, challenges, challenge, start, intervalId))
                }
            }, TIMEOUT / 100 /* in percents */)
        }
    }

    render() {
        const {dispatch, isBigBookFetching, bigbook, areChallengesFetching, challenges, challenge} = this.props
        const quiz = challenge.quiz
        return (
          <div>
            {isBigBookFetching ? (<h2>fetch bigbook...</h2>) : (
                (bigbook.length > 0 && challenges.length > 0) ?
                (
                challenge.indexToChallenges < 0 ? (
                    <button onClick={(e) => {
                        e.preventDefault()
                        dispatch(haveChallengeReady(bigbook, challenges, 0, null))
                    }}>Start...</button>
                ) : (
                    <div>
                    <div>{quiz.question}</div>
                    {quiz.choices.map((choice, index) =>
                        <div key={index}>
                        <input type='radio' name='choice' value={index}/>
                        {choice}
                        </div>
                    )}
                    <div>
                    <div id="progress">
                      <div id="bar"></div>
                    </div>
                    <div id="confidence">Confidence
                        <input type='radio' name='confidence-level' value='1' />
                        <input type='radio' name='confidence-level' value='2' />
                        <input type='radio' name='confidence-level' value='3' />
                        <input type='radio' name='confidence-level' value='4' />
                        <input type='radio' name='confidence-level' value='5' />
                    </div>
                    <button onClick={(e) => {
                        e.preventDefault()
                        dispatch(chooseAnswer(bigbook, challenges, challenge, start, intervalId))
                        }}>next</button>
                    </div>
                    </div>
                )
                ) : (
                <div>
                <h2># of questions: {bigbook.length}</h2>
                {areChallengesFetching ? (<h2>fetch challenges...</h2>) : (
                    <h2> # of challenges: {challenges.length}</h2>
                )}
                </div>
                )
            )}
          </div>
        )
    }
}

const mapStateToProps = state => {
  const {isBigBookFetching, bigbook, areChallengesFetching, challenges, challenge} = state
  return {
    isBigBookFetching,
    bigbook,
    areChallengesFetching,
    challenges,
    challenge
  }
}

export default connect(mapStateToProps)(BigBook)
