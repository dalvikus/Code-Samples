import React from 'react'
import {connect} from 'react-redux'

import {fetchEpisodes, fetchEpisodesIfNeeded} from '../actions'
import FetchClips from './FetchClips';

class FetchEpisodes extends React.Component {

  componentWillReceiveProps(nextProps) {
    const {dispatch, fetchedShow} = this.props
    if (fetchedShow.toString() !== nextProps.fetchedShow.toString())
        dispatch(fetchEpisodesIfNeeded(nextProps.fetchedShow))
  }
  componentDidMount() {
    const {dispatch, fetchedShow} = this.props
    dispatch(fetchEpisodesIfNeeded(fetchedShow))
  }

  render() {
    const {dispatch, fetchedShow, isFetching, episodes, pages} = this.props
    return (
      <div id="content-div">
        <h1 id="mid-title-h1"><div id="mid-div">{fetchedShow.mid}</div><div id="mid-title-separator">/</div><div id="title-div">{fetchedShow.title}</div></h1>
        {isFetching
           ? (<h3>fetch episodes...</h3>)
           : (episodes.length === 0 ? (<h2>Empty</h2>) : (
               <div>
               <ul id="episodes-ul">
                 {episodes.map((episode, i) =>
                   <FetchClips key={i} fetchedShow={fetchedShow} episode={episode}/>
                 )}
               </ul>
               <div id="pages-div">
                 {pages.map((page, i) =>
                   <button key={i} className="page-button" onClick={(e) => {
                     e.preventDefault();
                     dispatch(fetchEpisodes(fetchedShow, page.href))
                   }} disabled={page.href === ''}>{page.text}</button>
                 )}
               </div>
               </div>
             ))
        }
      </div>
    )
  }
}

const mapStateToProps = state => {
  const {fetchedShow, episodesByShow} = state
  const {
    isFetching,
    episodes,
    pages
  } = episodesByShow[fetchedShow.toString()] || {
    isFetching: true,
    episodes: [],
    pages: []
  }

  return {
    fetchedShow,
    isFetching,
    episodes,
    pages
  }
}

export default connect(mapStateToProps)(FetchEpisodes)
