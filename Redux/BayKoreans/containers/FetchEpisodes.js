import React from 'react'
import {connect} from 'react-redux'
import {fetchEpisodes, fetchEpisodesIfNeeded} from '../actions'

const FetchClips = ({title, href}) => (
  <div>
  <a href={href}>{title}</a>
  </div>
)

class FetchEpisodes extends React.Component {
/*
  static propTypes = {
    selectedReddit: PropTypes.string.isRequired,
    posts: PropTypes.array.isRequired,
    isFetching: PropTypes.bool.isRequired,
    lastUpdated: PropTypes.number,
    dispatch: PropTypes.func.isRequired
  }

  componentDidMount() {
    const { dispatch, selectedReddit } = this.props
    dispatch(fetchPostsIfNeeded(selectedReddit))
  }

  componentWillReceiveProps(nextProps) {
    if (nextProps.selectedReddit !== this.props.selectedReddit) {
      const { dispatch, selectedReddit } = nextProps
      dispatch(fetchPostsIfNeeded(selectedReddit))
    }
  }

  handleChange = nextReddit => {
    this.props.dispatch(selectReddit(nextReddit))
  }

  handleRefreshClick = e => {
    e.preventDefault()

    const { dispatch, selectedReddit } = this.props
    dispatch(invalidateReddit(selectedReddit))
    dispatch(fetchPostsIfNeeded(selectedReddit))
  }
 */

  componentDidMount() {
    const {dispatch, fetchedShow} = this.props
    dispatch(fetchEpisodesIfNeeded(fetchedShow))
  }

  render() {
    const {dispatch, fetchedShow, isFetching, episodes, pages} = this.props
    return (
            <div id="content-div">
              <h1 id="mid-title-h1"><div id="mid-div">{fetchedShow.mid}</div><div id="mid-title-separator">/</div><div id="title-div">{fetchedShow.title}</div></h1>
              <ul id="episodes-ul">
                {episodes.map((episode, i) =>
                  <FetchClips key={i} title={episode.title} href={episode.href}/>
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
    )
/*
    const { selectedReddit, posts, isFetching, lastUpdated } = this.props
    const isEmpty = posts.length === 0
    return (
      <div>
        <Picker value={selectedReddit}
                onChange={this.handleChange}
                options={[ 'reactjs', 'frontend' ]} />
        <p>
          {lastUpdated &&
            <span>
              Last updated at {new Date(lastUpdated).toLocaleTimeString()}.
              {' '}
            </span>
          }
          {!isFetching &&
            <button onClick={this.handleRefreshClick}>
              Refresh
            </button>
          }
        </p>
        {isEmpty
          ? (isFetching ? <h2>Loading...</h2> : <h2>Empty.</h2>)
          : <div style={{ opacity: isFetching ? 0.5 : 1 }}>
              <Posts posts={posts} />
            </div>
        }
      </div>
    )
 */
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
