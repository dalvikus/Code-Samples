import React from 'react'
import {connect} from 'react-redux'
import '../index.css'

import {fetchImage, fetchQuizSet, fetchChallenges, haveChallengeReady, chooseAnswer} from '../actions'

var start
var intervalId = -1

const SetData = ({conf, isQuizSetFetching, quizSet, quizSetStatus, areChallengesFetching, challenges, challengesStatus}) => {
  return (
    <div>
    <h1>Collection: {conf.collection}</h1>
    {isQuizSetFetching
      ? (
          <h2>fetch quiz set...</h2>
        )
      : (
          <div>
          <h2># of questions: {quizSet.length} ({quizSetStatus})</h2>
          {
            quizSetStatus === 'Ok'
              ? (
                  areChallengesFetching
                    ? (
                        <h2>fetch challenges...</h2>
                      )
                    : (
                        <h2> # of challenges: {challenges.length} ({challengesStatus})</h2>
                      )
                )
              : (null)
          }
          </div>
        )
    }
    </div>
  )
}

const Challenge = ({dispatch, conf, quizSet, challenges, challenge}) => {
    const quiz = challenge.quiz
    return (
      <div>
        <div className="question"><div id="serial">{quiz.serial}. </div><div id="question">{quiz.question}</div></div>
        {quiz.choices.map((choice, index) => (
          <div id="choices" key={index}>
            <input type='radio' name='choice' value={index}/>{choice}
          </div>
        ))}
        <div>
        <div id="progress">
          <div id="bar"></div>
        </div>
        <div id="confidence">Confidence:
          <input type='radio' name='confidence-level' value='1' />
          <input type='radio' name='confidence-level' value='2' />
          <input type='radio' name='confidence-level' value='3' />
          <input type='radio' name='confidence-level' value='4' />
          <input type='radio' name='confidence-level' value='5' />
        </div>
        <button id="next" onClick={(e) => {
          e.preventDefault()
          dispatch(chooseAnswer(conf, quizSet, challenges, challenge, start, intervalId))
        }}>next</button>
        </div>
      </div>
    )
}

class QuizChallenge extends React.Component {

    componentWillReceiveProps(nextProps) {
        const {conf, dispatch, quizSet, areChallengesFetching} = this.props
        if (nextProps.areChallengesFetching && areChallengesFetching !== nextProps.areChallengesFetching) {
            dispatch(fetchChallenges(conf, quizSet))
        }
    }

    componentDidMount() {
        this.props.dispatch(fetchImage(this.props.conf))
        this.props.dispatch(fetchQuizSet(this.props.conf))
    }

    componentDidUpdate() {
        const {dispatch, conf, isQuizSetFetching, quizSet, challenges, challenge} = this.props
        const bar = document.getElementById('bar')
        const confidenceLevels = document.getElementsByName('confidence-level')
        if (!isQuizSetFetching && quizSet.length > 0 && challenges.length > 0 && challenge.indexToChallenges >= 0 && challenge.indexToChallenges < challenges.length) {
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
                    dispatch(chooseAnswer(conf, quizSet, challenges, challenge, start, intervalId))
                }
            }, conf.option.timeout * 10 /* in percents */)
        }
    }

    render() {
        const {
            conf,
            dispatch,
            isQuizSetFetching, quizSet, quizSetStatus,
            areChallengesFetching, challenges, challengesStatus,
            challenge
        } = this.props
        const allSet =
            !isQuizSetFetching &&
            !areChallengesFetching &&
            challengesStatus === 'Ok' &&
            quizSet.length > 0 &&
            challenges.length > 0
        return (
          <div>
            {
              !allSet
                ? (
                    <SetData
                      conf={conf}
                      collection={conf.collection}
                      isQuizSetFetching={isQuizSetFetching}
                      quizSet={quizSet}
                      quizSetStatus={quizSetStatus}
                      areChallengesFetching={areChallengesFetching}
                      challenges={challenges}
                      challengesStatus={challengesStatus}
                    />
                  )
                :
                  (
                    challenge.indexToChallenges < 0
                      ? (
                          <button onClick={(e) => {
                            e.preventDefault()
                            dispatch(haveChallengeReady(quizSet, challenges, 0, null))
                          }}>Start...</button>
                        )
                      : (
                          <Challenge
                            dispatch={dispatch}
                            conf={conf}
                            quizSet={quizSet}
                            challenges={challenges}
                            challenge={challenge}
                          />
                        )
                  )
            }
          </div>
        )
    }
}

const mapStateToProps = state => {
  const {isQuizSetFetching, quizSet, quizSetStatus, areChallengesFetching, challenges, challengesStatus, challenge} = state
  return {
    isQuizSetFetching,
    quizSet, quizSetStatus,
    areChallengesFetching,
    challenges, challengesStatus,
    challenge
  }
}

export default connect(mapStateToProps)(QuizChallenge)
