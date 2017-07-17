import React from 'react'
import {connect} from 'react-redux'
import {deleteBookmark, fetchShow} from '../actions'

const Bookmark = ({bookmark, onFetchClick, onDeleteClick}) => (
  <div style={{display: 'inline'}}>
    <button onClick={onFetchClick}>{bookmark.title}</button>
    <button onClick={onDeleteClick}>X</button>
  </div>
)

const BookmarkList = ({bookmarks, onFetchClick, onDeleteClick}) => (
  <div>
    {bookmarks.map((bookmark, index) =>
      <Bookmark
        key={index}
        bookmark={bookmark}
        onFetchClick = {() => {
            onFetchClick(bookmarks[index])
        }}
        onDeleteClick={() => onDeleteClick(index)}
      />
    )}
  </div>
)

const mapStateToProps = (state) => ({
  bookmarks: state.bookmarks
})

const mapDispatchToProps = {
  onFetchClick: fetchShow,
  onDeleteClick: deleteBookmark
}

const VisibleBookmarkList = connect(
  mapStateToProps,
  mapDispatchToProps
)(BookmarkList)

export default VisibleBookmarkList
