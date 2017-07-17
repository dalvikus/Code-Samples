import React from 'react'
import {connect} from 'react-redux'

import {toggleHideClips, fetchClips} from '../actions'

const ShowLink = ({href, text}) => (
  {href} === '' ? null : <li className="clip-li" onClick={(e) => {
    e.preventDefault()
    window.open(href, '_blank')}}>{text}</li>
)

class FetchClips extends React.Component {
  render() {
    const {dispatch, fetchedShow, episode} = this.props
    const {href, text, areClipsFetching, hideClips, clips} = episode
    return (
      <div className="episode-div">
      <li className="episode-li" onClick={(e) => {
        e.preventDefault()
        if (clips.length === 0)
            dispatch(fetchClips(fetchedShow, href, href))
        else
            dispatch(toggleHideClips(fetchedShow, href, href))
      }}>{text}</li>
      {areClipsFetching
        ? (<h3>fetch clips...</h3>)
        : (hideClips
             ? (null)
             : (clips.length === 0 ? (<h2>Empty</h2>) : (
                 <ul className="links-ul">
                   {clips.map((clip, i) =>
                     <ShowLink key={i} href={clip.href} text={clip.text}/>
                   )}
                 </ul>
               ))
         )
      }
      </div>
    )
  }
}

export default connect()(FetchClips)
