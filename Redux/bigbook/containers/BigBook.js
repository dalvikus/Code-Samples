import React from 'react'
import {connect} from 'react-redux'
import '../index.css'

import {fetchBigBook, fetchChallenges, setChallenge, chooseAnswer} from '../actions'

var intervalId = 0
var start
//import VisibleForm from '../containers/VisibleForm'
//import VisibleBookmarkList from '../containers/VisibleBookmarkList'
//import FetchEpisodes from '../containers/FetchEpisodes'

class BigBook extends React.Component {

  componentWillReceiveProps(nextProps) {
    const {dispatch, areChallengesFetching} = this.props
    if (nextProps.areChallengesFetching && areChallengesFetching !== nextProps.areChallengesFetching)
        dispatch(fetchChallenges())
  }
    componentDidMount() {
        this.props.dispatch(fetchBigBook('http://localhost:1337/user'))
    }

    componentDidUpdate() {
        const {dispatch, isBigBookFetching, bigbook, areChallengesFetching, challenges, challenge} = this.props
        if (!isBigBookFetching && bigbook.length > 0 && challenges.length > 0 && challenge.serial >= 0) {
            const progress = document.getElementById('progress')
            const bar = document.getElementById('bar')
            let n = 0
            intervalId = window.setInterval(() => {
                ++n
                bar.style.width = n + '%'
                if (n === 100)
                    dispatch(chooseAnswer(challenges, challenge, start, intervalId))
            }, 300)
console.log(intervalId)
        }
    }

    render() {
        const {dispatch, isBigBookFetching, bigbook, areChallengesFetching, challenges, challenge} = this.props
        start = Date.now()
        return (
          <div>
            {isBigBookFetching ? (<h2>fetch bigbook...</h2>) : (
                (bigbook.length > 0 && challenges.length > 0) ?
                (
                challenge.serial < 0 ? (
                    <button onClick={(e) => {
                        e.preventDefault()
                        dispatch(setChallenge(0))
                    }}>Start...</button>
                ) : (
                    <div>
                    <div>{challenge.question}</div>
                    {challenge.choices.map((choice, index) =>
                        <div key={index}>
                        <input type='radio' name='choose' value={index}/>
                        {choice}
                        </div>
                    )}
                    <div>
                    <div id="progress">
                      <div id="bar"></div>
                    </div>
                    <button onClick={(e) => {
                        e.preventDefault()
                        dispatch(chooseAnswer(challenges, challenge, start, intervalId))
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
